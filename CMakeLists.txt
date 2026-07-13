# ============================================================
# 校园RPG冒险游戏 — 顶层 CMakeLists.txt
# 项目包含三个独立可执行目标 + 共享模块：
#   1. gui_app    — Win32 GUI 角色管理系统
#   2. console_app — 控制台物品/商店/数据库系统
#   3. battle_app  — 控制台战斗系统
#   4. shared      — 跨模块集成代码（预留）
# ============================================================

cmake_minimum_required(VERSION 3.14)
project(CampusRPG VERSION 1.0 LANGUAGES C CXX)

# ---- C++ 标准 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- 编译选项 ----
if(MSVC)
    add_compile_options(/W3 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

# ---- 输出目录统一 ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/bin")

# ---- 子目录 ----
add_subdirectory(gui_app)
add_subdirectory(console_app)
add_subdirectory(battle_app)
add_subdirectory(shared)
