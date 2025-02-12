#pragma once
#include "DxLib.h"
#include "Game.h"
#include <memory>
#include <cmath>
#include <unordered_map>

#define PLAYER_MAX_HITCOLL 21836 //処理するコリジョンポリゴンの最大数

// 最大ＨＰ
#define PLAYER_HP_MAX		25
// ＨＰ１でどれだけバーを伸ばすか
#define PLAYER_DRAW_SIZE	20


class Enemy;
class BossEnemy;
class Stage;

class Player
{
public:
	Player();
	virtual ~Player();

	void Load();
	void Delete();

	void Init();
	void Update(std::shared_ptr<Enemy> m_pEnemy, std::shared_ptr<BossEnemy> m_pBossEnemy, Stage& stage);
	void Draw();

	//カメラの方向を取得
	void SetCameraAngle(float angle) { m_cameraAngle = angle; }

	//プレイヤーの座標を取得
	const VECTOR& GetPos() const { return m_pos; }

	//プレイヤーの座標を取得
	const VECTOR& GetPrevPos() const { return m_prevPos; }

	//プレイヤーのHPを取得
	const int& GetHp() const { return m_hp; }
	void SetHp(const int hp) { m_hp = hp; }


	//攻撃の取得
	const bool& GetUnderAttack() const { return m_isEnemyUnderAttack; }
	//攻撃の取得
	const bool& GetUnderBossAttack() const { return m_isBossUnderAttack; }

	const bool& GetAttack() const { return m_isAttack; }

	//当たり判定の半径
	float GetRadius() { return m_modelRadius; }



	//カプセルの当たり判定
	bool IsEnemyCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy);


	//カプセルの当たり判定
	bool IsBossEnemyCapsuleColliding(std::shared_ptr<BossEnemy> m_pBossEnemy);

	//攻撃の当たり判定(敵)
	bool IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy);


	//攻撃の当たり判定(ボス)
	bool IsBossAttackColliding(std::shared_ptr<BossEnemy> m_pBossEnemy);

	//踏む攻撃の当たり判定
	bool IsStepOnAttockColliding(std::shared_ptr<Enemy> m_pEnemy);

	void CorrectPosition(Stage& stage);

	int SetColor(int color) { return m_color = color; }

	//ダメージのフラグ取得
	void SetDamage(const bool damage) { m_isDamage = damage; }
	//ダメージのフラグ取得
	const bool& GetMove() const { return m_isMove; }


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

	std::unordered_map<State, float> m_animSpeedMap; // 各ステートに対応するアニメーションの再生速度


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
	int m_handle;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度

	float m_angle;

	float m_modelRadius;
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


	//カプセルの点
	VECTOR m_topA;
	VECTOR m_bottomA;
	VECTOR m_topB;
	VECTOR m_bottomB;

	//移動
	VECTOR move;
	float m_analogX;
	float m_analogZ;

	//HP
	int m_hp;
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

	//アニメーションフラグ
	bool m_isStopEnd;	// アニメーション最後で停止させる
	int m_runFrame;


	int m_color = 0xffffff;

	//当たり判定の発生フラグ
	bool m_isEnemyUnderAttack;
	bool m_isBossUnderAttack;


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
};