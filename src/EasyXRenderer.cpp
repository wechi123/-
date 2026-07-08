#include "EasyXRenderer.h"
#include "Character.h"
#include "Enemy.h"
#include "Inventory.h"
#include "Item.h"
#include "Shop.h"
#include "TaskSystem.h"
#include "Task.h"
#include <conio.h>       // _getch()
#include <sstream>
#include <iomanip>

// ========== 颜色常量 ==========
#define COLOR_BG           RGB(25,  25,  35)    // 深色背景
#define COLOR_PANEL        RGB(40,  40,  55)    // 面板背景
#define COLOR_TITLE        RGB(255, 215, 0)     // 金色标题
#define COLOR_TEXT         RGB(220, 220, 220)   // 浅色文字
#define COLOR_TEXT_DIM     RGB(150, 150, 160)   // 暗色文字
#define COLOR_HP_BAR       RGB(50,  200, 50)    // HP 绿
#define COLOR_HP_BAR_BG    RGB(60,  60,  60)    // HP 背景
#define COLOR_HP_LOW       RGB(220, 50,  50)    // HP 低红
#define COLOR_BUTTON       RGB(55,  55,  75)    // 按钮
#define COLOR_BUTTON_HOVER RGB(80,  80,  120)   // 按钮高亮
#define COLOR_BORDER       RGB(100, 100, 120)   // 边框
#define COLOR_GOLD         RGB(255, 200, 50)    // 金币色
#define COLOR_EXP_BAR      RGB(100, 150, 255)   // EXP 蓝
#define COLOR_EXP_BAR_BG   RGB(50,  50,  65)    // EXP 背景

// ========== 构造 / 析构 ==========
EasyXRenderer::EasyXRenderer() : windowWidth(900), windowHeight(650) {}

EasyXRenderer::~EasyXRenderer() {
    closeWindow();
}

// ========== 窗口初始化 ==========
void EasyXRenderer::initWindow(int width, int height, const std::string& title) {
    windowWidth  = width;
    windowHeight = height;
    initgraph(windowWidth, windowHeight);
    setbkcolor(COLOR_BG);
    cleardevice();
    // 设置窗口标题
    HWND hwnd = GetHWnd();
    SetWindowText(hwnd, title.c_str());
}

// ========== 关闭窗口 ==========
void EasyXRenderer::closeWindow() {
    closegraph();
}

// ========== 画 HP 血条 ==========
void EasyXRenderer::drawHPBar(int x, int y, int barWidth, int current, int max) {
    int barHeight = 18;

    // 背景
    setfillcolor(COLOR_HP_BAR_BG);
    solidrectangle(x, y, x + barWidth, y + barHeight);

    if (max <= 0) return;

    // 前景
    int fillWidth = static_cast<int>((static_cast<double>(current) / max) * barWidth);
    if (fillWidth < 0) fillWidth = 0;

    double ratio = static_cast<double>(current) / max;
    if (ratio > 0.3)
        setfillcolor(COLOR_HP_BAR);
    else
        setfillcolor(COLOR_HP_LOW);

    solidrectangle(x, y, x + fillWidth, y + barHeight);

    // 边框
    setlinecolor(COLOR_BORDER);
    rectangle(x, y, x + barWidth, y + barHeight);

    // 文字
    std::stringstream ss;
    ss << current << "/" << max;
    settextcolor(COLOR_TEXT);
    setbkmode(TRANSPARENT);
    outtextxy(x + barWidth / 2 - 30, y + 2, ss.str().c_str());
}

// ========== 画按钮 ==========
void EasyXRenderer::drawButton(int x, int y, int w, int h, const std::string& text, bool selected) {
    if (selected)
        setfillcolor(COLOR_BUTTON_HOVER);
    else
        setfillcolor(COLOR_BUTTON);

    solidroundrect(x, y, x + w, y + h, 8, 8);

    setlinecolor(COLOR_BORDER);
    roundrect(x, y, x + w, y + h, 8, 8);

    settextcolor(COLOR_TEXT);
    setbkmode(TRANSPARENT);
    int textW = textwidth(text.c_str());
    outtextxy(x + (w - textW) / 2, y + (h - textheight(text.c_str())) / 2, text.c_str());
}

