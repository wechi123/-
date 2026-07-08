#include "Player.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace RPG {

// ==================== 构造 / 析构 ====================

Player::Player()
    : BaseCharacter(), exp(0), expNeed(100), gold(50), skillPoint(0) {}

Player::Player(const std::string& name)
    : BaseCharacter(), exp(0), expNeed(100), gold(50), skillPoint(0)
{
    this->name = name;
    // Lv1 初始四维
    str = 8; vit = 8; agi = 4; intl = 8;
    recalcStats();
    hp = maxHp;
    mp = maxMp;
}

Player::~Player() {
    for (size_t i = 0; i < skillList.size(); ++i)
        delete skillList[i];
    skillList.clear();
}

// 深拷贝
Player::Player(const Player& other)
    : BaseCharacter(other)
{
    exp = other.exp;
    expNeed = other.expNeed;
    gold = other.gold;
    skillPoint = other.skillPoint;
    bagItems = other.bagItems;
    taskList = other.taskList;

    for (size_t i = 0; i < other.skillList.size(); ++i)
        skillList.push_back(other.skillList[i]->clone());
}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        // 释放旧技能
        for (size_t i = 0; i < skillList.size(); ++i)
            delete skillList[i];
        skillList.clear();

        // 拷贝基类
        BaseCharacter::operator=(other);

        exp = other.exp;
        expNeed = other.expNeed;
        gold = other.gold;
        skillPoint = other.skillPoint;
        bagItems = other.bagItems;
        taskList = other.taskList;

        for (size_t i = 0; i < other.skillList.size(); ++i)
            skillList.push_back(other.skillList[i]->clone());
    }
    return *this;
}

// ==================== Getter ====================

int Player::getExp() const { return exp; }
int Player::getExpNeed() const { return expNeed; }
int Player::getGold() const { return gold; }
int Player::getSkillPoint() const { return skillPoint; }
const std::vector<Skill*>& Player::getSkillList() const { return skillList; }
std::vector<int>& Player::getBagItems() { return bagItems; }
const std::vector<int>& Player::getBagItems() const { return bagItems; }
std::vector<int>& Player::getTaskList() { return taskList; }
const std::vector<int>& Player::getTaskList() const { return taskList; }

// ==================== Setter ====================

void Player::setExp(int v)      { exp = (std::max)(0, v); }
void Player::setExpNeed(int v)  { expNeed = (std::max)(1, v); }
void Player::setGold(int v)     { gold = (std::max)(0, v); }
void Player::setSkillPoint(int v) { skillPoint = (std::max)(0, v); }

// ==================== 等级成长 ====================

void Player::addExp(int addNum) {
    if (addNum <= 0) return;
    exp += addNum;
    std::cout << "  获得 " << addNum << " 点经验值！\n";

    // 循环判断是否满足升级条件
    int count = 0;
    while (exp >= expNeed) {
        levelUp();
        ++count;
    }
    if (count > 0) {
        std::cout << "  ★ 恭喜升级 x" << count << "！当前等级 " << level << " ★\n";
    }
}

void Player::levelUp() {
    // 扣除升级所需经验
    exp -= expNeed;
    // 等级 +1
    level++;
    // 四维增量：str+2, vit+2, agi+1, intl+2
    str += 2; vit += 2; agi += 1; intl += 2;
    // 技能点 +1
    skillPoint++;
    // 重算属性
    recalcStats();
    // 满血满蓝
    hp = maxHp;
    mp = maxMp;
    // 设置下一级所需经验：100 + (level-1)×80
    expNeed = 100 + (level - 1) * 80;
}

// ==================== 技能管理 ====================

bool Player::learnSkill(int skillId) {
    // 检查是否已学
    if (hasSkill(skillId)) {
        std::cout << "  技能已学习！\n";
        return false;
    }
    // 检查技能点
    if (skillPoint < 1) {
        std::cout << "  技能点不足！需要1技能点。\n";
        return false;
    }
    // 创建技能
    Skill* skill = SkillFactory::createSkill(skillId);
    if (!skill) {
        std::cout << "  无效技能ID！\n";
        return false;
    }
    // 检查解锁等级
    if (level < skill->getUnlockLv()) {
        std::cout << "  需要等级 " << skill->getUnlockLv() << " 才能解锁 "
                  << skill->getName() << "！\n";
        delete skill;
        return false;
    }
    // 消耗技能点，加入技能列表
    skillPoint--;
    skillList.push_back(skill);
    std::cout << "  ✅ 学会技能：" << skill->getName() << "！\n";
    return true;
}

bool Player::upgradeSkill(int skillId) {
    Skill* skill = getSkill(skillId);
    if (!skill) {
        std::cout << "  未学习该技能！\n";
        return false;
    }
    if (!skill->canUpgrade()) {
        std::cout << "  技能已达最高级！\n";
        return false;
    }
    int cost = skill->upgradeCost();
    if (skillPoint < cost) {
        std::cout << "  技能点不足！需要 " << cost << " 点。\n";
        return false;
    }
    skillPoint -= cost;
    skill->setLevel(skill->getLevel() + 1);
    std::cout << "  ✅ 技能 " << skill->getName()
              << " 升级至 Lv" << skill->getLevel() << "！\n";
    return true;
}

