#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "Item.h"
#include "Inventory.h"
#include "Shop.h"

// ============================================================
// Database ?????? - ????SQLite
// ?? SQLite3 C API ???????
// ?????/??????????????
// ============================================================

struct sqlite3;   // ????????????

class Database {
private:
    sqlite3*    m_db;
    std::string m_dbPath;
    bool        m_connected;

    bool executeSQL(const std::string& sql);
    bool createTables();
    static std::string itemTypeToString(ItemType type);
    static ItemType stringToItemType(const std::string& str);

public:
    Database(const std::string& dbPath = "campus_rpg.db");
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool connect();
    void disconnect();
    bool isConnected() const { return m_connected; }

    bool saveInventory(const Inventory& inventory, int playerId = 1);
    bool loadInventory(Inventory& inventory, int playerId = 1);
    bool saveShop(const Shop& shop);
    bool loadShop(Shop& shop);
    bool savePlayerGold(int playerId, int gold);
    int  loadPlayerGold(int playerId = 1);
    bool clearInventory(int playerId = 1);
    bool clearShop();
};

#endif