// ========== 画标题 ==========
void EasyXRenderer::drawTitle(const std::string& title, int y) {
    settextcolor(COLOR_TITLE);
    setbkmode(TRANSPARENT);
    settextstyle(24, 0, _T("微软雅黑"));
    int w = textwidth(title.c_str());
    outtextxy((windowWidth - w) / 2, y, title.c_str());
    settextstyle(16, 0, _T("微软雅黑"));  // 恢复默认
}

// ========== 通用菜单选择 ==========
int EasyXRenderer::getMenuSelection(int maxOption, int startY, const std::vector<std::string>& options) {
    int sel = 0;
    int btnW = 260, btnH = 45, gap = 10;

    while (true) {
        cleardevice();

        // 标题
        drawTitle("校园 RPG 冒险游戏", 30);

        // 按钮
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            int bx = (windowWidth - btnW) / 2;
            int by = startY + i * (btnH + gap);
            drawButton(bx, by, btnW, btnH, options[i], i == sel);
        }

        // 提示
        settextcolor(COLOR_TEXT_DIM);
        setbkmode(TRANSPARENT);
        outtextxy((windowWidth - 200) / 2, windowHeight - 40, "↑↓ 选择  Enter 确认");

        FlushBatchDraw();

        int ch = _getch();
        if (ch == 224 || ch == 0) {  // 方向键
            ch = _getch();
            if (ch == 72) sel = (sel - 1 + maxOption) % maxOption;       // 上
            if (ch == 80) sel = (sel + 1) % maxOption;                   // 下
        } else if (ch == 13) {  // Enter
            return sel;
        }
    }
}

// ========== 主菜单 ==========
int EasyXRenderer::drawMainMenu(int /*selection*/) {
    BeginBatchDraw();

    std::vector<std::string> menuItems = {
        "查看角色信息",
        "打开背包",
        "进入商店",
        "查看任务",
        "开始战斗",
        "保存游戏",
        "读取存档",
        "退出游戏"
    };

    return getMenuSelection(static_cast<int>(menuItems.size()), 110, menuItems);
}

// ========== 角色面板 ==========
void EasyXRenderer::drawCharacterPanel(const Character& player) {
    cleardevice();
    BeginBatchDraw();

    drawTitle("角色信息", 25);

    // 面板背景
    int px = 80, py = 80, pw = windowWidth - 160, ph = 420;
    setfillcolor(COLOR_PANEL);
    solidroundrect(px, py, px + pw, py + ph, 12, 12);
    setlinecolor(COLOR_BORDER);
    roundrect(px, py, px + pw, py + ph, 12, 12);

    int leftX  = px + 40;
    int rightX = px + pw / 2 + 20;
    int rowH   = 42;
    int yPos   = py + 40;

    settextstyle(20, 0, _T("微软雅黑"));

    auto drawRow = [&](const std::string& label, const std::string& value, COLORREF valColor = COLOR_TEXT) {
        settextcolor(COLOR_TEXT_DIM);
        outtextxy(leftX, yPos, label.c_str());
        settextcolor(valColor);
        outtextxy(leftX + 120, yPos, value.c_str());
    };

    // 左列
    drawRow("名称：", player.getName(), RGB(255, 220, 100));
    yPos += rowH;
    drawRow("等级：", std::to_string(player.getLevel()));
    yPos += rowH;
    drawRow("攻击力：", std::to_string(player.getAttack()), RGB(255, 150, 100));
    yPos += rowH;
    drawRow("防御力：", std::to_string(player.getDefense()), RGB(100, 180, 255));

    // 右列
    yPos = py + 40;
    drawRow("金币：", std::to_string(player.getGold()), COLOR_GOLD);

    yPos += rowH + 10;
    settextcolor(COLOR_TEXT_DIM);
    outtextxy(rightX, yPos, "HP：");
    drawHPBar(rightX + 40, yPos + 5, 260, player.getHP(), player.getMaxHP());

    yPos += rowH + 10;
    settextcolor(COLOR_TEXT_DIM);
    outtextxy(rightX, yPos, "EXP：");

    // EXP 进度条
    {
        int expForLevel = 0;
        int lv = player.getLevel();
        if (lv <= 10)      expForLevel = lv * 100;
        else if (lv <= 20) expForLevel = lv * 150;
        else               expForLevel = lv * 200;

        int barW = 260, barH = 14;
        int bx = rightX + 40, by = yPos + 8;

        setfillcolor(COLOR_EXP_BAR_BG);
        solidrectangle(bx, by, bx + barW, by + barH);

        if (expForLevel > 0) {
            int fill = static_cast<int>((static_cast<double>(player.getExp()) / expForLevel) * barW);
            if (fill > barW) fill = barW;
            setfillcolor(COLOR_EXP_BAR);
            solidrectangle(bx, by, bx + fill, by + barH);
        }

        setlinecolor(COLOR_BORDER);
        rectangle(bx, by, bx + barW, by + barH);

        std::stringstream ss;
        ss << player.getExp() << "/" << expForLevel;
        settextcolor(COLOR_TEXT);
        setbkmode(TRANSPARENT);
        settextstyle(12, 0, _T("微软雅黑"));
        outtextxy(bx + barW / 2 - 25, by, ss.str().c_str());
        settextstyle(20, 0, _T("微软雅黑"));
    }

    // 底部提示
    settextcolor(COLOR_TEXT_DIM);
    settextstyle(16, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 160) / 2, windowHeight - 45, "按任意键返回主菜单...");

    FlushBatchDraw();
    _getch();
}

