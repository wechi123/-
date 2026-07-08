#include "GameManager.h"
#include "GameConfig.h"
#include <fstream>

GameManager::GameManager(IGameUI& uiRef)
    : ui(uiRef), battleSystem(uiRef), running(false), hasSavedData(false)
{
}

void GameManager::init() {
    ui.clearScreen();
    showWelcomeScreen();

    enemyPool = Enemy::createEnemyPool();

    if (std::ifstream(GameConfig::PLAYER_SAVE_FILE).good()) {
        hasSavedData = true;
    }

    if (hasSavedData && ui.confirm("检测到存档文件，是否读取？")) {
        menuLoad();
    } else {
        ui.clearScreen();
        ui.drawTitle("创建新角色", 48);
        std::string name = ui.readString("  请输入角色名称：");
        if (name.empty()) name = "无名勇者";
        player.create(name);
        ui.showMessage("角色创建成功！欢迎，" + player.getName() + "！");
        ui.pause();
    }
}

void GameManager::run() {
    running = true;
    while (running) {
        ui.clearScreen();
        showMainMenu();
        int choice = ui.readChoice(1, 5);
        handleChoice(choice);
    }
}

void GameManager::cleanup() {
    if (ui.confirm("退出前是否保存游戏？")) {
        menuSave();
    }
    ui.clearScreen();
    ui.drawTitle("感谢游玩！", 48);
    ui.showMessage("再见，" + player.getName() + "！");
    ui.drawLine('=', 48);
    ui.println("  校园RPG冒险游戏 - 瓦香小酥肉组");
    ui.drawLine('=', 48);
}

void GameManager::showWelcomeScreen() {
    ui.drawLine('=', 52);
    ui.println();
    ui.printlnColored("      欢迎来到 校园 RPG 冒险游戏系统", UIColor::CYAN);
    ui.println();
    ui.drawLine('=', 52);
    ui.println("                  瓦香小酥肉 组");
    ui.println("        组长：张佳怡 | 孙赫研 | 卢宇涵 | 何雨婷");
    ui.drawLine('=', 52);
    ui.println();
}

void GameManager::showMainMenu() {
    std::vector<std::string> options = {
        "查看角色信息",
        "挑战敌人",
        "保存游戏",
        "读取存档",
        "退出游戏"
    };
    ui.drawMenu("主菜单", options, 48);
}

void GameManager::handleChoice(int choice) {
    switch (choice) {
        case 1: showPlayerInfo(); break;
        case 2: menuBattle();    break;
        case 3: menuSave();      break;
        case 4: menuLoad();      break;
        case 5: running = false;  break;
    }
}

void GameManager::showPlayerInfo() {
    ui.clearScreen();
    ui.drawInfoBox("角色信息", player.getInfoLines(), 48);
    ui.pause();
}

void GameManager::menuBattle() {
    ui.clearScreen();
    if (!player.isAlive()) {
        ui.showError("你已经阵亡了！HP已恢复为1，请先去治疗。");
        player.setHp(1);
        ui.pause();
        return;
    }
    battleSystem.startBattle(player, enemyPool);
}

void GameManager::menuSave() {
    ui.clearScreen();
    ui.drawTitle("保存游戏", 48);

    std::ofstream file(GameConfig::PLAYER_SAVE_FILE);
    if (file.is_open()) {
        file << player.serialize();
        file.close();
        hasSavedData = true;
        ui.showMessage("游戏保存成功！");
    } else {
        ui.showError("存档文件写入失败！请检查 data/ 目录是否存在。");
    }
    ui.pause();
}

void GameManager::menuLoad() {
    ui.clearScreen();
    ui.drawTitle("读取存档", 48);

    std::ifstream file(GameConfig::PLAYER_SAVE_FILE);
    if (file.is_open()) {
        std::string data;
        std::getline(file, data);
        file.close();
        if (!data.empty()) {
            player.deserialize(data);
            ui.showMessage("存档读取成功！欢迎回来，" + player.getName() + "！");
            ui.pause();
            return;
        }
    }
    ui.showError("未找到存档文件或存档已损坏！");
    ui.pause();
}
