// 물고기 스폰과 전체 업데이트/렌더링, 충돌 처리 등을 총괄하는 매니저 구현입니다.
#include "FishManager.h"

#include <algorithm>

// 물고기 풀 초기화.
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

// 각 크기별 스프라이트를 세팅.
void FishManager::SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite)
{
    smallSprite_ = smallSprite;
    mediumSprite_ = mediumSprite;
    largeSprite_ = largeSprite;
}

// 물 영역 갱신.
void FishManager::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
}

// 모든 물고기를 제거.
void FishManager::Clear()
{
    fishes_.clear();
}

// 무작위 물고기를 하나 생성하고 목록에 추가.
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

// 스폰 타이머/물고기 이동을 처리하고 벗어난 물고기를 제거한다.
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
        // 확장 영역을 완전히 벗어나면 즉시 제거 플래그.
        FishTraits t = GetTraits(fish.GetType());
        RECT r = fish.GetRect();
        if (r.right < bounds.left - t.size.cx || r.left > bounds.right + t.size.cx)
        {
            fish.Kill();
        }
    }

    fishes_.remove_if([](const Fish& f) { return !f.IsAlive(); });
}

// 물 영역 근처의 물고기만 렌더링.
void FishManager::Render(HDC hdc) const
{
    // 단순 컬링: 물 영역 + 여백을 벗어난 물고기는 스킵.
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

// 바늘 영역과 겹치는 살아있는 물고기를 찾는다.
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
