#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class UI
{
public:
    UI();

    void SetClientSize(int width, int height);

    void Render(HDC hdc, int score, int money, float lineTension, int rodLevel, int lineLevel, bool shopOpen,
        float chargeRatio, bool charging, bool showCheatHint) const;

private:
    void DrawText(Gdiplus::Graphics& g, const std::wstring& text, float x, float y, Gdiplus::Color color, float size) const;

    int width_;
    int height_;
};
