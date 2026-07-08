#pragma once

#include <string>
#include <vector>
#include <graphics.h>    // EasyX 图形库

// 前向声明
class Character;
class Enemy;
class Inventory;
class Shop;
class TaskSystem;

// EasyXRenderer：图形界面渲染器（挑战三）
// 负责所有可视化界面：主菜单、角色面板、战斗场景、背包、商店、任务列表
class EasyXRenderer {
private:
    int windowWidth;    // 窗口宽度
    int windowHeight;   // 窗口高度

    // ---- 内部绘制工具 ----
    void drawHPBar(int x, int y, int barWidth, int current, int max);
    void drawButton(int x, int y, int w, int h, const std::string& text, bool selected);
    void drawTitle(const std::string& title, int y);
    int  getMenuSelection(int maxOption, int startY, const std::vector<std::string>& options);

public:
    EasyXRenderer();
    ~EasyXRenderer();

    // ---- 窗口生命周期 ----
    void initWindow(int width, int height, const std::string& title);
    void closeWindow();

    // ---- 界面渲染 ----
    int  drawMainMenu(int selection);
    void drawCharacterPanel(const Character& player);
    void drawBattleScene(const Character& player, const Enemy& enemy);
    int  drawInventory(const Inventory& inventory, int selection);
    int  drawShop(const Shop& shop, int selection);
    int  drawTaskList(const TaskSystem& taskSystem, int selection);
    void showMessage(const std::string& msg);
};
