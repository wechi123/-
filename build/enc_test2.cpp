#include <windows.h>
#include <cstdio>
int main() {
    const char* s = "校园面包";  // UTF-8 bytes
    wchar_t w[100];
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, 100);
    wprintf(L"CP_UTF8 Result: %s\n", w);
    return 0;
}
