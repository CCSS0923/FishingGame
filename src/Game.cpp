// 게임 초기화부터 입력 처리, 업데이트, 렌더링까지 전체 사이클을 구현한 소스입니다.
#include "Game.h"

#include <gdiplus.h>
#include <string>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <initializer_list>

// 게임 상태 기본값 초기화.
Game::Game()
    : hwnd_(nullptr)
    , width_(1280)
    , height_(720)
    , state_(GameState::InGame)
    , score_(0)
    , goldBalance_(0.0f)
    , totalGold_(0.0f)
    , rodLevel_(1)
    , lineLevel_(1)
    , waterTop_(0.0f)
    , waterBottom_(0.0f)
    , goldMilestone_(0)
    , messageTimer_(0.0f)
    , goldMaxed_(false)
    , debugPanelOpen_(false)
    , mouseDown_(false)
    , mousePressed_(false)
    , mouseReleased_(false)
    , inputBlocked_(false)
    , backDC_(nullptr)
    , backBmp_(nullptr)
    , backOld_(nullptr)
{
    keyDown_.fill(false);
    keyPressed_.fill(false);
    keyReleased_.fill(false);
    waterRect_ = { 0,0,0,0 };
    mousePos_.x = 0;
    mousePos_.y = 0;
}

// 윈도우 핸들/크기를 받아 리소스와 객체를 초기화한다.
bool Game::Initialize(HWND hwnd, int clientWidth, int clientHeight)
{
    hwnd_ = hwnd;
    width_ = clientWidth;
    height_ = clientHeight;
    UpdateWaterRect();

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    exeDir_ = std::filesystem::path(exePath).parent_path();

    auto tryLoad = [&](Sprite& dst, std::initializer_list<const wchar_t*> rels)
    {
        for (auto rel : rels)
        {
            if (LoadSprite(dst, ResourcePath(rel).c_str()))
                return true;
        }
        return false;
    };

    // 최신 자산 경로부터 시도 후 레거시 경로를 시도.
    tryLoad(spritePlayer_, { L"res/textures/boat.png", L"res/textures/player_boat.png" });
    tryLoad(spriteFishSmall_, { L"res/textures/fishes/fish_03.png", L"res/textures/fish_small.png" });
    tryLoad(spriteFishMedium_, { L"res/textures/fishes/fish_07.png", L"res/textures/fish_medium.png" });
    tryLoad(spriteFishLarge_, { L"res/textures/fishes/fish_12.png", L"res/textures/fish_large.png" });

    player_.SetSprite(spritePlayer_.IsValid() ? &spritePlayer_ : nullptr);
    player_.SetPosition(static_cast<float>(width_) * 0.5f, waterTop_ - 70.0f);
    player_.SetSpeed(260.0f);

    fishing_.SetLevels(rodLevel_, lineLevel_);
    fishing_.SetWaterRect(waterRect_);

    fishManager_.SetSprites(spriteFishSmall_.IsValid() ? &spriteFishSmall_ : nullptr,
        spriteFishMedium_.IsValid() ? &spriteFishMedium_ : nullptr,
        spriteFishLarge_.IsValid() ? &spriteFishLarge_ : nullptr);
    fishManager_.SetWaterRect(waterRect_);

    ui_.SetClientSize(width_, height_);

    shop_.SetClientSize(width_, height_);

    return true;
}

// 창 크기에 맞춰 수면/수역/백버퍼 등을 갱신한다.
void Game::UpdateWaterRect()
{
    waterTop_ = static_cast<float>(height_) * 0.35f;
    waterBottom_ = static_cast<float>(height_) - 80.0f;
    waterRect_.left = 0;
    waterRect_.right = width_;
    waterRect_.top = static_cast<LONG>(waterTop_);
    waterRect_.bottom = static_cast<LONG>(waterBottom_);

    fishManager_.SetWaterRect(waterRect_);
    fishing_.SetWaterRect(waterRect_);
    ui_.SetClientSize(width_, height_);
    shop_.SetClientSize(width_, height_);
    EnsureBackBuffer(width_, height_);
}

