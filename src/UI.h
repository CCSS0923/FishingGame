// HUD, 점수/재화 표시, 기본 입력 힌트 등 게임 화면 UI를 선언하는 인터페이스입니다.
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class UI
{
public:
    UI(); // 기본 캔버스 크기로 초기화.

    // 현재 클라이언트 크기를 설정.
    void SetClientSize(int width, int height);

    // HUD를 그린다: 점수/돈/라인 장력/업그레이드/충전 표시 등.
    void Render(HDC hdc, int score, int money, float lineTension, int rodLevel, int lineLevel, bool shopOpen,
        float chargeRatio, bool charging, bool showCheatHint) const;

private:
    // 공통 텍스트 출력 헬퍼.
    void DrawText(Gdiplus::Graphics& g, const std::wstring& text, float x, float y, Gdiplus::Color color, float size) const;

    int width_;  // 클라이언트 폭.
    int height_; // 클라이언트 높이.
};
