#pragma once
#include "DxLib.h"
#include "SceneBase.h"

#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "Stage.h"
#include "SkyDome.h"

class Camera;
class Player;
class Enemy;
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
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<SkyDome> m_pSkyDome;



private:
	//フェードイン、アウト
	int m_fadeAlpha;

	int m_modelHandle;

	//シーン終了フラグ
	bool m_isSceneEnd;
	bool m_isCommand;

	//
	bool m_isGimmickHit;
	bool m_isTranslation;
	bool m_isEnemyDeath;
	bool m_isPlayerAttack;
	bool m_isEnemyAttack;
	bool m_isStepOnAttock;

	bool m_isPlayerAttackCoolTime;		//プレイヤーの攻撃クールタイム
	bool m_isEnemyAttackCoolTime;		//敵の攻撃クールタイム
	int m_playerSkillHitFrame;
	int m_enemySkillHitFrame;

	int m_playerDeath;
	int m_enemyDeath;


	int m_playerFrame;
	int m_enemyFrame;

	//表示情報
	//プレイヤーの座標
	VECTOR m_playerPos;

	//カメラの座標
	VECTOR m_cameraPos;

};

