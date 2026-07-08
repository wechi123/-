#include "Enemy.h"
#include <cstdlib>
#include <algorithm>

Enemy::Enemy(const std::string& n, const std::string& t,
             int h, int atk, int def, int expR, int goldR)
    : name(n), typeName(t), hp(h), maxHp(h)
    , attack(atk), defense(def), expReward(expR), goldReward(goldR)
{
}

std::vector<std::string> Enemy::getInfoLines() const {
    std::vector<std::string> lines;
    lines.push_back("名称：" + name + " (" + typeName + ")");
    lines.push_back("HP  ：[" + std::to_string(hp) + "/" + std::to_string(maxHp) + "]");
    lines.push_back("攻击：" + std::to_string(attack));
    lines.push_back("防御：" + std::to_string(defense));
    lines.push_back("经验奖励：" + std::to_string(expReward));
    lines.push_back("金币奖励：" + std::to_string(goldReward) + " G");
    return lines;
}

AttackResult Enemy::calculateDamage() const {
    AttackResult result;
    result.damage = std::max(1, attack + (rand() % 5));
    result.isSkill = false;
    return result;
}

std::string Enemy::getBattleCry() const {
    return name + " 出现了！";
}

int Enemy::takeDamage(int damage) {
    int actualDamage = std::max(1, damage - defense / 2);
    hp -= actualDamage;
    if (hp < 0) hp = 0;
    return actualDamage;
}

bool Enemy::isAlive() const {
    return hp > 0;
}

std::unique_ptr<Enemy> Enemy::clone() const {
    return std::make_unique<Enemy>(name, typeName, maxHp, attack, defense, expReward, goldReward);
}

std::vector<std::unique_ptr<Enemy>> Enemy::createEnemyPool() {
    std::vector<std::unique_ptr<Enemy>> pool;
    pool.push_back(std::make_unique<Slime>());
    pool.push_back(std::make_unique<Slime>());
    pool.push_back(std::make_unique<Skeleton>());
    pool.push_back(std::make_unique<Skeleton>());
    pool.push_back(std::make_unique<Boss>());
    return pool;
}

// ==================== Slime ====================

Slime::Slime()
    : Enemy("绿色史莱姆", "史莱姆", 50, 10, 2, 30, 20)
{
}

std::string Slime::getBattleCry() const {
    return "噗噜噗噜～ 一只 " + name + " 蹦了出来！";
}

// ==================== Skeleton ====================

Skeleton::Skeleton()
    : Enemy("骷髅士兵", "骷髅", 100, 18, 6, 60, 40)
{
}

std::string Skeleton::getBattleCry() const {
    return "咔嗒咔嗒... " + name + " 挥舞着骨剑冲了过来！";
}

// ==================== Boss ====================

Boss::Boss()
    : Enemy("期末考试大魔王", "Boss", 250, 30, 12, 200, 150)
{
}

AttackResult Boss::calculateDamage() const {
    AttackResult result;
    result.damage = attack + (rand() % 10);
    if (rand() % 100 < 25) {
        result.isSkill = true;
        result.skillName = "重击";
        result.damage *= 2;
    }
    return result;
}

std::string Boss::getBattleCry() const {
    return "吼！！！" + name + " 降临了！你感到一阵恐惧...";
}
