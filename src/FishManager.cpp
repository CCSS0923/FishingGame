// 물고기 스폰, 이동/제거 업데이트, 렌더링과 충돌 검사를 관리하는 매니저 구현입니다.
#include "FishManager.h"

#include <algorithm>

FishManager::FishManager()
    : waterRect_{ 0, 0, 0, 0 }
    , spawnTimer_(0.0f)     // 스폰까지 남은 시간
    , spawnInterval_(2.0f)  // 기본 스폰 주기(초)
    , rng_(static_cast<unsigned int>(std::random_device{}()))
    , smallSprite_(nullptr)
    , mediumSprite_(nullptr)
    , largeSprite_(nullptr)
{
}

// 각 크기별 물고기 스프라이트 연결.
void FishManager::SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite)
{
    smallSprite_ = smallSprite;
    mediumSprite_ = mediumSprite;
    largeSprite_ = largeSprite;
}

// 물 영역 설정.
void FishManager::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
}

// 모든 물고기 제거.
void FishManager::Clear()
{
    fishes_.clear();
}

// 무작위 물고기 한 마리를 생성해 등록.
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

// 스폰 타이머/물고기 이동/제거를 갱신한다.
void FishManager::Update(float deltaTime)
{
    spawnTimer_ -= deltaTime;
    if (spawnTimer_ <= 0.0f)
    {
        Spawn();
        spawnInterval_ = 1.5f + static_cast<float>((rng_() % 150) / 100.0f); // 1.5~3.0초 랜덤 스폰
        spawnTimer_ = spawnInterval_;
    }

    RECT bounds = waterRect_;
    bounds.left -= 20;   // 화면 밖 여유 영역
    bounds.right += 20;  // 화면 밖 여유 영역

    for (auto& fish : fishes_)
    {
        fish.Update(deltaTime, bounds);
        // 수면 밖으로 지나친 물고기는 바로 제거.
        FishTraits t = GetTraits(fish.GetType());
        RECT r = fish.GetRect();
        if (r.right < bounds.left - t.size.cx || r.left > bounds.right + t.size.cx)
        {
            fish.Kill();
        }
    }

    fishes_.remove_if([](const Fish& f) { return !f.IsAlive(); });
}

// 화면 근처에 있는 물고기만 렌더링.
void FishManager::Render(HDC hdc) const
{
    // 간단 컬링: 물 영역 + 여백을 벗어나면 스킵.
    RECT view = waterRect_;
    InflateRect(&view, 32, 32); // 광역 체크 박스 확장
    for (const auto& fish : fishes_)
    {
        RECT r = fish.GetRect();
        RECT overlap{};
        if (!IntersectRect(&overlap, &view, &r))
            continue;
        fish.Render(hdc);
    }
}

// 바늘 충돌 박스와 겹치는 살아있는 물고기 포인터 반환.
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
