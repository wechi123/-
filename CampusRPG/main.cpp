/*
 * ============================================================
 * 校园RPG冒险游戏系统
 * 本文件演示：Item物品体系 / Inventory背包 / Shop商店 / SQLite数据库
 *
 * 负责同学的分工模块：
 *   1. Item 物品类体系（Food/Medicine/Equipment 继承+多态）
 *   2. Inventory 背包管理
 *   3. Shop 商店系统
 *   4. 挑战一：SQLite 数据持久化
 *
 * 编译方式：
 *   g++ -std=c++17 -I include -I lib src/*.cpp main.cpp lib/sqlite3.c -o CampusRPG
 * ============================================================
 */

#include <iostream>
#include <string>
#include <limits>
#include "Character.h"
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
    std::cout << "\n按 Enter 键继续...";
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
            std::cout << "【错误】请输入有效的数字！\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

void showMainMenu() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  校园RPG冒险游戏 - 物品与商店模块\n";
    std::cout << "========================================\n";
    std::cout << "  1. 查看角色状态\n";
    std::cout << "  2. 查看背包\n";
    std::cout << "  3. 使用物品\n";
    std::cout << "  4. 删除物品\n";
    std::cout << "  5. 进入商店\n";
    std::cout << "  6. 保存游戏（SQLite数据库）\n";
    std::cout << "  7. 加载游戏（SQLite数据库）\n";
    std::cout << "  8. 获得测试物品（演示多态）\n";
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
        std::cout << "0. 离开商店\n";
        int choice = readInt("请选择：");
        switch (choice) {
            case 1: {
                int idx = readInt("请输入要购买的商品编号：");
                shop.buyItem(idx, inventory);
                pauseScreen();
                break;
            }
            case 2: {
                inventory.showInventory();
                int idx = readInt("请输入要出售的物品编号：");
                shop.sellItem(idx, inventory);
                pauseScreen();
                break;
            }
            case 0: return;
            default:
                std::cout << "【错误】无效选项！\n";
                pauseScreen();
        }
    }
}

void addDemoItems(Inventory& inventory) {
    std::cout << "\n===== 获得测试物品（演示多态） =====\n";
    inventory.addItemOrStack(new Food(1001, "面包", "松软的白面包", 5, 20));
    inventory.addItemOrStack(new Food(1002, "鸡腿", "香喷喷的烤鸡腿", 15, 50));
    inventory.addItemOrStack(new Food(1003, "蛋糕", "精美的奶油蛋糕", 30, 100));
    inventory.addItemOrStack(new Medicine(2001, "红药水", "恢复100点HP", 20, 100, ""));
    inventory.addItemOrStack(new Medicine(2002, "解毒剂", "解除中毒+恢复HP", 25, 30, "中毒"));
    inventory.addItemOrStack(new Equipment(3001, "木剑", "新手武器", 30, 5, 0, EquipmentSlot::WEAPON));
    inventory.addItemOrStack(new Equipment(3003, "布甲", "基础防具", 25, 0, 5, EquipmentSlot::ARMOR));
    std::cout << "===== 测试物品已添加 =====\n";
}

int main() {
    Character player("小明", 1, 100, 10, 5);
    Inventory inventory(30);
    inventory.setOwnerGold(player.getGold());

    Shop shop("校园商店");
    shop.initDefaultShop();

    Database db("campus_rpg.db");

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  欢迎来到校园RPG冒险游戏！\n";
    std::cout << "  本模块演示：Item/Inventory/Shop/SQLite\n";
    std::cout << "========================================\n";
    pauseScreen();

    bool running = true;
    while (running) {
        clearScreen();
        showMainMenu();
        int choice = readInt("请选择操作：");

        switch (choice) {
            case 1: player.showStatus(); pauseScreen(); break;
            case 2: inventory.showInventory(); pauseScreen(); break;
            case 3:
                inventory.showInventory();
                if (!inventory.isEmpty()) {
                    int idx = readInt("请输入要使用的物品编号：");
                    Item* item = inventory.getItem(idx);
                    if (item && item->getType() == ItemType::EQUIPMENT) {
                        Equipment* eq = dynamic_cast<Equipment*>(item);
                        if (eq && eq->isEquipped()) {
                            std::cout << "【提示】该装备已装备，输入1卸下，其他取消：";
                            if (readInt("") == 1) eq->unequip(player);
                        } else { inventory.useItem(idx, player); }
                    } else { inventory.useItem(idx, player); }
                }
                pauseScreen(); break;
            case 4:
                inventory.showInventory();
                if (!inventory.isEmpty()) {
                    int idx = readInt("请输入要删除的物品编号：");
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
                    std::cout << "\n===== 游戏已从 SQLite 数据库加载 =====\n";
                }
                pauseScreen(); break;
            case 8: addDemoItems(inventory); pauseScreen(); break;
            case 0:
                std::cout << "\n是否保存游戏后再退出？(1=保存, 0=直接退出): ";
                if (readInt("") == 1) {
                    if (!db.isConnected()) db.connect();
                    if (db.isConnected()) {
                        inventory.setOwnerGold(player.getGold());
                        db.saveInventory(inventory); db.saveShop(shop);
                        std::cout << "游戏已保存。\n";
                    }
                }
                std::cout << "感谢游玩，再见！\n";
                running = false;
                break;
            default:
                std::cout << "【错误】无效选项，请重新选择！\n";
                pauseScreen();
        }
    }
    return 0;
}
