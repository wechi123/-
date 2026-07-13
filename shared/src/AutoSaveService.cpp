#include "AutoSaveService.h"
#include "Character.h"
#include "Inventory.h"
#include "TaskSystem.h"
#include "SaveManager.h"
#include <iostream>
#include <chrono>

// ========== 构造 ==========
AutoSaveService::AutoSaveService(int intervalSec)
    : running(false)
    , intervalSeconds(intervalSec)
    , player(nullptr)
    , inventory(nullptr)
    , taskSystem(nullptr)
{}

// ========== 析构：确保线程安全退出 ==========
AutoSaveService::~AutoSaveService() {
    stop();
}

// ========== 启动后台存档线程 ==========
void AutoSaveService::start(Character* p, Inventory* inv, TaskSystem* ts) {
    if (running.load()) {
        std::cout << "[AutoSave] 后台存档已在运行中。\n";
        return;
    }

    player     = p;
    inventory  = inv;
    taskSystem = ts;

    if (!player || !inventory || !taskSystem) {
        std::cerr << "[AutoSave] 启动失败：数据指针为空。\n";
        return;
    }

    running.store(true);
    saveThread = std::thread(&AutoSaveService::saveTask, this);

    std::cout << "[AutoSave] 后台自动存档已启动（每 "
              << intervalSeconds << " 秒自动保存）。\n";
}

// ========== 停止后台存档线程 ==========
void AutoSaveService::stop() {
    if (!running.load()) return;

    running.store(false);  // 通知线程退出

    if (saveThread.joinable()) {
        saveThread.join();  // 等待线程结束
    }

    std::cout << "[AutoSave] 后台自动存档已停止。\n";
}

// ========== 查询运行状态 ==========
bool AutoSaveService::isRunning() const {
    return running.load();
}

// ========== 后台线程主循环 ==========
void AutoSaveService::saveTask() {
    while (running.load()) {
        // 等待 intervalSeconds 秒，每秒检查一次 running 标志
        for (int i = 0; i < intervalSeconds && running.load(); ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (!running.load()) break;

        // 执行保存
        SaveManager sm;
        if (sm.saveAll(*player, *inventory, *taskSystem)) {
            std::cout << "[AutoSave] 自动存档完成。\n";
        } else {
            std::cerr << "[AutoSave] 自动存档失败！\n";
        }
    }
}
