# ============================================================
# gui_app --- Win32 GUI ????????RPG??????
# ??Win32 API, comctl32
# 
# ??resources.rc ?? GBK ??windres ??????
#       C++ ???? UTF-8 BOM
# ============================================================
cmake_minimum_required(VERSION 3.10)
project(RPGAdventureGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MINGW)
    set(CMAKE_CXX_FLAGS " -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()

set(GUI_SOURCES
    main.cpp
    src/BaseCharacter.cpp
    src/Player.cpp
    src/Skill.cpp
    src/LevelSystem.cpp
    src/SaveManager.cpp
    src/Item.cpp
    src/Food.cpp
    src/Medicine.cpp
    src/Equipment.cpp
    src/Inventory.cpp
    src/Shop.cpp
    src/Task.cpp
    src/TaskSystem.cpp
)

# Windows ??? (GBK ??)
if(WIN32)
    list(APPEND GUI_SOURCES resources.rc)
endif()

add_executable(RPGAdventureGUI WIN32 )
target_include_directories(RPGAdventureGUI PRIVATE include)
target_link_libraries(RPGAdventureGUI PRIVATE comctl32)

set_target_properties(RPGAdventureGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)
