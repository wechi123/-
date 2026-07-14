# ============================================================
# battle_app — 统一校园RPG（整合全部6大模块）
# 
# 模块清单：
#   1. 角色管理   — Character.cpp/h
#   2. 背包管理   — Item, Food, Medicine, Equipment, Inventory
#   3. 商店系统   — Shop.cpp/h
#   4. 任务系统   — Task.cpp/h, TaskSystem.cpp/h
#   5. 战斗系统   — BattleSystem, Enemy (Slime/Skeleton/Boss)
#   6. 等级成长   — Character::gainExp/levelUpIfNeeded
#   持久化        — Database (SQLite) + 文件存档
# ============================================================

set(BATTLE_SOURCES
    main.cpp
    src/Character.cpp
    src/BattleSystem.cpp
    src/ConsoleUI.cpp
    src/Enemy.cpp
    src/GameManager.cpp
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

add_executable(battle_app  )
target_include_directories(battle_app PRIVATE include lib)

# 复制数据文件到输出目录
add_custom_command(TARGET battle_app POST_BUILD
    COMMAND  -E copy_directory
    "/data"
    "$<TARGET_FILE_DIR:battle_app>/data"
    COMMENT "复制数据文件..."
)

set_target_properties(battle_app PROPERTIES
    OUTPUT_NAME "CampusRPG_Unified"
)
