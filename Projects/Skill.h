#pragma once
#include "DxLib.h"
#include "Game.h"
#include <memory>
#include <cmath>
#include <unordered_map>
#include "Player.h"

class Player;

class Skill
{
public:
	Skill();
	virtual ~Skill();

	void Init();
	void Update(Player& player);
	void Draw();
	void End();

	//プレイヤーの座標を取得
	VECTOR& GetPos() { return m_pos; }
	void SetPos(const VECTOR pos) { m_pos = pos; }

	VECTOR& GetHeadPos() { return m_headPos; }

	//プレイヤーの手座標を取得
	VECTOR& GetHandPos() { return m_handPos; }
	VECTOR& GetAttackPos() { return m_attackPos; }

	const bool& GetAttackSkill() const { return m_isAttack; }
	const bool& GetIdleSkill() const { return m_isIdle; }

	int SetColor(int color) { return m_color = color; }
	
	//当たり判定の半径
	float GetRadius() { return m_radius; }


	enum State
	{
		kIdle,		//待機
		kAttack,	//攻撃

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

	/*フラグ*/
	State m_nowState;
	State m_backState;
	State isGetState();

	/*アニメーション*/
	//待機
	void IdleAnim();
	//攻撃
	void AttackAnim();


private:

	//モデルハンドル
	int m_modelHandle;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度

	float m_angle;

	float m_radius;

	//表示情報
	VECTOR m_pos;
	VECTOR m_headPos;
	VECTOR m_attackPos;
	VECTOR m_mapHitColl;    //キャラクターのマップとの当たり判定
	VECTOR m_handPos;

	//stateフラグ
	bool m_isAttack;	//攻撃
	bool m_isIdle;		//待機
	bool m_isSkill;		//スキル	


	//アニメーションフラグ
	bool m_isStopEnd;	// アニメーション最後で停止させる
	int m_runFrame;

	int m_color = 0xffffff;

	int m_currentAttackAnimIndex; // 現在の攻撃アニメーションのインデックス

};