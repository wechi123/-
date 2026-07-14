#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if(m==WM_DESTROY){ PostQuitMessage(0); return 0; }
    return DefWindowProcW(h,m,w,l);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    INITCOMMONCONTROLSEX icc={sizeof(icc),ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    WNDCLASSW wc={};
    wc.lpfnWndProc=WndProc; wc.hInstance=hInst;
    wc.lpszClassName=L"TestWindow";
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor=LoadCursor(0,IDC_ARROW);
    RegisterClassW(&wc);

    HWND hw = CreateWindowExW(0, L"TestWindow", L"Test Title",
        WS_OVERLAPPEDWINDOW, 100, 100, 500, 400,
        0,0,hInst,0);
    ShowWindow(hw, SW_SHOW);
    UpdateWindow(hw);

    MSG msg;
    while(GetMessage(&msg,0,0,0)){
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    return 0;
}
