#include "Skill.h"
#include "DxLib.h"
#include "Player.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include <cmath>
#include <cassert>

namespace
{
	//モデルのファイル名
	const char* const kModelFilename = "data/model/skeleton/Skill1.mv1";

	//モデルの向いてる位置の初期化
	constexpr float kInitAngle = 3.143059f;

	//モデルのサイズ変更
	constexpr float kExpansion = 0.35f;

	//アニメーション番号
	constexpr int kIdleAnimIndex = 0;
	constexpr int kAttack1AnimIndex = 1;
	constexpr int kAttack2AnimIndex = 2;
	constexpr int kAttack3AnimIndex = 3;


	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//カプセルの座標
	constexpr int upperPart = 15;	//上部
	constexpr int bottom = 2;		//下部

	constexpr int kModelRadius = 10.0f;

}

Skill::Skill() :
	m_modelHandle(0),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_headPos(VGet(0.0f, 0.0f, 0.0f)),
	m_attackPos(VGet(0.0f, 0.0f, 0.0f)),
	m_mapHitColl(VGet(0.0f, 0.0f, 0.0f)),
	m_handPos(VGet(0.0f, 0.0f, 0.0f)),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_animSpeed(0.0f),
	m_angle(kInitAngle),
	m_runFrame(0),
	m_nowState(State::kIdle),
	m_backState(State::kIdle),
	m_radius(kModelRadius),
	m_animIndex(),
	m_isAttack(false),
	m_isIdle(false),
	m_isSkill(false),
	m_isStopEnd(false),
	m_currentAttackAnimIndex(kAttack1AnimIndex) // 初期化
{
	// 各ステートに対応するアニメーションの再生速度を設定
	m_animSpeedMap[State::kIdle] = 1.5f;

}

Skill::~Skill()
{
}

void Skill::Init()
{
	m_modelHandle = MV1LoadModel(kModelFilename);

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;

	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

}

void Skill::Update(Player& player)
{


	m_isAttack = player.GetAttack();
	m_isIdle = player.GetIdle();
	m_pos = player.GetPos();
	m_angle = player.GetAngle();


	//アニメーションの切り替え
	if (m_prevAnimNo != -1)
	{
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= 1.0f)	m_animBlendRate = 1.0f;
		//変更前のアニメーション100%
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
		//変更後のアニメーション0%
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);

	}

	//現在のステートを確認
	m_nowState = isGetState();

	//アニメーションを進める
	bool isLoop = UpdateAnim(m_currentAnimNo);
	if (isLoop)
	{
		ChangeAnim(m_animIndex);
	}
	UpdateAnim(m_prevAnimNo);

	/*現在のアニメーション*/
	if (m_nowState == State::kIdle) { IdleAnim(); }		//待機
	if (m_nowState == State::kAttack) { AttackAnim(); }	//攻撃


	//座標に移動量を足す
	m_headPos = MV1GetFramePosition(m_modelHandle, 6);
	m_handPos = MV1GetFramePosition(m_modelHandle, 38);
	m_attackPos = MV1GetFramePosition(m_modelHandle, 39);

	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetRotationXYZ(m_modelHandle, VGet(0, m_angle, 0));

}

void Skill::Draw()
{
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG


	DrawCapsule3D(m_attackPos, m_handPos, m_radius, 8, m_color, m_color, false);


#endif // _DEBUG


}

void Skill::End()
{
}


bool Skill::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.5f * m_animSpeed; // アニメーションを再生速度に応じて進める

	//現在再生中のアニメーションの総カウントを取得
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		if (m_isStopEnd)
		{
			now = total;
		}
		else
		{
			now -= total;
		}
		isLoop = true;

	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void Skill::ChangeAnim(int animIndex)
{
	//さらに古いアニメーションがアタッチされている場合はこの時点で削除しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//現在再生中の待機アニメーションは変更前のアニメーション扱いに
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex, -1, false);

	//切り替えの瞬間は変更後のアニメーションが再生される
	m_animBlendRate = 0.0f;

	//変更前のアニメーション100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);

	// 現在のステートに応じたアニメーションの再生速度を設定
	if (animIndex == kAttack1AnimIndex || animIndex == kAttack2AnimIndex || animIndex == kAttack3AnimIndex)
	{
		m_animSpeed = GetAttackAnimSpeed(animIndex);
	}
	else
	{
		m_animSpeed = m_animSpeedMap[m_nowState];
	}
}


float Skill::GetAttackAnimSpeed(int animIndex)
{
	switch (animIndex)
	{
	case kAttack1AnimIndex:
		return 1.5f; // 攻撃1の再生速度
	case kAttack2AnimIndex:
		return 1.6f; // 攻撃2の再生速度
	case kAttack3AnimIndex:
		return 2.0f; // 攻撃3の再生速度
	default:
		return 1.5f; // デフォルトの再生速度
	}

}


Skill::State Skill::isGetState()
{
	if (m_isAttack)
	{
		return State::kAttack;
	}
	else if (m_isIdle)
	{
		return State::kIdle;
	}

	// どちらでもなければIdleにする（デフォルト）
	return State::kIdle;
}

void Skill::IdleAnim()
{
	//前のステートIdleじゃないなら、アニメーション切り替え
	if (m_backState != State::kIdle)
	{
		ChangeAnim(kIdleAnimIndex);
		m_isStopEnd = false;
	}
	m_animIndex = kIdleAnimIndex;
	m_backState = State::kIdle;

}

void Skill::AttackAnim()
{
	//前のステートAttackじゃないなら、アニメーション切り替え
	if (m_backState != State::kAttack)
	{
		ChangeAnim(m_currentAttackAnimIndex);
		m_isStopEnd = true;

		// 次の攻撃アニメーションに切り替える
		if (m_currentAttackAnimIndex == kAttack1AnimIndex)
		{
			m_currentAttackAnimIndex = kAttack2AnimIndex;
		}
		else if (m_currentAttackAnimIndex == kAttack2AnimIndex)
		{
			m_currentAttackAnimIndex = kAttack3AnimIndex;
		}
		else
		{
			m_currentAttackAnimIndex = kAttack1AnimIndex;
		}
	}
	m_animIndex = m_currentAttackAnimIndex;
	m_backState = State::kAttack;
}