// ========== 战斗场景 ==========
void EasyXRenderer::drawBattleScene(const Character& player, const Enemy& enemy) {
    cleardevice();
    BeginBatchDraw();

    drawTitle("战斗！", 20);

    // ---- 玩家区域 ----
    {
        int px = 60, py = 280, pw = 350, ph = 280;
        setfillcolor(COLOR_PANEL);
        solidroundrect(px, py, px + pw, py + ph, 12, 12);
        setlinecolor(COLOR_BORDER);
        roundrect(px, py, px + pw, py + ph, 12, 12);

        settextcolor(COLOR_TITLE);
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy(px + 20, py + 15, player.getName().c_str());

        settextcolor(COLOR_TEXT);
        settextstyle(16, 0, _T("微软雅黑"));
        outtextxy(px + 20, py + 50, ("Lv." + std::to_string(player.getLevel())).c_str());
        outtextxy(px + 20, py + 75, ("ATK: " + std::to_string(player.getAttack())).c_str());
        outtextxy(px + 20, py + 100, ("DEF: " + std::to_string(player.getDefense())).c_str());

        settextcolor(COLOR_TEXT_DIM);
        outtextxy(px + 20, py + 135, "HP:");
        drawHPBar(px + 50, py + 137, 220, player.getHP(), player.getMaxHP());
    }

    // ---- 敌人区域 ----
    {
        int ex = windowWidth - 410, ey = 280, ew = 350, eh = 280;
        setfillcolor(COLOR_PANEL);
        solidroundrect(ex, ey, ex + ew, ey + eh, 12, 12);
        setlinecolor(COLOR_BORDER);
        roundrect(ex, ey, ex + ew, ey + eh, 12, 12);

        settextcolor(RGB(255, 100, 100));
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy(ex + 20, ey + 15, enemy.getName().c_str());

        settextcolor(COLOR_TEXT);
        settextstyle(16, 0, _T("微软雅黑"));
        outtextxy(ex + 20, ey + 50, ("ATK: " + std::to_string(enemy.getAttack())).c_str());
        outtextxy(ex + 20, ey + 75, ("DEF: " + std::to_string(enemy.getDefense())).c_str());

        settextcolor(COLOR_TEXT_DIM);
        outtextxy(ex + 20, ey + 135, "HP:");
        drawHPBar(ex + 50, ey + 137, 220, enemy.getHP(), enemy.getMaxHP());
    }

    // ---- VS 标记 ----
    settextcolor(RGB(255, 80, 80));
    settextstyle(28, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 40) / 2, 380, "VS");

    // ---- 操作按钮 ----
    int btnW = 200, btnH = 45;
    int btn1X = (windowWidth / 2) - btnW - 30;
    int btn2X = (windowWidth / 2) + 30;
    int btnY  = 590;

    drawButton(btn1X, btnY, btnW, btnH, "攻击 (A)", true);
    drawButton(btn2X, btnY, btnW, btnH, "逃跑 (R)", false);

    settextcolor(COLOR_TEXT_DIM);
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 200) / 2, windowHeight - 50, "A - 攻击  |  R - 逃跑");

    settextstyle(16, 0, _T("微软雅黑"));
    FlushBatchDraw();
}

