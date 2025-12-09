// 게임 루프와 자원 로딩, 입력/업데이트/렌더링 흐름을 총괄하는 상위 게임 인터페이스입니다.
#pragma once

#include <array>
#include <filesystem>
#include <windows.h>
#include "Player.h"
#include "FishingSystem.h"
#include "FishManager.h"
#include "UI.h"
#include "Shop.h"
#include "State.h"
#include "Sprite.h"

class Game
{
public:
    Game(); // 게임 전체 상태 초기화.

    bool Initialize(HWND hwnd, int clientWidth, int clientHeight); // 윈도우 핸들과 크기를 받아 리소스 초기화.
    void OnResize(int width, int height);                         // 창 크기 변경 시 호출.

    void OnKeyDown(WPARAM key); // 키 다운 이벤트 기록.
    void OnKeyUp(WPARAM key);   // 키 업 이벤트 기록.
    void OnMouseDown(int x, int y); // 마우스 버튼 눌림 처리.
    void OnMouseUp(int x, int y);   // 마우스 버튼 뗌 처리.
    void OnMouseMove(int x, int y); // 마우스 이동 기록.

    void Update(float deltaTime); // 프레임마다 게임 로직 갱신.
    void Render(HDC hdc);         // 더블 버퍼를 사용해 화면 그리기.

private:
    void UpdateWaterRect(); // 창 크기에 맞춰 수역/백버퍼 등 재계산.
    bool LoadSprite(Sprite& sprite, const wchar_t* path); // 스프라이트 로드 헬퍼.
    void EnsureBackBuffer(int width, int height); // 백버퍼 크기 확인/재생성.
    void DestroyBackBuffer(); // 백버퍼 자원 해제.
    void CheckMilestone(float deltaGold); // 누적 골드 milestone 체크 및 메시지 갱신.
    void ResetGame(); // 점수/골드/레벨 초기화.
    bool HandleDebugClick(const POINT& pt); // 디버그 패널 클릭 처리.
    void LayoutDebugPanel(); // 디버그 버튼 영역 계산.
    std::wstring ResourcePath(const wchar_t* relative) const; // 리소스 경로 해석.

    HWND hwnd_;   // 윈도우 핸들.
    int width_;   // 클라이언트 폭.
    int height_;  // 클라이언트 높이.
    GameState state_; // 게임 상태(인게임/상점/일시정지).

    Player player_;           // 플레이어(보트) 객체.
    FishingSystem fishing_;   // 낚시 시스템.
    FishManager fishManager_; // 물고기 스폰/관리.
    UI ui_;                   // HUD 렌더러.
    Shop shop_;               // 상점 UI/로직.

    int score_;          // 누적 점수.
    float goldBalance_;  // 현재 소지 골드.
    float totalGold_;    // 누적 획득 골드(가격 상승 계산용).
    int rodLevel_;       // 로드 레벨.
    int lineLevel_;      // 라인 레벨.
    int goldMilestone_;  // 다음 골드 상승 단계 인덱스.
    float messageTimer_; // 메시지 표시 잔여 시간.
    std::wstring messageText_; // 메시지 내용.
    bool goldMaxed_;     // 골드 상승 상한 도달 여부.
    bool debugPanelOpen_; // 디버그 패널 열림 여부.
    RECT debugPanelRect_; // 디버그 패널 영역.
    RECT debugAddGoldRect_; // 디버그: 골드 추가 버튼 영역.
    RECT debugResetRect_;   // 디버그: 리셋 버튼 영역.
    RECT debugRodUpRect_;   // 디버그: 로드 업 버튼.
    RECT debugLineUpRect_;  // 디버그: 라인 업 버튼.

    float waterTop_;    // 수면의 Y 좌표.
    float waterBottom_; // 수심 끝 Y 좌표.
    RECT waterRect_;    // 수역 사각형.

    Sprite spritePlayer_;      // 보트 스프라이트.
    Sprite spriteFishSmall_;   // 소형 물고기 스프라이트.
    Sprite spriteFishMedium_;  // 중형 물고기 스프라이트.
    Sprite spriteFishLarge_;   // 대형 물고기 스프라이트.
    std::filesystem::path exeDir_; // 실행 파일 위치 경로.

    std::array<bool, 256> keyDown_;     // 현재 눌림 상태.
    std::array<bool, 256> keyPressed_;  // 이번 프레임 눌림.
    std::array<bool, 256> keyReleased_; // 이번 프레임 뗌.

    bool mouseDown_;    // 마우스 버튼 지속 눌림.
    bool mousePressed_; // 이번 프레임 눌림.
    bool mouseReleased_; // 이번 프레임 뗌.
    bool inputBlocked_;  // UI 등으로 입력 소비 여부.
    POINT mousePos_;     // 현재 마우스 위치.

    HDC backDC_;       // 백버퍼 DC.
    HBITMAP backBmp_;  // 백버퍼 비트맵.
    HBITMAP backOld_;  // 선택되어 있던 이전 비트맵.

};
