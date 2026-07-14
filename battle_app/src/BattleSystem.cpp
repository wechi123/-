#include "BattleSystem.h"
#include "GameConfig.h"
#include <cstdlib>
#include <ctime>

BattleSystem::BattleSystem(IGameUI& uiRef)
    : ui(uiRef)
{
    srand((unsigned)time(nullptr));
}

Enemy* BattleSystem::pickEnemy(std::vector<std::unique_ptr<Enemy>>& enemies) {
    ui.clearScreen();
    ui.drawTitle("可挑战的敌人", 48);

    std::vector<Enemy*> aliveList;
    for (auto& e : enemies) {
        if (e->isAlive()) aliveList.push_back(e.get());
    }

    if (aliveList.empty()) {
        ui.showMessage("所有敌人都已被击败！重新生成敌人...");
        enemies = Enemy::createEnemyPool();
        for (auto& e : enemies) aliveList.push_back(e.get());
    }

    for (std::size_t i = 0; i < aliveList.size(); ++i) {
        auto& e = aliveList[i];
        ui.println("  " + std::to_string(i + 1) + ". "
                    + e->getName()
                    + "  HP:" + ui.hpBar(e->getHp(), e->getMaxHp(), 15)
                    + "  EXP:" + std::to_string(e->getExpReward())
                    + "  Gold:" + std::to_string(e->getGoldReward()));
    }
    ui.drawLine('-', 48);
    ui.println("  0. 返回主菜单");

    int choice = ui.readChoice(0, (int)aliveList.size());
    if (choice == 0) return nullptr;
    return aliveList[choice - 1];
}

BattleResult BattleSystem::startBattle(Character& player,
                                        std::vector<std::unique_ptr<Enemy>>& enemies) {
    Enemy* target = pickEnemy(enemies);
    if (!target) return BattleResult::RUN;
    Enemy& enemy = *target;

    ui.clearScreen();
    ui.printlnColored(enemy.getBattleCry(), UIColor::RED);
    ui.pause();

    while (true) {
        ui.clearScreen();
        showBattleStatus(player, enemy);

        if (!enemy.isAlive())
            return finishBattle(player, enemy, true);
        if (!player.isAlive())
            return finishBattle(player, enemy, false);

        playerTurn(player, enemy);
        if (!enemy.isAlive())
            return finishBattle(player, enemy, true);

        enemyTurn(player, enemy);
        if (!player.isAlive())
            return finishBattle(player, enemy, false);
    }
}

void BattleSystem::showBattleStatus(const Character& player, const Enemy& enemy) {
    ui.println();
    ui.drawLine('=', 48);
    ui.printlnColored("  >> 回合制战斗 <<", UIColor::CYAN);
    ui.drawLine('-', 48);
    ui.println("  [玩家] " + player.getName()
               + "  Lv." + std::to_string(player.getLevel()));
    ui.println("  HP: " + ui.hpBar(player.getHp(), player.getMaxHp(), 20));
    ui.drawLine('-', 48);
    ui.println("  [敌人] " + enemy.getName()
               + " (" + enemy.getTypeName() + ")");
    ui.println("  HP: " + ui.hpBar(enemy.getHp(), enemy.getMaxHp(), 20));
    ui.drawLine('=', 48);
    ui.println();
}

void BattleSystem::playerTurn(Character& player, Enemy& enemy) {
    std::vector<std::string> actions = {
        "普通攻击",
        "全力攻击（伤害x1.5，20%概率失误）"
    };
    ui.drawMenu("你的回合", actions, 48);
    int choice = ui.readChoice(1, 2);

    if (choice == 1) {
        int dmg = player.getAttack() + (rand() % GameConfig::PLAYER_RAND_MAX);
        ui.showMessage("你发动了普通攻击！");
        int actual = enemy.takeDamage(dmg);
        ui.showMessage("造成 " + std::to_string(dmg) + " 点伤害，"
                        + enemy.getName() + " 受到 " + std::to_string(actual) + " 点");
    } else {
        if (rand() % 100 < 20) {
            ui.showError("失误了！你没有造成任何伤害...");
        } else {
            int dmg = (int)((player.getAttack()
                      + (rand() % GameConfig::PLAYER_RAND_MAX)) * 1.5);
            int actual = enemy.takeDamage(dmg);
            ui.showMessage("全力攻击！造成 " + std::to_string(dmg)
                            + " 点伤害，" + enemy.getName()
                            + " 受到 " + std::to_string(actual) + " 点");
        }
    }
}

void BattleSystem::enemyTurn(Character& player, Enemy& enemy) {
    AttackResult atk = enemy.calculateDamage();
    if (atk.isSkill) {
        ui.printlnColored("  !! " + enemy.getName()
                          + " 发动了 " + atk.skillName + " 技能 !!", UIColor::RED);
    }
    ui.showMessage(enemy.getName() + " 发动了攻击！");
    int actual = player.takeDamage(atk.damage);
    ui.showMessage(player.getName() + " 受到 " + std::to_string(actual)
                    + " 点伤害（原始 " + std::to_string(atk.damage)
                    + "，防御减免 " + std::to_string(atk.damage - actual) + "）");
}

BattleResult BattleSystem::finishBattle(Character& player, Enemy& enemy, bool playerWon) {
    ui.clearScreen();

    if (playerWon) {
        ui.printlnColored("  *** 战斗胜利！***", UIColor::GREEN);
        std::vector<std::string> lines;
        lines.push_back("击败了 " + enemy.getName());
        lines.push_back("经验值 +" + std::to_string(enemy.getExpReward()));
        lines.push_back("金币 +" + std::to_string(enemy.getGoldReward()) + " G");
        ui.drawInfoBox("战利品", lines, 48);

        std::vector<LevelUpEvent> levelUps;
        player.gainExp(enemy.getExpReward(), levelUps);
        player.gainGold(enemy.getGoldReward());

        for (const auto& evt : levelUps) {
            ui.printlnColored("  *** 升级！当前等级：Lv."
                              + std::to_string(evt.newLevel) + " ***", UIColor::YELLOW);
            ui.println("  生命+" + std::to_string(evt.hpGain)
                        + "  攻击+" + std::to_string(evt.atkGain)
                        + "  防御+" + std::to_string(evt.defGain));
        }

        ui.pause();
        return BattleResult::WIN;
    } else {
        ui.printlnColored("  *** 战斗失败！***", UIColor::RED);
        ui.showMessage("你被 " + enemy.getName() + " 击败了...");
        int lostGold = player.getGold() / 2;
        if (player.costGold(lostGold)) {
            ui.showMessage("失去了 " + std::to_string(lostGold) + " 金币...");
        }
        player.setHp(1);
        ui.pause();
        return BattleResult::LOSE;
    }
}
