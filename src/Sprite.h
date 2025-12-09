// GDI+ 湲곕컲?쇰줈 ?ㅽ봽?쇱씠?몃? 濡쒕뱶?섍퀬 ?붾㈃??洹몃━??媛꾨떒???좏떥由ы떚 ?대옒?ㅻ? ?좎뼵?⑸땲??
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>

class Sprite
{
public:
    Sprite() = default;  // 湲곕낯 ?앹꽦??
    ~Sprite() = default; // 湲곕낯 ?뚮㈇??

    // ?뚯씪 寃쎈줈?먯꽌 ?대?吏瑜?濡쒕뱶.
    bool Load(const std::wstring& path);
    // ?대?吏瑜?吏???꾩튂/?ш린濡?洹몃━湲?
    void Draw(HDC hdc, int x, int y, int width = -1, int height = -1) const;
    // ?대?吏瑜?醫뚯슦 諛섏쟾?섏뿬 洹몃━湲?
    void DrawFlippedH(HDC hdc, int x, int y, int width = -1, int height = -1) const;

    UINT GetWidth() const;
    UINT GetHeight() const;
    bool IsValid() const { return image_ != nullptr; }

private:
    std::unique_ptr<Gdiplus::Image> image_; // 濡쒕뱶??GDI+ ?대?吏.
};
