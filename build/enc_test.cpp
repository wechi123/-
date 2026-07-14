#include <windows.h>
#include <cstdio>
int main() {
    const char* s = "校园面包";
    printf("Bytes: ");
    for(int i=0; s[i]; i++) printf("%02X ", (unsigned char)s[i]);
    printf("\nCP_ACP MultiByteToWideChar test:\n");
    wchar_t w[100];
    MultiByteToWideChar(CP_ACP, 0, s, -1, w, 100);
    wprintf(L"Result: %s\n", w);
    return 0;
}
