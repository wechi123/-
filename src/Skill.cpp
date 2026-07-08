#include "Skill.h"
#include "BaseCharacter.h"
#include <algorithm>
#include <cmath>
#include <cassert>

namespace RPG {

// ==================== Skill 基类 ====================

Skill::Skill(int id, const std::string& name, int mp, float coeff, int unlock, const std::string& d)
    : skillId(id), skillName(name), skillLv(1), mpCost(mp),
      damageCoeff(coeff), unlockLv(unlock), desc(d) {}

int Skill::getId() const { return skillId; }
std::string Skill::getName() const { return skillName; }
int Skill::getLevel() const { return skillLv; }
int Skill::getMpCost() const { return mpCost; }
float Skill::getCoeff() const { return damageCoeff; }
int Skill::getUnlockLv() const { return unlockLv; }
std::string Skill::getDesc() const { return desc; }

bool Skill::setLevel(int lv) {
    if (lv < 1 || lv > 3) return false;
    skillLv = lv;
    return true;
}

bool Skill::canUpgrade() const { return skillLv < 3; }

int Skill::upgradeCost() const {
    // 1→2 消耗1点, 2→3 消耗2点
    return (skillLv == 1) ? 1 : (skillLv == 2) ? 2 : 0;
}

float Skill::coeffAtLevel(int lv) const {
    return damageCoeff; // 子类重写
}

std::string Skill::serialize() const {
    std::ostringstream oss;
    oss << skillId << "," << skillLv;
    return oss.str();
}

// ==================== MeleeSkill ====================

MeleeSkill::MeleeSkill(int id, const std::string& name, int mp,
                       float c1, float c2, float c3,
                       int unlock, const std::string& d)
    : Skill(id, name, mp, c1, unlock, d)
{
    coeffs[0] = c1; coeffs[1] = c2; coeffs[2] = c3;
}

float MeleeSkill::coeffAtLevel(int lv) const {
    if (lv < 1 || lv > 3) lv = 1;
    return coeffs[lv - 1];
}

int MeleeSkill::calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const {
    // 物理伤害 = 力量 × 系数 - 目标物防
    float coeff = coeffAtLevel(skillLv);
    int dmg = (int)(caster.getStr() * coeff);
    if (target) dmg -= target->getDef();
    return (std::max)(1, dmg);
}

Skill* MeleeSkill::clone() const {
    MeleeSkill* s = new MeleeSkill(skillId, skillName, mpCost,
                                    coeffs[0], coeffs[1], coeffs[2],
                                    unlockLv, desc);
    s->skillLv = skillLv;
    return s;
}

// ==================== MagicSkill ====================

MagicSkill::MagicSkill(int id, const std::string& name, int mp,
                       float c1, float c2, float c3,
                       int unlock, const std::string& d)
    : Skill(id, name, mp, c1, unlock, d)
{
    coeffs[0] = c1; coeffs[1] = c2; coeffs[2] = c3;
}

float MagicSkill::coeffAtLevel(int lv) const {
    if (lv < 1 || lv > 3) lv = 1;
    return coeffs[lv - 1];
}

int MagicSkill::calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const {
    // 魔法伤害 = 智力 × 系数 - 目标魔防
    float coeff = coeffAtLevel(skillLv);
    int dmg = (int)(caster.getIntl() * coeff);
    if (target) dmg -= target->getMdef();
    return (std::max)(1, dmg);
}

Skill* MagicSkill::clone() const {
    MagicSkill* s = new MagicSkill(skillId, skillName, mpCost,
                                    coeffs[0], coeffs[1], coeffs[2],
                                    unlockLv, desc);
    s->skillLv = skillLv;
    return s;
}

// ==================== HealSkill ====================

HealSkill::HealSkill(int id, const std::string& name, int mp,
                     int heal1, int heal3,
                     int unlock, const std::string& d)
    : Skill(id, name, mp, (float)heal1, unlock, d)
{
    healPerInt[0] = heal1; // Lv1: 智力×4
    healPerInt[1] = (heal1 + heal3) / 2; // Lv2: 取中值
    healPerInt[2] = heal3; // Lv3: 智力×6
}

float HealSkill::coeffAtLevel(int lv) const {
    if (lv < 1 || lv > 3) lv = 1;
    return (float)healPerInt[lv - 1];
}

int HealSkill::calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const {
    // 治疗量 = 智力 × 每点治疗量
    return caster.getIntl() * healPerInt[skillLv - 1];
}

Skill* HealSkill::clone() const {
    HealSkill* s = new HealSkill(skillId, skillName, mpCost,
                                  healPerInt[0], healPerInt[2],
                                  unlockLv, desc);
    s->skillLv = skillLv;
    return s;
}

// ==================== DebuffSkill ====================

DebuffSkill::DebuffSkill(int id, const std::string& name, int mp,
                         float c1, int unlock, const std::string& d)
    : Skill(id, name, mp, c1, unlock, d) {}

int DebuffSkill::calcDamage(const BaseCharacter& caster, const BaseCharacter* target) const {
    // 0.8 倍魔法伤害
    int baseDmg = (int)(caster.getIntl() * 0.8f);
    if (target) baseDmg -= target->getMdef();
    return (std::max)(1, baseDmg);
}

float DebuffSkill::getDebuffDefReduction() const {
    return 0.3f; // 物防降低 30%
}

int DebuffSkill::getDebuffDuration() const {
    return 1; // 持续 1 回合
}

Skill* DebuffSkill::clone() const {
    DebuffSkill* s = new DebuffSkill(skillId, skillName, mpCost,
                                      damageCoeff, unlockLv, desc);
    s->skillLv = skillLv;
    return s;
}

// ==================== SkillFactory ====================

namespace SkillFactory {

static std::vector<Skill*>* g_skills = nullptr;

const std::vector<Skill*>& getAllSkills() {
    if (g_skills) return *g_skills;

    g_skills = new std::vector<Skill*>();

    // --- ID 1: 挥拳重击 (Melee) ---
    g_skills->push_back(new MeleeSkill(1, "挥拳重击", 10,
        1.2f, 1.4f, 1.7f, 1, "物理伤害 = 力量×系数 - 目标物防"));

    // --- ID 2: 冲刺冲撞 (Melee) ---
    g_skills->push_back(new MeleeSkill(2, "冲刺冲撞", 22,
        1.8f, 2.1f, 2.5f, 3, "强力冲撞，造成高额物理伤害"));

    // --- ID 3: 粉笔飞弹 (Magic) ---
    g_skills->push_back(new MagicSkill(3, "粉笔飞弹", 12,
        1.3f, 1.5f, 1.8f, 1, "魔法伤害 = 智力×系数 - 目标魔防"));

    // --- ID 4: 试卷风暴 (Magic) ---
    g_skills->push_back(new MagicSkill(4, "试卷风暴", 30,
        2.2f, 2.6f, 3.0f, 4, "召唤试卷风暴，大范围魔法攻击"));

    // --- ID 5: 课间补给 (Heal) ---
    g_skills->push_back(new HealSkill(5, "课间补给", 15,
        4, 6, 2, "恢复自身HP = 智力×N (Lv1=4, Lv3=6)"));

    // --- ID 6: 说教威慑 (Debuff) ---
    g_skills->push_back(new DebuffSkill(6, "说教威慑", 18,
        0.8f, 3, "0.8倍魔法伤害 + 降低敌方物防30%持续1回合"));

    return *g_skills;
}

Skill* createSkill(int id) {
    const auto& all = getAllSkills();
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i]->getId() == id) {
            return all[i]->clone();
        }
    }
    return nullptr;
}

std::string getSkillName(int id) {
    const auto& all = getAllSkills();
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i]->getId() == id) return all[i]->getName();
    }
    return "未知技能";
}

Skill* createFromId(int id) {
    return createSkill(id);
}

void cleanup() {
    if (g_skills) {
        for (size_t i = 0; i < g_skills->size(); ++i) {
            delete (*g_skills)[i];
        }
        delete g_skills;
        g_skills = nullptr;
    }
}

} // namespace SkillFactory

} // namespace RPG