// 창 크기 변경 시 수역과 플레이어 위치 재배치.
void Game::OnResize(int width, int height)
{
    width_ = width;
    height_ = height;
    UpdateWaterRect();
    player_.SetPosition(player_.GetX(), waterTop_ - 70.0f);
}

// 키가 눌렸을 때 상태를 기록.
void Game::OnKeyDown(WPARAM key)
{
    if (key < keyDown_.size())
    {
        if (!keyDown_[key])
        {
            keyPressed_[key] = true;
        }
        keyDown_[key] = true;
    }
}

// 키가 떼어졌을 때 상태를 기록.
void Game::OnKeyUp(WPARAM key)
{
    if (key < keyDown_.size())
    {
        keyDown_[key] = false;
        keyReleased_[key] = true;
    }
}

// 마우스 버튼 눌림 처리: 디버그/상점 클릭을 우선 소비.
void Game::OnMouseDown(int x, int y)
{
    POINT pt{ x, y };
    mousePos_ = pt;
    mouseDown_ = true;
    mousePressed_ = true;

    if (debugPanelOpen_ && HandleDebugClick(pt))
    {
        inputBlocked_ = true;
        return;
    }

    inputBlocked_ = false;
    if (shop_.HandleClick(pt, goldBalance_, rodLevel_, lineLevel_))
    {
        fishing_.SetLevels(rodLevel_, lineLevel_);
        inputBlocked_ = true; // consume click so fishing doesn't charge
    }
}

// 마우스 버튼 떼기 처리.
void Game::OnMouseUp(int x, int y)
{
    POINT pt{ x, y };
    mousePos_ = pt;
    mouseDown_ = false;
    mouseReleased_ = true;
}

// 마우스 이동 위치만 기록.
void Game::OnMouseMove(int x, int y)
{
    mousePos_.x = x;
    mousePos_.y = y;
}

// 입력/낚시/물고기/상점 상태를 한 프레임 갱신한다.
void Game::Update(float deltaTime)
{
    const bool moveLeft = keyDown_[VK_LEFT] || keyDown_['A'];
    const bool moveRight = keyDown_[VK_RIGHT] || keyDown_['D'];
    // 마우스 입력은 별도 처리.

    if (keyPressed_[VK_F12])
    {
        debugPanelOpen_ = !debugPanelOpen_;
    }

    if (keyPressed_['S'])
    {
        shop_.Toggle();
        state_ = shop_.IsOpen() ? GameState::Shop : GameState::InGame;
    }

    player_.Update(moveLeft, moveRight, deltaTime, width_);
    fishManager_.Update(deltaTime);
    fishing_.Update(deltaTime, player_, fishManager_, mousePos_, mouseDown_, mousePressed_, mouseReleased_, inputBlocked_);

    CatchResult result;
    if (fishing_.ConsumeCatch(result))
    {
        auto GetScaledGold = [](int baseGold, float totalGold)
        {
            constexpr float GOLD_SCALE = 0.0025f;
            constexpr float GOLD_MAX_MULT = 10.0f;
            float factor = 1.0f + totalGold * GOLD_SCALE;
            if (factor > GOLD_MAX_MULT) factor = GOLD_MAX_MULT;
            return static_cast<int>(std::round(baseGold * factor));
        };
        auto GetFishGold = [&](FishType type)
        {
            int baseGold = 0;
            switch (type)
            {
            case FishType::Small: baseGold = 5; break;
            case FishType::Medium: baseGold = 10; break;
            case FishType::Large: baseGold = 20; break;
            }
            return GetScaledGold(baseGold, totalGold_);
        };

        int reward = GetFishGold(result.type);
        goldBalance_ += static_cast<float>(reward);
        CheckMilestone(static_cast<float>(reward));
        score_ += reward;
    }

    if (messageTimer_ > 0.0f)
    {
        messageTimer_ -= deltaTime;
        if (messageTimer_ < 0.0f) messageTimer_ = 0.0f;
    }

    keyPressed_.fill(false);   // 프레임 단위 플래그 초기화.
    keyReleased_.fill(false);  // 프레임 단위 플래그 초기화.
    mousePressed_ = false;     // 프레임 단위 플래그 초기화.
    mouseReleased_ = false;    // 프레임 단위 플래그 초기화.
    inputBlocked_ = false;     // 상점/디버그 클릭 소비 플래그 초기화.
}

