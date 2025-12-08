#pragma once

#include <windows.h>
#include "Sprite.h"

class Player
{
public:
    Player();

    void SetPosition(float x, float y);
    void SetSpeed(float speed);
    void SetSprite(Sprite* sprite) { sprite_ = sprite; }

    void Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth);
    void Render(HDC hdc) const;

    RECT GetHitRect() const;
    POINT GetLineOrigin() const;

    float GetX() const { return x_; }
    float GetY() const { return y_; }

private:
    float x_;
    float y_;
    float speed_;
    static constexpr int kWidth = 80;
    static constexpr int kHeight = 32;
    Sprite* sprite_;
};
