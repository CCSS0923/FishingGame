#pragma once

#include <array>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "Player.h"
#include "FishingSystem.h"
#include "FishManager.h"
#include "UI.h"
#include "Shop.h"
#include "State.h"
#include "Sprite.h"

class Game
{
public:
    Game();

    bool Initialize(HWND hwnd, int clientWidth, int clientHeight);
    void OnResize(int width, int height);

    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);
    void OnMouseDown(int x, int y);
    void OnMouseUp(int x, int y);
    void OnMouseMove(int x, int y);

    void Update(float deltaTime);
    void Render(HDC hdc);

private:
    void UpdateWaterRect();
    bool LoadSprite(Sprite& sprite, const wchar_t* path);
    void EnsureBackBuffer(int width, int height);
    void DestroyBackBuffer();
    void CheckMilestone(float deltaGold);
    void ResetGame();
    bool HandleDebugClick(const POINT& pt);
    void LayoutDebugPanel();
    void ToggleWorld();
    void PositionPlayer();
    std::wstring ResourcePath(const wchar_t* relative) const;

    HWND hwnd_;
    int width_;
    int height_;
    GameState state_;

    Player player_;
    FishingSystem fishing_;
    FishManager fishManager_;
    UI ui_;
    Shop shop_;

    int score_;
    float goldBalance_;
    float totalGold_;
    int rodLevel_;
    int lineLevel_;
    int goldMilestone_;
    float messageTimer_;
    std::wstring messageText_;
    bool goldMaxed_;
    bool debugPanelOpen_;
    bool worldFlipped_ = false;

    RECT debugPanelRect_;
    RECT debugAddGoldRect_;
    RECT debugResetRect_;
    RECT debugRodUpRect_;
    RECT debugLineUpRect_;

    float waterTop_;
    float waterBottom_;
    RECT waterRect_;

    Sprite spritePlayer_;
    Sprite spriteFishSmall_;
    Sprite spriteFishMedium_;
    Sprite spriteFishLarge_;
    std::wstring exeDir_;

    std::array<bool, 256> keyDown_;
    std::array<bool, 256> keyPressed_;
    std::array<bool, 256> keyReleased_;

    bool mouseDown_;
    bool mousePressed_;
    bool mouseReleased_;
    bool inputBlocked_;
    POINT mousePos_;

    HDC backDC_;
    HBITMAP backBmp_;
    HBITMAP backOld_;
};
