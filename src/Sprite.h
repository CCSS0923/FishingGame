#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>

class Sprite
{
public:
    Sprite() = default;
    ~Sprite() = default;

    bool Load(const std::wstring& path);
    void Draw(HDC hdc, int x, int y, int width = -1, int height = -1) const;
    void DrawFlippedH(HDC hdc, int x, int y, int width = -1, int height = -1) const;

    UINT GetWidth() const;
    UINT GetHeight() const;
    bool IsValid() const { return image_ != nullptr; }

private:
    std::unique_ptr<Gdiplus::Image> image_;
};
