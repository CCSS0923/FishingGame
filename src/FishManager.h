// 臾쇨퀬湲????愿由ы븯硫??앹꽦, 異⑸룎 寃?? ?낅뜲?댄듃, ?뺣━瑜??대떦?섎뒗 留ㅻ땲? ?ㅻ뜑?낅땲??
#pragma once

#include <list>
#include <random>
#include <windows.h>
#include "Fish.h"
#include "Sprite.h"

class FishManager
{
public:
    FishManager(); // 臾쇨퀬湲?? 愿由?珥덇린??

    // 媛??ш린蹂?臾쇨퀬湲??ㅽ봽?쇱씠???곌껐.
    void SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite);
    // 臾??곸뿭 ?ㅼ젙.
    void SetWaterRect(const RECT& rect);
    // 紐⑤뱺 臾쇨퀬湲??쒓굅.
    void Clear();

    // 臾쇨퀬湲??앹꽦/?대룞/?쒓굅 媛깆떊.
    void Update(float deltaTime);
    // 媛???곸뿭???덈뒗 臾쇨퀬湲??뚮뜑留?
    void Render(HDC hdc) const;

    // 諛붾뒛 ?곸뿭怨?異⑸룎??臾쇨퀬湲??ъ씤??諛섑솚.
    Fish* CheckHookCollision(const RECT& hookRect);

    size_t GetFishCount() const { return fishes_.size(); }

private:
    Fish* Spawn(); // ??臾쇨퀬湲???留덈━瑜??앹꽦/?깅줉.

    std::list<Fish> fishes_; // 愿由?以묒씤 臾쇨퀬湲?紐⑸줉.
    RECT waterRect_;         // 臾??곸뿭.
    float spawnTimer_;       // ?ㅼ쓬 ?ㅽ룿源뚯? ?⑥? ?쒓컙.
    float spawnInterval_;    // ?ㅽ룿 二쇨린.
    std::mt19937 rng_;       // ?쒕뜡 ?앹꽦湲?

    Sprite* smallSprite_;    // ?뚰삎 ?ㅽ봽?쇱씠??
    Sprite* mediumSprite_;   // 以묓삎 ?ㅽ봽?쇱씠??
    Sprite* largeSprite_;    // ????ㅽ봽?쇱씠??
};
