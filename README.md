# 校园RPG冒险游戏 (Campus RPG Adventure)

三人合作开发的 C++ RPG 游戏项目，包含三个独立模块。

## 项目结构

`
├── CMakeLists.txt          # 顶层构建文件
├── gui_app/                # Win32 GUI 角色管理系统
│   ├── main.cpp            # 入口（WinMain）
│   ├── resources.rc        # Windows 资源
│   ├── include/            # BaseCharacter, Player, Skill, LevelSystem, SaveManager
│   └── src/                # 对应实现
├── console_app/            # 控制台物品/商店/SQLite 系统
│   ├── main.cpp            # 入口
│   ├── include/            # Character, Item, Food, Medicine, Equipment, Inventory, Shop, Database, Task, TaskSystem
│   ├── src/                # 对应实现
│   └── lib/                # SQLite3 (内嵌)
├── battle_app/             # 控制台战斗 RPG 系统
│   ├── main.cpp            # 入口
│   ├── include/            # BattleSystem, Character, ConsoleUI, Enemy, GameConfig, GameManager, IGameUI
│   ├── src/                # 对应实现
│   └── data/               # 游戏数据文件
├── shared/                 # 跨模块集成代码（预留）
│   ├── include/            # AutoSaveService, EasyXRenderer
│   └── src/
├── data/                   # 全局游戏存档
└── backup/                 # 历史备份文件
`

## 构建方式

### 前置条件
- CMake >= 3.14
- C++17 编译器 (MSVC / MinGW / GCC)
- Windows SDK（gui_app 需要）

### 使用 CMake 构建全部模块

`ash
mkdir build && cd build
cmake ..
cmake --build .
`

### 单独构建某个模块

`ash
cmake --build . --target gui_app      # Win32 GUI 版
cmake --build . --target console_app   # 控制台物品版
cmake --build . --target battle_app    # 控制台战斗版
`

### MinGW 手动编译（gui_app 需额外步骤）

`ash
# 1. 编译资源文件
windres gui_app/resources.rc -o resources.o -O coff

# 2. 编译 GUI 应用
g++ -std=c++17 -I gui_app/include gui_app/main.cpp gui_app/src/*.cpp resources.o -o RPGAdventureGUI.exe -static -mwindows -lcomctl32

# 3. 编译控制台应用
g++ -std=c++17 -I console_app/include -I console_app/lib console_app/main.cpp console_app/src/*.cpp console_app/lib/sqlite3.c -o CampusRPG.exe

# 4. 编译战斗应用
g++ -std=c++17 -I battle_app/include battle_app/main.cpp battle_app/src/*.cpp -o BattleRPG.exe
`

## 模块说明

| 模块 | 类型 | 功能 |
|------|------|------|
| gui_app | Win32 GUI | 角色创建、属性管理、技能学习、存档读档 |
| console_app | 控制台 | 物品系统、背包管理、商店交易、SQLite 持久化 |
| attle_app | 控制台 | 回合制战斗、敌人AI、任务系统 |

## 协作分工

- **gui_app**: RPG 命名空间下的 Player/BaseCharacter/Skill 体系 + Win32 界面
- **console_app**: Item 多态体系（Food/Medicine/Equipment）+ Inventory + Shop + SQLite
- **battle_app**: BattleSystem + Enemy + GameManager + ConsoleUI
