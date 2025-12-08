// High level game and fishing state definitions.
#pragma once

enum class GameState
{
    InGame,
    Shop,
    Pause
};

enum class FishingState
{
    Idle,
    Charging,
    Casting,
    Floating,
    Bite,
    Reeling,
    Caught,
    Fail
};

struct Float2
{
    float x = 0.0f;
    float y = 0.0f;
};