// ========== 背包视图 ==========
int EasyXRenderer::drawInventory(const Inventory& inventory, int selection) {
    cleardevice();
    BeginBatchDraw();

    drawTitle("背包", 20);

    int itemCount = inventory.getItemCount();
    if (itemCount == 0) {
        settextcolor(COLOR_TEXT_DIM);
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 120) / 2, windowHeight / 2 - 20, "背包为空");
        settextstyle(16, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 160) / 2, windowHeight - 45, "按任意键返回主菜单...");
        FlushBatchDraw();
        _getch();
        return -1;
    }

    // 网格参数
    int cols = 4;
    int cardW = 180, cardH = 120;
    int gapX = 20, gapY = 20;
    int startX = (windowWidth - (cols * cardW + (cols - 1) * gapX)) / 2;
    int startY = 80;

    for (int i = 0; i < itemCount; ++i) {
        int col = i % cols;
        int row = i / cols;
        int cx = startX + col * (cardW + gapX);
        int cy = startY + row * (cardH + gapY);

        bool sel = (i == selection);

        // 卡片背景
        if (sel) {
            setfillcolor(COLOR_BUTTON_HOVER);
            setlinecolor(COLOR_TITLE);
        } else {
            setfillcolor(COLOR_PANEL);
            setlinecolor(COLOR_BORDER);
        }

        solidroundrect(cx, cy, cx + cardW, cy + cardH, 10, 10);
        roundrect(cx, cy, cx + cardW, cy + cardH, 10, 10);

        // 物品类型标签
        Item* item = inventory.getItem(i);
        if (item) {
            std::string typeStr = item->getType();
            COLORREF typeColor = COLOR_TEXT;
            if (typeStr == "Food")      typeColor = RGB(100, 220, 100);
            else if (typeStr == "Potion") typeColor = RGB(220, 100, 220);
            else if (typeStr == "Equipment") typeColor = RGB(255, 180, 60);

            settextcolor(typeColor);
            settextstyle(12, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 8, ("[" + typeStr + "]").c_str());

            // 物品名
            settextcolor(COLOR_TEXT);
            settextstyle(16, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 32, item->getName().c_str());

            // 描述
            settextcolor(COLOR_TEXT_DIM);
            settextstyle(12, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 60, item->getDescription().c_str());

            // 价格
            settextcolor(COLOR_GOLD);
            outtextxy(cx + 12, cy + 90, ("价格: " + std::to_string(item->getPrice()) + " 金币").c_str());
        }
    }

    // 底部提示
    settextcolor(COLOR_TEXT_DIM);
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 280) / 2, windowHeight - 45, "↑↓←→ 选择  Enter 使用  D 删除  ESC 返回");

    settextstyle(16, 0, _T("微软雅黑"));
    FlushBatchDraw();

    // 键盘交互
    while (true) {
        int ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();
            int newSel = selection;
            if (ch == 72) newSel -= 4;       // 上
            if (ch == 80) newSel += 4;       // 下
            if (ch == 75) newSel -= 1;       // 左
            if (ch == 77) newSel += 1;       // 右
            if (newSel >= 0 && newSel < itemCount) {
                return newSel;  // 返回新选择，让调用者重绘
            }
        } else if (ch == 13) {  // Enter
            return selection;   // 使用当前物品
        } else if (ch == 'd' || ch == 'D') {
            return -100 - selection;  // 删除信号
        } else if (ch == 27) {  // ESC
            return -1;
        }
    }
}

