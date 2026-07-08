cmake_minimum_required(VERSION 3.14)
project(CampusRPG LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/Item.cpp src/Food.cpp src/Medicine.cpp
    src/Equipment.cpp src/Inventory.cpp src/Shop.cpp
    src/Database.cpp main.cpp
)
set(SQLITE_SOURCES lib/sqlite3.c)

add_executable(CampusRPG ${SOURCES} ${SQLITE_SOURCES})
target_include_directories(CampusRPG PRIVATE include lib)

if(MSVC)
    target_compile_options(CampusRPG PRIVATE /W3 /utf-8)
else()
    target_compile_options(CampusRPG PRIVATE -Wall -Wextra)
endif()
