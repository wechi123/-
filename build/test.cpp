#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#include <windows.h>
#pragma comment(lib, "comctl32.lib")
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    MessageBoxW(0, L"Test OK!", L"Debug", MB_OK);
    return 0;
}
