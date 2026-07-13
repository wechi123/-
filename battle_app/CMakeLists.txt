# ============================================================
# battle_app — 控制台战斗RPG系统
# 包含：BattleSystem, Character, ConsoleUI, Enemy,
#       GameConfig, GameManager, IGameUI
# ============================================================

set(BATTLE_SOURCES
    main.cpp
    src/BattleSystem.cpp
    src/Character.cpp
    src/ConsoleUI.cpp
    src/Enemy.cpp
    src/GameManager.cpp
)

add_executable(battle_app )
target_include_directories(battle_app PRIVATE include)

# 复制数据文件到输出目录
add_custom_command(TARGET battle_app POST_BUILD
    COMMAND  -E copy_directory
    "/data"
    "$<TARGET_FILE_DIR:battle_app>/data"
    COMMENT "复制 battle_app 数据文件..."
)

set_target_properties(battle_app PROPERTIES
    OUTPUT_NAME "BattleRPG"
)
