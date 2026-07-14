#pragma once

#include "Task.h"
#include <vector>
#include <string>

// 前向声明，避免循环依赖
namespace RPG { class Player; }

// TaskSystem 类：管理所有任务的查看、接受、完成、领奖
class TaskSystem {
private:
    std::vector<Task> tasks;   // 任务列表

public:
    // ---- 数据加载 ----
    void loadTasks(const std::string& filePath);

    // ---- 查看 ----
    void showTasks() const;

    // ---- 操作 ----
    bool acceptTask(int index);                         // 接受任务
    bool completeTask(int index);                       // 完成任务
    bool claimReward(int index, RPG::Player& player);     // 领取奖励

    // ---- 查询 ----
    int   getTaskCount() const;
    Task* getTask(int index);        // 返回指针，方便外部修改状态

    // ---- 序列化（存档用） ----
    std::string serialize() const;
    void        deserialize(const std::string& data);
};
