#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <memory>
#include <vector>

struct AttackResult {
    int damage;
    bool isSkill;
    std::string skillName;
};

class Enemy {
protected:
    std::string name;
    std::string typeName;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int expReward;
    int goldReward;

public:
    Enemy(const std::string& n, const std::string& t,
          int h, int atk, int def, int expR, int goldR);
    virtual ~Enemy() = default;

    std::vector<std::string> getInfoLines() const;

    virtual AttackResult calculateDamage() const;

    virtual std::string getBattleCry() const;
    int takeDamage(int damage);
    bool isAlive() const;

    const std::string& getName() const { return name; }
    const std::string& getTypeName() const { return typeName; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getExpReward() const { return expReward; }
    int getGoldReward() const { return goldReward; }

    void restoreHp() { hp = maxHp; }

    static std::vector<std::unique_ptr<Enemy>> createEnemyPool();
    std::unique_ptr<Enemy> clone() const;
};

class Slime : public Enemy {
public:
    Slime();
    std::string getBattleCry() const override;
};

class Skeleton : public Enemy {
public:
    Skeleton();
    std::string getBattleCry() const override;
};

class Boss : public Enemy {
public:
    Boss();
    AttackResult calculateDamage() const override;
    std::string getBattleCry() const override;
};

#endif
