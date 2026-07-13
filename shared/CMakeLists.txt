# ============================================================
# shared — 跨模块集成代码（预留）
# 包含：AutoSaveService（自动存档服务）
#       EasyXRenderer（EasyX 图形渲染器）
#
# 注意：这两个模块依赖 console_app 和 battle_app 的类，
# 需要额外的 include 路径且尚在开发中。
# 默认不参与编译；解除注释即可单独构建。
# ============================================================

# set(SHARED_SOURCES
#     src/AutoSaveService.cpp
#     src/EasyXRenderer.cpp
# )

# add_library(shared_lib STATIC )
# target_include_directories(shared_lib PUBLIC include)
# target_include_directories(shared_lib PRIVATE
#     "/console_app/include"
#     "/battle_app/include"
#     "/gui_app/include"
# )
