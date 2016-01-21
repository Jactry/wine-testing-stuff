/* 
   This testcase was wrote for reproducing a bug on OS X:
   On OS X, when you try to use 'Hiragino Sans GB W3' to draw some text,
   some specific characters drew by ScriptStringOut will overlap others.
   This is a picture to show it: 
   https://img.vim-cn.com/42/5e96b1d98b62f94b14a22ac3a4e69fb1eb1b0d.png
   This bug can't be reproduced on Linux. And the interesting thing is
   native usp10.dll also can make it happier too.
   ('Hiragino Sans GB W3' is a Chinese font shipped by OS X, you need to 
   copy it to Linux if you want to run this testcase on Linux.)
*/

#include <windows.h>
#include <usp10.h>
#include <stdio.h>


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    WCHAR sometext[] = {'C','h','i','n','a','1','2','3',0};
    int len = (sizeof(sometext) / sizeof(WCHAR)) - 1;
    RECT rc = {0, 0, 100, 100};
    SCRIPT_STRING_ANALYSIS ssa = NULL;
    HFONT hFont;

    switch (message)
    {
        case WM_CREATE:
            return 0;

        case WM_COMMAND:
            break;

        case WM_PAINT:
            BeginPaint(hwnd, &ps);

            hdc = GetDC(hwnd);
            hFont = CreateFontA(-32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Hiragino Sans GB W3");
            SelectObject(hdc, hFont);
            ScriptStringAnalyse(hdc, sometext, len, len * 2, -1, SSA_GLYPHS|SSA_FALLBACK, 0,
                                     NULL, NULL, NULL, NULL, NULL, &ssa);
            ScriptStringOut(ssa, 10, 10, 0, &rc, 0, 0, FALSE);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = "usp10";
    MSG msg;
    HWND hwnd;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, szAppName);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBoxA(NULL, "This program requires Windows NT!",
                    szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindowA(szAppName, "usp10: Test ScriptTextOut",
                         WS_OVERLAPPEDWINDOW, 200, 200, 400, 300,
                         NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
