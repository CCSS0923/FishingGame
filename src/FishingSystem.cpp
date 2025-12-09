// 낚시 시스템의 상태 전환과 캐스팅/릴링 흐름을 모두 구현한 소스입니다.
#include "FishingSystem.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kPi = 3.1415926535f; // 원주율 상수.
}

// 낚시 상태 초기화
FishingSystem::FishingSystem()
    : state_(FishingState::Idle)
    , castPower_(0.0f)
    , castPowerMax_(240.0f)      // 기본 캐스팅 최대 거리(px)
    , castAngleDeg_(75.0f)       // 기본 투사각(도)
    , lineTension_(0.0f)
    , lineTensionDecay_(0.35f)   // 라인 장력 자연 감소 속도
    , chargeRate_(140.0f)        // 캐스팅 파워 충전 속도(px/s)
    , reelSpeed_(180.0f)         // 릴링 속도(px/s)
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

// 물 영역 설정
void FishingSystem::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
    RecalculateStats();
}

// 장비 레벨 설정
void FishingSystem::SetLevels(int rodLevel, int lineLevel)
{
    rodLevel_ = rodLevel;
    lineLevel_ = lineLevel;
    RecalculateStats();
}

// 레벨/수면 높이에 따른 스탯 재계산
void FishingSystem::RecalculateStats()
{
    float waterHeight = static_cast<float>(waterRect_.bottom - waterRect_.top);

    // 장비 레벨이 범위를 벗어나지 않도록 클램프.
    rodLevel_ = std::clamp(rodLevel_, 1, kMaxLevel);
    lineLevel_ = std::clamp(lineLevel_, 1, kMaxLevel);

    auto lerp = [](float a, float b, float t) { return a + (b - a) * t; };
    auto levelFactor = [this](int level) {
        float t = static_cast<float>(level - 1) / static_cast<float>(kMaxLevel - 1);
        return std::clamp(t, 0.0f, 1.0f);
    };

    // 로드: LV1은 수심 10%까지, LV10은 수심 100%까지 도달.
    float rodT = levelFactor(rodLevel_);
    float depthFraction = lerp(0.10f, 1.0f, rodT); // 최소 10%~100% 수심 비율로 캐스팅
    castPowerMax_ = waterHeight * depthFraction;

    // 라인: 충전/릴링 속도가 선형 증가하며, LV1은 최저 수치.
    float lineT = levelFactor(lineLevel_);
    chargeRate_ = lerp(50.0f, 300.0f, lineT);      // LV1~LV10 캐스팅 파워 충전 속도 범위
    reelSpeed_ = lerp(90.0f, 380.0f, lineT);       // LV1~LV10 릴링 속도 범위
    lineTensionDecay_ = lerp(0.15f, 0.45f, lineT); // LV1~LV10 장력 감소 속도 범위
}

// 캐스팅 파워 충전 시작
void FishingSystem::StartCharge()
{
    state_ = FishingState::Charging;
    castPower_ = 0.0f;
}

// 현재 각도/파워로 캐스팅
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
    hookPos_.x = std::clamp(hookPos_.x, static_cast<float>(waterRect_.left + 10), static_cast<float>(waterRect_.right - 10)); // 물가 여유 10px
    hookPos_.y = std::clamp(hookPos_.y, static_cast<float>(waterRect_.top + 10), static_cast<float>(waterRect_.bottom - 5));  // 수면/바닥 여유

    bobberPos_ = hookPos_;

    biteTimer_ = 1.0f + static_cast<float>((rng_() % 120)) / 60.0f; // 1.0~3.0초 랜덤 대기
    state_ = FishingState::Floating;
}

// 릴 시작
void FishingSystem::StartReel()
{
    if (state_ == FishingState::Floating || state_ == FishingState::Bite)
    {
        state_ = FishingState::Reeling;
    }
}

// 상태/위치 초기화
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

