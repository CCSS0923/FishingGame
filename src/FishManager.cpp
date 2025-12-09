// 臾쇨퀬湲??ㅽ룿, ?대룞/?쒓굅 ?낅뜲?댄듃, ?뚮뜑留곴낵 異⑸룎 寃?щ? 愿由ы븯??留ㅻ땲? 援ы쁽?낅땲??
#include "FishManager.h"

#include <algorithm>

FishManager::FishManager()
    : waterRect_{ 0, 0, 0, 0 }
    , spawnTimer_(0.0f)     // ?ㅽ룿源뚯? ?⑥? ?쒓컙
    , spawnInterval_(2.0f)  // 湲곕낯 ?ㅽ룿 二쇨린(珥?
    , rng_(static_cast<unsigned int>(std::random_device{}()))
    , smallSprite_(nullptr)
    , mediumSprite_(nullptr)
    , largeSprite_(nullptr)
{
}

// 媛??ш린蹂?臾쇨퀬湲??ㅽ봽?쇱씠???곌껐.
void FishManager::SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite)
{
    smallSprite_ = smallSprite;
    mediumSprite_ = mediumSprite;
    largeSprite_ = largeSprite;
}

// 臾??곸뿭 ?ㅼ젙.
void FishManager::SetWaterRect(const RECT& rect)
{
    waterRect_ = rect;
}

// 紐⑤뱺 臾쇨퀬湲??쒓굅.
void FishManager::Clear()
{
    fishes_.clear();
}

// 臾댁옉??臾쇨퀬湲???留덈━瑜??앹꽦???깅줉.
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

// ?ㅽ룿 ??대㉧/臾쇨퀬湲??대룞/?쒓굅瑜?媛깆떊?쒕떎.
void FishManager::Update(float deltaTime)
{
    spawnTimer_ -= deltaTime;
    if (spawnTimer_ <= 0.0f)
    {
        Spawn();
        spawnInterval_ = 1.5f + static_cast<float>((rng_() % 150) / 100.0f); // 1.5~3.0珥??쒕뜡 ?ㅽ룿
        spawnTimer_ = spawnInterval_;
    }

    RECT bounds = waterRect_;
    bounds.left -= 20;   // ?붾㈃ 諛??ъ쑀 ?곸뿭
    bounds.right += 20;  // ?붾㈃ 諛??ъ쑀 ?곸뿭

    for (auto& fish : fishes_)
    {
        fish.Update(deltaTime, bounds);
        // ?섎㈃ 諛뽰쑝濡?吏?섏튇 臾쇨퀬湲곕뒗 諛붾줈 ?쒓굅.
        FishTraits t = GetTraits(fish.GetType());
        RECT r = fish.GetRect();
        if (r.right < bounds.left - t.size.cx || r.left > bounds.right + t.size.cx)
        {
            fish.Kill();
        }
    }

    fishes_.remove_if([](const Fish& f) { return !f.IsAlive(); });
}

// ?붾㈃ 洹쇱쿂???덈뒗 臾쇨퀬湲곕쭔 ?뚮뜑留?
void FishManager::Render(HDC hdc) const
{
    // 媛꾨떒 而щ쭅: 臾??곸뿭 + ?щ갚??踰쀬뼱?섎㈃ ?ㅽ궢.
    RECT view = waterRect_;
    InflateRect(&view, 32, 32); // 愿묒뿭 泥댄겕 諛뺤뒪 ?뺤옣
    for (const auto& fish : fishes_)
    {
        RECT r = fish.GetRect();
        RECT overlap{};
        if (!IntersectRect(&overlap, &view, &r))
            continue;
        fish.Render(hdc);
    }
}

// 諛붾뒛 異⑸룎 諛뺤뒪? 寃뱀튂???댁븘?덈뒗 臾쇨퀬湲??ъ씤??諛섑솚.
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
