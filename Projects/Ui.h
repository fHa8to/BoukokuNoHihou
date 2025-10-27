#pragma once
#include "DxLib.h"
#include "Game.h"
#include "Player.h"
#include "BossEnemy.h"

class Ui
{
public:
	Ui();
	virtual ~Ui();

	void Init();
	void Update();
	void TitleDraw();
	void ClearDraw();
	void OverDraw();
	void PlayerDraw(const Player& player);
	void ExplanationDraw();
	void ButtonDraw();
	void BossDraw();
	void End();

	//プレイヤーのHPを取得
	const int& GetPlayerHp() const { return PlayerHp; }
	void SetPlayerHp(const int playerHp) { PlayerHp = playerHp; }

	//プレイヤーのHPを取得
	const int& GetBossHp() const { return BossHp; }
	void SetBossHp(const int bossHp) { BossHp = bossHp; }

	//スキル時間を取得する
	const int& GetSkill() const { return Skill; }
	void SetSkillp(const int skill) { Skill = skill; }


private:

	int m_TitleHandle;
	int m_ClearHandle;
	int m_OverHandle;

	//ボタン
	int m_modelHandle;
	int m_buttonBoxHandle;
	int m_buttonAHandle;
	int m_buttonBHandle;
	int m_buttonXHandle;
	int m_buttonYHandle;
	int m_buttonRBHandle;
	int m_explanationHandle;

	int m_skillHandle;

	int m_PlayerHandle;
	int m_BossHandle;
	int PlayerHp;
	int BossHp;
	int Skill;

	bool BossRange;

};

