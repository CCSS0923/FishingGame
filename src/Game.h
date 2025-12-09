// 寃뚯엫 猷⑦봽? ?먯썝 濡쒕뵫, ?낅젰/?낅뜲?댄듃/?뚮뜑留??먮쫫??珥앷큵?섎뒗 ?곸쐞 寃뚯엫 ?명꽣?섏씠?ㅼ엯?덈떎.
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
    Game(); // 寃뚯엫 ?꾩껜 ?곹깭 珥덇린??

    bool Initialize(HWND hwnd, int clientWidth, int clientHeight); // ?덈룄???몃뱾怨??ш린瑜?諛쏆븘 由ъ냼??珥덇린??
    void OnResize(int width, int height);                         // 李??ш린 蹂寃????몄텧.

    void OnKeyDown(WPARAM key); // ???ㅼ슫 ?대깽??湲곕줉.
    void OnKeyUp(WPARAM key);   // ?????대깽??湲곕줉.
    void OnMouseDown(int x, int y); // 留덉슦??踰꾪듉 ?뚮┝ 泥섎━.
    void OnMouseUp(int x, int y);   // 留덉슦??踰꾪듉 ??泥섎━.
    void OnMouseMove(int x, int y); // 留덉슦???대룞 湲곕줉.

    void Update(float deltaTime); // ?꾨젅?꾨쭏??寃뚯엫 濡쒖쭅 媛깆떊.
    void Render(HDC hdc);         // ?붾툝 踰꾪띁瑜??ъ슜???붾㈃ 洹몃━湲?

private:
    void UpdateWaterRect(); // 李??ш린??留욎떠 ?섏뿭/諛깅쾭?????ш퀎??
    bool LoadSprite(Sprite& sprite, const wchar_t* path); // ?ㅽ봽?쇱씠??濡쒕뱶 ?ы띁.
    void EnsureBackBuffer(int width, int height); // 諛깅쾭???ш린 ?뺤씤/?ъ깮??
    void DestroyBackBuffer(); // 諛깅쾭???먯썝 ?댁젣.
    void CheckMilestone(float deltaGold); // ?꾩쟻 怨⑤뱶 milestone 泥댄겕 諛?硫붿떆吏 媛깆떊.
    void ResetGame(); // ?먯닔/怨⑤뱶/?덈꺼 珥덇린??
    bool HandleDebugClick(const POINT& pt); // ?붾쾭洹??⑤꼸 ?대┃ 泥섎━.
    void LayoutDebugPanel(); // ?붾쾭洹?踰꾪듉 ?곸뿭 怨꾩궛.
    std::wstring ResourcePath(const wchar_t* relative) const; // 由ъ냼??寃쎈줈 ?댁꽍.

    HWND hwnd_;   // ?덈룄???몃뱾.
    int width_;   // ?대씪?댁뼵????
    int height_;  // ?대씪?댁뼵???믪씠.
    GameState state_; // 寃뚯엫 ?곹깭(?멸쾶???곸젏/?쇱떆?뺤?).

    Player player_;           // ?뚮젅?댁뼱(蹂댄듃) 媛앹껜.
    FishingSystem fishing_;   // ?싳떆 ?쒖뒪??
    FishManager fishManager_; // 臾쇨퀬湲??ㅽ룿/愿由?
    UI ui_;                   // HUD ?뚮뜑??
    Shop shop_;               // ?곸젏 UI/濡쒖쭅.

    int score_;          // ?꾩쟻 ?먯닔.
    float goldBalance_;  // ?꾩옱 ?뚯? 怨⑤뱶.
    float totalGold_;    // ?꾩쟻 ?띾뱷 怨⑤뱶(媛寃??곸듅 怨꾩궛??.
    int rodLevel_;       // 濡쒕뱶 ?덈꺼.
    int lineLevel_;      // ?쇱씤 ?덈꺼.
    int goldMilestone_;  // ?ㅼ쓬 怨⑤뱶 ?곸듅 ?④퀎 ?몃뜳??
    float messageTimer_; // 硫붿떆吏 ?쒖떆 ?붿뿬 ?쒓컙.
    std::wstring messageText_; // 硫붿떆吏 ?댁슜.
    bool goldMaxed_;     // 怨⑤뱶 ?곸듅 ?곹븳 ?꾨떖 ?щ?.
    bool debugPanelOpen_; // ?붾쾭洹??⑤꼸 ?대┝ ?щ?.
    RECT debugPanelRect_; // ?붾쾭洹??⑤꼸 ?곸뿭.
    RECT debugAddGoldRect_; // ?붾쾭洹? 怨⑤뱶 異붽? 踰꾪듉 ?곸뿭.
    RECT debugResetRect_;   // ?붾쾭洹? 由ъ뀑 踰꾪듉 ?곸뿭.
    RECT debugRodUpRect_;   // ?붾쾭洹? 濡쒕뱶 ??踰꾪듉.
    RECT debugLineUpRect_;  // ?붾쾭洹? ?쇱씤 ??踰꾪듉.

    float waterTop_;    // ?섎㈃??Y 醫뚰몴.
    float waterBottom_; // ?섏떖 ??Y 醫뚰몴.
    RECT waterRect_;    // ?섏뿭 ?ш컖??

    Sprite spritePlayer_;      // 蹂댄듃 ?ㅽ봽?쇱씠??
    Sprite spriteFishSmall_;   // ?뚰삎 臾쇨퀬湲??ㅽ봽?쇱씠??
    Sprite spriteFishMedium_;  // 以묓삎 臾쇨퀬湲??ㅽ봽?쇱씠??
    Sprite spriteFishLarge_;   // ???臾쇨퀬湲??ㅽ봽?쇱씠??
    std::filesystem::path exeDir_; // ?ㅽ뻾 ?뚯씪 ?꾩튂 寃쎈줈.

    std::array<bool, 256> keyDown_;     // ?꾩옱 ?뚮┝ ?곹깭.
    std::array<bool, 256> keyPressed_;  // ?대쾲 ?꾨젅???뚮┝.
    std::array<bool, 256> keyReleased_; // ?대쾲 ?꾨젅????

    bool mouseDown_;    // 留덉슦??踰꾪듉 吏???뚮┝.
    bool mousePressed_; // ?대쾲 ?꾨젅???뚮┝.
    bool mouseReleased_; // ?대쾲 ?꾨젅????
    bool inputBlocked_;  // UI ?깆쑝濡??낅젰 ?뚮퉬 ?щ?.
    POINT mousePos_;     // ?꾩옱 留덉슦???꾩튂.

    HDC backDC_;       // 諛깅쾭??DC.
    HBITMAP backBmp_;  // 諛깅쾭??鍮꾪듃留?
    HBITMAP backOld_;  // ?좏깮?섏뼱 ?덈뜕 ?댁쟾 鍮꾪듃留?

};
