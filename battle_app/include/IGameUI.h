#ifndef IGAMEUI_H
#define IGAMEUI_H

#include <string>
#include <vector>

enum class UIColor {
    DEFAULT = 0,
    RED     = 1,
    GREEN   = 2,
    YELLOW  = 3,
    BLUE    = 4,
    MAGENTA = 5,
    CYAN    = 6
};

class IGameUI {
public:
    virtual ~IGameUI() = default;

    // ========== 显示方法 ==========
    virtual void clearScreen() = 0;

    virtual void drawTitle(const std::string& title, int width = 48) = 0;
    virtual void drawLine(char ch = '=', int width = 48) = 0;

    virtual void drawMenu(const std::string& title,
                          const std::vector<std::string>& options,
                          int width = 48) = 0;

    virtual void drawInfoBox(const std::string& title,
                             const std::vector<std::string>& lines,
                             int width = 48) = 0;

    virtual void showMessage(const std::string& msg) = 0;
    virtual void showError(const std::string& msg) = 0;

    virtual void print(const std::string& text) = 0;
    virtual void println(const std::string& text = "") = 0;
    virtual void printlnColored(const std::string& text, UIColor color) = 0;

    // ========== 输入方法 ==========
    virtual int readChoice(int min, int max) = 0;
    virtual std::string readString(const std::string& prompt) = 0;
    virtual int readInt(const std::string& prompt) = 0;
    virtual bool confirm(const std::string& question) = 0;

    // ========== 暂停 ==========
    virtual void pause(const std::string& msg = "") = 0;

    // ========== 工具方法 ==========
    virtual std::string hpBar(int current, int maxHp, int barWidth = 20) = 0;
};

#endif
