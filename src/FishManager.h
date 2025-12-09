// 물고기 풀을 관리하며 생성, 충돌 검사, 업데이트, 정리를 담당하는 매니저 헤더입니다.
#pragma once

#include <list>
#include <random>
#include <windows.h>
#include "Fish.h"
#include "Sprite.h"

class FishManager
{
public:
    FishManager(); // 물고기 풀 관리 초기화.

    // 각 크기별 물고기 스프라이트 연결.
    void SetSprites(Sprite* smallSprite, Sprite* mediumSprite, Sprite* largeSprite);
    // 물 영역 설정.
    void SetWaterRect(const RECT& rect);
    // 모든 물고기 제거.
    void Clear();

    // 물고기 생성 및 이동/제거 갱신.
    void Update(float deltaTime);
    // 가시 영역에 있는 물고기 렌더링.
    void Render(HDC hdc) const;

    // 바늘 영역과 충돌한 물고기 포인터 반환.
    Fish* CheckHookCollision(const RECT& hookRect);

    size_t GetFishCount() const { return fishes_.size(); }

private:
    Fish* Spawn(); // 새 물고기 한 마리를 생성/등록.

    std::list<Fish> fishes_; // 관리 중인 물고기 목록.
    RECT waterRect_;         // 물 영역.
    float spawnTimer_;       // 다음 스폰까지 남은 시간.
    float spawnInterval_;    // 스폰 주기.
    std::mt19937 rng_;       // 랜덤 생성기.

    Sprite* smallSprite_;    // 소형 스프라이트.
    Sprite* mediumSprite_;   // 중형 스프라이트.
    Sprite* largeSprite_;    // 대형 스프라이트.

};
