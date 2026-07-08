#pragma once

#include <thread>
#include <atomic>
#include <string>

// 前向声明，避免引入完整头文件
class Character;
class Inventory;
class TaskSystem;

// AutoSaveService：后台多线程自动存档服务（挑战二）
// 使用 std::thread 创建后台线程，每隔 intervalSeconds 秒自动保存
// 使用 std::atomic<bool> 实现线程安全的启停标志
class AutoSaveService {
private:
    std::thread        saveThread;       // 后台存档线程
    std::atomic<bool>  running;          // 线程运行标志（原子操作）
    int                intervalSeconds;  // 存档间隔（秒）

    // 指向游戏核心数据的指针（由 GameManager 传入，不拥有所有权）
    Character*   player;
    Inventory*   inventory;
    TaskSystem*  taskSystem;

    // 后台线程主循环
    void saveTask();

public:
    // 构造：默认 60 秒间隔
    explicit AutoSaveService(int intervalSec = 60);
    ~AutoSaveService();

    // 禁止拷贝
    AutoSaveService(const AutoSaveService&) = delete;
    AutoSaveService& operator=(const AutoSaveService&) = delete;

    // 启动后台存档
    void start(Character* p, Inventory* inv, TaskSystem* ts);

    // 停止后台存档（阻塞等待线程退出）
    void stop();

    // 查询运行状态
    bool isRunning() const;
};