// 바늘 충돌 박스 계산
RECT FishingSystem::GetHookRect() const
{
    const int size = 24; // 훅 판정 박스 크기(px)
    RECT r{};
    r.left = static_cast<LONG>(hookPos_.x - size / 2);
    r.top = static_cast<LONG>(hookPos_.y - size / 2);
    r.right = r.left + size;
    r.bottom = r.top + size;
    return r;
}

// pendingCatch_가 있다면 outResult로 복사하고 초기화
bool FishingSystem::ConsumeCatch(CatchResult& outResult)
{
    if (!pendingCatch_.caught)
        return false;

    outResult = pendingCatch_;
    pendingCatch_ = CatchResult{};
    return true;
}

// 입력/플레이어/물고기 상태를 반영해 낚시 로직 업데이트
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
        // 충전 중에는 마우스 방향에 맞춰 각도 갱신.


        // 조준 거리와 최대 파워를 고려해 캐스팅 파워를 축적.

            float targetPower = std::min(aimDistance, castPowerMax_);
            castPower_ = std::min(targetPower, castPower_ + chargeRate_ * deltaTime);

            if (!mouseDown || mouseReleased)
            {
                // 버튼을 떼면 현재 각도/파워로 던짐.

                Cast(player);
            }
        }
        break;
        // 캐스팅 직후 바로 플로팅 상태로 전환.

        // 캐스팅 직후 바로 플로팅 상태로 전환.
        break;
    case FishingState::Floating:
        biteTimer_ -= deltaTime;
        bobberPos_.y = hookPos_.y + std::sin(floatTimer_ * 3.0f) * 4.0f; // 떠있을 때 작은 흔들림

        if (biteTimer_ <= 0.0f)
        {
            state_ = FishingState::Bite;
            biteTimer_ = 1.0f; // 물기 상태 유지 시간
        }

        if (mousePressed && !inputBlocked)
        {
            StartReel();
        }
        break;
    case FishingState::Bite:
        bobberPos_.y = hookPos_.y + std::sin(floatTimer_ * 5.0f) * 6.0f; // 물기 중 더 큰 흔들림
        lineTension_ = std::min(1.0f, lineTension_ + 0.15f * deltaTime); // 물기 중 장력 상승 속도

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

        // 캐치 허용 거리(px).


        if (dist > 1.0f) // 남은 거리가 1px 이상이면 끌어오기
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
            caughtTimer_ = 0.6f; // 플레이어에 붙었을 때 연출 시간

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

        // 물고기가 가까우면 즉시 낚은 것으로 처리.

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
            caughtTimer_ = 0.4f; // 즉시 낚았을 때 연출 시간
            break;
        }

        float decay = lineTensionDecay_ * deltaTime;
        if (!mouseDown)
            decay *= 1.2f; // 마우스를 떼면 추가 감쇠
        lineTension_ = std::max(0.0f, lineTension_ - decay);

        if (hookedFish_)
        {
            FishTraits traits = GetTraits(hookedFish_->GetType());
            lineTension_ += traits.tensionAdd * deltaTime;
            hookedFish_->SetPosition(hookPos_.x - traits.size.cx * 0.5f, hookPos_.y - traits.size.cy * 0.5f);
        }

        if (lineTension_ >= 1.0f) // 장력 한계 도달
        {
            state_ = FishingState::Fail;
            failTimer_ = 0.8f; // 실패 연출 시간
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
        lineTension_ = std::max(0.0f, lineTension_ - 0.8f * deltaTime); // 실패 상태 감쇠 속도
        if (failTimer_ <= 0.0f)
        {
            ResetLine(player);
        }
        break;
    }

    // 플로팅/바이트 상태에서는 장력을 서서히 줄이고 충돌을 검사.

    {
        lineTension_ = std::max(0.0f, lineTension_ - lineTensionDecay_ * deltaTime); // 부력 상태 장력 감소

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

// 낚싯줄과 찌, 바늘을 현재 상태에 맞게 그린다.

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

