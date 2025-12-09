// GDI+로 스프라이트를 로드하고 그리기 및 좌우 반전 기능을 제공하는 구현입니다.
#include "Sprite.h"

// 주어진 경로에서 GDI+ 이미지를 로드한다.
bool Sprite::Load(const std::wstring& path)
{
    image_ = std::make_unique<Gdiplus::Image>(path.c_str());
    if (!image_ || image_->GetLastStatus() != Gdiplus::Ok)
    {
        image_.reset();
        return false;
    }
    return true;
}

// 지정한 크기/위치에 스프라이트를 그린다.
void Sprite::Draw(HDC hdc, int x, int y, int width, int height) const
{
    if (!image_)
        return;

    Gdiplus::Graphics g(hdc);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    UINT w = image_->GetWidth();
    UINT h = image_->GetHeight();
    int dw = (width <= 0) ? static_cast<int>(w) : width;
    int dh = (height <= 0) ? static_cast<int>(h) : height;
    g.DrawImage(image_.get(), static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y),
        static_cast<Gdiplus::REAL>(dw), static_cast<Gdiplus::REAL>(dh));
}

// 좌우 반전하여 스프라이트를 그린다.
void Sprite::DrawFlippedH(HDC hdc, int x, int y, int width, int height) const
{
    if (!image_)
        return;

    Gdiplus::Graphics g(hdc);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    UINT w = image_->GetWidth();
    UINT h = image_->GetHeight();
    int dw = (width <= 0) ? static_cast<int>(w) : width;
    int dh = (height <= 0) ? static_cast<int>(h) : height;

    Gdiplus::Matrix m;
    m.Translate(static_cast<Gdiplus::REAL>(x + dw), static_cast<Gdiplus::REAL>(y));
    m.Scale(-1.0f, 1.0f);
    g.SetTransform(&m);
    g.DrawImage(image_.get(), 0.0f, 0.0f, static_cast<Gdiplus::REAL>(dw), static_cast<Gdiplus::REAL>(dh));
    g.ResetTransform();
}

// 원본 이미지 너비 반환.
UINT Sprite::GetWidth() const
{
    return image_ ? image_->GetWidth() : 0;
}

// 원본 이미지 높이 반환.
UINT Sprite::GetHeight() const
{
    return image_ ? image_->GetHeight() : 0;
}
