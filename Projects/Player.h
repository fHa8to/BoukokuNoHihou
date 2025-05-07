#pragma once
#include "DxLib.h"
#include "Game.h"
#include <memory>
#include <cmath>
#include <unordered_map>



class Enemy;
class BossEnemy;
class Stage;
class Ui;
class Skill;
class Explanation;


class Player
{
public:
	Player();
	virtual ~Player();

	void Load();
	void Delete();

	void Init();
	void Update(std::shared_ptr<Enemy> m_pEnemy, std::shared_ptr<BossEnemy> m_pBossEnemy, std::shared_ptr<Ui> m_pUi, std::shared_ptr<Explanation> m_pExplanation, Stage& stage);
	void Draw();

	//カメラの方向を取得
	void SetCameraAngle(float angle) { m_cameraAngle = angle; }

	//プレイヤーの座標を取得
	const VECTOR& GetPos() const { return m_pos; }

	const float& GetAngle() const { return m_angle; }

	//攻撃の取得
	const bool& GetUnderAttack() const { return m_isEnemyUnderAttack; }
	//攻撃の取得
	const bool& GetUnderExplanationAttack() const { return m_isExplanationUnderAttack; }
	//攻撃の取得
	const bool& GetUnderBossAttack() const { return m_isBossUnderAttack; }
	//攻撃の取得
	const bool& GetSkillUnderAttack() const { return m_isSkillEnemyUnderAttack; }
	//攻撃の取得
	const bool& GetSkillUnderBossAttack() const { return m_isSkillBossUnderAttack; }

	float GetSkillEffectDuration() const { return m_effectDuration; }
	float GetSkillCooldown() const { return m_currentCooldown; }
	float GetSkillMaxEffectDuration() const { return 10.0f; } // スキルの最大効果時間
	float GetSkillMaxCooldown() const { return 50.0f; } // スキルの最大クールタイム

	const bool& GetIdle() const { return m_isIdle; }

	const bool& GetAttack() const { return m_isAttack; }

	const bool& GetSkill() const { return m_isSkill; }


	bool IsSkillAttackHit() const { return m_skillAttackHit; }
	void SetSkillAttackHit(bool skillhit) { m_skillAttackHit = skillhit; }

	bool IsAttackHit() const { return m_attackHit; }
	void SetAttackHit(bool hit) { m_attackHit = hit; }


	//当たり判定の半径
	float GetRadius() { return m_modelRadius; }

	bool IsSkillEffectPlayed() const { return m_isSkillEffectPlayed; }
	void SetSkillEffectPlayed(bool val) { m_isSkillEffectPlayed = val; }

	//カプセルの当たり判定(敵)
	bool IsExplanationCapsuleColliding(std::shared_ptr<Explanation> m_pExplanation);

	//カプセルの当たり判定(敵)
	bool IsEnemyCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy);

	//カプセルの当たり判定(ボス)
	bool IsBossEnemyCapsuleColliding(std::shared_ptr<BossEnemy> m_pBossEnemy);

	//攻撃の当たり判定(敵)
	bool IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy);

	//攻撃の当たり判定(ボス)
	bool IsBossAttackColliding(std::shared_ptr<BossEnemy> m_pBossEnemy);

	//攻撃の当たり判定(スキル敵)
	bool IsSkillAttackColliding(std::shared_ptr<Enemy> m_pEnemy);

	//攻撃の当たり判定(スキルボス)
	bool IsSkillBossAttackColliding(std::shared_ptr<BossEnemy> m_pBossEnemy);

	void CorrectPosition(Stage& stage);

	int SetColor(int color) { return m_color = color; }

	//ダメージのフラグ取得
	void SetDamage(const bool damage) { m_isDamage = damage; }
	//ダメージのフラグ取得
	const bool& GetMove() const { return m_isMove; }

	void ActivateSkill();
	void UpdateSkillCooldown();

	//ダメージ座標を取得
	VECTOR& GetDamgePos() { return m_damagePos; }

	enum State
	{
		kIdle,		//待機
		kWalk,		//歩き
		kRun,		//ダッシュ
		kJump,		//ジャンプ
		kAttack,	//攻撃
		kFall,		//落下中
		kDamage,	//ダメージ
		kDeath		//死

	};

	State GetState() { return m_nowState; }
	void SetState(State state) { m_nowState = state; }


