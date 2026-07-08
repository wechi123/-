/*
 * ============================================================
 * 校园RPG游戏系統 - Item/Inventory/Shop/SQLite 模墆測試
 *
 * 負責模組（本同學）：
 *   1. Item 物品類體系 (Food/Medicine/Equipment 繼承+多態)
 *   2. Inventory 背包管理
 *   3. Shop 商店系統
 *   4. 挑戰一：SQLite 数据库保存
 *
 * 編讯：
 *   g++ -std=c++17 -I include -I lib src/*.cpp main.cpp lib/sqlite3.c -o CampusRPG
 * ============================================================
 */

#include <iostream>
#include <string>
#include <limits>
#include "Item.h"
#include "Inventory.h"
#include "Shop.h"
#include "Database.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    std::cout << "\n按 Enter 鍵繼續...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "【错误】請輸入有效的數字！\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

void showMainMenu() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  校园RPG游戏 - 物品與商店模墆\n";
    std::cout << "========================================\n";
    std::cout << "  1. 查看角色状态\n";
    std::cout << "  2. 查看背包\n";
    std::cout << "  3. 使用物品\n";
    std::cout << "  4. 删除物品\n";
    std::cout << "  5. 進入商店\n";
    std::cout << "  6. 保存游戏（SQLite数据库）\n";
    std::cout << "  7. 加载游戏（SQLite数据库）\n";
    std::cout << "  8. 获得物品（演示多態）\n";
    std::cout << "  0. 退出游戏\n";
    std::cout << "========================================\n";
}

void shopMenu(Shop& shop, Inventory& inventory) {
    while (true) {
        clearScreen();
        shop.showShop();
        std::cout << "\n你的金币：" << inventory.getOwnerGold() << "\n";
        std::cout << "\n--- 商店操作 ---\n";
        std::cout << "1. 购买商品\n";
        std::cout << "2. 出售物品\n";
        std::cout << "0. 離開商店\n";
        int choice = readInt("請選擇：");
        switch (choice) {
            case 1: {
                int idx = readInt("請輸入要购买的商品編號：");
                shop.buyItem(idx, inventory); pauseScreen(); break;
            }
            case 2: {
                inventory.showInventory();
                int idx = readInt("請輸入要出售的物品編號：");
                shop.sellItem(idx, inventory); pauseScreen(); break;
            }
            case 0: return;
            default: std::cout << "【错误】無效選項！\n"; pauseScreen();
        }
    }
}

void addDemoItems(Inventory& inventory) {
    std::cout << "\n===== 获得物品（演示多態） =====\n";
    inventory.addItemOrStack(new Food(1001, "面包", "鬆軟的白面包", 5, 20));
    inventory.addItemOrStack(new Food(1002, "雞腿", "香噴噴的烤雞腿", 15, 50));
    inventory.addItemOrStack(new Food(1003, "蛋糕", "精美的奶油蛋糕", 30, 100));
    inventory.addItemOrStack(new Medicine(2001, "紅藥水", "恢复100點HP", 20, 100, ""));
    inventory.addItemOrStack(new Medicine(2002, "解毒劑", "解除中毒+恢复HP", 25, 30, "中毒"));
    inventory.addItemOrStack(new Equipment(3001, "木劍", "新手武器", 30, 5, 0, EquipmentSlot::WEAPON));
    inventory.addItemOrStack(new Equipment(3003, "布甲", "基礎防具", 25, 0, 5, EquipmentSlot::ARMOR));
    std::cout << "===== 測試物品已添加 =====\n";
}

int main() {
    Character player{"小明", 100, 100, 10, 5};
    Inventory inventory(30);
    inventory.setOwnerGold(player.getGold());

    Shop shop("校园商店");
    shop.initDefaultShop();

    Database db("campus_rpg.db");

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  歡迎來到校园RPG游戏！\n";
    std::cout << "  本模墆演示：Item/Inventory/Shop/SQLite\n";
    std::cout << "========================================\n";
    pauseScreen();

    bool running = true;
    while (running) {
        clearScreen();
        showMainMenu();
        int choice = readInt("請選擇操作：");

        switch (choice) {
            case 1: player.showStatus(); pauseScreen(); break;
            case 2: inventory.showInventory(); pauseScreen(); break;
            case 3:
                inventory.showInventory();
                if (!inventory.isEmpty()) {
                    int idx = readInt("請輸入要使用的物品編號：");
                    Item* item = inventory.getItem(idx);
                    if (item && item->getType() == ItemType::EQUIPMENT) {
                        Equipment* eq = dynamic_cast<Equipment*>(item);
                        if (eq && eq->isEquipped()) {
                            std::cout << "【提示】該装备已装备，輸入1卸下，其他取消：";
                            if (readInt("") == 1) eq->unequip(player);
                        } else { inventory.useItem(idx, player); }
                    } else { inventory.useItem(idx, player); }
                }
                pauseScreen(); break;
            case 4:
                inventory.showInventory();
                if (!inventory.isEmpty()) {
                    int idx = readInt("請輸入要刪除的物品編號：");
                    Item* item = inventory.getItem(idx);
                    if (item && item->getType() == ItemType::EQUIPMENT) {
                        Equipment* eq = dynamic_cast<Equipment*>(item);
                        if (eq && eq->isEquipped()) eq->unequip(player);
                    }
                    inventory.removeItem(idx);
                }
                pauseScreen(); break;
            case 5:
                inventory.setOwnerGold(player.getGold());
                shopMenu(shop, inventory);
                player.addGold(inventory.getOwnerGold() - player.getGold());
                break;
            case 6:
                if (!db.isConnected()) db.connect();
                if (db.isConnected()) {
                    inventory.setOwnerGold(player.getGold());
                    db.saveInventory(inventory); db.saveShop(shop);
                    std::cout << "\n===== 游戏已保存到 SQLite 数据库 =====\n";
                }
                pauseScreen(); break;
            case 7:
                if (!db.isConnected()) db.connect();
                if (db.isConnected()) {
                    while (!inventory.isEmpty()) inventory.removeItem(0);
                    db.loadInventory(inventory);
                    player.addGold(inventory.getOwnerGold() - player.getGold());
                    std::cout << "\n===== 游戏已從 SQLite 数据库加载 =====\n";
                }
                pauseScreen(); break;
            case 8: addDemoItems(inventory); pauseScreen(); break;
            case 0:
                std::cout << "\n否初保存游戏後再退出？(1=保存, 0=直接退出): ";
                if (readInt("") == 1) {
                    if (!db.isConnected()) db.connect();
                    if (db.isConnected()) {
                        inventory.setOwnerGold(player.getGold());
                        db.saveInventory(inventory); db.saveShop(shop);
                        std::cout << "游戏已保存。\n";
                    }
                }
                std::cout << "感謝遊玩，再見！\n";
                running = false; break;
            default:
                std::cout << "【错误】無效選項，請重新選擇！\n";
                pauseScreen();
        }
    }
    return 0;
}
