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
	void PlayerDraw(const Player& player);
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
	int TitleHandle;

	//ボタン
	int m_modelHandle;
	int m_buttonBoxHandle;
	int m_buttonAHandle;
	int m_buttonBHandle;
	int m_buttonXHandle;
	int m_buttonLBHandle;

	int m_skillHandle;

	int PlayerHandle;
	int BossHandle;
	int PlayerHp;
	int BossHp;
	int Skill;

	bool BossRange;

};

