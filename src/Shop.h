#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class Shop
{
public:
    Shop();

    void SetClientSize(int width, int height);

    void Toggle();
    bool IsOpen() const { return open_; }

    void Render(HDC hdc, float money, int rodLevel, int lineLevel) const;
    bool HandleClick(POINT pt, float& money, int& rodLevel, int& lineLevel);

private:
    RECT panelRect_;
    RECT rodButton_;
    RECT lineButton_;

    bool open_;
    int width_;
    int height_;

};
