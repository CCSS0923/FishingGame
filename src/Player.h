// 플레이어 입력 처리와 이동, 발사 위치 제공 등을 담당하는 플레이어 객체 헤더입니다.
#pragma once

#include <windows.h>
#include "Sprite.h"

class Player
{
public:
    Player(); // 플레이어 보트 초기화.

    // 화면 상 위치 지정.
    void SetPosition(float x, float y);
    // 이동 속도 설정.
    void SetSpeed(float speed);
    void SetSprite(Sprite* sprite) { sprite_ = sprite; }

    // 입력과 델타 시간에 따라 이동 처리.
    void Update(bool moveLeft, bool moveRight, float deltaTime, int windowWidth);
    // 보트 스프라이트/대체 도형 렌더링.
    void Render(HDC hdc) const;

    // 충돌/그리기에 쓰이는 사각형 반환.
    RECT GetHitRect() const;
    // 낚싯줄이 나가는 기준점을 반환.
    POINT GetLineOrigin() const;

    float GetX() const { return x_; }
    float GetY() const { return y_; }

private:
    float x_;   // 보트의 좌상단 X 좌표.
    float y_;   // 보트의 좌상단 Y 좌표.
    float speed_; // 초당 이동 속도.
    static constexpr int kWidth = 80;  // 보트 너비.
    static constexpr int kHeight = 32; // 보트 높이.
    Sprite* sprite_; // 보트 스프라이트 포인터.
};