// 더블 버퍼에 배경/물고기/플레이어/UI/디버그 패널을 그린다.
void Game::Render(HDC hdc)
{
    if (width_ <= 0 || height_ <= 0)
        return;
    EnsureBackBuffer(width_, height_);
    HDC target = backDC_ ? backDC_ : hdc;

    RECT client{};
    client.left = 0; client.top = 0; client.right = width_; client.bottom = height_;

    HBRUSH sky = CreateSolidBrush(RGB(26, 46, 76));
    FillRect(target, &client, sky);
    DeleteObject(sky);

    RECT water = waterRect_;
    HBRUSH waterBrush = CreateSolidBrush(RGB(30, 80, 140));
    FillRect(target, &water, waterBrush);
    DeleteObject(waterBrush);

    RECT ground{ 0, static_cast<LONG>(waterBottom_), width_, height_ };
    HBRUSH groundBrush = CreateSolidBrush(RGB(80, 60, 40));
    FillRect(target, &ground, groundBrush);
    DeleteObject(groundBrush);

    fishManager_.Render(target);
    fishing_.Render(target, player_);
    player_.Render(target);

    ui_.Render(target, score_, static_cast<int>(goldBalance_), fishing_.GetLineTension(), rodLevel_, lineLevel_, shop_.IsOpen(), fishing_.GetChargeRatio(), mouseDown_, true);
    shop_.Render(target, goldBalance_, rodLevel_, lineLevel_);

    if (messageTimer_ > 0.0f && !messageText_.empty())
    {
        Gdiplus::Graphics g(backDC_ ? backDC_ : hdc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        Gdiplus::FontFamily family(L"Segoe UI");
        Gdiplus::Font font(&family, 16, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush(Gdiplus::Color(240, 240, 200));
        Gdiplus::RectF layout(0.0f, 8.0f, static_cast<Gdiplus::REAL>(width_), 30.0f);
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentNear);
        g.DrawString(messageText_.c_str(), -1, &font, layout, &fmt, &brush);
    }

    if (debugPanelOpen_)
    {
        LayoutDebugPanel();
        HDC drawDC = backDC_ ? backDC_ : hdc;
        Gdiplus::Graphics g(drawDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        Gdiplus::RectF panel(static_cast<Gdiplus::REAL>(debugPanelRect_.left), static_cast<Gdiplus::REAL>(debugPanelRect_.top),
            static_cast<Gdiplus::REAL>(debugPanelRect_.right - debugPanelRect_.left),
            static_cast<Gdiplus::REAL>(debugPanelRect_.bottom - debugPanelRect_.top));
        Gdiplus::SolidBrush bg(Gdiplus::Color(40, 60, 90, 220));
        g.FillRectangle(&bg, panel);
        Gdiplus::Pen pen(Gdiplus::Color(200, 220, 240));
        g.DrawRectangle(&pen, panel);
        Gdiplus::FontFamily family(L"Segoe UI");
        Gdiplus::Font font(&family, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(230, 230, 240));
        g.DrawString(L"Debug", -1, &font, Gdiplus::PointF(panel.X + 6.0f, panel.Y + 4.0f), &textBrush);

        Gdiplus::RectF addRect(static_cast<Gdiplus::REAL>(debugAddGoldRect_.left), static_cast<Gdiplus::REAL>(debugAddGoldRect_.top),
            static_cast<Gdiplus::REAL>(debugAddGoldRect_.right - debugAddGoldRect_.left),
            static_cast<Gdiplus::REAL>(debugAddGoldRect_.bottom - debugAddGoldRect_.top));
        Gdiplus::RectF resetRect(static_cast<Gdiplus::REAL>(debugResetRect_.left), static_cast<Gdiplus::REAL>(debugResetRect_.top),
            static_cast<Gdiplus::REAL>(debugResetRect_.right - debugResetRect_.left),
            static_cast<Gdiplus::REAL>(debugResetRect_.bottom - debugResetRect_.top));
        Gdiplus::RectF rodRect(static_cast<Gdiplus::REAL>(debugRodUpRect_.left), static_cast<Gdiplus::REAL>(debugRodUpRect_.top),
            static_cast<Gdiplus::REAL>(debugRodUpRect_.right - debugRodUpRect_.left),
            static_cast<Gdiplus::REAL>(debugRodUpRect_.bottom - debugRodUpRect_.top));
        Gdiplus::RectF lineRect(static_cast<Gdiplus::REAL>(debugLineUpRect_.left), static_cast<Gdiplus::REAL>(debugLineUpRect_.top),
            static_cast<Gdiplus::REAL>(debugLineUpRect_.right - debugLineUpRect_.left),
            static_cast<Gdiplus::REAL>(debugLineUpRect_.bottom - debugLineUpRect_.top));

        auto drawButton = [&](const Gdiplus::RectF& r, const wchar_t* label)
        {
            Gdiplus::SolidBrush b(Gdiplus::Color(70, 120, 180));
            g.FillRectangle(&b, r);
            g.DrawRectangle(&pen, r);
            g.DrawString(label, -1, &font, Gdiplus::PointF(r.X + 6.0f, r.Y + 4.0f), &textBrush);
        };
        drawButton(addRect, L"+100 Gold");
        drawButton(resetRect, L"Reset Game");
        drawButton(rodRect, L"Rod +1");
        drawButton(lineRect, L"Line +1");
    }

    if (backDC_)
    {
        BitBlt(hdc, 0, 0, width_, height_, backDC_, 0, 0, SRCCOPY);
    }
}

// 백버퍼 크기가 맞지 않으면 새로 만든다.
void Game::EnsureBackBuffer(int width, int height)
{
    if (backDC_ && backBmp_)
    {
        BITMAP bm{};
        GetObject(backBmp_, sizeof(bm), &bm);
        if (bm.bmWidth == width && bm.bmHeight == height)
            return;
    }

    DestroyBackBuffer();
    HDC screen = GetDC(hwnd_);
    backDC_ = CreateCompatibleDC(screen);
    backBmp_ = CreateCompatibleBitmap(screen, width, height);
    backOld_ = (HBITMAP)SelectObject(backDC_, backBmp_);
    ReleaseDC(hwnd_, screen);
}

// 백버퍼 리소스를 해제한다.
void Game::DestroyBackBuffer()
{
    if (backDC_)
    {
        if (backOld_)
            SelectObject(backDC_, backOld_);
        DeleteDC(backDC_);
        backDC_ = nullptr;
    }
    if (backBmp_)
    {
        DeleteObject(backBmp_);
        backBmp_ = nullptr;
    }
    backOld_ = nullptr;
}

// 스프라이트 로드 헬퍼 함수.
bool Game::LoadSprite(Sprite& sprite, const wchar_t* path)
{
    return sprite.Load(path);
}

// 상대 경로를 실행 파일 기준의 실제 경로로 변환한다.
std::wstring Game::ResourcePath(const wchar_t* relative) const
{
    if (!relative || relative[0] == L'\0')
    {
        return exeDir_.wstring();
    }

    std::filesystem::path rel(relative);
    if (rel.is_absolute())
    {
        return rel.wstring();
    }

    std::array<std::filesystem::path, 4> candidates{
        exeDir_ / rel,
        exeDir_.parent_path() / rel,            // e.g. build/Debug -> build
        exeDir_.parent_path().parent_path() / rel, // e.g. build/Debug -> project root
        std::filesystem::current_path() / rel
    };

    for (const auto& c : candidates)
    {
        if (std::filesystem::exists(c))
        {
            return c.wstring();
        }
    }

    // 찾지 못해도 exeDir 기준 경로를 반환.
    return (exeDir_ / rel).wstring();
}

// 누적 골드에 따른 보너스/메시지를 계산한다.
void Game::CheckMilestone(float deltaGold)
{
    constexpr float kMilestoneStep = 500.0f;
    constexpr float kGoldMax = 4000.0f;
    totalGold_ += deltaGold;

    if (!goldMaxed_ && totalGold_ >= kGoldMax)
    {
        goldMaxed_ = true;
        messageText_ = L"누적 획득 금액이 최대치(" + std::to_wstring(static_cast<int>(kGoldMax)) +
            L")에 도달했습니다. 더 이상 생선의 판매 금액이 상승하지 않습니다.";
        messageTimer_ = 3.0f;
        return;
    }

    if (goldMaxed_)
        return;

    while (totalGold_ >= (static_cast<float>(goldMilestone_ + 1) * kMilestoneStep))
    {
        goldMilestone_ += 1;
        messageText_ = L"누적 획득 금액이 " + std::to_wstring(static_cast<int>(goldMilestone_ * kMilestoneStep)) +
            L"이(가) 되어 생선 판매 가격이 상승했습니다.";
        messageTimer_ = 3.0f;
    }
}

// 점수/골드/레벨 및 물고기 풀을 초기화한다.
void Game::ResetGame()
{
    score_ = 0;
    goldBalance_ = 0.0f;
    totalGold_ = 0.0f;
    rodLevel_ = 1;
    lineLevel_ = 1;
    goldMilestone_ = 0;
    messageTimer_ = 0.0f;
    messageText_.clear();
    goldMaxed_ = false;
    fishing_.SetLevels(rodLevel_, lineLevel_);
    fishManager_.Clear();
}

// 디버그 패널 영역 클릭 여부를 검사하고 동작을 실행한다.
bool Game::HandleDebugClick(const POINT& pt)
{
    LayoutDebugPanel();
    if (!PtInRect(&debugPanelRect_, pt))
        return false;

    if (PtInRect(&debugAddGoldRect_, pt))
    {
        goldBalance_ += 100.0f;
        CheckMilestone(100.0f);
        return true;
    }

    if (PtInRect(&debugResetRect_, pt))
    {
        ResetGame();
        return true;
    }

    if (PtInRect(&debugRodUpRect_, pt))
    {
        if (rodLevel_ < 10)
        {
            rodLevel_ += 1;
            fishing_.SetLevels(rodLevel_, lineLevel_);
        }
        return true;
    }

    if (PtInRect(&debugLineUpRect_, pt))
    {
        if (lineLevel_ < 10)
        {
            lineLevel_ += 1;
            fishing_.SetLevels(rodLevel_, lineLevel_);
        }
        return true;
    }

    return false;
}

// 디버그 패널과 버튼들의 위치를 계산한다.
void Game::LayoutDebugPanel()
{
    const int panelWidth = 160;
    const int panelHeight = 140;
    int left = 12;
    int top = height_ - panelHeight - 12;
    debugPanelRect_ = { left, top, left + panelWidth, top + panelHeight };

    int btnHeight = 24;
    int btnMargin = 6;
    int yBase = top + 22;
    debugAddGoldRect_ = { left + btnMargin, yBase, left + panelWidth - btnMargin, yBase + btnHeight };
    debugResetRect_ = { left + btnMargin, yBase + (btnHeight + 6) * 1, left + panelWidth - btnMargin, yBase + (btnHeight + 6) * 1 + btnHeight };
    debugRodUpRect_ = { left + btnMargin, yBase + (btnHeight + 6) * 2, left + panelWidth - btnMargin, yBase + (btnHeight + 6) * 2 + btnHeight };
    debugLineUpRect_ = { left + btnMargin, yBase + (btnHeight + 6) * 3, left + panelWidth - btnMargin, yBase + (btnHeight + 6) * 3 + btnHeight };
}
