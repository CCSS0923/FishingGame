// HUD, ?먯닔/?ы솕 ?쒖떆, 湲곕낯 ?낅젰 ?뚰듃 ??寃뚯엫 ?붾㈃ UI瑜??좎뼵?섎뒗 ?명꽣?섏씠?ㅼ엯?덈떎.
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class UI
{
public:
    UI(); // 湲곕낯 罹붾쾭???ш린濡?珥덇린??

    // ?꾩옱 ?대씪?댁뼵???ш린瑜??ㅼ젙.
    void SetClientSize(int width, int height);

    // HUD瑜?洹몃┛?? ?먯닔/???쇱씤 ?λ젰/?낃렇?덉씠??異⑹쟾 ?쒖떆 ??
    void Render(HDC hdc, int score, int money, float lineTension, int rodLevel, int lineLevel, bool shopOpen,
        float chargeRatio, bool charging, bool showCheatHint) const;

private:
    // 怨듯넻 ?띿뒪??異쒕젰 ?ы띁.
    void DrawText(Gdiplus::Graphics& g, const std::wstring& text, float x, float y, Gdiplus::Color color, float size) const;

    int width_;  // ?대씪?댁뼵????
    int height_; // ?대씪?댁뼵???믪씠.
};
