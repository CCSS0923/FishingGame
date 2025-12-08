#include "UI.h"

#include <algorithm>

UI::UI()
    : width_(800)
    , height_(600)
{
}

void UI::SetClientSize(int width, int height)
{
    width_ = width;
    height_ = height;
}

void UI::DrawText(Gdiplus::Graphics& g, const std::wstring& text, float x, float y, Gdiplus::Color color, float size) const
{
    Gdiplus::FontFamily family(L"Segoe UI");
    Gdiplus::Font font(&family, size, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush brush(color);
    Gdiplus::PointF pos(x, y);
    g.DrawString(text.c_str(), static_cast<INT>(text.size()), &font, pos, &brush);
}

void UI::Render(HDC hdc, int score, int money, float lineTension, int rodLevel, int lineLevel, bool shopOpen,
    float chargeRatio, bool charging, bool showCheatHint) const
{
    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    (void)lineTension; // unused in UI-only layout

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

    // Upgrade bars (replaces tension bar area)
    drawProgress(L"Rod Level", rodLevel, barX, baseY);
    drawProgress(L"Line Level", lineLevel, barX, baseY + barHeight + 10.0f);

    DrawText(g, moneyText, barX + barWidth + 36.0f, baseY - 2.0f, Gdiplus::Color(240, 240, 200), 18.0f);

    // Cast charge indicator
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

    // Upgrade effects display (no duplicate levels)
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
