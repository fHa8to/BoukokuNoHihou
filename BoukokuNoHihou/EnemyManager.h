#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>

class Player;
class Enemy;
class Stage;

class EnemyManager
{
public:
    EnemyManager();
    virtual ~EnemyManager();

    void Init();

    void Update(Stage& stage, Player& player);

    void Draw();

    void End();

    int GetEnemyNum() const { return m_enemyNum; }

private:
    int m_enemyNum;
    std::vector<std::shared_ptr<Enemy>> m_enemies;
};