// ========== 商店视图 ==========
int EasyXRenderer::drawShop(const Shop& shop, int selection) {
    cleardevice();
    BeginBatchDraw();

    drawTitle("商店", 20);

    int itemCount = shop.getGoodsCount();
    if (itemCount == 0) {
        settextcolor(COLOR_TEXT_DIM);
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 120) / 2, windowHeight / 2 - 20, "商店暂无商品");
        settextstyle(16, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 160) / 2, windowHeight - 45, "按任意键返回主菜单...");
        FlushBatchDraw();
        _getch();
        return -1;
    }

    // 商品卡片网格
    int cols = 4;
    int cardW = 180, cardH = 130;
    int gapX = 20, gapY = 20;
    int startX = (windowWidth - (cols * cardW + (cols - 1) * gapX)) / 2;
    int startY = 70;

    for (int i = 0; i < itemCount; ++i) {
        int col = i % cols;
        int row = i / cols;
        int cx = startX + col * (cardW + gapX);
        int cy = startY + row * (cardH + gapY);

        bool sel = (i == selection);

        if (sel) {
            setfillcolor(COLOR_BUTTON_HOVER);
            setlinecolor(COLOR_TITLE);
        } else {
            setfillcolor(COLOR_PANEL);
            setlinecolor(COLOR_BORDER);
        }

        solidroundrect(cx, cy, cx + cardW, cy + cardH, 10, 10);
        roundrect(cx, cy, cx + cardW, cy + cardH, 10, 10);

        const std::unique_ptr<Item>& goods = shop.getGoods(i);
        if (goods) {
            std::string typeStr = goods->getType();
            COLORREF typeColor = COLOR_TEXT;
            if (typeStr == "Food")      typeColor = RGB(100, 220, 100);
            else if (typeStr == "Potion") typeColor = RGB(220, 100, 220);
            else if (typeStr == "Equipment") typeColor = RGB(255, 180, 60);

            settextcolor(typeColor);
            settextstyle(12, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 8, ("[" + typeStr + "]").c_str());

            settextcolor(COLOR_TEXT);
            settextstyle(16, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 30, goods->getName().c_str());

            settextcolor(COLOR_TEXT_DIM);
            settextstyle(12, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 58, goods->getDescription().c_str());

            settextcolor(COLOR_GOLD);
            settextstyle(16, 0, _T("微软雅黑"));
            outtextxy(cx + 12, cy + 90, (std::to_string(goods->getPrice()) + " 金币").c_str());
        }
    }

    settextcolor(COLOR_TEXT_DIM);
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 320) / 2, windowHeight - 45, "↑↓←→ 选择  Enter 购买  S 出售  ESC 返回");

    settextstyle(16, 0, _T("微软雅黑"));
    FlushBatchDraw();

    while (true) {
        int ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();
            int newSel = selection;
            if (ch == 72) newSel -= 4;
            if (ch == 80) newSel += 4;
            if (ch == 75) newSel -= 1;
            if (ch == 77) newSel += 1;
            if (newSel >= 0 && newSel < itemCount) return newSel;
        } else if (ch == 13) {  // Enter - 购买
            return selection;
        } else if (ch == 's' || ch == 'S') {  // 出售
            return -200;
        } else if (ch == 27) {
            return -1;
        }
    }
}

