// 플레이어 이동, 입력 반영, 스프라이트 렌더링을 구현한 소스입니다.
#include "Player.h"

Player::Player()
    : x_(100.0f), y_(100.0f), speed_(200.0f), sprite_(nullptr)
{
}

// 위치를 즉시 좌표로 설정.
void Player::SetPosition(float x, float y)
{
    x_ = x;
    y_ = y;
}

void Player::SetSpeed(float speed)
{
    speed_ = speed;
}

// 입력 플래그와 델타 시간에 따라 수평 이동 처리.
void Player::Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth)
{
    if (moveLeft)
        x_ -= speed_ * deltaTime;
    if (moveRight)
        x_ += speed_ * deltaTime;

    const float minX = 0.0f;   // 화면 좌측 한계
    const float maxX = static_cast<float>(windowWidth - kWidth);
    if (x_ < minX) x_ = minX;
    if (x_ > maxX) x_ = maxX;
}

// 보트 스프라이트가 없으면 단순 도형으로 렌더링.
void Player::Render(HDC hdc) const
{
    RECT r = GetHitRect();
    if (sprite_ && sprite_->IsValid())
    {
        sprite_->Draw(hdc, r.left, r.top, kWidth, kHeight);
    }
    else
    {
        HBRUSH body = CreateSolidBrush(RGB(50, 90, 160));
        HGDIOBJ oldBrush = SelectObject(hdc, body);
        HPEN border = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
        HGDIOBJ oldPen = SelectObject(hdc, border);
        Rectangle(hdc, r.left, r.top, r.right, r.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(border);
        SelectObject(hdc, oldBrush);
        DeleteObject(body);
    }
}

RECT Player::GetHitRect() const
{
    RECT r{};
    r.left = static_cast<LONG>(x_);
    r.top = static_cast<LONG>(y_);
    r.right = r.left + kWidth;
    r.bottom = r.top + kHeight;
    return r;
}

POINT Player::GetLineOrigin() const
{
    // 낚싯줄이 나가는 기준점을 보트 상단 중앙으로 설정.
    return POINT{
        static_cast<LONG>(x_ + kWidth / 2),
        static_cast<LONG>(y_)
    };
}
