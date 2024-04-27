#include "CorePch.h"

#ifdef _DEBUG
#pragma comment (lib, "Debug/ServerCore.lib")
#else
#pragma comment (lib, "Release/ServerCore.lib")
#endif


#include "OvlpCallback.h"
#include "PacketQueue.h"
#include "ServerSession.h"
#include "Piece.h"
#include "Game.h"
//#include <Windows.h>

using namespace std;

const int PX_PER_BLOCK = 25;    // Cell size in pixels
const int SCREEN_WIDTH = 10;    // Level width in cells
const int SCREEN_HEIGHT = 20;   // Level height in cells
const int GAME_SPEED = 33;      // Update the game every GAME_SPEED millisecs (= 1/fps)
const int TIMER_ID = 1;
HINSTANCE hInst;
static Session* session = nullptr;

#pragma region Procedures
LRESULT CALLBACK GameInputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    static Game* game;
    static DrawEngine* de;

    switch (message) {
        case WM_CREATE:
            hdc = GetDC(hwnd);

            de = new DrawEngine(hdc, hwnd, PX_PER_BLOCK);
            game = new Game(*de);
            SetTimer(hwnd, TIMER_ID, GAME_SPEED, NULL);

            ReleaseDC(hwnd, hdc);
            return 0;

        case WM_KEYDOWN:
            game->keyPress(wParam);
            return 0;

        case WM_TIMER:
            game->timerUpdate();
            return 0;

        case WM_KILLFOCUS:
            KillTimer(hwnd, TIMER_ID);
            game->pause(true);
            return 0;

        case WM_SETFOCUS:
            SetTimer(hwnd, TIMER_ID, GAME_SPEED, NULL);
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            game->repaint();
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            delete de;
            delete game;
            KillTimer(hwnd, TIMER_ID);
            PostQuitMessage(0);
            return 0;

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK InputBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    WCHAR word[1024];

    switch (message) {
        case WM_CREATE:
            CreateWindowW(L"edit", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_RIGHT, 100, 20, 100, 25, hWnd, NULL, hInst, &lParam);
            CreateWindowW(L"button", L"Connect", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 50, 100, 25, hWnd, NULL, hInst, NULL);
            break;
        case WM_LBUTTONDOWN:
            hdc = GetDC(hWnd); //DC을얻는 방법 1
            ReleaseDC(hWnd, hdc);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_COMMAND:
            switch (HIWORD(wParam)) {
                case EN_CHANGE:
                    hdc = GetDC(hWnd);
                    TextOutW(hdc, 100, 100, L"Edit컨트롤에 변화가 있습니다!", 19);
                    ReleaseDC(hWnd, hdc);
                    break;

                case BN_CLICKED:


                    break;
            }
            break;
        case WM_SETTEXT:

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
#pragma endregion

#pragma region Windows
int WINAPI ShowInputBox(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = InputBoxProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Test";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    //적용한 윈도우 생성 및 업데이트
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    HWND hWnd = CreateWindowW(L"Test",
                              L"Test",
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              300,
                              150,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);
    if (!hWnd) {
        return FALSE;
    }
    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

int WINAPI ShowGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("tetris");
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wc;

    // We need to repaint a lot, using CS_OWNDC is more efficient
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = GameInputProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;
    wc.hIconSm = NULL;

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,
                        TEXT("Eliang's Tetris"),
                        WS_MINIMIZEBOX | WS_SYSMENU,  // No window resizing
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        (SCREEN_WIDTH * PX_PER_BLOCK + 156) * 2,
                        SCREEN_HEIGHT * PX_PER_BLOCK + 25,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
#pragma endregion



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {

    SessionManager<ServerSession>::GetInstance().Init();
    std::thread packetFetchLoop(&PacketQueue::Flush, &PacketQueue::GetInstance());

    if (OvlpCallback::GetInstance().Start() == SOCKET_ERROR)
        return -1;

    session = OvlpCallback::GetInstance().Connect<ServerSession>(PF_INET, "127.0.0.1", 9190);
    if (session == nullptr)
        return -1;

    ///ShowInputBox(hInstance, hPrevInstance, szCmdLine, iCmdShow);

    ShowGame(hInstance, hPrevInstance, szCmdLine, iCmdShow);





    OvlpCallback::GetInstance().Close();
    PacketQueue::GetInstance().Close();

    packetFetchLoop.join();
    

    std::cout << "Server Closed" << std::endl;

    return 0;




}