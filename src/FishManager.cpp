#include "FishManager.h"

#include <algorithm>

FishManager::FishManager()
    : waterRect_{ 0, 0, 0, 0 }
    , spawnTimer_(0.0f)
    , spawnInterval_(2.0f)
    , rng_(static_cast<unsigned int>(std::random_device{}()))
    , smallSprite_(nullptr)
    , mediumSprite_(nullptr)
    , largeSprite_(nullptr)
{
}

void FishManager::SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite)
{
    smallSprite_ = smallSprite;
    mediumSprite_ = mediumSprite;
    largeSprite_ = largeSprite;
}

void FishManager::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
}

void FishManager::Clear()
{
    fishes_.clear();
}

Fish* FishManager::Spawn()
{
    Fish fish = Fish::SpawnRandom(waterRect_, rng_);
    switch (fish.GetType())
    {
    case FishType::Small:
        fish.SetSprite(smallSprite_);
        break;
    case FishType::Medium:
        fish.SetSprite(mediumSprite_);
        break;
    case FishType::Large:
        fish.SetSprite(largeSprite_);
        break;
    }
    fishes_.push_back(fish);
    return &fishes_.back();
}

void FishManager::Update(float deltaTime)
{
    spawnTimer_ -= deltaTime;
    if (spawnTimer_ <= 0.0f)
    {
        Spawn();
        spawnInterval_ = 1.5f + static_cast<float>((rng_() % 150) / 100.0f);
        spawnTimer_ = spawnInterval_;
    }

    RECT bounds = waterRect_;
    bounds.left -= 20;
    bounds.right += 20;

    for (auto& fish : fishes_)
    {
        fish.Update(deltaTime, bounds);
        // If a fish leaves the extended bounds completely, kill it immediately.
        FishTraits t = GetTraits(fish.GetType());
        RECT r = fish.GetRect();
        if (r.right < bounds.left - t.size.cx || r.left > bounds.right + t.size.cx)
        {
            fish.Kill();
        }
    }

    fishes_.remove_if([](const Fish& f) { return !f.IsAlive(); });
}

void FishManager::Render(HDC hdc) const
{
    // Simple culling: skip fish that are outside the water rect plus a margin.
    RECT view = waterRect_;
    InflateRect(&view, 32, 32);
    for (const auto& fish : fishes_)
    {
        RECT r = fish.GetRect();
        RECT overlap{};
        if (!IntersectRect(&overlap, &view, &r))
            continue;
        fish.Render(hdc);
    }
}

Fish* FishManager::CheckHookCollision(const RECT& hookRect)
{
    RECT overlap{};
    for (auto& fish : fishes_)
    {
        if (!fish.IsAlive() || fish.IsAttached())
            continue;

        RECT fishRect = fish.GetRect();
        InflateRect(&fishRect, 6, 6);
        if (IntersectRect(&overlap, &hookRect, &fishRect))
        {
            return &fish;
        }
    }
    return nullptr;
}
