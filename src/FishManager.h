#pragma once

#include <list>
#include <random>
#include <windows.h>
#include "Fish.h"
#include "Sprite.h"

class FishManager
{
public:
    FishManager();

    void SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite);
    void SetWaterRect(const RECT& rect);
    void Clear();

    void Update(float deltaTime);
    void Render(HDC hdc) const;

    Fish* CheckHookCollision(const RECT& hookRect);

    size_t GetFishCount() const { return fishes_.size(); }

private:
    Fish* Spawn();

    std::list<Fish> fishes_;
    RECT waterRect_;
    float spawnTimer_;
    float spawnInterval_;
    std::mt19937 rng_;

    Sprite* smallSprite_;
    Sprite* mediumSprite_;
    Sprite* largeSprite_;

};
