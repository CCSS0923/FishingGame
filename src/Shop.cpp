#include "Shop.h"

Shop::Shop()
    : panelRect_{ 0, 0, 0, 0 }
    , rodButton_{ 0, 0, 0, 0 }
    , lineButton_{ 0, 0, 0, 0 }
    , open_(false)
    , width_(800)
    , height_(600)
{
}

void Shop::SetClientSize(int width, int height)
{
    width_ = width;
    height_ = height;

    const int panelWidth = 260;
    const int panelHeight = 200;
    const int margin = 20;
    panelRect_.right = width_ - margin;
    panelRect_.left = panelRect_.right - panelWidth;
    panelRect_.top = height_ - panelHeight - margin;
    panelRect_.bottom = height_ - margin;

    rodButton_ = { panelRect_.left + 16, panelRect_.top + 48, panelRect_.right - 16, panelRect_.top + 48 + 48 };
    lineButton_ = { panelRect_.left + 16, panelRect_.top + 110, panelRect_.right - 16, panelRect_.top + 110 + 48 };
}

void Shop::Toggle()
{
    open_ = !open_;
}

namespace
{
    constexpr int kMaxLevel = 10;
    constexpr int ROD_COST_BASE = 20;
    constexpr int LINE_COST_BASE = 15;

    int GetRodUpgradeCost(int level) { return ROD_COST_BASE * level * level; }
    int GetLineUpgradeCost(int level) { return LINE_COST_BASE * level * level; }
}

bool Shop::HandleClick(POINT pt, float& money, int& rodLevel, int& lineLevel)
{
    if (!open_)
        return false;

    if (PtInRect(&rodButton_, pt) && rodLevel < kMaxLevel)
    {
        int cost = GetRodUpgradeCost(rodLevel);
        if (money >= cost)
        {
            money -= static_cast<float>(cost);
            rodLevel += 1;
            return true;
        }
    }

    if (PtInRect(&lineButton_, pt) && lineLevel < kMaxLevel)
    {
        int cost = GetLineUpgradeCost(lineLevel);
        if (money >= cost)
        {
            money -= static_cast<float>(cost);
            lineLevel += 1;
            return true;
        }
    }

    return false;
}

void Shop::Render(HDC hdc, float money, int rodLevel, int lineLevel) const
{
    if (!open_)
        return;

    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    Gdiplus::SolidBrush bg(Gdiplus::Color(200, 40, 40, 60));
    Gdiplus::RectF rect(static_cast<Gdiplus::REAL>(panelRect_.left), static_cast<Gdiplus::REAL>(panelRect_.top),
        static_cast<Gdiplus::REAL>(panelRect_.right - panelRect_.left), static_cast<Gdiplus::REAL>(panelRect_.bottom - panelRect_.top));
    g.FillRectangle(&bg, rect);
    Gdiplus::Pen outline(Gdiplus::Color(220, 220, 240));
    g.DrawRectangle(&outline, rect);

    Gdiplus::FontFamily family(L"Segoe UI");
    Gdiplus::Font fontTitle(&family, 18, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontBody(&family, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(240, 240, 240));
    g.DrawString(L"Shop", -1, &fontTitle, Gdiplus::PointF(static_cast<Gdiplus::REAL>(panelRect_.left + 12), static_cast<Gdiplus::REAL>(panelRect_.top + 12)), &textBrush);

    auto drawItem = [&](const wchar_t* label, int level, int price, const RECT& area)
    {
        Gdiplus::PointF pos(static_cast<Gdiplus::REAL>(area.left + 52), static_cast<Gdiplus::REAL>(area.top + 4));
        std::wstring text = std::wstring(label) + L" Lv." + std::to_wstring(level) + L"/10";
        g.DrawString(text.c_str(), -1, &fontBody, pos, &textBrush);
        std::wstring priceText;
        if (level >= kMaxLevel)
            priceText = L"Maxed";
        else
            priceText = L"Cost: " + std::to_wstring(price);
        g.DrawString(priceText.c_str(), -1, &fontBody, Gdiplus::PointF(pos.X, pos.Y + 20), &textBrush);

        Gdiplus::Pen pen(Gdiplus::Color(200, 220, 240));
        g.DrawRectangle(&pen, Gdiplus::Rect(area.left, area.top, area.right - area.left, area.bottom - area.top));
    };

    int rodCost = (rodLevel >= kMaxLevel) ? 0 : GetRodUpgradeCost(rodLevel);
    int lineCost = (lineLevel >= kMaxLevel) ? 0 : GetLineUpgradeCost(lineLevel);

    drawItem(L"Rod", rodLevel, rodCost, rodButton_);
    drawItem(L"Line", lineLevel, lineCost, lineButton_);

    std::wstring wallet = L"Wallet: " + std::to_wstring(static_cast<int>(money));
    g.DrawString(wallet.c_str(), -1, &fontBody, Gdiplus::PointF(static_cast<Gdiplus::REAL>(panelRect_.left + 16), static_cast<Gdiplus::REAL>(panelRect_.bottom - 32)), &textBrush);
}
