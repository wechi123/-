#include "Database.h"
#include "Food.h"
#include "Medicine.h"
#include "Equipment.h"
#include <iostream>
#include <sstream>
#include "../lib/sqlite3.h"

// ============================================================
// Database 类实现 - SQLite 数据持久化（挑战一）
// 表结构：
//   player_gold (player_id, gold)
//   inventory   (id, player_id, item_id, item_name, item_type, price, quantity, extra_data)
//   shop_items  (id, item_id, item_name, item_type, price, extra_data)
// extra_data 按物品类型用 | 分隔存储特有属性：
//   FOOD:      hpRestore
//   MEDICINE:  hpRestore|cureEffect
//   EQUIPMENT: attackBonus|defenseBonus|slotInt
// ============================================================

Database::Database(const std::string& dbPath)
    : m_db(nullptr), m_dbPath(dbPath), m_connected(false) {}

Database::~Database() { disconnect(); }

bool Database::connect() {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "【数据库错误】无法打开数据库：" << sqlite3_errmsg(m_db) << "\n";
        m_connected = false; return false;
    }
    m_connected = true;
    std::cout << "【数据库】已连接：" << m_dbPath << "\n";
    if (!createTables()) {
        std::cerr << "【数据库错误】创建数据表失败！\n";
        return false;
    }
    return true;
}

void Database::disconnect() {
    if (m_db) { sqlite3_close(m_db); m_db = nullptr; m_connected = false; }
}

bool Database::executeSQL(const std::string& sql) {
    if (!m_connected || !m_db) return false;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "【SQL错误】" << errMsg << "\n";
        sqlite3_free(errMsg); return false;
    }
    return true;
}

bool Database::createTables() {
    const char* sql = R"SQL(
        CREATE TABLE IF NOT EXISTS player_gold (
            player_id   INTEGER PRIMARY KEY,
            gold        INTEGER NOT NULL DEFAULT 0
        );
        CREATE TABLE IF NOT EXISTS inventory (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            player_id   INTEGER NOT NULL,
            item_id     INTEGER NOT NULL,
            item_name   TEXT NOT NULL,
            item_desc   TEXT,
            item_type   TEXT NOT NULL,
            price       INTEGER NOT NULL,
            quantity    INTEGER NOT NULL,
            extra_data  TEXT,
            FOREIGN KEY (player_id) REFERENCES player_gold(player_id)
        );
        CREATE TABLE IF NOT EXISTS shop_items (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            item_id     INTEGER NOT NULL UNIQUE,
            item_name   TEXT NOT NULL,
            item_desc   TEXT,
            item_type   TEXT NOT NULL,
            price       INTEGER NOT NULL,
            extra_data  TEXT
        );
    )SQL";
    return executeSQL(sql);
}

std::string Database::itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::FOOD:      return "FOOD";
        case ItemType::MEDICINE:  return "MEDICINE";
        case ItemType::EQUIPMENT: return "EQUIPMENT";
        default:                  return "UNKNOWN";
    }
}

ItemType Database::stringToItemType(const std::string& str) {
    if (str == "FOOD")      return ItemType::FOOD;
    if (str == "MEDICINE")  return ItemType::MEDICINE;
    if (str == "EQUIPMENT") return ItemType::EQUIPMENT;
    return ItemType::FOOD;
}

bool Database::saveInventory(const Inventory& inventory, int playerId) {
    if (!m_connected) { std::cerr << "【数据库错误】未连接！\n"; return false; }
    std::ostringstream clr;
    clr << "DELETE FROM inventory WHERE player_id = " << playerId << ";";
    executeSQL(clr.str());

    const auto& items = inventory.getItems();
    for (const Item* item : items) {
        std::ostringstream extra;
        std::string typeStr = itemTypeToString(item->getType());
        switch (item->getType()) {
            case ItemType::FOOD: {
                const Food* f = dynamic_cast<const Food*>(item);
                if (f) extra << f->getHpRestore(); break;
            }
            case ItemType::MEDICINE: {
                const Medicine* m = dynamic_cast<const Medicine*>(item);
                if (m) extra << m->getHpRestore() << "|" << m->getCureEffect(); break;
            }
            case ItemType::EQUIPMENT: {
                const Equipment* e = dynamic_cast<const Equipment*>(item);
                if (e) extra << e->getAttackBonus() << "|" << e->getDefenseBonus() << "|" << static_cast<int>(e->getSlot()); break;
            }
        }
        std::ostringstream sql;
        sql << "INSERT INTO inventory (player_id, item_id, item_name, item_desc, "
            << "item_type, price, quantity, extra_data) VALUES ("
            << playerId << ", " << item->getID() << ", '"
            << item->getName() << "', '', '" << typeStr << "', "
            << item->getPrice() << ", " << item->getQuantity()
            << ", '" << extra.str() << "');";
        executeSQL(sql.str());
    }
    savePlayerGold(playerId, inventory.getOwnerGold());
    std::cout << "【数据库】背包已保存（" << items.size() << " 种物品）\n";
    return true;
}

