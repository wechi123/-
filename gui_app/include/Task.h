#pragma once

#include <string>

// 任务状态枚举
enum class TaskStatus {
    NotAccepted,   // 未接受
    InProgress,    // 进行中
    Completed,     // 已完成（待领奖）
    RewardClaimed  // 奖励已领取
};

// Task 类：封装单个任务的数据和行为
class Task {
private:
    std::string name;           // 任务名称
    std::string description;    // 任务描述
    std::string condition;      // 完成条件描述
    int expReward;              // 经验奖励
    int goldReward;             // 金币奖励
    TaskStatus status;          // 当前状态

public:
    // 默认构造
    Task();

    // 带参构造
    Task(const std::string& name,
         const std::string& desc,
         const std::string& cond,
         int expR,
         int goldR);

    // ---- 访问器 ----
    std::string getName()        const;
    std::string getDescription() const;
    std::string getCondition()   const;
    int         getExpReward()   const;
    int         getGoldReward()  const;
    TaskStatus  getStatus()      const;

    // ---- 状态流转 ----
    bool accept();        // 接受任务
    bool complete();      // 完成任务
    bool claimReward();   // 领取奖励（领取后状态变为 RewardClaimed）

    // ---- 序列化（存档用） ----
    std::string serialize() const;
    void        deserialize(const std::string& data);
};
