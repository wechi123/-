#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "IGameUI.h"
#include "Character.h"
#include "Enemy.h"
#include <vector>
#include <memory>

enum class BattleResult { WIN, LOSE, RUN };

class BattleSystem {
private:
    IGameUI& ui;

public:
    BattleSystem(IGameUI& uiRef);

    BattleResult startBattle(Character& player,
                             std::vector<std::unique_ptr<Enemy>>& enemies);

private:
    Enemy* pickEnemy(std::vector<std::unique_ptr<Enemy>>& enemies);
    void showBattleStatus(const Character& player, const Enemy& enemy);
    void playerTurn(Character& player, Enemy& enemy);
    void enemyTurn(Character& player, Enemy& enemy);
    BattleResult finishBattle(Character& player, Enemy& enemy, bool playerWon);
};

#endif
