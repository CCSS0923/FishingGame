// ?곸젏 UI? 援щℓ 濡쒖쭅???뺤쓽?섎뒗 ?ㅻ뜑濡??꾩씠??紐⑸줉, 媛寃? 援щℓ 泥섎━ ?명꽣?섏씠?ㅻ? ?쒓났?⑸땲??
#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class Shop
{
public:
    Shop(); // ?곸젏 UI 珥덇린??

    // 寃뚯엫 李??ш린??留욎떠 ?곸뿭 ?ш퀎??
    void SetClientSize(int width, int height);

    // ?곸젏 ?닿린/?リ린 ?좉?.
    void Toggle();
    bool IsOpen() const { return open_; }

    // ?곸젏 UI ?뚮뜑留?
    void Render(HDC hdc, float money, int rodLevel, int lineLevel) const;
    // ?곸젏 ?대┃ 泥섎━: ?낃렇?덉씠??援щℓ ??湲덉븸/?덈꺼 ?섏젙.
    bool HandleClick(POINT pt, float& money, int& rodLevel, int& lineLevel);

private:
    RECT panelRect_;  // ?곸젏 ?⑤꼸 ?곸뿭.
    RECT rodButton_;  // 濡쒕뱶 ?낃렇?덉씠??踰꾪듉 ?곸뿭.
    RECT lineButton_; // ?쇱씤 ?낃렇?덉씠??踰꾪듉 ?곸뿭.

    bool open_;  // ?대┝ ?곹깭 ?뚮옒洹?
    int width_;  // ?대씪?댁뼵????
    int height_; // ?대씪?댁뼵???믪씠.

};
