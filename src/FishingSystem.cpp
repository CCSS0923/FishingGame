#include "FishingSystem.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kPi = 3.1415926535f;
}

FishingSystem::FishingSystem()
    : state_(FishingState::Idle)
    , castPower_(0.0f)
    , castPowerMax_(240.0f)
    , castAngleDeg_(75.0f)
    , lineTension_(0.0f)
    , lineTensionDecay_(0.35f)
    , chargeRate_(140.0f)
    , reelSpeed_(180.0f)
    , biteTimer_(0.0f)
    , floatTimer_(0.0f)
    , failTimer_(0.0f)
    , caughtTimer_(0.0f)
    , hookPos_{ 0, 0 }
    , bobberPos_{ 0, 0 }
    , waterRect_{ 0,0,0,0 }
    , hookedFish_(nullptr)
    , rodLevel_(1)
    , lineLevel_(1)
    , rng_(static_cast<unsigned int>(std::random_device{}()))
{
}

void FishingSystem::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
    RecalculateStats();
}

void FishingSystem::SetLevels(int rodLevel, int lineLevel)
{
    rodLevel_ = rodLevel;
    lineLevel_ = lineLevel;
    RecalculateStats();
}

void FishingSystem::RecalculateStats()
{
    float waterHeight = static_cast<float>(waterRect_.bottom - waterRect_.top);

    rodLevel_ = std::clamp(rodLevel_, 1, kMaxLevel);
    lineLevel_ = std::clamp(lineLevel_, 1, kMaxLevel);

    auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };
    auto levelFactor = [this](int level) {
        float t = static_cast<float>(level - 1) / static_cast<float>(kMaxLevel - 1);
        return std::clamp(t, 0.0f, 1.0f);
    };

    float rodT = levelFactor(rodLevel_);
    float depthFraction = lerp(0.10f, 1.0f, rodT);
    castPowerMax_ = waterHeight * depthFraction;

    float lineT = levelFactor(lineLevel_);
    chargeRate_ = lerp(50.0f, 300.0f, lineT);
    reelSpeed_ = lerp(90.0f, 380.0f, lineT);
    lineTensionDecay_ = lerp(0.15f, 0.45f, lineT);
}

void FishingSystem::StartCharge()
{
    state_ = FishingState::Charging;
    castPower_ = 0.0f;
}

void FishingSystem::Cast(const Player& player)
{
    state_ = FishingState::Casting;
    POINT origin = player.GetLineOrigin();
    const float clampedPower = std::min(castPower_, castPowerMax_);

    float rad = castAngleDeg_ * kPi / 180.0f;
    float dx = std::cos(rad) * clampedPower;
    float dy = std::sin(rad) * clampedPower;

    hookPos_.x = static_cast<float>(origin.x) + dx;
    hookPos_.y = static_cast<float>(origin.y) + dy;
    hookPos_.x = std::clamp(hookPos_.x, static_cast<float>(waterRect_.left + 10), static_cast<float>(waterRect_.right - 10));
    hookPos_.y = std::clamp(hookPos_.y, static_cast<float>(waterRect_.top + 10), static_cast<float>(waterRect_.bottom - 5));

    bobberPos_ = hookPos_;

    biteTimer_ = 1.0f + static_cast<float>((rng_() % 120)) / 60.0f;
    state_ = FishingState::Floating;
}

void FishingSystem::StartReel()
{
    if (state_ == FishingState::Floating || state_ == FishingState::Bite)
    {
        state_ = FishingState::Reeling;
    }
}

void FishingSystem::ResetLine(const Player& player)
{
    state_ = FishingState::Idle;
    hookPos_.x = static_cast<float>(player.GetLineOrigin().x);
    hookPos_.y = static_cast<float>(player.GetLineOrigin().y);
    bobberPos_ = hookPos_;
    lineTension_ = 0.0f;
    if (hookedFish_)
    {
        hookedFish_->Kill();
        hookedFish_ = nullptr;
    }
    failTimer_ = 0.0f;
    caughtTimer_ = 0.0f;
}

