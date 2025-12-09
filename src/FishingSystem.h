// 낚시 시스템 상태, 파라미터, 잡기 결과와 인터페이스를 선언하는 핵심 헤더입니다.
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gdiplus.h>
#include <random>
#include "Player.h"
#include "FishManager.h"
#include "State.h"

struct CatchResult
{
    bool caught = false;          // 잡혔는지 여부.
    FishType type = FishType::Small; // 잡힌 물고기 종류.
    int score = 0;                // 획득 점수.
    int money = 0;                // 획득 골드.
};

class FishingSystem
{
public:
    FishingSystem(); // 낚시 시스템 초기 설정.

    // 물 영역 지정.
    void SetWaterRect(const RECT& rect);

    // 장비 레벨을 설정한다.
    void SetLevels(int rodLevel, int lineLevel);

    // 입력/플레이어/물고기 정보로 낚시 상태 업데이트.
    void Update(float deltaTime, const Player& player, FishManager& fishManager,
        const POINT& mousePos, bool mouseDown, bool mousePressed, bool mouseReleased, bool inputBlocked);

    // 낚싯줄/찌/바늘을 그린다.
    void Render(HDC hdc, const Player& player) const;

    float GetLineTension() const { return lineTension_; }
    FishingState GetState() const { return state_; }
    float GetChargeRatio() const { return castPowerMax_ > 0.0f ? castPower_ / castPowerMax_ : 0.0f; }

    // pendingCatch_ 정보를 한 번 소비하고 초기화한다.
    bool ConsumeCatch(CatchResult& outResult);

private:
    void StartCharge(); // 캐스팅 파워 충전 시작.
    void Cast(const Player& player); // 현재 파워/각도로 던진다.
    void StartReel(); // 릴 당기기 시작.
    void ResetLine(const Player& player); // 상태/위치 초기화.

    RECT GetHookRect() const; // 바늘 충돌 영역 반환.

    FishingState state_; // 현재 낚시 단계.

    float castPower_;    // 현재 충전된 던지기 힘.
    float castPowerMax_; // 최대 던지기 힘(수심/로드 레벨 기반).
    float castAngleDeg_; // 던질 각도(도 단위).

    float lineTension_;      // 현재 라인 장력(0~1).
    float lineTensionDecay_; // 장력 감소 속도.

    float chargeRate_;  // 던지기 힘 충전 속도.
    float reelSpeed_;   // 릴 당기는 속도.

    float biteTimer_;   // 입질까지 남은 시간/딜레이.
    float floatTimer_;  // 찌 흔들림 애니메이션 타이머.
    float failTimer_;   // 실패 이후 대기 시간.
    float caughtTimer_; // 성공 후 대기 시간.

    Float2 hookPos_;   // 바늘 위치.
    Float2 bobberPos_; // 찌 위치.

    RECT waterRect_; // 낚시 가능한 수역 영역.

    Fish* hookedFish_; // 현재 걸려 있는 물고기 포인터.

    int rodLevel_; // 로드 레벨.
    int lineLevel_; // 라인 레벨.

    CatchResult pendingCatch_; // 잡기 완료 후 전달될 결과.
    std::mt19937 rng_;         // 낚시 랜덤 동작용 RNG.

    void RecalculateStats(); // 레벨/수면 높이에 따라 스탯 재계산.
    static constexpr int kMaxLevel = 10; // 장비 최대 레벨.
};
