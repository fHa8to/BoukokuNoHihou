#pragma once
#include "DxLib.h"
#include "SceneBase.h"

#include "Player.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "BossEnemy.h"
#include "Camera.h"
#include "Stage.h"
#include "SkyDome.h"

class Camera;
class Player;
class Enemy;
class BossEnemy;
class Stage;
class SkyDome;

class SceneGame : public SceneBase
{
public:
	SceneGame();
	virtual ~SceneGame();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;

	void DrawGrid();

	

private:
	std::shared_ptr<Enemy> m_pEnemy;
	std::shared_ptr<BossEnemy> m_pBossEnemy;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<SkyDome> m_pSkyDome;


private:
	//フェードイン、アウト
	int m_fadeAlpha;

	int m_modelHandle;
	int m_buttonBoxHandle;
	int m_buttonAHandle;
	int m_buttonBHandle;
	int m_buttonXHandle;
	int m_buttonYHandle;


	//シーン終了フラグ
	bool m_isSceneEnd;
	bool m_isCommand;

	//
	bool m_isGimmickHit;
	bool m_isGimmickHit1;
	bool m_isEnemyTranslation;
	bool m_isBossEnemyTranslation;
	bool m_isEnemyDeath;
	bool m_isPlayerAttack;
	bool m_isEnemyAttack;
	bool m_isBossEnemyAttack;
	bool m_isBossAttack;
	bool m_isStepOnAttock;
	bool m_isEnemyStop;
	bool m_isBossEnemyStop;

	bool m_isPlayerAttackCoolTime;		//プレイヤーの攻撃クールタイム
	bool m_isPlayerBossAttackCoolTime;		//プレイヤーの攻撃クールタイム
	bool m_isEnemyAttackCoolTime;		//敵の攻撃クールタイム
	bool m_isBossEnemyAttackCoolTime;		//ボスの攻撃クールタイム

	// クールタイムの変数を追加
	int m_playerAttackCoolTimeCounter;
	int m_enemyAttackCoolTimeCounter;
	int m_bossEnemyAttackCoolTimeCounter;

	int m_playerSkillHitFrame;
	int m_enemySkillHitFrame;
	int m_bossEnemySkillHitFrame;

	int m_playerDeath;
	int m_enemyDeath;
	int m_bossEnemyDeath;


	int m_playerFrame;
	int m_enemyFrame;
	int m_bossEnemyFrame;

	VECTOR m_prevPlayerPos;

	//カメラの座標
	VECTOR m_cameraPos;
};