RECT FishingSystem::GetHookRect() const
{
    const int size = 24;
    RECT r{};
    r.left = static_cast<LONG>(hookPos_.x - size / 2);
    r.top = static_cast<LONG>(hookPos_.y - size / 2);
    r.right = r.left + size;
    r.bottom = r.top + size;
    return r;
}

bool FishingSystem::ConsumeCatch(CatchResult& outResult)
{
    if (!pendingCatch_.caught)
        return false;

    outResult = pendingCatch_;
    pendingCatch_ = CatchResult{};
    return true;
}

void FishingSystem::Update(float deltaTime, const Player& player, FishManager& fishManager,
    const POINT& mousePos, bool mouseDown, bool mousePressed, bool mouseReleased, bool inputBlocked)
{
    if (hookedFish_ && !hookedFish_->IsAlive())
    {
        hookedFish_ = nullptr;
    }

    floatTimer_ += deltaTime;
    POINT origin = player.GetLineOrigin();

    auto computeAim = [&](float& outAngleDeg, float& outDistance)
    {
        float dx = static_cast<float>(mousePos.x - origin.x);
        float dy = static_cast<float>(mousePos.y - origin.y);
        outAngleDeg = std::atan2(dy, dx) * 180.0f / kPi;
        outDistance = std::sqrt(dx * dx + dy * dy);
    };

    float aimAngleDeg = castAngleDeg_;
    float aimDistance = 0.0f;
    computeAim(aimAngleDeg, aimDistance);

    switch (state_)
    {
    case FishingState::Idle:
        ResetLine(player);
        if (mousePressed && !inputBlocked)
        {
            StartCharge();
        }
        break;

    case FishingState::Charging:
    {
        castAngleDeg_ = aimAngleDeg;
        float targetPower = std::min(aimDistance, castPowerMax_);
        castPower_ = std::min(targetPower, castPower_ + chargeRate_ * deltaTime);

        if (!mouseDown || mouseReleased)
        {
            castPower_ = std::min(castPower_, targetPower);
            Cast(player);
        }
        break;
    }

    case FishingState::Casting:
        state_ = FishingState::Floating;
        break;

    case FishingState::Floating:
        biteTimer_ -= deltaTime;
        bobberPos_.y = hookPos_.y + std::sin(floatTimer_ * 3.0f) * 4.0f;

        if (biteTimer_ <= 0.0f)
        {
            state_ = FishingState::Bite;
            biteTimer_ = 1.0f;
        }

        if (mousePressed && !inputBlocked)
        {
            StartReel();
        }
        break;

    case FishingState::Bite:
        bobberPos_.y = hookPos_.y + std::sin(floatTimer_ * 5.0f) * 6.0f;
        lineTension_ = std::min(1.0f, lineTension_ + 0.15f * deltaTime);

        if (mousePressed && !inputBlocked)
        {
            StartReel();
        }
        break;

    case FishingState::Reeling:
    {
        POINT reelOrigin = player.GetLineOrigin();
        float dx = static_cast<float>(reelOrigin.x) - hookPos_.x;
        float dy = static_cast<float>(reelOrigin.y) - hookPos_.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        const float catchThreshold = 28.0f;

        if (dist > 1.0f)
        {
            float step = reelSpeed_ * deltaTime;
            if (step > dist)
                step = dist;
            float nx = dx / dist;
            float ny = dy / dist;
            hookPos_.x += nx * step;
            hookPos_.y += ny * step;
            bobberPos_ = hookPos_;
        }
        else
        {
            hookPos_.x = static_cast<float>(origin.x);
            hookPos_.y = static_cast<float>(origin.y);
            state_ = FishingState::Caught;
            caughtTimer_ = 0.6f;

            if (hookedFish_)
            {
                pendingCatch_.caught = true;
                pendingCatch_.type = hookedFish_->GetType();
                pendingCatch_.score = hookedFish_->GetScoreValue();
                pendingCatch_.money = hookedFish_->GetMoneyValue();
                hookedFish_->Kill();
                hookedFish_ = nullptr;
            }
            lineTension_ = 0.0f;
            break;
        }

        if (!hookedFish_)
        {
            Fish* hit = fishManager.CheckHookCollision(GetHookRect());
            if (hit && !hit->IsAttached())
            {
                hit->AttachTo(POINT{ static_cast<LONG>(hookPos_.x), static_cast<LONG>(hookPos_.y) });
                hookedFish_ = hit;
            }
        }

        if (hookedFish_ && dist <= catchThreshold)
        {
            hookPos_.x = static_cast<float>(origin.x);
            hookPos_.y = static_cast<float>(origin.y);
            pendingCatch_.caught = true;
            pendingCatch_.type = hookedFish_->GetType();
            pendingCatch_.score = hookedFish_->GetScoreValue();
            pendingCatch_.money = hookedFish_->GetMoneyValue();
            hookedFish_->Kill();
            hookedFish_ = nullptr;
            lineTension_ = 0.0f;
            state_ = FishingState::Caught;
            caughtTimer_ = 0.4f;
            break;
        }

        float decay = lineTensionDecay_ * deltaTime;
        if (!mouseDown)
            decay *= 1.2f;
        lineTension_ = std::max(0.0f, lineTension_ - decay);

        if (hookedFish_)
        {
            FishTraits traits = GetTraits(hookedFish_->GetType());
            lineTension_ += traits.tensionAdd * deltaTime;
            hookedFish_->SetPosition(hookPos_.x - traits.size.cx * 0.5f, hookPos_.y - traits.size.cy * 0.5f);
        }

        if (lineTension_ >= 1.0f)
        {
            state_ = FishingState::Fail;
            failTimer_ = 0.8f;
            lineTension_ = 1.0f;
            if (hookedFish_)
            {
                hookedFish_->Kill();
                hookedFish_ = nullptr;
            }
        }
        break;
    }

    case FishingState::Caught:
        caughtTimer_ -= deltaTime;
        if (caughtTimer_ <= 0.0f)
        {
            ResetLine(player);
        }
        break;

    case FishingState::Fail:
        failTimer_ -= deltaTime;
        lineTension_ = std::max(0.0f, lineTension_ - 0.8f * deltaTime);
        if (failTimer_ <= 0.0f)
        {
            ResetLine(player);
        }
        break;
    }

    if (state_ == FishingState::Floating || state_ == FishingState::Bite)
    {
        lineTension_ = std::max(0.0f, lineTension_ - lineTensionDecay_ * deltaTime);

        if (!hookedFish_)
        {
            Fish* hit = fishManager.CheckHookCollision(GetHookRect());
            if (hit && !hit->IsAttached())
            {
                hookedFish_ = hit;
                hit->AttachTo(POINT{ static_cast<LONG>(hookPos_.x), static_cast<LONG>(hookPos_.y) });
                state_ = FishingState::Reeling;
            }
        }
    }
}

void FishingSystem::Render(HDC hdc, const Player& player) const
{
    POINT origin = player.GetLineOrigin();
    int penWidth = 2 + lineLevel_;
    penWidth = std::min(penWidth, 6);
    HPEN pen = CreatePen(PS_SOLID, penWidth, RGB(240, 240, 240));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    MoveToEx(hdc, origin.x, origin.y, nullptr);
    LineTo(hdc, static_cast<int>(hookPos_.x), static_cast<int>(hookPos_.y));
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    const int bobberSize = 20;
    HBRUSH brush = CreateSolidBrush(RGB(230, 50, 50));
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    Ellipse(hdc, static_cast<int>(bobberPos_.x - bobberSize / 2), static_cast<int>(bobberPos_.y - bobberSize / 2),
        static_cast<int>(bobberPos_.x + bobberSize / 2), static_cast<int>(bobberPos_.y + bobberSize / 2));
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);

    HBRUSH brushHook = CreateSolidBrush(RGB(200, 200, 200));
    RECT r = GetHookRect();
    FillRect(hdc, &r, brushHook);
    DeleteObject(brushHook);
}
