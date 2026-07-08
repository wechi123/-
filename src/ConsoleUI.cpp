#include "ConsoleUI.h"
#include <iostream>
#include <limits>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

void ConsoleUI::clearScreen() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    COORD homeCoords = {0, 0};
    if (hConsole == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hConsole, homeCoords);
#else
    std::cout << "\033[2J\033[H";
#endif
}

void ConsoleUI::pause(const std::string& msg) {
    std::string m = msg.empty() ? "按回车键继续..." : msg;
    std::cout << "\n" << m;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int ConsoleUI::colorToAnsi(UIColor color) {
    switch (color) {
        case UIColor::RED:     return 31;
        case UIColor::GREEN:   return 32;
        case UIColor::YELLOW:  return 33;
        case UIColor::BLUE:    return 34;
        case UIColor::MAGENTA: return 35;
        case UIColor::CYAN:    return 36;
        default:               return 0;
    }
}

void ConsoleUI::print(const std::string& text) {
    std::cout << text;
}

void ConsoleUI::println(const std::string& text) {
    std::cout << text << "\n";
}

void ConsoleUI::printlnColored(const std::string& text, UIColor color) {
    int ansi = colorToAnsi(color);
    if (ansi == 0)
        std::cout << text << "\n";
    else
        std::cout << "\033[" << ansi << "m" << text << "\033[0m\n";
}

void ConsoleUI::drawTitle(const std::string& title, int width) {
    drawLine('=', width);
    int padding = (width - (int)title.length()) / 2;
    if (padding < 0) padding = 0;
    std::cout << std::string((std::size_t)padding, ' ') << title << "\n";
    drawLine('=', width);
}

void ConsoleUI::drawLine(char ch, int width) {
    std::cout << std::string((std::size_t)width, ch) << "\n";
}

void ConsoleUI::drawMenu(const std::string& title,
                          const std::vector<std::string>& options,
                          int width) {
    drawLine('=', width);
    int padding = (width - (int)title.length()) / 2;
    if (padding < 0) padding = 0;
    std::cout << std::string((std::size_t)padding, ' ') << title << "\n";
    drawLine('=', width);
    for (std::size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << "\n";
    }
    drawLine('=', width);
}

int ConsoleUI::readChoice(int min, int max) {
    int choice;
    while (true) {
        std::cout << "  请选择操作（" << min << "-" << max << "）：";
        std::cin >> choice;
        if (std::cin.fail() || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            showError("输入无效，请重新输入！");
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}

std::string ConsoleUI::readString(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int ConsoleUI::readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            showError("请输入有效数字！");
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

std::string ConsoleUI::hpBar(int current, int maxHp, int barWidth) {
    if (maxHp <= 0) maxHp = 1;
    int filled = (int)((double)current / maxHp * barWidth);
    if (filled > barWidth) filled = barWidth;
    if (filled < 0) filled = 0;
    std::ostringstream oss;
    oss << "[";
    for (int i = 0; i < filled; ++i) oss << "#";
    for (int i = filled; i < barWidth; ++i) oss << " ";
    oss << "] " << current << "/" << maxHp;
    return oss.str();
}

void ConsoleUI::showMessage(const std::string& msg) {
    std::cout << "  [*] " << msg << "\n";
}

void ConsoleUI::showError(const std::string& msg) {
    std::cout << "  [!] 错误：" << msg << "\n";
}

void ConsoleUI::drawInfoBox(const std::string& title,
                             const std::vector<std::string>& lines,
                             int width) {
    drawLine('-', width);
    std::cout << "  " << title << "\n";
    drawLine('-', width);
    for (const auto& line : lines) {
        std::cout << "  " << line << "\n";
    }
    drawLine('-', width);
}

bool ConsoleUI::confirm(const std::string& question) {
    std::cout << "  [?] " << question << " (y/n): ";
    char c;
    std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (c == 'y' || c == 'Y');
}