bool Database::loadInventory(Inventory& inventory, int playerId) {
    if (!m_connected) { std::cerr << "【数据库错误】未连接！\n"; return false; }
    std::ostringstream query;
    query << "SELECT item_id, item_name, item_type, price, quantity, extra_data "
          << "FROM inventory WHERE player_id = " << playerId << " ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, query.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "【数据库错误】查询失败：" << sqlite3_errmsg(m_db) << "\n";
        return false;
    }
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int itemId      = sqlite3_column_int(stmt, 0);
        std::string name= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string type= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int price       = sqlite3_column_int(stmt, 3);
        int quantity    = sqlite3_column_int(stmt, 4);
        const char* et  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::string extra = et ? et : "";

        ItemType it = stringToItemType(type);
        Item* item = nullptr;
        switch (it) {
            case ItemType::FOOD: {
                int hp = extra.empty() ? 0 : std::stoi(extra);
                item = new Food(itemId, name, "", price, hp, quantity); break;
            }
            case ItemType::MEDICINE: {
                int hp = 0; std::string cure;
                size_t p = extra.find('|');
                if (p != std::string::npos) {
                    hp = std::stoi(extra.substr(0, p)); cure = extra.substr(p + 1);
                } else if (!extra.empty()) hp = std::stoi(extra);
                item = new Medicine(itemId, name, "", price, hp, cure, quantity); break;
            }
            case ItemType::EQUIPMENT: {
                int atk = 0, def = 0, slot = 0;
                size_t p1 = extra.find('|'), p2 = extra.find('|', p1 + 1);
                if (p1 != std::string::npos && p2 != std::string::npos) {
                    atk = std::stoi(extra.substr(0, p1));
                    def = std::stoi(extra.substr(p1 + 1, p2 - p1 - 1));
                    slot = std::stoi(extra.substr(p2 + 1));
                }
                item = new Equipment(itemId, name, "", price, atk, def,
                                     static_cast<EquipmentSlot>(slot), quantity); break;
            }
        }
        if (item) { inventory.addItem(item); count++; }
    }
    sqlite3_finalize(stmt);
    int gold = loadPlayerGold(playerId);
    inventory.setOwnerGold(gold);
    std::cout << "【数据库】背包已加载（" << count << " 种物品，金币 " << gold << "）\n";
    return true;
}

bool Database::saveShop(const Shop& shop) {
    if (!m_connected) return false;
    executeSQL("DELETE FROM shop_items;");
    const auto& items = shop.getItems();
    for (const Item* item : items) {
        std::ostringstream extra;
        std::string ts = itemTypeToString(item->getType());
        switch (item->getType()) {
            case ItemType::FOOD:
                { const Food* f = dynamic_cast<const Food*>(item);
                  if (f) extra << f->getHpRestore(); break; }
            case ItemType::MEDICINE:
                { const Medicine* m = dynamic_cast<const Medicine*>(item);
                  if (m) extra << m->getHpRestore() << "|" << m->getCureEffect(); break; }
            case ItemType::EQUIPMENT:
                { const Equipment* e = dynamic_cast<const Equipment*>(item);
                  if (e) extra << e->getAttackBonus() << "|" << e->getDefenseBonus() << "|" << static_cast<int>(e->getSlot()); break; }
        }
        std::ostringstream sql;
        sql << "INSERT OR REPLACE INTO shop_items (item_id, item_name, item_desc, "
            << "item_type, price, extra_data) VALUES ("
            << item->getID() << ", '" << item->getName() << "', '', '"
            << ts << "', " << item->getPrice() << ", '" << extra.str() << "');";
        executeSQL(sql.str());
    }
    std::cout << "【数据库】商店已保存（" << items.size() << " 种商品）\n";
    return true;
}

bool Database::loadShop(Shop& shop) {
    if (!m_connected) return false;
    const char* q = "SELECT item_id, item_name, item_type, price, extra_data FROM shop_items ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, q, -1, &stmt, nullptr) != SQLITE_OK) return false;
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string nm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string tp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int pr = sqlite3_column_int(stmt, 3);
        const char* et = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string ex = et ? et : "";
        ItemType it = stringToItemType(tp);
        Item* item = nullptr;
        switch (it) {
            case ItemType::FOOD:
                { int hp = ex.empty() ? 0 : std::stoi(ex);
                  item = new Food(id, nm, "", pr, hp); break; }
            case ItemType::MEDICINE: {
                int hp = 0; std::string cu;
                size_t pp = ex.find('|');
                if (pp != std::string::npos) { hp=std::stoi(ex.substr(0,pp)); cu=ex.substr(pp+1); }
                else if (!ex.empty()) hp = std::stoi(ex);
                item = new Medicine(id, nm, "", pr, hp, cu); break; }
            case ItemType::EQUIPMENT: {
                int atk=0,def=0,sl=0;
                size_t p1=ex.find('|'),p2=ex.find('|',p1+1);
                if (p1!=std::string::npos && p2!=std::string::npos) {
                    atk=std::stoi(ex.substr(0,p1)); def=std::stoi(ex.substr(p1+1,p2-p1-1)); sl=std::stoi(ex.substr(p2+1)); }
                item = new Equipment(id, nm, "", pr, atk, def, static_cast<EquipmentSlot>(sl)); break; }
        }
        if (item) { shop.addItem(item); count++; }
    }
    sqlite3_finalize(stmt);
    std::cout << "【数据库】商店已加载（" << count << " 种商品）\n";
    return true;
}

bool Database::savePlayerGold(int playerId, int gold) {
    if (!m_connected) return false;
    std::ostringstream sql;
    sql << "INSERT OR REPLACE INTO player_gold (player_id, gold) VALUES (" << playerId << ", " << gold << ");";
    return executeSQL(sql.str());
}

int Database::loadPlayerGold(int playerId) {
    if (!m_connected) return 0;
    std::ostringstream q;
    q << "SELECT gold FROM player_gold WHERE player_id = " << playerId << ";";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, q.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) return 0;
    int gold = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) gold = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return gold;
}

bool Database::clearInventory(int playerId) {
    if (!m_connected) return false;
    std::ostringstream s;
    s << "DELETE FROM inventory WHERE player_id = " << playerId << ";";
    return executeSQL(s.str());
}

bool Database::clearShop() {
    if (!m_connected) return false;
    return executeSQL("DELETE FROM shop_items;");
}
