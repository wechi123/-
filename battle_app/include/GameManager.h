#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "IGameUI.h"
#include "Character.h"
#include "Enemy.h"
#include "BattleSystem.h"
#include "Inventory.h"
#include "Shop.h"
#include "TaskSystem.h"
#include "Database.h"
#include <vector>
#include <memory>

class GameManager {
private:
    IGameUI& ui;
    Character player;
    std::vector<std::unique_ptr<Enemy>> enemyPool;
    BattleSystem battleSystem;
    Inventory inventory;
    Shop shop;
    TaskSystem taskSystem;
    Database* db;
    bool running;
    bool hasSavedData;

public:
    GameManager(IGameUI& uiRef);
    ~GameManager();

    void init();
    void run();
    void cleanup();

private:
    void showMainMenu();
    void handleChoice(int choice);

    void menuShowInfo();
    void menuInventory();
    void menuShop();
    void menuTasks();
    void menuBattle();
    void menuSave();
    void menuLoad();

    void showWelcomeScreen();
    void initDemoItems();

    // 辅助函数
    void syncGold();
};

#endif
