# ============================================================
# console_app — 控制台物品/商店/SQLite数据库系统
# 包含：Character, Item, Food, Medicine, Equipment,
#       Inventory, Shop, Database, Task, TaskSystem
# 依赖：SQLite3 (内嵌)
# ============================================================

set(CONSOLE_SOURCES
    main.cpp
    src/Character.cpp
    src/Item.cpp
    src/Food.cpp
    src/Medicine.cpp
    src/Equipment.cpp
    src/Inventory.cpp
    src/Shop.cpp
    src/Database.cpp
    src/Task.cpp
    src/TaskSystem.cpp
)

# SQLite3 (C 源码)
set(SQLITE_SOURCES lib/sqlite3.c)

add_executable(console_app  )
target_include_directories(console_app PRIVATE include lib)

set_target_properties(console_app PROPERTIES
    OUTPUT_NAME "CampusRPG"
)
