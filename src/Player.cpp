// ?뚮젅?댁뼱 ?대룞, ?낅젰 諛섏쁺, ?ㅽ봽?쇱씠???뚮뜑留곸쓣 援ы쁽???뚯뒪?낅땲??
#include "Player.h"

Player::Player()
    : x_(100.0f), y_(100.0f), speed_(200.0f), sprite_(nullptr)
{
}

// ?꾩튂瑜?利됱떆 醫뚰몴濡??ㅼ젙.
void Player::SetPosition(float x, float y)
{
    x_ = x;
    y_ = y;
}

void Player::SetSpeed(float speed)
{
    speed_ = speed;
}

// ?낅젰 ?뚮옒洹몄? ?명? ?쒓컙???곕씪 ?섑룊 ?대룞 泥섎━.
void Player::Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth)
{
    if (moveLeft)
        x_ -= speed_ * deltaTime;
    if (moveRight)
        x_ += speed_ * deltaTime;

    const float minX = 0.0f;   // ?붾㈃ 醫뚯륫 ?쒓퀎
    const float maxX = static_cast<float>(windowWidth - kWidth);
    if (x_ < minX) x_ = minX;
    if (x_ > maxX) x_ = maxX;
}

// 蹂댄듃 ?ㅽ봽?쇱씠?멸? ?놁쑝硫??⑥닚 ?꾪삎?쇰줈 ?뚮뜑留?
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
    // ?싳떙以꾩씠 ?섍???湲곗??먯쓣 蹂댄듃 ?곷떒 以묒븰?쇰줈 ?ㅼ젙.
    return POINT{
        static_cast<LONG>(x_ + kWidth / 2),
        static_cast<LONG>(y_)
    };
}
