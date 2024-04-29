#include "DrawEngine.h"

DrawEngine::DrawEngine(HDC hdc, HWND hwnd, int pxPerBlock,
                       int x, int y,
                       int width, int height) :
hdc(hdc), hwnd(hwnd), width(width), height(height), _x(x), _y(y)
{
    GetClientRect(hwnd, &rect);

    SaveDC(hdc);

    // Set up coordinate system
    SetMapMode(hdc, MM_ISOTROPIC);
    SetViewportExtEx(hdc, pxPerBlock, pxPerBlock, 0);
    SetWindowExtEx(hdc, 1, -1, 0);
    SetViewportOrgEx(hdc, x, y + rect.bottom, 0);

    // Set default colors
    SetTextColor(hdc, RGB(255,255,255));
    SetBkColor(hdc, RGB(70,70,70));
    SetBkMode(hdc, TRANSPARENT);
}

DrawEngine::~DrawEngine()
{
    // Restore coordinate system
    RestoreDC(hdc, -1);
}

void DrawEngine::drawBlock(int x, int y, COLORREF color)
{
    HBRUSH hBrush = CreateSolidBrush(color);
    rect.left = _x + x;
    rect.right = _x + x + 1;
    rect.top = _y + y;
    rect.bottom = _y + y + 1;

    FillRect(hdc, &rect, hBrush);

    // Draw left and bottom black border
    MoveToEx(hdc, _x + x, _y + y + 1, NULL);
    LineTo(hdc, _x + x, _y + y);
    LineTo(hdc, _x + x + 1, _y + y);
    DeleteObject(hBrush);
}

void DrawEngine::drawInterface()
{
    // Draw a gray area at the right
    HBRUSH hBrush = CreateSolidBrush(RGB(70,70,70));
    rect.top = height;
    rect.left = width;
    rect.bottom = 0;
    rect.right = width + 8;
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    //hBrush = CreateSolidBrush(RGB(70, 70, 70));
    //rect.top = height;
    //rect.left = 203 + width;
    //rect.bottom = 0;
    //rect.right = 203 + width + 8;
    //FillRect(hdc, &rect, hBrush);
    //DeleteObject(hBrush);
}

void DrawEngine::drawText(TCHAR *szText, int x, int y) const
{
    TextOut(hdc, _x + x, _y + y, szText, lstrlen(szText));
}

void DrawEngine::drawScore(int score, int x, int y) const
{
    TCHAR szBuffer[20];
    int len = wsprintf(szBuffer, TEXT("Score: %6d"), score);
    SetBkMode(hdc, OPAQUE);
    TextOut(hdc, _x + x, _y + y, szBuffer, len);
    SetBkMode(hdc, TRANSPARENT);
}

void DrawEngine::drawSpeed(int speed, int x, int y) const
{
    TCHAR szBuffer[20];
    int len = wsprintf(szBuffer, TEXT("Speed: %6d"), speed);
    SetBkMode(hdc, OPAQUE);
    TextOut(hdc, _x + x, _y + y, szBuffer, len);
    SetBkMode(hdc, TRANSPARENT);
}

void DrawEngine::drawNextPiece(Piece &piece, int x, int y)
{
    TCHAR szBuffer[] = TEXT("Next:");
    TextOut(hdc, _x + x, _y + y + 5, szBuffer, lstrlen(szBuffer));
    COLORREF color = piece.getColor();

    // Draw the piece in a 4x4 square area
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (piece.isPointExists(i, j))
                drawBlock(i + x + _x, j + y + _y, color);
            else
                drawBlock(i + x + _x, j + y + _y, RGB(0,0,0));
        }
    }
}
