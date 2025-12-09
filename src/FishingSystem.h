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
    bool caught = false;
    FishType type = FishType::Small;
    int score = 0;
    int money = 0;
};

class FishingSystem
{
public:
    FishingSystem();

    void SetWaterRect(const RECT& rect);
    void SetLevels(int rodLevel, int lineLevel);

    void Update(float deltaTime, const Player& player, FishManager& fishManager,
        const POINT& mousePos, bool mouseDown, bool mousePressed, bool mouseReleased, bool inputBlocked);

    void Render(HDC hdc, const Player& player) const;

    float GetLineTension() const { return lineTension_; }
    FishingState GetState() const { return state_; }
    float GetChargeRatio() const { return castPowerMax_ > 0.0f ? castPower_ / castPowerMax_ : 0.0f; }

    bool ConsumeCatch(CatchResult& outResult);

private:
    void StartCharge();
    void Cast(const Player& player);
    void StartReel();
    void ResetLine(const Player& player);

    RECT GetHookRect() const;

    FishingState state_;

    float castPower_;
    float castPowerMax_;
    float castAngleDeg_;

    float lineTension_;
    float lineTensionDecay_;

    float chargeRate_;
    float reelSpeed_;

    float biteTimer_;
    float floatTimer_;
    float failTimer_;
    float caughtTimer_;

    Float2 hookPos_;
    Float2 bobberPos_;

    RECT waterRect_;

    Fish* hookedFish_;

    int rodLevel_;
    int lineLevel_;

    CatchResult pendingCatch_;
    std::mt19937 rng_;

    void RecalculateStats();
    static constexpr int kMaxLevel = 10;
};
