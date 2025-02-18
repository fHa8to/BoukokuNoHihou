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
	void PlayerDraw();
	void BossDraw();
	void End();

	//�v���C���[��HP���擾
	const int& GetPlayerHp() const { return PlayerHp; }
	void SetPlayerHp(const int playerHp) { PlayerHp = playerHp; }

	//�v���C���[��HP���擾
	const int& GetBossHp() const { return BossHp; }
	void SetBossHp(const int bossHp) { BossHp = bossHp; }


private:
	int TitleHandle;

	//�{�^��
	int m_modelHandle;
	int m_buttonBoxHandle;
	int m_buttonAHandle;
	int m_buttonBHandle;
	int m_buttonXHandle;
	int m_buttonYHandle;

	int PlayerHandle;
	int BossHandle;
	int PlayerHp;
	int BossHp;

	bool BossRange;
};

