#include "DxLib.h"
#include <math.h>
#include <memory>

// 最大ＨＰ
#define ENEMY_HP_MAX		10
// ＨＰ１でどれだけバーを伸ばすか
#define ENEMY_DRAW_SIZE	50

//ライントレースの点数
#define MAX_POINT 6

class Player;
class Enemy
{
public:
	Enemy();
	virtual ~Enemy();

	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer);
	void Draw();
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
		kDeath,		//死
	};

	State GetState() { return m_state; }
	void SetState(State state) { m_state = state; }

	bool Translation(std::shared_ptr<Player> m_pPlayer);

	//攻撃の当たり判定
	bool IsAttackColliding(std::shared_ptr<Player> m_pPlayer);

private:
	//アニメーションの進行
//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);


private:
	//モデルハンドル
	int m_modelHandle;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合


	//表示情報
	VECTOR m_pos;
	VECTOR m_attackPos;
	VECTOR m_angle;
	VECTOR m_playerPos;
	VECTOR m_direction;


	//進む距離
	VECTOR m_distance;

	VECTOR PointPos[MAX_POINT];
	VECTOR PointPos1[MAX_POINT];

	//当たり判定の半径
	float m_modelRadius;
	float m_discoveryRadius;
	float m_attackRadius;

	int TargetNumber;


	//アニメーションフラグ
	int m_isIdle;
	int m_isAttack;
	int m_isRnu;
	int m_isDead;


	//State変数
	State m_state;

	//当たり判定の発生フラグ
	bool m_isUnderAttack;

	int m_color = 0xffffff;

	//HP
	int m_hp;

};