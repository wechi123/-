#include "Task.h"
#include <sstream>
#include <vector>

// ========== 辅助：按分隔符切分字符串 ==========
namespace {
    std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, delim)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

// ========== 默认构造 ==========
Task::Task()
    : name(""), description(""), condition("")
    , expReward(0), goldReward(0)
    , status(TaskStatus::NotAccepted)
{}

// ========== 带参构造 ==========
Task::Task(const std::string& name,
           const std::string& desc,
           const std::string& cond,
           int expR, int goldR)
    : name(name), description(desc), condition(cond)
    , expReward(expR), goldReward(goldR)
    , status(TaskStatus::NotAccepted)
{}

// ========== 访问器 ==========
std::string Task::getName()        const { return name; }
std::string Task::getDescription() const { return description; }
std::string Task::getCondition()   const { return condition; }
int         Task::getExpReward()   const { return expReward; }
int         Task::getGoldReward()  const { return goldReward; }
TaskStatus  Task::getStatus()      const { return status; }

// ========== 接受任务 ==========
bool Task::accept() {
    if (status != TaskStatus::NotAccepted) return false;
    status = TaskStatus::InProgress;
    return true;
}

// ========== 完成任务 ==========
bool Task::complete() {
    if (status != TaskStatus::InProgress) return false;
    status = TaskStatus::Completed;
    return true;
}

// ========== 领取奖励 ==========
bool Task::claimReward() {
    if (status != TaskStatus::Completed) return false;
    status = TaskStatus::RewardClaimed;
    return true;
}

// ========== 序列化：name|status ==========
std::string Task::serialize() const {
    std::string statusStr;
    switch (status) {
        case TaskStatus::NotAccepted:   statusStr = "NotAccepted";   break;
        case TaskStatus::InProgress:    statusStr = "InProgress";    break;
        case TaskStatus::Completed:     statusStr = "Completed";     break;
        case TaskStatus::RewardClaimed: statusStr = "RewardClaimed"; break;
    }
    return name + "|" + statusStr;
}

// ========== 反序列化 ==========
void Task::deserialize(const std::string& data) {
    auto parts = split(data, '|');
    if (parts.size() >= 2) {
        // 仅恢复状态，name/desc 等信息由 loadTasks 提供
        const std::string& s = parts[1];
        if      (s == "NotAccepted")   status = TaskStatus::NotAccepted;
        else if (s == "InProgress")    status = TaskStatus::InProgress;
        else if (s == "Completed")     status = TaskStatus::Completed;
        else if (s == "RewardClaimed") status = TaskStatus::RewardClaimed;
    }
}
