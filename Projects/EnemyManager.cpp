#include "EnemyManager.h"
#include "Enemy.h"

EnemyManager::EnemyManager() : m_enemyNum(0) {}

EnemyManager::~EnemyManager() {}

void EnemyManager::Init()
{
    m_enemies.clear();

    std::shared_ptr<Enemy> enemy1 = std::make_shared<Enemy>();
    enemy1->SetPos(VGet(-193.0f, 170.0f, -190.0f));
    m_enemies.push_back(enemy1);

    std::shared_ptr<Enemy> enemy2 = std::make_shared<Enemy>();
    enemy2->SetPos(VGet(-87.0f, 170.0f, -111.0f));
    m_enemies.push_back(enemy2);

    std::shared_ptr<Enemy> enemy3 = std::make_shared<Enemy>();
    enemy3->SetPos(VGet(-73.0f, 215.0f, 113.0f));
    m_enemies.push_back(enemy3);

    std::shared_ptr<Enemy> enemy4 = std::make_shared<Enemy>();
    enemy4->SetPos(VGet(-154.0f, 220.0f, 84.0f));
    m_enemies.push_back(enemy4);

    m_enemyNum = static_cast<int>(m_enemies.size());
}

void EnemyManager::Update(Stage& stage, Player& player)
{
    // Update logic for each enemy
    for (auto& enemy : m_enemies)
    {
        // Update each enemy
    }
}

void EnemyManager::Draw()
{
    // Draw each enemy
    for (auto& enemy : m_enemies)
    {
        // Draw each enemy
    }
}

void EnemyManager::End()
{
    m_enemies.clear();
    m_enemyNum = 0;
}
