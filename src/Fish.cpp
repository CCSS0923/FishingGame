// 물고기 이동, 스폰, 렌더링 로직을 구현한 소스입니다.
#include "Fish.h"

#include <algorithm>
#include <cmath>

// 물고기 종류에 맞는 능력치를 돌려준다.
FishTraits GetTraits(FishType type)
{
    switch (type)
    {
    case FishType::Small:
        return {
            5,            // score: 작은 물고기 점수
            5,            // money: 작은 물고기 보상 골드
            80.0f,        // speed: 초당 이동 속도(px/s)
            0.15f,        // tensionAdd: 초당 라인 긴장도 증가치
            { 64, 32 }    // size: 렌더링 크기(px)
        };
    case FishType::Medium:
        return {
            12,           // score: 중간 물고기 점수
            10,           // money: 중간 물고기 보상 골드
            65.0f,        // speed: 초당 이동 속도(px/s)
            0.25f,        // tensionAdd: 초당 라인 긴장도 증가치
            { 96, 48 }    // size: 렌더링 크기(px)
        };
    case FishType::Large:
    default:
        return {
            25,           // score: 큰 물고기 점수
            20,           // money: 큰 물고기 보상 골드
            50.0f,        // speed: 초당 이동 속도(px/s)
            0.35f,        // tensionAdd: 초당 라인 긴장도 증가치
            { 128, 64 }   // size: 렌더링 크기(px)
        };
    }
}

// 기본 상태의 물고기 생성.
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

// 수역 영역을 기준으로 랜덤한 위치/종류의 물고기를 만든다.
Fish Fish::SpawnRandom(const RECT& waterRect, std::mt19937& rng)
{
    std::uniform_real_distribution<float> sideDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> typeDist(0.0f, 1.0f);
    // 로드 LV1도 닿을 수 있도록 수면 가까이서도 스폰 허용.
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

// 이동 및 생존 여부 갱신.
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

// 살아 있는 경우 화면에 물고기를 그린다.
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

    // 스프라이트가 없을 때의 단순 사각형 대체 렌더.
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
    const int margin = 2; // 외곽선까지 포함하도록 여유를 둔다.
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
