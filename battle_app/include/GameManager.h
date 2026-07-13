#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "IGameUI.h"
#include "Character.h"
#include "Enemy.h"
#include "BattleSystem.h"
#include <vector>
#include <memory>

class GameManager {
private:
    IGameUI& ui;
    Character player;
    std::vector<std::unique_ptr<Enemy>> enemyPool;
    BattleSystem battleSystem;
    bool running;
    bool hasSavedData;

public:
    GameManager(IGameUI& uiRef);

    void init();
    void run();
    void cleanup();

private:
    void showMainMenu();
    void handleChoice(int choice);

    void menuShowInfo();
    void menuBattle();
    void menuSave();
    void menuLoad();

    void showWelcomeScreen();
    void showPlayerInfo();
};

#endif
