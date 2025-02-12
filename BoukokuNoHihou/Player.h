#pragma once
#include "DxLib.h"
#include "Game.h"
#include <memory>
#include <cmath>

#define PLAYER_MAX_HITCOLL 21872 //処理するコリジョンポリゴンの最大数

// 最大ＨＰ
#define PLAYER_HP_MAX		50
// ＨＰ１でどれだけバーを伸ばすか
#define PLAYER_DRAW_SIZE	10


class Enemy;
class Stage;

class Player
{
public:
	Player();
	virtual ~Player();

	void Load();
	void Delete();

	void Init();
	void Update(Stage& stage);
	void Draw();

	//カメラの方向を取得
	void SetCameraAngle(float angle) { m_cameraAngle = angle; }

	//プレイヤーの座標を取得
	const VECTOR& GetPos() const { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }

	//プレイヤーのHPを取得
	const int& GetHp() const { return m_hp; }
	void SetHp(const int hp) { m_hp = hp; }


	//攻撃の取得
	const bool& GetUnderAttack() const { return m_isUnderAttack; }


	void SetIsDown(const bool isDeath) { m_isDeath = isDeath; }

	//当たり判定の半径
	float GetRadius() { return m_modelRadius; }


	//カプセルの当たり判定
	bool IsCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy);

	//攻撃の当たり判定
	bool IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy);

	//踏む攻撃の当たり判定
	bool IsStepOnAttockColliding(std::shared_ptr<Enemy> m_pEnemy);

	void CorrectPosition(Stage& stage);

	int SetColor(int color) { return m_color = color; }


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

private:
	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);


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

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合

	float m_angle;

	float m_modelRadius;
	float m_radius;
	float m_attackRadius;

	float m_JumpPower;

	//カメラ情報
	float m_cameraAngle;

	//表示情報
	VECTOR m_pos;
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
	bool m_isDamag;		//ダメージ
	bool m_isDeath;		//死

	//アニメーションフラグ
	bool m_isStopEnd;	// アニメーション最後で停止させる
	int m_runFrame;


	int m_color = 0xffffff;

	//当たり判定の発生フラグ
	bool m_isUnderAttack;


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
	VECTOR NowPos;						// 移動後の座標};
};