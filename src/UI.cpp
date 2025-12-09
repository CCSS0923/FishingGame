// 게임 HUD와 텍스트 출력 등 UI 렌더링 로직을 구현한 소스입니다.
#include "UI.h"

#include <algorithm>

// 기본 UI 크기 설정.
UI::UI()
    : width_(800)
    , height_(600)
{
}

// 클라이언트 크기 갱신.
void UI::SetClientSize(int width, int height)
{
    width_ = width;
    height_ = height;
}

// 지정된 색상/크기로 텍스트를 출력한다.
void UI::DrawText(Gdiplus::Graphics& g, const std::wstring& text, float x, float y, Gdiplus::Color color, float size) const
{
    Gdiplus::FontFamily family(L"Segoe UI");
    Gdiplus::Font font(&family, size, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush brush(color);
    Gdiplus::PointF pos(x, y);
    g.DrawString(text.c_str(), static_cast<INT>(text.size()), &font, pos, &brush);
}

// HUD 전반을 그린다. (점수/돈/업그레이드/캐스팅 게이지/힌트 등)
void UI::Render(HDC hdc, int score, int money, float lineTension, int rodLevel, int lineLevel, bool shopOpen,
    float chargeRatio, bool charging, bool showCheatHint) const
{
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    (void)lineTension; // UI에서는 현재 사용하지 않으므로 경고 방지.

    const float margin = 16.0f;
    std::wstring scoreText = L"Score: " + std::to_wstring(score);
    std::wstring moneyText = L"Money: " + std::to_wstring(money);
    DrawText(g, scoreText, margin, margin, Gdiplus::Color(240, 240, 240), 20.0f);
    // Level text removed; bars below show current levels.

    const float barX = margin;
    const float baseY = margin + 58.0f;
    const float barWidth = 220.0f;
    const float barHeight = 14.0f;

    auto drawProgress = [&](const std::wstring& label, int level, float x, float y)
    {
        float t = std::clamp(level / 10.0f, 0.0f, 1.0f);
        Gdiplus::RectF rect(x, y, barWidth, barHeight);
        Gdiplus::SolidBrush bg(Gdiplus::Color(40, 50, 80, 220));
        g.FillRectangle(&bg, rect);
        Gdiplus::RectF fill(x, y, barWidth * t, barHeight);
        Gdiplus::LinearGradientBrush grad(fill, Gdiplus::Color(120, 200, 255), Gdiplus::Color(70, 140, 220), Gdiplus::LinearGradientModeHorizontal);
        g.FillRectangle(&grad, fill);
        Gdiplus::Pen pen(Gdiplus::Color(200, 200, 220));
        g.DrawRectangle(&pen, rect);
        DrawText(g, label, x + 4.0f, y - 14.0f, Gdiplus::Color(200, 220, 240), 12.0f);
        std::wstring val = std::to_wstring(level) + L"/10";
        DrawText(g, val, x + barWidth - 34.0f, y - 14.0f, Gdiplus::Color(220, 230, 240), 12.0f);
    };

    // 업그레이드 진행도 바(라인 장력 표시 대신 사용).
    drawProgress(L"Rod Level", rodLevel, barX, baseY);
    drawProgress(L"Line Level", lineLevel, barX, baseY + barHeight + 10.0f);

    DrawText(g, moneyText, barX + barWidth + 36.0f, baseY - 2.0f, Gdiplus::Color(240, 240, 200), 18.0f);

    // 캐스팅 게이지 표시.
    const float chargeBarY = baseY + (barHeight + 10.0f) * 2 + 8.0f;
    const float chargeW = barWidth;
    const float chargeH = 10.0f;
    Gdiplus::RectF chargeRect(barX, chargeBarY, chargeW, chargeH);
    Gdiplus::SolidBrush chargeBg(Gdiplus::Color(40, 40, 50, 200));
    g.FillRectangle(&chargeBg, chargeRect);
    float fill = std::clamp(chargeRatio, 0.0f, 1.0f) * chargeW;
    Gdiplus::RectF chargeFill(barX, chargeBarY, fill, chargeH);
    Gdiplus::SolidBrush chargeBrush(charging ? Gdiplus::Color(120, 200, 255) : Gdiplus::Color(120, 160, 200));
    g.FillRectangle(&chargeBrush, chargeFill);
    Gdiplus::Pen chargePen(Gdiplus::Color(200, 220, 240));
    g.DrawRectangle(&chargePen, chargeRect);
    DrawText(g, L"Cast Power", barX, chargeBarY - 14.0f, Gdiplus::Color(210, 220, 230), 12.0f);

    // 업그레이드 효과 설명 표시.
    std::wstring rodInfo = L"Rod: deeper cast range increases with level.";
    std::wstring lineInfo = L"Line: faster casting/reeling & tension recovery as level rises.";
    DrawText(g, rodInfo, margin, chargeBarY + chargeH + 12.0f, Gdiplus::Color(200, 230, 255), 13.0f);
    DrawText(g, lineInfo, margin, chargeBarY + chargeH + 30.0f, Gdiplus::Color(200, 230, 255), 13.0f);

    if (showCheatHint)
    {
        DrawText(g, L"[F12] Debug window", margin, height_ - 28.0f, Gdiplus::Color(180, 210, 255), 12.0f);
    }

    if (shopOpen)
    {
        const float shopWidth = 180.0f;
        const float shopHeight = 32.0f;
        Gdiplus::RectF shopRect(static_cast<float>(width_) - shopWidth - margin, margin, shopWidth, shopHeight);
        Gdiplus::SolidBrush brush(Gdiplus::Color(80, 140, 200, 180));
        g.FillRectangle(&brush, shopRect);
        DrawText(g, L"Shop Open", shopRect.X + 12.0f, shopRect.Y + 6.0f, Gdiplus::Color(250, 250, 250), 16.0f);
    }
    else
    {
        const float shopWidth = 180.0f;
        Gdiplus::RectF shopRect(static_cast<float>(width_) - shopWidth - margin, margin, shopWidth, 28.0f);
        Gdiplus::Pen pen(Gdiplus::Color(200, 220, 240));
        g.DrawRectangle(&pen, shopRect);
        DrawText(g, L"Press [S] to open shop", shopRect.X + 12.0f, shopRect.Y + 4.0f, Gdiplus::Color(230, 230, 230), 14.0f);
    }
}