// ========== 任务列表 ==========
int EasyXRenderer::drawTaskList(const TaskSystem& taskSystem, int selection) {
    cleardevice();
    BeginBatchDraw();

    drawTitle("任务列表", 20);

    int count = taskSystem.getTaskCount();
    if (count == 0) {
        settextcolor(COLOR_TEXT_DIM);
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 120) / 2, windowHeight / 2 - 20, "暂无任务");
        settextstyle(16, 0, _T("微软雅黑"));
        outtextxy((windowWidth - 160) / 2, windowHeight - 45, "按任意键返回主菜单...");
        FlushBatchDraw();
        _getch();
        return -1;
    }

    int cardW = 700, cardH = 100;
    int startX = (windowWidth - cardW) / 2;
    int startY = 70;

    for (int i = 0; i < count; ++i) {
        int cy = startY + i * (cardH + 12);
        bool sel = (i == selection);

        if (sel) {
            setfillcolor(COLOR_BUTTON_HOVER);
            setlinecolor(COLOR_TITLE);
        } else {
            setfillcolor(COLOR_PANEL);
            setlinecolor(COLOR_BORDER);
        }

        solidroundrect(startX, cy, startX + cardW, cy + cardH, 10, 10);
        roundrect(startX, cy, startX + cardW, cy + cardH, 10, 10);

        Task* t = taskSystem.getTask(i);
        if (t) {
            // 状态标签
            std::string statusStr;
            COLORREF statusColor;
            switch (t->getStatus()) {
                case TaskStatus::NotAccepted:   statusStr = "[未接受]"; statusColor = COLOR_TEXT_DIM; break;
                case TaskStatus::InProgress:    statusStr = "[进行中]"; statusColor = RGB(100, 200, 255); break;
                case TaskStatus::Completed:     statusStr = "[已完成]"; statusColor = RGB(100, 255, 100); break;
                case TaskStatus::RewardClaimed: statusStr = "[已领奖]"; statusColor = RGB(255, 200, 100); break;
            }

            settextcolor(statusColor);
            settextstyle(14, 0, _T("微软雅黑"));
            outtextxy(startX + 520, cy + 10, statusStr.c_str());

            settextcolor(COLOR_TEXT);
            settextstyle(18, 0, _T("微软雅黑"));
            outtextxy(startX + 15, cy + 10, t->getName().c_str());

            settextcolor(COLOR_TEXT_DIM);
            settextstyle(14, 0, _T("微软雅黑"));
            outtextxy(startX + 15, cy + 38, ("描述: " + t->getDescription()).c_str());
            outtextxy(startX + 15, cy + 60, ("条件: " + t->getCondition()).c_str());

            std::stringstream rewardSS;
            rewardSS << "奖励: " << t->getExpReward() << " EXP + " << t->getGoldReward() << " 金币";
            outtextxy(startX + 15, cy + 78, rewardSS.str().c_str());
        }
    }

    settextcolor(COLOR_TEXT_DIM);
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 360) / 2, windowHeight - 45, "↑↓ 选择  Enter 操作  A 接受  C 完成  R 领奖  ESC 返回");

    settextstyle(16, 0, _T("微软雅黑"));
    FlushBatchDraw();

    while (true) {
        int ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();
            int newSel = selection;
            if (ch == 72) newSel = (selection - 1 + count) % count;
            if (ch == 80) newSel = (selection + 1) % count;
            if (newSel != selection) return newSel;
        } else if (ch == 'a' || ch == 'A') {
            return -300;  // 接受任务
        } else if (ch == 'c' || ch == 'C') {
            return -400;  // 完成任务
        } else if (ch == 'r' || ch == 'R') {
            return -500;  // 领奖
        } else if (ch == 13) {
            return selection;
        } else if (ch == 27) {
            return -1;
        }
    }
}

// ========== 消息弹窗 ==========
void EasyXRenderer::showMessage(const std::string& msg) {
    // 半透明遮罩
    setfillcolor(RGB(0, 0, 0));
    solidrectangle(0, 0, windowWidth, windowHeight);

    // 弹窗
    int pw = 500, ph = 120;
    int px = (windowWidth - pw) / 2;
    int py = (windowHeight - ph) / 2;

    setfillcolor(COLOR_PANEL);
    setlinecolor(COLOR_TITLE);
    solidroundrect(px, py, px + pw, py + ph, 12, 12);
    roundrect(px, py, px + pw, py + ph, 12, 12);

    settextcolor(COLOR_TEXT);
    settextstyle(18, 0, _T("微软雅黑"));
    int tw = textwidth(msg.c_str());
    outtextxy(px + (pw - tw) / 2, py + 35, msg.c_str());

    settextcolor(COLOR_TEXT_DIM);
    settextstyle(14, 0, _T("微软雅黑"));
    outtextxy((windowWidth - 150) / 2, py + 75, "按任意键继续...");

    settextstyle(16, 0, _T("微软雅黑"));
    FlushBatchDraw();
    _getch();
}
