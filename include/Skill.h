#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace RPG {

class BaseCharacter;

// ============================================================
// Skill 基类
// ============================================================
class Skill {
protected:
    int skillId;
    std::string skillName;
    int skillLv;
    int mpCost;
    float damageCoeff;
    int unlockLv;
    std::string desc;

public:
    Skill(int id, const std::string& name, int mp, float coeff, int unlock, const std::string& desc);
    virtual ~Skill() = default;

    int getId() const;
    std::string getName() const;
    int getLevel() const;
    int getMpCost() const;
    float getCoeff() const;
    int getUnlockLv() const;
    std::string getDesc() const;

    bool setLevel(int lv);
    bool canUpgrade() const;
    int upgradeCost() const;

    virtual int calcDamage(const BaseCharacter& caster, const BaseCharacter* target = nullptr) const = 0;
    virtual float coeffAtLevel(int lv) const;

    std::string serialize() const;
    virtual Skill* clone() const = 0;
};

// ============================================================
// MeleeSkill
// ============================================================
class MeleeSkill : public Skill {
    float coeffs[3];
public:
    MeleeSkill(int id, const std::string& name, int mp,
               float c1, float c2, float c3,
               int unlock, const std::string& d);
    int calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const override;
    float coeffAtLevel(int lv) const override;
    Skill* clone() const override;
};

// ============================================================
// MagicSkill
// ============================================================
class MagicSkill : public Skill {
    float coeffs[3];
public:
    MagicSkill(int id, const std::string& name, int mp,
               float c1, float c2, float c3,
               int unlock, const std::string& d);
    int calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const override;
    float coeffAtLevel(int lv) const override;
    Skill* clone() const override;
};

// ============================================================
// HealSkill
// ============================================================
class HealSkill : public Skill {
    int healPerInt[3];
public:
    HealSkill(int id, const std::string& name, int mp,
              int heal1, int heal3,
              int unlock, const std::string& d);
    int calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const override;
    float coeffAtLevel(int lv) const override;
    Skill* clone() const override;
};

// ============================================================
// DebuffSkill
// ============================================================
class DebuffSkill : public Skill {
public:
    DebuffSkill(int id, const std::string& name, int mp,
                float c1, int unlock, const std::string& d);
    int calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const override;
    Skill* clone() const override;
    float getDebuffDefReduction() const;
    int getDebuffDuration() const;
};

// ============================================================
// SkillFactory
// ============================================================
namespace SkillFactory {
    const std::vector<Skill*>& getAllSkills();
    Skill* createSkill(int id);
    std::string getSkillName(int id);
    Skill* createFromId(int id);
    void cleanup();
}

} // namespace RPG
