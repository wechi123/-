#define WINVER 0x0601
#include <windows.h>
LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if(m==WM_DESTROY){ PostQuitMessage(0); return 0; }
    return DefWindowProcW(h,m,w,l);
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASSW wc={0};
    wc.lpfnWndProc=WndProc; wc.hInstance=hInst;
    wc.lpszClassName=L"SimpleWin2";
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor=LoadCursorW(0,(LPCWSTR)IDC_ARROW);
    RegisterClassW(&wc);

    HWND hw=CreateWindowExW(0,L"SimpleWin2",L"Hello World 你好",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE, 200,200,400,300,0,0,hInst,0);

    MSG msg;
    while(GetMessageW(&msg,0,0,0)){
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    return 0;
}