private:
	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);

	float GetAttackAnimSpeed(int animIndex);


	std::unordered_map<State, float> m_animSpeedMap; // 各ステートに対応するアニメーションの再生速度
	std::shared_ptr<Skill> m_pSkill;


	/*フラグ*/
	State m_nowState;
	State m_backState;
	State isGetState();


	/*動作*/
	//移動
	void Move();
	//ジャンプ
	void Jump();


	/*アニメーション*/
	//待機
	void IdleAnim();
	//歩き
	void WalkAnim();
	//ダッシュ
	void RunAnim();
	//ジャンプ
	void JumpAnim();
	//落下中
	void FallAnim();
	//攻撃
	void AttackAnim();
	//ダメージ
	void DamageAnim();
	//死
	void DeathAnim();


private:
	//モデルハンドル
	int m_modelHandle;
	int m_modelHandle1;
	int m_handle;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度

	float m_angle;

	float m_modelRadius;
	float m_skillRadius;
	float m_radius;
	float m_attackRadius;

	float m_JumpPower;

	//カメラ情報
	float m_cameraAngle;

	//表示情報
	VECTOR m_pos;
	VECTOR m_prevPos;
	VECTOR m_attackPos;
	VECTOR m_attackDir;
	VECTOR m_move;
	VECTOR m_enemyPos;
	VECTOR m_mapHitColl;    //キャラクターのマップとの当たり判定
	VECTOR m_handPos;
	VECTOR m_swordPos;
	VECTOR m_damagePos;


	//カプセルの点
	VECTOR m_topA;
	VECTOR m_bottomA;
	VECTOR m_topB;
	VECTOR m_bottomB;

	//移動
	VECTOR move;
	float m_analogX;
	float m_analogZ;

	float m_speed;  //キャラのスピード

	//stateフラグ
	bool m_isMove;
	bool m_isAttack;	//攻撃
	bool m_isWalk;		//歩き
	bool m_isRun;		//ダッシュ
	bool m_isJump;		//ジャンプ
	bool m_isIdle;		//待機
	bool m_isFloor;		//落下中
	bool m_isDamage;		//ダメージ
	bool m_isDeath;		//死

	bool m_jumpMove;

	bool m_isSkill;		//スキル

	//アニメーションフラグ
	bool m_isStopEnd;	// アニメーション最後で停止させる
	int m_runFrame;


	int m_color = 0xffffff;

	//当たり判定の発生フラグ
	bool m_isEnemyUnderAttack;
	bool m_isBossUnderAttack;
	bool m_isExplanationUnderAttack;
	bool m_isSkillEnemyUnderAttack;
	bool m_isSkillBossUnderAttack;


	bool m_moveFlag;
	bool m_hitFlag;
	MV1_COLL_RESULT_POLY_DIM HitDim;
	int m_kabeNum;
	int m_yukaNum;
	MV1_COLL_RESULT_POLY* Kabe[PLAYER_MAX_HITCOLL];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* Yuka[PLAYER_MAX_HITCOLL];	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* Poly;				// ポリゴンの構造体にアクセスするために使用するポインタ( 使わなくても済ませられますがプログラムが長くなるので・・・ )
	HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
	VECTOR OldPos;						// 移動前の座標	
	VECTOR NowPos;						// 移動後の座標

	// ダメージ処理用の変数
	int m_damageCounter;
	static constexpr int kDamageDuration = 60; // ダメージアニメーションのフレーム数

	//スキル処理用の変数
	float m_effectDuration; // 効果時間
	float m_cooldownTime;  // クールタイム
	float m_currentCooldown; // 現在のクールタイム
	bool m_isOnCooldown; // クールタイム中かどうか

	bool m_skillAttackHit; // スキル攻撃が当たったかどうかを管理するフラグ
	bool m_attackHit; // スキル攻撃が当たったかどうかを管理するフラグ

	int m_currentAttackAnimIndex; // 現在の攻撃アニメーションのインデックス

	bool m_isSkillEffectPlayed = false;

	bool m_wasRunOnJump = false;

};