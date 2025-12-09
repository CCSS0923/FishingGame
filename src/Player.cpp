// 플레이어 이동, 입력 반영, 스프라이트 렌더링을 구현한 소스입니다.
#include "Player.h"

// 보트 기본 속성 설정.
Player::Player()
    : x_(100.0f), y_(100.0f), speed_(200.0f), sprite_(nullptr)
{
}

// 위치를 절대 좌표로 설정.
void Player::SetPosition(float x, float y)
{
    x_ = x;
    y_ = y;
}

// 이동 속도 설정.
void Player::SetSpeed(float speed)
{
    speed_ = speed;
}

// 입력 플래그와 델타 시간으로 수평 이동을 처리한다.
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

// 보트 스프라이트 또는 대체 도형을 그린다.
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
    // 스프라이트가 없을 때의 기본 사각형 표현.
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

// 충돌 판정 및 렌더 기준이 되는 보트 외접 사각형을 계산.
RECT Player::GetHitRect() const
{
    const int margin = 2; // 외곽선 두께를 포함하기 위한 여백.
    const int w = kWidth + margin * 2;
    const int h = kHeight + margin * 2;
    RECT r{};
    r.left = static_cast<LONG>(x_) - margin;
    r.top = static_cast<LONG>(y_) - margin;
    r.right = r.left + w;
    r.bottom = r.top + h;
    return r;
}

// 낚싯줄이 시작되는 위치를 반환.
POINT Player::GetLineOrigin() const
{
    RECT hit = GetHitRect();
    POINT p{};
    p.x = (hit.left + hit.right) / 2;
    p.y = hit.bottom - 4;
    return p;
}
