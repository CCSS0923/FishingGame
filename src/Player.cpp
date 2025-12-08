#include "Player.h"

Player::Player()
    : x_(100.0f), y_(100.0f), speed_(200.0f), sprite_(nullptr)
{
}

void Player::SetPosition(float x, float y)
{
    x_ = x;
    y_ = y;
}

void Player::SetSpeed(float speed)
{
    speed_ = speed;
}

void Player::Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth)
{
    if (moveLeft)
    {
        x_ -= speed_ * deltaTime;
    }
    if (moveRight)
    {
        x_ += speed_ * deltaTime;
    }

    const float minX = 0.0f;
    const float maxX = static_cast<float>(windowWidth - kWidth);
    if (x_ < minX)
        x_ = minX;
    if (x_ > maxX)
        x_ = maxX;
}

void Player::Render(HDC hdc) const
{
    RECT boat = GetHitRect();
    if (sprite_ && sprite_->IsValid())
    {
        sprite_->Draw(hdc, boat.left, boat.top, boat.right - boat.left, boat.bottom - boat.top);
        HPEN outline = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
        HGDIOBJ oldPen = SelectObject(hdc, outline);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        Rectangle(hdc, boat.left, boat.top, boat.right, boat.bottom);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(outline);
        return;
    }
    // Legacy simple rectangle fallback
    HBRUSH body = CreateSolidBrush(RGB(50, 90, 160));
    HPEN border = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
    HGDIOBJ oldBrush = SelectObject(hdc, body);
    HGDIOBJ oldPen = SelectObject(hdc, border);
    Rectangle(hdc, boat.left, boat.top, boat.right, boat.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(body);
    DeleteObject(border);
}

RECT Player::GetHitRect() const
{
    const int margin = 2; // include outline thickness
    const int w = kWidth + margin * 2;
    const int h = kHeight + margin * 2;
    RECT r{};
    r.left = static_cast<LONG>(x_) - margin;
    r.top = static_cast<LONG>(y_) - margin;
    r.right = r.left + w;
    r.bottom = r.top + h;
    return r;
}

POINT Player::GetLineOrigin() const
{
    RECT hit = GetHitRect();
    POINT p{};
    p.x = (hit.left + hit.right) / 2;
    p.y = hit.bottom - 4;
    return p;
}
