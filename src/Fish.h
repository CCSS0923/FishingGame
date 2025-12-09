// 물고기 타입, 특성, 개별 물고기 객체를 정의하고 업데이트/렌더링 인터페이스를 제공하는 헤더입니다.
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <random>
#include "Sprite.h"
#include "State.h"

enum class FishType
{
    Small,  // 가장 빠르고 가치가 낮은 물고기.
    Medium, // 속도/보상이 중간인 물고기.
    Large   // 느리지만 보상이 큰 물고기.
};

struct FishTraits
{
    int score = 0;        // 잡았을 때 얻는 점수.
    int money = 0;        // 잡았을 때 얻는 골드.
    float speed = 0.0f;   // 수평 이동 속도.
    float tensionAdd = 0.0f; // 라인 장력을 얼마나 빨리 올리는지.
    SIZE size{ 0, 0 };    // 렌더링/충돌에 쓰이는 크기.
};

// 전달된 물고기 종류에 맞는 능력치를 반환.
FishTraits GetTraits(FishType type);

class Fish
{
public:
    Fish(); // 기본값으로 초기화된 물고기 생성.

    // 주어진 수역 영역 안에 무작위 물고기를 생성.
    static Fish SpawnRandom(const RECT& waterRect, std::mt19937& rng);

    // 경계와 경과 시간에 따라 이동/생존 상태 갱신.
    void Update(float deltaTime, const RECT& bounds);
    // 살아 있을 경우 물고기 그림을 그린다.
    void Render(HDC hdc) const;

    // 컬링/충돌에 쓰이는 외접 사각형 반환.
    RECT GetRect() const;
    bool IsAlive() const { return alive_; }
    void Kill() { alive_ = false; }

    // 낚싯바늘 위치에 붙여 찌를 따라가게 한다.
    void AttachTo(const POINT& hookPos);
    bool IsAttached() const { return attached_; }

    FishType GetType() const { return type_; }
    int GetScoreValue() const { return traits_.score; }
    int GetMoneyValue() const { return traits_.money; }
    float GetTensionAdd() const { return traits_.tensionAdd; }

    // 릴 당길 때 위치 보정에 사용.
    void SetPosition(float x, float y) { x_ = x; y_ = y; }
    Float2 GetPosition() const { return Float2{ x_, y_ }; }
    void SetSprite(Sprite* sprite) { sprite_ = sprite; }

private:
    FishTraits traits_;   // 종류에 따라 결정된 능력치.
    float x_;             // 현재 그려질 왼쪽 X 좌표.
    float y_;             // 현재 그려질 위쪽 Y 좌표.
    float direction_;     // 수평 이동 방향 (-1 또는 +1).
    FishType type_;       // 보상/스프라이트 결정에 사용되는 종류.
    bool alive_;          // 업데이트/렌더 대상 여부.
    bool attached_;       // 바늘에 붙어 있는지 여부.
    Sprite* sprite_;      // 사용할 스프라이트 포인터.
};
