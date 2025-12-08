#include "Sprite.h"

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

UINT Sprite::GetWidth() const
{
    return image_ ? image_->GetWidth() : 0;
}

UINT Sprite::GetHeight() const
{
    return image_ ? image_->GetHeight() : 0;
}
