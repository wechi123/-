cmake_minimum_required(VERSION 3.14)
project(RPGAdventureGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译方式（使用 MinGW）:
# 1. windres resources.rc -o resources.o -O coff
# 2. g++ -std=c++17 -fno-exceptions -I include main_gui.cpp src/*.cpp resources.o -o RPGAdventureGUI.exe -static -mwindows -lcomctl32

set(SOURCES
    main_gui.cpp
    src/Character.cpp
    src/LevelSystem.cpp
    src/SaveManager.cpp
)

include_directories(include)
add_executable(RPGAdventureGUI WIN32 ${SOURCES})
target_link_libraries(RPGAdventureGUI comctl32)
