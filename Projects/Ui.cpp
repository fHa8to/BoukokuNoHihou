#include "Ui.h"
#include "Player.h"
#include "BossEnemy.h"

Ui::Ui():
	PlayerHandle(),
	BossHandle(),
	PlayerHp(0),
	BossHp(0)
{
	TitleHandle = LoadGraph("data/image/GameTitle.png");
	
	m_buttonBoxHandle = LoadGraph("data/image/buttonBox.png");

	m_buttonAHandle = LoadGraph("data/image/buttonA.png");
	m_buttonBHandle = LoadGraph("data/image/buttonB.png");
	m_buttonXHandle = LoadGraph("data/image/buttonX.png");
	m_buttonYHandle = LoadGraph("data/image/buttonY.png");

	PlayerHandle = LoadGraph("data/image/GameUI.png");
	BossHandle = LoadGraph("data/image/GameUI1.png");

}

Ui::~Ui()
{
}

void Ui::Init()
{
	PlayerHp = PLAYER_HP_MAX;
	BossHp = BOSS_ENEMY_HP_MAX;
}

void Ui::Update()
{
}

void Ui::TitleDraw()
{
	DrawGraph(Game::kScreenWidth / 4, Game::kScreenHeight / 6, TitleHandle, true);

}

void Ui::PlayerDraw()
{
	// HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(39, 50, 42 + PLAYER_HP_MAX * PLAYER_DRAW_SIZE, 80, 0x000000, true);
	DrawBox(40, 50, 40 + PlayerHp * PLAYER_DRAW_SIZE, 80, 0x26b609, true);

	//UIの画像を描画
	DrawGraph(23, 35, PlayerHandle, true);

	DrawGraph(Game::kScreenWidth - 215, Game::kScreenHeight - 255, m_buttonBoxHandle, true);

	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 250, m_buttonAHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 200, m_buttonBHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 150, m_buttonXHandle, true);


}

void Ui::BossDraw()
{
	//HPバー
	//HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(Game::kScreenWidth / 2 - 410, Game::kScreenHeight - 180, Game::kScreenWidth / 2 - 410 + BOSS_ENEMY_HP_MAX * BOSS_ENEMY_DRAW_SIZE, Game::kScreenHeight - 150, 0x000000, true);
	DrawBox(Game::kScreenWidth / 2 - 410, Game::kScreenHeight - 180, Game::kScreenWidth / 2 - 410 + BossHp * BOSS_ENEMY_DRAW_SIZE, Game::kScreenHeight - 150, 0xff0000, true);

	//UIの画像を描画
	DrawGraph(Game::kScreenWidth / 2 - 427, Game::kScreenHeight - 200, BossHandle, true);

}


void Ui::End()
{
	DeleteGraph(PlayerHandle);
	DeleteGraph(BossHandle);
	DeleteGraph(TitleHandle);
	DeleteGraph(m_buttonBoxHandle);
	DeleteGraph(m_buttonAHandle);
	DeleteGraph(m_buttonBHandle);
	DeleteGraph(m_buttonXHandle);

}
