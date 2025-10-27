#pragma once
#include "DxLib.h"
#include <math.h>
#include <memory>
#include <unordered_map>

class Player;
class Enemy;
class BossEnemy;

class EffectManager
{
public:

	EffectManager();
	~EffectManager();
	void Init();
	void Update();
	void Draw();
	void End();

	void ResetFlags();

	//プレイヤーの攻撃エフェクト
	void DrawPlayerAttackEffect();

	//プレイヤーのスキルエフェクト
	void DrawPlayerSkillEffect(std::shared_ptr<Player> m_pPlayer);

	//プレイヤーのダメージエフェクト
	void DrawPlayerDamageEffect(std::shared_ptr<Player> m_pPlayer);

	//エネミーのダメージエフェクト
	void DrawEnemyDamageEffect(std::shared_ptr<Enemy> m_pEnemy);

	//ボスの攻撃エフェクト
	void DrawBossEnemyAttackEffect(std::shared_ptr<BossEnemy> m_pBossEnemy);

	//ボスのダメージエフェクト
	void DrawBossEnemyDamageEffect(std::shared_ptr<BossEnemy> m_pBossEnemy);

	void DrawArrowEffect();

	bool IsEffekseerEffectPlaying(int handle);

private:

	//表示情報
	VECTOR m_pos;


	//プレイヤーがダメージを受けた時のエフェクト
	int m_playerDamageEffectHandle;

	//敵がダメージを受けた時のエフェクト
	int m_enemyDamageEffectHandle;

	//ボスがダメージを受けた時のエフェクト
	int m_bossEnemyDamageEffectHandle;

	//プレイヤーのスキルを使った時のエフェクト
	int m_playerSkillEffectHandle;

	//ボスの攻撃を使った時のエフェクト
	int m_bossEnemySkillEffectHandle;

	//矢印のエフェクト
	int m_arrowEffecthandle;


	//プレイヤーがダメージを受けた時エフェクトを再生する時間
	float m_playerDamageEffectTime;

	//敵がダメージを受けた時のエフェクトを再生する時間
	float m_enemyDamageEffectTime;

	//ボスがダメージを受けた時のエフェクトを再生する時間
	float m_bossEnemyDamageEffectTime;

	//プレイヤーのスキルを使った時のエフェクトを再生する時間
	float m_playerSkillEffectTime;

	//ボスの攻撃を使った時のエフェクトを再生する時間
	float m_bossEnemySkillEffectTime;


	float m_arrowEffectTime;


	float m_arrowTime;



	//ダメージエフェクトが再生されているか
	bool m_isPlayerDamageEffect;

	//プレイヤーのスキルのエフェクトが再生されているか
	bool m_isPlayerSkillEffect;

	//敵がダメージを受けた時のエフェクトが再生されているか
	bool m_isEnemyDamageEffect;

	//ボスがダメージを受けた時のエフェクトが再生されているか
	bool m_isBossEnemyDamageEffect;


	//ボスの攻撃を使った時のエフェクトが再生されているか
	bool m_isBossEnemySkillEffect;


	bool m_isArrowEffect;

};

