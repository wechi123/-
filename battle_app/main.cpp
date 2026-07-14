#include "GameManager.h"
#include "ConsoleUI.h"
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

int main() {
    srand((unsigned)time(nullptr));

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    ConsoleUI consoleUI;
    GameManager game(consoleUI);
    game.init();
    game.run();
    game.cleanup();

    consoleUI.pause();
    return 0;
}
