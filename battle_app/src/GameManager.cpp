#include "GameManager.h"
#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>

GameManager::GameManager(IGameUI& uiRef)
    : ui(uiRef), battleSystem(uiRef), inventory(30),
      shop("校园商店"), db(nullptr), running(false), hasSavedData(false)
{
}

GameManager::~GameManager() {
    if (db) { db->disconnect(); delete db; db = nullptr; }
}

void GameManager::init() {
    ui.clearScreen();
    showWelcomeScreen();

    enemyPool = Enemy::createEnemyPool();
    shop.initDefaultShop();
    initDemoItems();

    // 加载任务
    taskSystem.loadTasks("battle_app/data/tasks.txt");

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

    syncGold();
}

void GameManager::initDemoItems() {
    inventory.addItemOrStack(new Food(1001, "面包", "松软的白面包，恢复少量体力", 5, 20));
    inventory.addItemOrStack(new Food(1002, "鸡腿", "香喷喷的烤鸡腿，恢复较多体力", 15, 50));
    inventory.addItemOrStack(new Food(1003, "蛋糕", "精美的奶油蛋糕，恢复大量体力", 30, 100));
    inventory.addItemOrStack(new Medicine(2001, "红药水", "恢复100点HP的基础药水", 20, 100, ""));
    inventory.addItemOrStack(new Medicine(2002, "解毒剂", "解除中毒状态，并恢复30点HP", 25, 30, "中毒"));
}

void GameManager::syncGold() {
    inventory.setOwnerGold(player.getGold());
}

