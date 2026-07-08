#pragma once

#include "BaseCharacter.h"
#include "Skill.h"
#include <vector>

namespace RPG {

// ============================================================
// Player — 玩家子类（角色创建 + 等级成长 + 技能管理）
// ============================================================
class Player : public BaseCharacter {
private:
    // 玩家独有属性
    int exp;           // 当前经验值
    int expNeed;       // 升级所需经验值
    int gold;          // 持有金币
    int skillPoint;    // 技能点（升级获得，用于学习/升级技能）

    // STL 容器
    std::vector<Skill*> skillList;    // 已学技能
    std::vector<int> bagItems;        // 背包道具ID列表
    std::vector<int> taskList;        // 进行中任务ID列表

public:
    // —— 构造 ——
    Player();                          // 无参（读档专用）
    explicit Player(const std::string& name);  // 有参（新建角色）

    // —— 析构（释放技能对象）——
    ~Player() override;

    // 拷贝构造 & 赋值（深拷贝技能列表）
    Player(const Player& other);
    Player& operator=(const Player& other);

    // —— Getter ——
    int getExp() const;
    int getExpNeed() const;
    int getGold() const;
    int getSkillPoint() const;
    const std::vector<Skill*>& getSkillList() const;
    std::vector<int>& getBagItems();
    const std::vector<int>& getBagItems() const;
    std::vector<int>& getTaskList();
    const std::vector<int>& getTaskList() const;

    // —— Setter ——
    void setExp(int v);
    void setExpNeed(int v);
    void setGold(int v);
    void setSkillPoint(int v);

    // —— 等级成长 ——
    void addExp(int addNum);        // 累加经验，自动检测升级
    void levelUp();                 // 等级+1，刷新属性，满血满蓝
    void levelUpIfNeeded();         // 文档接口：检测并执行升级

    // —— 金币操作（文档接口）——
    void gainGold(int amount);      // 增加金币
    bool costGold(int amount);      // 消耗金币，返回是否足够

    // —— 技能管理 ——
    bool learnSkill(int skillId);           // 消耗1技能点学习技能
    bool upgradeSkill(int skillId);         // 消耗技能点升级技能
    bool hasSkill(int skillId) const;       // 是否已学
    Skill* getSkill(int skillId) const;     // 获取技能指针
    int getSkillCount() const;

    // —— 背包/任务 ——
    void addItem(int itemId);
    void removeItem(int itemId);
    bool hasItem(int itemId) const;
    void addTask(int taskId);
    void removeTask(int taskId);
    bool hasTask(int taskId) const;

    // —— 序列化（实现纯虚接口）——
    std::string serialize() const override;
    void deserialize(const std::string& data) override;

    // —— 展示 ——
    void displayInfo() const override;
    void displaySkills() const;
};

} // namespace RPG