bool Player::hasSkill(int skillId) const {
    for (size_t i = 0; i < skillList.size(); ++i) {
        if (skillList[i]->getId() == skillId) return true;
    }
    return false;
}

Skill* Player::getSkill(int skillId) const {
    for (size_t i = 0; i < skillList.size(); ++i) {
        if (skillList[i]->getId() == skillId) return skillList[i];
    }
    return nullptr;
}

int Player::getSkillCount() const { return (int)skillList.size(); }

// ==================== 背包 / 任务 ====================

void Player::addItem(int itemId) { bagItems.push_back(itemId); }
void Player::removeItem(int itemId) {
    auto it = std::find(bagItems.begin(), bagItems.end(), itemId);
    if (it != bagItems.end()) bagItems.erase(it);
}
bool Player::hasItem(int itemId) const {
    return std::find(bagItems.begin(), bagItems.end(), itemId) != bagItems.end();
}

void Player::addTask(int taskId) { taskList.push_back(taskId); }
void Player::removeTask(int taskId) {
    auto it = std::find(taskList.begin(), taskList.end(), taskId);
    if (it != taskList.end()) taskList.erase(it);
}
bool Player::hasTask(int taskId) const {
    return std::find(taskList.begin(), taskList.end(), taskId) != taskList.end();
}

// ==================== 序列化 ====================

std::string Player::serialize() const {
    std::ostringstream oss;
    // 角色名
    oss << name << "\n";
    // 等级 + 四维
    oss << level << " " << str << " " << vit << " " << agi << " " << intl << "\n";
    // HP/MP
    oss << hp << " " << maxHp << " " << mp << " " << maxMp << "\n";
    // 防御
    oss << def << " " << mdef << "\n";
    // 经验 / 金币 / 技能点
    oss << exp << " " << expNeed << " " << gold << " " << skillPoint << "\n";
    // 技能数量 + 各技能 id,level
    oss << skillList.size() << "\n";
    for (size_t i = 0; i < skillList.size(); ++i) {
        oss << skillList[i]->serialize() << "\n";
    }
    // 背包道具数量 + 列表
    oss << bagItems.size();
    for (size_t i = 0; i < bagItems.size(); ++i) oss << " " << bagItems[i];
    oss << "\n";
    // 任务数量 + 列表
    oss << taskList.size();
    for (size_t i = 0; i < taskList.size(); ++i) oss << " " << taskList[i];
    oss << "\n";

    return oss.str();
}

void Player::deserialize(const std::string& data) {
    std::istringstream iss(data);

    // 角色名
    std::getline(iss, name);

    // 等级 + 四维
    iss >> level >> str >> vit >> agi >> intl;

    // HP/MP
    iss >> hp >> maxHp >> mp >> maxMp;

    // 防御
    iss >> def >> mdef;

    // 经验 / 金币 / 技能点
    iss >> exp >> expNeed >> gold >> skillPoint;

    // 技能
    size_t skillCount;
    iss >> skillCount;
    // 清理旧技能
    for (size_t i = 0; i < skillList.size(); ++i) delete skillList[i];
    skillList.clear();
    for (size_t i = 0; i < skillCount; ++i) {
        int sid, slv;
        char comma;
        iss >> sid >> comma >> slv;
        Skill* s = SkillFactory::createSkill(sid);
        if (s) {
            s->setLevel(slv);
            skillList.push_back(s);
        }
    }

    // 背包
    size_t bagCount;
    iss >> bagCount;
    bagItems.clear();
    for (size_t i = 0; i < bagCount; ++i) {
        int id; iss >> id; bagItems.push_back(id);
    }

    // 任务
    size_t taskCount;
    iss >> taskCount;
    taskList.clear();
    for (size_t i = 0; i < taskCount; ++i) {
        int id; iss >> id; taskList.push_back(id);
    }
}

// ==================== 展示 ====================

void Player::displayInfo() const {
    BaseCharacter::displayInfo();
    std::cout << "  经验: " << exp << "/" << expNeed << "\n";
    std::cout << "  金币: " << gold << "\n";
    std::cout << "  技能点: " << skillPoint << "\n";
    std::cout << "  已学技能: " << skillList.size() << "个\n";
    std::cout << "  背包物品: " << bagItems.size() << "个\n";
    std::cout << "  进行中任务: " << taskList.size() << "个\n";
}

void Player::displaySkills() const {
    std::cout << "\n── 已学技能 ──\n";
    if (skillList.empty()) {
        std::cout << "  无\n";
        return;
    }
    for (size_t i = 0; i < skillList.size(); ++i) {
        const Skill* s = skillList[i];
        std::cout << "  [" << s->getId() << "] " << s->getName()
                  << " Lv." << s->getLevel()
                  << " MP:" << s->getMpCost()
                  << " | " << s->getDesc() << "\n";
    }
}

} // namespace RPG
