// 상점 UI와 구매 로직을 정의하는 헤더로 아이템 목록, 가격, 구매 처리 인터페이스를 제공합니다.
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class Shop
{
public:
    Shop(); // 상점 UI 초기화.

    // 게임 창 크기에 맞춰 영역 재계산.
    void SetClientSize(int width, int height);

    // 상점 열기/닫기 토글.
    void Toggle();
    bool IsOpen() const { return open_; }

    // 상점 UI 렌더링.
    void Render(HDC hdc, float money, int rodLevel, int lineLevel) const;
    // 상점 클릭 처리: 업그레이드 구매 시 금액/레벨 수정.
    bool HandleClick(POINT pt, float& money, int& rodLevel, int& lineLevel);

private:
    RECT panelRect_;  // 상점 패널 영역.
    RECT rodButton_;  // 로드 업그레이드 버튼 영역.
    RECT lineButton_; // 라인 업그레이드 버튼 영역.

    bool open_;  // 열림 상태 플래그.
    int width_;  // 클라이언트 폭.
    int height_; // 클라이언트 높이.

};
