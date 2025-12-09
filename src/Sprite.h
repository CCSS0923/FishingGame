// GDI+ 기반으로 스프라이트를 로드하고 화면에 그리는 간단한 유틸리티 클래스를 선언합니다.
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>

class Sprite
{
public:
    Sprite() = default;  // 기본 생성자.
    ~Sprite() = default; // 기본 소멸자.

    // 파일 경로에서 이미지를 로드.
    bool Load(const std::wstring& path);
    // 이미지를 지정 위치/크기로 그리기.
    void Draw(HDC hdc, int x, int y, int width = -1, int height = -1) const;
    // 이미지를 좌우 반전하여 그리기.
    void DrawFlippedH(HDC hdc, int x, int y, int width = -1, int height = -1) const;

    UINT GetWidth() const;
    UINT GetHeight() const;
    bool IsValid() const { return image_ != nullptr; }

private:
    std::unique_ptr<Gdiplus::Image> image_; // 로드된 GDI+ 이미지.
};
