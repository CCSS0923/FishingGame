// ?뚮젅?댁뼱 ?낅젰 泥섎━? ?대룞, 諛쒖궗 ?꾩튂 ?쒓났 ?깆쓣 ?대떦?섎뒗 ?뚮젅?댁뼱 媛앹껜 ?ㅻ뜑?낅땲??
#pragma once

#include <windows.h>
#include "Sprite.h"

class Player
{
public:
    Player(); // ?뚮젅?댁뼱 蹂댄듃 珥덇린??

    // ?붾㈃ ???꾩튂 吏??
    void SetPosition(float x, float y);
    // ?대룞 ?띾룄 ?ㅼ젙.
    void SetSpeed(float speed);
    void SetSprite(Sprite* sprite) { sprite_ = sprite; }

    // ?낅젰 ?뚮옒洹몄? ?명? ?쒓컙?쇰줈 ?섑룊 ?대룞 泥섎━.
    void Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth);
    // 蹂댄듃 ?ㅽ봽?쇱씠???泥??꾪삎 ?뚮뜑留?
    void Render(HDC hdc) const;

    // 異⑸룎/洹몃━湲곗뿉 ?곗씠???ш컖??諛섑솚.
    RECT GetHitRect() const;
    // ?싳떙以꾩씠 ?섍???湲곗??먯쓣 諛섑솚.
    POINT GetLineOrigin() const;

    float GetX() const { return x_; }
    float GetY() const { return y_; }

private:
    float x_;   // 蹂댄듃??醫뚯긽??X 醫뚰몴.
    float y_;   // 蹂댄듃??醫뚯긽??Y 醫뚰몴.
    float speed_; // 珥덈떦 ?대룞 ?띾룄.
    static constexpr int kWidth = 80;  // 蹂댄듃 ?덈퉬.
    static constexpr int kHeight = 32; // 蹂댄듃 ?믪씠.
    Sprite* sprite_; // 蹂댄듃 ?ㅽ봽?쇱씠???ъ씤??
};
