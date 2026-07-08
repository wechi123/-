#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "IGameUI.h"
#include <string>
#include <vector>

class ConsoleUI : public IGameUI {
public:
    void clearScreen() override;

    void drawTitle(const std::string& title, int width = 48) override;
    void drawLine(char ch = '=', int width = 48) override;

    void drawMenu(const std::string& title,
                  const std::vector<std::string>& options,
                  int width = 48) override;

    void drawInfoBox(const std::string& title,
                     const std::vector<std::string>& lines,
                     int width = 48) override;

    void showMessage(const std::string& msg) override;
    void showError(const std::string& msg) override;

    void print(const std::string& text) override;
    void println(const std::string& text = "") override;
    void printlnColored(const std::string& text, UIColor color) override;

    int readChoice(int min, int max) override;
    std::string readString(const std::string& prompt) override;
    int readInt(const std::string& prompt) override;
    bool confirm(const std::string& question) override;

    void pause(const std::string& msg = "") override;

    std::string hpBar(int current, int maxHp, int barWidth = 20) override;

private:
    static int colorToAnsi(UIColor color);
};

#endif
