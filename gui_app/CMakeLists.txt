# ============================================================
# gui_app — Win32 GUI 校园RPG角色管理系统
# 依赖：Win32 API, comctl32
# 编译方式（MinGW）:
#   windres resources.rc -o resources.o -O coff
#   (CMake 会自动处理 .rc 文件)
# ============================================================

set(GUI_SOURCES
    main.cpp
    src/BaseCharacter.cpp
    src/Player.cpp
    src/Skill.cpp
    src/LevelSystem.cpp
    src/SaveManager.cpp
)

# Windows 资源文件
if(WIN32)
    list(APPEND GUI_SOURCES resources.rc)
endif()

add_executable(gui_app WIN32 )
target_include_directories(gui_app PRIVATE include)

target_link_libraries(gui_app PRIVATE comctl32)

# 设置输出名称
set_target_properties(gui_app PROPERTIES
    OUTPUT_NAME "RPGAdventureGUI"
)
