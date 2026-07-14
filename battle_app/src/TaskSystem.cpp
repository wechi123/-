#include "TaskSystem.h"
#include "Character.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// ========== 辅助：按分隔符切分字符串（与 Task.cpp 中相同） ==========
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

// ========== 从文件加载任务模板 ==========
void TaskSystem::loadTasks(const std::string& filePath) {
    tasks.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[TaskSystem] 无法打开任务文件: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto parts = split(line, '|');
        // 格式：名称|描述|条件|经验|金币
        if (parts.size() >= 5) {
            std::string name   = parts[0];
            std::string desc   = parts[1];
            std::string cond   = parts[2];
            int expR  = std::stoi(parts[3]);
            int goldR = std::stoi(parts[4]);
            tasks.emplace_back(name, desc, cond, expR, goldR);
        }
    }
    file.close();
}

// ========== 显示所有任务 ==========
void TaskSystem::showTasks() const {
    std::cout << "\n";
    std::cout << "  ================ 任务列表 ================\n\n";

    if (tasks.empty()) {
        std::cout << "  （暂无任务）\n";
        return;
    }

    for (size_t i = 0; i < tasks.size(); ++i) {
        const Task& t = tasks[i];

        // 状态转文字
        std::string statusStr;
        switch (t.getStatus()) {
            case TaskStatus::NotAccepted:   statusStr = "[未接受]";   break;
            case TaskStatus::InProgress:    statusStr = "[进行中]";   break;
            case TaskStatus::Completed:     statusStr = "[已完成]";   break;
            case TaskStatus::RewardClaimed: statusStr = "[已领奖]";   break;
        }

        std::cout << "  " << (i + 1) << ". " << t.getName()
                  << "  " << statusStr << "\n";
        std::cout << "     描述: " << t.getDescription() << "\n";
        std::cout << "     条件: " << t.getCondition() << "\n";
        std::cout << "     奖励: " << t.getExpReward() << " EXP, "
                  << t.getGoldReward() << " 金币\n\n";
    }

    std::cout << "  ==========================================\n";
}

// ========== 接受任务 ==========
bool TaskSystem::acceptTask(int index) {
    if (index < 0 || index >= static_cast<int>(tasks.size())) {
        std::cout << "  [错误] 任务编号无效！\n";
        return false;
    }
    if (!tasks[index].accept()) {
        std::cout << "  [提示] 该任务无法接受（可能已接受或已完成）。\n";
        return false;
    }
    std::cout << "  [成功] 已接受任务：「" << tasks[index].getName() << "」\n";
    return true;
}

// ========== 完成任务 ==========
bool TaskSystem::completeTask(int index) {
    if (index < 0 || index >= static_cast<int>(tasks.size())) {
        std::cout << "  [错误] 任务编号无效！\n";
        return false;
    }
    if (!tasks[index].complete()) {
        std::cout << "  [提示] 该任务无法完成（可能未接受或已完成）。\n";
        return false;
    }
    std::cout << "  [成功] 已完成任务：「" << tasks[index].getName() << "」\n";
    return true;
}

// ========== 领取奖励 ==========
bool TaskSystem::claimReward(int index, Character& player) {
    if (index < 0 || index >= static_cast<int>(tasks.size())) {
        std::cout << "  [错误] 任务编号无效！\n";
        return false;
    }
    Task& t = tasks[index];
    if (t.getStatus() != TaskStatus::Completed) {
        std::cout << "  [提示] 该任务尚未完成，无法领取奖励。\n";
        return false;
    }

    // 发放奖励
    player.gainExp(t.getExpReward());
    player.gainGold(t.getGoldReward());

    t.claimReward();  // 标记为已领奖

    std::cout << "  [奖励] 获得 " << t.getExpReward() << " EXP, "
              << t.getGoldReward() << " 金币！\n";
    return true;
}

// ========== 获取任务数量 ==========
int TaskSystem::getTaskCount() const {
    return static_cast<int>(tasks.size());
}

// ========== 获取指定任务指针 ==========
Task* TaskSystem::getTask(int index) {
    if (index < 0 || index >= static_cast<int>(tasks.size()))
        return nullptr;
    return &tasks[index];
}

// ========== 序列化所有任务状态 ==========
std::string TaskSystem::serialize() const {
    std::string result;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (i > 0) result += "\n";
        result += tasks[i].serialize();
    }
    return result;
}

// ========== 反序列化恢复任务状态 ==========
void TaskSystem::deserialize(const std::string& data) {
    auto lines = split(data, '\n');
    for (size_t i = 0; i < lines.size() && i < tasks.size(); ++i) {
        tasks[i].deserialize(lines[i]);
    }
}
