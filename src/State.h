// 게임 전역 상태와 낚시 상태를 열거형으로 정의하고 2D 벡터 유틸 구조체를 제공하는 헤더입니다.
// High level game and fishing state definitions.
#pragma once

// 전반적인 게임 진행 상태.
enum class GameState
{
    InGame, // 기본 플레이 중.
    Shop,   // 상점 UI가 열려 있음.
    Pause   // 일시 정지 상태.
};

// 낚시 라인의 현재 단계 상태.
enum class FishingState
{
    Idle,     // 아무 것도 하지 않는 상태.
    Charging, // 던지기 힘을 모으는 중.
    Casting,  // 던지기 직후 전환 단계.
    Floating, // 찌가 떠 있는 대기 중.
    Bite,     // 물고기가 미끼를 무는 중.
    Reeling,  // 릴 당기는 중.
    Caught,   // 낚시에 성공한 단계.
    Fail      // 장력이 꽉 차 실패한 단계.
};

struct Float2
{
    float x = 0.0f; // X 좌표.
    float y = 0.0f; // Y 좌표.
};
