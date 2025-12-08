#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <random>
#include "Sprite.h"
#include "State.h"

enum class FishType
{
    Small,
    Medium,
    Large
};

struct FishTraits
{
    int score = 0;
    int money = 0;
    float speed = 0.0f;
    float tensionAdd = 0.0f;
    SIZE size{ 0, 0 };
};

FishTraits GetTraits(FishType type);

class Fish
{
public:
    Fish();

    static Fish SpawnRandom(const RECT& waterRect, std::mt19937& rng);

    void Update(float deltaTime, const RECT& bounds);
    void Render(HDC hdc) const;

    RECT GetRect() const;
    bool IsAlive() const { return alive_; }
    void Kill() { alive_ = false; }

    void AttachTo(const POINT& hookPos);
    bool IsAttached() const { return attached_; }

    FishType GetType() const { return type_; }
    int GetScoreValue() const { return GetTraits(type_).score; }
    int GetMoneyValue() const { return GetTraits(type_).money; }
    float GetTensionAdd() const { return GetTraits(type_).tensionAdd; }

    void SetPosition(float x, float y) { x_ = x; y_ = y; }
    Float2 GetPosition() const { return Float2{ x_, y_ }; }
    void SetSprite(Sprite* sprite) { sprite_ = sprite; }

private:
    float x_;
    float y_;
    float direction_;
    FishType type_;
    bool alive_;
    bool attached_;
    Sprite* sprite_;
};
