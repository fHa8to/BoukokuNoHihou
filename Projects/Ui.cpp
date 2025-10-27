#include "Ui.h"
#include "Player.h"
#include "BossEnemy.h"

namespace
{
	/*UIの画像データ*/
	//ロゴ
	const char* const kTitle = "data/image/GameTitle.png";
	const char* const kClear = "data/image/GameClear.png";
	const char* const kOver = "data/image/GameOver.png";
	//ボタン
	const char* const kButtonA = "data/image/buttonA.png";
	const char* const kButtonB = "data/image/buttonB.png";
	const char* const kButtonX = "data/image/buttonX.png";
	const char* const kButtonY = "data/image/buttonY.png";
	const char* const kButtonRB = "data/image/buttonRB.png";
	const char* const kButtonkFrame = "data/image/buttonBox.png";
	//説明
	const char* const kExplanation = "data/image/explanation.png";
	//HPバー
	const char* const kFramePlayer = "data/image/GameUI.png";
	const char* const kFrameBoss = "data/image/GameUI1.png";
	//スキル
	const char* const kSkill = "data/image/skeleton1.png";


}

Ui::Ui() :
	m_TitleHandle(),
	m_ClearHandle(),
	m_OverHandle(),
	m_buttonBoxHandle(),
	m_buttonAHandle(),
	m_buttonBHandle(),
	m_buttonXHandle(),
	m_buttonYHandle(),
	m_buttonRBHandle(),
	m_skillHandle(),
	m_PlayerHandle(),
	m_BossHandle(),
	m_explanationHandle(),
	PlayerHp(0),
	BossHp(0),
	BossRange(false)
{
	m_TitleHandle = LoadGraph(kTitle);
	m_ClearHandle = LoadGraph(kClear);
	m_OverHandle = LoadGraph(kOver);
	
	m_buttonBoxHandle = LoadGraph(kButtonkFrame);

	m_buttonAHandle = LoadGraph(kButtonA);
	m_buttonBHandle = LoadGraph(kButtonB);
	m_buttonXHandle = LoadGraph(kButtonX);
	m_buttonYHandle = LoadGraph(kButtonY);
	m_buttonRBHandle = LoadGraph(kButtonRB);

	m_skillHandle = LoadGraph(kSkill);

	m_PlayerHandle = LoadGraph(kFramePlayer);
	m_BossHandle = LoadGraph(kFrameBoss);

	m_explanationHandle = LoadGraph(kExplanation);

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
	DrawGraph(Game::kScreenWidth / 2 - 285, Game::kScreenHeight / 6, m_TitleHandle, true);

}

void Ui::ClearDraw()
{
	DrawGraph(Game::kScreenWidth / 2 - 270, Game::kScreenHeight / 3, m_ClearHandle, true);
}

void Ui::OverDraw()
{
	DrawGraph(Game::kScreenWidth / 2 - 270, Game::kScreenHeight / 3, m_OverHandle, true);
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
	DrawGraph(23, 35, m_PlayerHandle, true);

	DrawGraph(Game::kScreenWidth - 215, Game::kScreenHeight - 305, m_buttonBoxHandle, true);


	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 300, m_buttonRBHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 250, m_buttonAHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 200, m_buttonBHandle, true);
	DrawGraph(Game::kScreenWidth - 200, Game::kScreenHeight - 150, m_buttonXHandle, true);

	DrawGraph(Game::kScreenWidth / 5 - 200, Game::kScreenHeight / 8, m_skillHandle, true);

}

void Ui::ExplanationDraw()
{
	DrawGraph(0, 0, m_explanationHandle, true);

}

void Ui::ButtonDraw()
{
	DrawGraph(Game::kScreenWidth / 2, Game::kScreenHeight / 2, m_buttonYHandle, true);
}

void Ui::BossDraw()
{
	//HPバー
	//HP の値分の大きさだが四角に収まるように値を大きくします
	DrawBox(Game::kScreenWidth / 2 - 410, Game::kScreenHeight - 180, Game::kScreenWidth / 2 - 410 + BOSS_ENEMY_HP_MAX * BOSS_ENEMY_DRAW_SIZE, Game::kScreenHeight - 150, 0x000000, true);
	DrawBox(Game::kScreenWidth / 2 - 410, Game::kScreenHeight - 180, Game::kScreenWidth / 2 - 410 + BossHp * BOSS_ENEMY_DRAW_SIZE, Game::kScreenHeight - 150, 0xff0000, true);

	//UIの画像を描画
	DrawGraph(Game::kScreenWidth / 2 - 427, Game::kScreenHeight - 200, m_BossHandle, true);

}


void Ui::End()
{
	DeleteGraph(m_TitleHandle);
	DeleteGraph(m_ClearHandle);
	DeleteGraph(m_OverHandle);
	DeleteGraph(m_PlayerHandle);
	DeleteGraph(m_BossHandle);
	DeleteGraph(m_buttonBoxHandle);
	DeleteGraph(m_buttonAHandle);
	DeleteGraph(m_buttonBHandle);
	DeleteGraph(m_buttonXHandle);
	DeleteGraph(m_buttonYHandle);

}

