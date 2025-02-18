#include "DxLib.h"
#include <math.h>
#include <memory>
#include <unordered_map>

#define PLAYER_MAX_HITCOLL 21836 //処理するコリジョンポリゴンの最大数

#define ENEMY_NOW 5  //敵の配列で出す数

// 最大ＨＰ
#define ENEMY_HP_MAX		3
// ＨＰ１でどれだけバーを伸ばすか
#define ENEMY_DRAW_SIZE	30

//ライントレースの点数
#define MAX_POINT 6

class Player;
class Stage;

class Enemy
{
public:
	Enemy();
	virtual ~Enemy();

	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer, Stage& stage);
	void Draw(std::shared_ptr<Player> m_pPlayer);
	void End();

	//エネミーの座標を取得
	VECTOR& GetPos() { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }

	//敵のHPを取得
	const int& GetHp() const { return m_hp; }
	void SetHp(const int hp) { m_hp = hp; }

	float GetRadius() { return m_modelRadius; }

	//攻撃の取得
	const bool& GetUnderAttack() const { return m_isUnderAttack; }

	int SetColor(int color) { return m_color = color; }

	enum State
	{
		kIdle,		//待機
		kRun,		//走る
		kAttack,	//攻撃
		kDamage,	//ダメージ
		kDeath,		//死
	};

	State GetState() { return m_state; }
	void SetState(State state) { m_state = state; }

	bool Translation(std::shared_ptr<Player> m_pPlayer);

	//攻撃の当たり判定
	bool IsAttackColliding(std::shared_ptr<Player> m_pPlayer);

	bool IsStopColliding(std::shared_ptr<Player> m_pPlayer);

	//ステージの当たり判定
	void CorrectPosition(Stage& stage);

	//ダメージのフラグ取得
	void SetDamage(const bool damage) { m_isDamage = damage; }


private:
	//アニメーションの進行
//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);

	std::unordered_map<State, float> m_animSpeedMap; // 各ステートに対応するアニメーションの再生速度

private:
	//モデルハンドル
	int m_modelHandle;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度


	//表示情報
	VECTOR m_pos;
	VECTOR m_attackPos;
	VECTOR m_angle;
	VECTOR m_move;
	VECTOR m_playerPos;
	VECTOR m_direction;
	VECTOR m_mapHitColl;    //キャラクターのマップとの当たり判定


	//進む距離
	VECTOR m_distance;

	VECTOR PointPos[MAX_POINT];
	VECTOR PointPos1[MAX_POINT];

	//当たり判定の半径
	float m_modelRadius;
	float m_discoveryRadius;
	float m_attackRadius;
	float m_stopRadius;

	int TargetNumber;


	//アニメーションフラグ
	int m_isIdle;
	int m_isAttack;
	int m_isRnu;
	int m_isDead;
	int m_isDamage;


	//State変数
	State m_state;

	//当たり判定の発生フラグ
	bool m_isUnderAttack;

	// 攻撃の遅延を管理する変数
	int m_attackDelayCounter;

	int m_color = 0xffffff;

	//HP
	int m_hp;

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