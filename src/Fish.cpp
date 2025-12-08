#include "Fish.h"

#include <algorithm>
#include <cmath>

FishTraits GetTraits(FishType type)
{
    switch (type)
    {
    case FishType::Small:
        return { 5, 5, 80.0f, 0.15f, {64, 32} };
    case FishType::Medium:
        return { 12, 10, 65.0f, 0.25f, {96, 48} };
    case FishType::Large:
    default:
        return { 25, 20, 50.0f, 0.35f, {128, 64} };
    }
}

Fish::Fish()
    : x_(0.0f)
    , y_(0.0f)
    , direction_(1.0f)
    , type_(FishType::Small)
    , alive_(true)
    , attached_(false)
    , sprite_(nullptr)
{
}

Fish Fish::SpawnRandom(const RECT& waterRect, std::mt19937& rng)
{
    std::uniform_real_distribution<float> sideDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> typeDist(0.0f, 1.0f);
    // Allow spawns very close to the water surface so Rod Lv1 (shallow cast) can reach.
    float spawnTop = static_cast<float>(waterRect.top) + 4.0f;
    float spawnBottom = static_cast<float>(waterRect.bottom) - 40.0f;
    if (spawnBottom < spawnTop) spawnBottom = spawnTop + 8.0f;
    std::uniform_real_distribution<float> yDist(spawnTop, spawnBottom);

    Fish fish;
    const float side = sideDist(rng);
    fish.direction_ = side < 0.5f ? 1.0f : -1.0f;

    const float t = typeDist(rng);
    if (t < 0.5f)
        fish.type_ = FishType::Small;
    else if (t < 0.8f)
        fish.type_ = FishType::Medium;
    else
        fish.type_ = FishType::Large;

    fish.traits_ = GetTraits(fish.type_);
    fish.y_ = yDist(rng);
    fish.x_ = (fish.direction_ > 0.0f) ? static_cast<float>(waterRect.left - fish.traits_.size.cx) : static_cast<float>(waterRect.right + fish.traits_.size.cx);

    return fish;
}

void Fish::Update(float deltaTime, const RECT& bounds)
{
    if (!alive_)
        return;

    if (attached_)
        return;

    if (!attached_)
    {
        x_ += direction_ * traits_.speed * deltaTime;

        if ((direction_ > 0 && x_ > bounds.right + traits_.size.cx) ||
            (direction_ < 0 && x_ + traits_.size.cx < bounds.left))
        {
            alive_ = false;
        }
    }
}

void Fish::Render(HDC hdc) const
{
    if (!alive_)
        return;

    const int w = traits_.size.cx;
    const int h = traits_.size.cy;

    const int drawX = static_cast<int>(x_);
    const int drawY = static_cast<int>(y_);

    COLORREF fillColor = RGB(220, 140, 80);
    COLORREF outline = RGB(120, 80, 50);
    if (type_ == FishType::Medium)
    {
        fillColor = RGB(240, 180, 80);
        outline = RGB(150, 110, 40);
    }
    else if (type_ == FishType::Large)
    {
        fillColor = RGB(200, 100, 60);
        outline = RGB(100, 60, 30);
    }

    if (sprite_ && sprite_->IsValid())
    {
        const bool flip = (direction_ < 0.0f);
        if (flip)
            sprite_->DrawFlippedH(hdc, drawX, drawY, w, h);
        else
            sprite_->Draw(hdc, drawX, drawY, w, h);
        HPEN outlinePen = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
        HGDIOBJ oldPen = SelectObject(hdc, outlinePen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        Rectangle(hdc, drawX, drawY, drawX + w, drawY + h);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(outlinePen);
        return;
    }

    // Legacy simple rectangle fallback
    HBRUSH bodyBrush = CreateSolidBrush(fillColor);
    HPEN bodyPen = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
    HGDIOBJ oldBrush = SelectObject(hdc, bodyBrush);
    HGDIOBJ oldPen = SelectObject(hdc, bodyPen);
    Rectangle(hdc, drawX, drawY, drawX + w, drawY + h);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(bodyBrush);
    DeleteObject(bodyPen);
}

RECT Fish::GetRect() const
{
    const int margin = 2; // include outline thickness
    RECT r{};
    r.left = static_cast<LONG>(x_) - margin;
    r.top = static_cast<LONG>(y_) - margin;
    r.right = r.left + traits_.size.cx + margin * 2;
    r.bottom = r.top + traits_.size.cy + margin * 2;
    return r;
}

void Fish::AttachTo(const POINT& hookPos)
{
    attached_ = true;
    x_ = static_cast<float>(hookPos.x) - traits_.size.cx * 0.5f;
    y_ = static_cast<float>(hookPos.y) - traits_.size.cy * 0.5f;
}
