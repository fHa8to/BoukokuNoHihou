#include "Ui.h"
#include "Player.h"
#include "BossEnemy.h"

namespace
{
	const char* const kButtonA = "data/image/buttonA.png";
	const char* const kButtonB = "data/image/buttonB.png";
	const char* const kButtonX = "data/image/buttonX.png";
	const char* const kButtonY = "data/image/buttonY.png";

}

Ui::Ui():
	PlayerHandle(),
	BossHandle(),
	PlayerHp(0),
	BossHp(0),
	BossRange(false)
{
	TitleHandle = LoadGraph("data/image/GameTitle.png");
	
	m_buttonBoxHandle = LoadGraph("data/image/buttonBox.png");

	m_buttonAHandle = LoadGraph(kButtonA);
	m_buttonBHandle = LoadGraph(kButtonB);
	m_buttonXHandle = LoadGraph(kButtonX);
	m_buttonYHandle = LoadGraph(kButtonY);

	m_skillHandle = LoadGraph("data/image/skeleton1.png");

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

void Ui::PlayerDraw(const Player& player)
{
	// HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(39, 50, 42 + PLAYER_HP_MAX * PLAYER_DRAW_SIZE, 80, 0x000000, true);
	DrawBox(40, 50, 40 + PlayerHp * PLAYER_DRAW_SIZE, 80, 0x26b609, true);

	// スキルの効果時間バー
	float effectDuration = player.GetSkillEffectDuration();
	float maxEffectDuration = player.GetSkillMaxEffectDuration();
	int effectBarWidth = static_cast<int>((effectDuration / maxEffectDuration) * 150); // バーの幅を200ピクセルに設定

	// スキルのクールタイムバー
	float cooldown = player.GetSkillCooldown();
	float maxCooldown = player.GetSkillMaxCooldown();
	int cooldownBarWidth = static_cast<int>((cooldown / maxCooldown) * 150); // バーの幅を200ピクセルに設定

	// HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(Game::kScreenWidth / 5 - 200, Game::kScreenHeight  / 8, Game::kScreenWidth / 5 - 95, Game::kScreenHeight / 8 + 150, 0x00ff00, true);
	DrawBox(Game::kScreenWidth / 5 - 200, Game::kScreenHeight  / 8, Game::kScreenWidth / 5 - 95, (Game::kScreenHeight / 8) + cooldownBarWidth, 0xffffff, true);

	// HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(Game::kScreenWidth / 5 - 200, (Game::kScreenHeight / 8 + 150) - effectBarWidth, Game::kScreenWidth / 5 - 95, Game::kScreenHeight / 8 + 150, 0xff0000, true);


	//UIの画像を描画
	DrawGraph(23, 35, PlayerHandle, true);

	DrawGraph(Game::kScreenWidth - 215, Game::kScreenHeight - 255, m_buttonBoxHandle, true);

	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 250, m_buttonAHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 200, m_buttonBHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 150, m_buttonXHandle, true);

	DrawGraph(Game::kScreenWidth / 5 - 200, Game::kScreenHeight / 8, m_skillHandle, true);

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