void GameManager::run() {
    running = true;
    while (running) {
        ui.clearScreen();
        showMainMenu();
        int choice = ui.readChoice(1, 8);
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

// ============================================================
// 主菜单 — 8 个选项，覆盖全部 6 大模块
// ============================================================
void GameManager::showMainMenu() {
    std::vector<std::string> options = {
        "查看角色信息",
        "背包管理（物品）",
        "进入商店（交易）",
        "任务系统",
        "挑战敌人（战斗）",
        "保存游戏",
        "读取存档",
        "退出游戏"
    };
    ui.drawMenu("主菜单", options, 48);
}

void GameManager::handleChoice(int choice) {
    switch (choice) {
        case 1: menuShowInfo(); break;
        case 2: menuInventory(); break;
        case 3: menuShop();      break;
        case 4: menuTasks();     break;
        case 5: menuBattle();    break;
        case 6: menuSave();      break;
        case 7: menuLoad();      break;
        case 8: running = false; break;
    }
}

// ============================================================
// 1. 角色管理模块
// ============================================================
void GameManager::menuShowInfo() {
    ui.clearScreen();
    ui.drawInfoBox("角色信息", player.getInfoLines(), 48);
    ui.pause();
}

// ============================================================
// 2. 背包管理模块
// ============================================================
void GameManager::menuInventory() {
    while (true) {
        ui.clearScreen();
        inventory.showInventory();

        std::vector<std::string> options = {
            "使用物品",
            "删除物品",
            "返回主菜单"
        };
        ui.drawMenu("背包操作", options, 48);
        int choice = ui.readChoice(1, 3);

        if (choice == 1) {
            if (inventory.isEmpty()) {
                ui.showMessage("背包是空的！");
                ui.pause();
                continue;
            }
            int idx = ui.readInt("请输入要使用的物品编号：");
            if (idx >= 1 && idx <= inventory.getItemCount()) {
                Item* item = inventory.getItem(idx - 1);
                if (item && item->getType() == ItemType::EQUIPMENT) {
                    Equipment* eq = dynamic_cast<Equipment*>(item);
                    if (eq && eq->isEquipped()) {
                        if (ui.confirm("该装备已装备，是否卸下？")) {
                            eq->unequip(player);
                        }
                    } else {
                        inventory.useItem(idx - 1, player);
                    }
                } else {
                    inventory.useItem(idx - 1, player);
                }
                ui.pause();
            } else {
                ui.showError("无效的物品编号！");
                ui.pause();
            }
        } else if (choice == 2) {
            if (inventory.isEmpty()) {
                ui.showMessage("背包是空的！");
                ui.pause();
                continue;
            }
            int idx = ui.readInt("请输入要删除的物品编号：");
            if (idx >= 1 && idx <= inventory.getItemCount()) {
                Item* item = inventory.getItem(idx - 1);
                if (item && item->getType() == ItemType::EQUIPMENT) {
                    Equipment* eq = dynamic_cast<Equipment*>(item);
                    if (eq && eq->isEquipped()) eq->unequip(player);
                }
                inventory.removeItem(idx - 1);
                ui.showMessage("物品已删除！");
            } else {
                ui.showError("无效的物品编号！");
            }
            ui.pause();
        } else {
            break;
        }
    }
}

// ============================================================
// 3. 商店系统模块
// ============================================================
void GameManager::menuShop() {
    while (true) {
        ui.clearScreen();
        shop.showShop();
        syncGold();
        ui.println("你的金币：" + std::to_string(player.getGold()));
        ui.drawLine('-', 48);

        std::vector<std::string> options = {
            "购买物品",
            "出售物品",
            "返回主菜单"
        };
        ui.drawMenu("商店操作", options, 48);
        int choice = ui.readChoice(1, 3);

        if (choice == 1) {
            int idx = ui.readInt("请输入要购买的物品编号：");
            if (idx >= 1 && idx <= shop.getItemCount()) {
                Item* shopItem = shop.getItem(idx - 1);
                if (shopItem && player.getGold() < shopItem->getPrice()) {
                    ui.showError("金币不足！");
                } else if (inventory.isFull()) {
                    ui.showError("背包已满！");
                } else {
                    syncGold();
                    shop.buyItem(idx, inventory);
                    int newGold = inventory.getOwnerGold();
                    int diff = newGold - player.getGold();
                    if (diff < 0) player.costGold(-diff);
                    if (diff > 0) player.gainGold(diff);
                    syncGold();
                }
            } else {
                ui.showError("无效的商品编号！");
            }
            ui.pause();
        } else if (choice == 2) {
            if (inventory.isEmpty()) {
                ui.showMessage("背包是空的，没有可出售的物品！");
                ui.pause();
                continue;
            }
            inventory.showInventory();
            int idx = ui.readInt("请输入要出售的物品编号：");
            if (idx >= 1 && idx <= inventory.getItemCount()) {
                Item* invItem = inventory.getItem(idx - 1);
                if (invItem && invItem->getType() == ItemType::EQUIPMENT) {
                    Equipment* eq = dynamic_cast<Equipment*>(invItem);
                    if (eq && eq->isEquipped()) eq->unequip(player);
                }
                shop.sellItem(idx, inventory);
                int newGold = inventory.getOwnerGold();
                int diff = newGold - player.getGold();
                if (diff < 0) player.costGold(-diff);
                if (diff > 0) player.gainGold(diff);
                syncGold();
            } else {
                ui.showError("无效的物品编号！");
            }
            ui.pause();
        } else {
            syncGold();
            break;
        }
    }
}

// ============================================================
// 4. 任务系统模块
// ============================================================
void GameManager::menuTasks() {
    while (true) {
        ui.clearScreen();
        taskSystem.showTasks();

        std::vector<std::string> options = {
            "接受任务",
            "完成任务",
            "领取奖励",
            "返回主菜单"
        };
        ui.drawMenu("任务操作", options, 48);
        int choice = ui.readChoice(1, 4);

        if (choice == 1) {
            int idx = ui.readInt("请输入要接受的任务编号：");
            taskSystem.acceptTask(idx - 1);
            ui.pause();
        } else if (choice == 2) {
            int idx = ui.readInt("请输入要完成的任务编号：");
            taskSystem.completeTask(idx - 1);
            ui.pause();
        } else if (choice == 3) {
            int idx = ui.readInt("请输入要领奖的任务编号：");
            taskSystem.claimReward(idx - 1, player);
            ui.pause();
        } else {
            break;
        }
    }
}

// ============================================================
// 5. 战斗系统模块
// ============================================================
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

// ============================================================
// 6. 存档与读取
// ============================================================
void GameManager::menuSave() {
    ui.clearScreen();
    ui.drawTitle("保存游戏", 48);

    syncGold();

    std::ofstream file(GameConfig::PLAYER_SAVE_FILE);
    if (file.is_open()) {
        file << player.serialize() << "\n";
        file << inventory.serialize() << "\n";
        file << taskSystem.serialize();
        file.close();
        hasSavedData = true;
        ui.showMessage("游戏保存成功！（角色 + 背包 + 任务）");
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
        std::string playerData, invData, taskData;
        std::getline(file, playerData);
        std::getline(file, invData);
        std::getline(file, taskData);
        file.close();

        if (!playerData.empty()) {
            player.deserialize(playerData);
            if (!invData.empty()) inventory.deserialize(invData);
            if (!taskData.empty()) taskSystem.deserialize(taskData);
            syncGold();
            ui.showMessage("存档读取成功！欢迎回来，" + player.getName() + "！");
            ui.pause();
            return;
        }
    }
    ui.showError("未找到存档文件或存档已损坏！");
    ui.pause();
}
