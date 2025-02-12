#include "Enemy.h"
#include "DxLib.h"
#include "Pad.h"
#include "Player.h"

#define D2R(deg) ((deg)* DX_PI_F/180.0f)

//スピード
#define MOVE_SPEED 0.3f

//この距離まで近づいたら次のポイントへ
#define CHECK_LENGTH 2.0f

namespace
{
	//モデルのサイズ変更
	constexpr float kExpansion = 0.1f;

	//アニメーション番号
	constexpr int kIdleAnimIndex = 5;
	constexpr int kWalkAnimIndex = 0;
	constexpr int kRnuAnimIndex = 1;
	constexpr int kAttackAnimIndex = 2;
	constexpr int kDeadAnimIndex = 3;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;


	//敵の速さ
	constexpr float kSpeed = 0.7f;

}

Enemy::Enemy():
	m_modelHandle(MV1LoadModel("data/model/skeleton/skeleton1.mv1")),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_attackPos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(),
	m_modelRadius(4.0f),
	m_discoveryRadius(50.0f),
	m_attackRadius(13.0f),
	m_direction(VGet(0, 0, 0)),
	m_isIdle(false),
	m_isAttack(false),
	m_isRnu(false),
	m_isDead(false),
	m_hp(0)
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kWalkAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;


	m_attackPos = VGet(m_pos.x, m_pos.y, m_pos.z - 10);

	m_angle = VGet(0.0f, D2R(0.0f), 0.0f);

	//移動する座標
	PointPos[0] = VGet(0.0f, 0.0f, 100.0f);
	PointPos[1] = VGet(80.0f, 0.0f, 70.0f);
	PointPos[2] = VGet(80.0f, 0.0f, -70.0f);
	PointPos[3] = VGet(0.0f, 0.0f, -100.0f);
	PointPos[4] = VGet(-80.0f, 0.0f, -70.0f);
	PointPos[5] = VGet(-80.0f, 0.0f, 70.0f);
	



	m_pos = PointPos[0];

	TargetNumber = 1;

	m_hp = ENEMY_HP_MAX;


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

}

void Enemy::Update(std::shared_ptr<Player> m_pPlayer)
{

	Pad::Update();

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


	//アニメーションを進める
	bool isLoop = UpdateAnim(m_currentAnimNo);

	if (isLoop)
	{
		ChangeAnim(m_animIndex);
	}
	UpdateAnim(m_prevAnimNo);
	


	//死んだ時
	if (m_state == kDeath)
	{

		if (!m_isDead)
		{
			m_isDead = true;
			ChangeAnim(kDeadAnimIndex);  // 死亡アニメーション

		}


	}

	if (!m_isAttack)
	{
		//攻撃の時
		if (m_state == kAttack)
		{
			m_isAttack = true;
			ChangeAnim(kAttackAnimIndex);
		}

		//待機の時
		if (m_state == kIdle)
		{

			//ポイント座標
			VECTOR TargetPos = PointPos[TargetNumber];

			//ターゲットの座標までの距離
			VECTOR Vec = VSub(TargetPos, m_pos);
			float Length = VSize(Vec);

			//移動ベクトル
			VECTOR Move = VScale(VNorm(Vec), MOVE_SPEED);

			//移動ベクトルに合わせて向きを変更
			m_angle.y = atan2f(-Move.x, -Move.z);

			//座標を進める
			m_pos = VAdd(m_pos, Move);

			//移動後の座標
			Vec = VSub(TargetPos, m_pos);


			//ポイント座標の距離が近づいたら
			if (VSize(Vec) < CHECK_LENGTH)
			{

				ChangeAnim(kWalkAnimIndex);
				TargetNumber++;
				if (TargetNumber >= MAX_POINT)
				{
					TargetNumber = 0;
				}
			}


			m_animIndex = kWalkAnimIndex;
			m_isIdle = true;

		}

		//追いかけている
		if (m_state == kRun)
		{

			//プレイヤーの座標
			VECTOR toTarget = VSub(m_pPlayer->GetPos(), m_pos);

			//プレイヤーの座標に移動
			toTarget = VNorm(toTarget);
			m_distance.x = toTarget.x * kSpeed;
			m_distance.y = m_pos.y;
			m_distance.z = toTarget.z * kSpeed;


			m_pos = VAdd(m_pos, m_distance);


			//モデルの向きを変える
			VECTOR SubVector = VSub(m_pPlayer->GetPos(), m_pos);

			// atan を使用して角度を取得
			m_angle.y = atan2f(-SubVector.x, -SubVector.z);


			m_attackPos = VAdd(m_pos, m_distance);

			//プレイヤーの方向を向く
			MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angle.y + DX_PI_F, 0.0f));

			if (!m_isRnu)
			{
				ChangeAnim(kRnuAnimIndex);
			}
			m_animIndex = kRnuAnimIndex;

			m_isRnu = true;

		}
	}
	else
	{
		//攻撃アニメーションが終了したら待機アニメーションを再生する
		if (isLoop)
		{
			m_isIdle = false;
			m_isAttack = false;
			m_isRnu = false;
		}
	}

	if (!m_isAttack)
	{

		VECTOR m_attackDir = VScale(m_direction, 50.0f);
		m_attackPos = VAdd(m_pos, m_attackDir);

	}


	//HPをマイナスにさせないため
	if (m_hp <= 0) m_hp = 0;

	if (m_isAttack)
	{
		m_isUnderAttack = true;
	}
	else
	{
		m_isUnderAttack = false;
	}


	//エネミーモデルの座標
	MV1SetPosition(m_modelHandle, m_pos);
	//エネミーモデルの回転値
	MV1SetRotationXYZ(m_modelHandle, m_angle);
}

void Enemy::Draw()
{	
	//エネミーモデル描画
	MV1DrawModel(m_modelHandle);

	//HPバー
	DrawBox(696, 9, 704 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 44, 0x000000, true);
	DrawBox(697, 10, 703 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 43, 0xffffff, true);
	DrawBox(699, 12, 701 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 41, 0x000000, true);
	DrawBox(700, 13, 700 + m_hp * ENEMY_DRAW_SIZE, 40, 0xff0000, true);


#ifdef _DEBUG

	//エネミーの移動線
	for (int i = 0; i < MAX_POINT; i++)
	{
		int n1 = i;
		int n2 = (i + 1) % MAX_POINT;
		DrawLine3D(PointPos[n1], PointPos[n2], GetColor(0, 255, 0));
	}

	DrawFormatString(0, 40, 0x000000, "Enemy(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

	//カプセル
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 15, m_pos.z), VGet(m_pos.x, m_pos.y + 2, m_pos.z), m_modelRadius, 8, 0xffffff, 0xffffff, false);

	//攻撃球
	DrawSphere3D(VAdd(m_attackPos, VGet(0, 10, 0)), m_attackRadius, 8, 0xffffff, 0xffffff, false);
	
	//索敵範囲球
	DrawSphere3D(VAdd(m_pos, VGet(0, 0, 0)), m_discoveryRadius, 10, m_color, m_color, false);

#endif // _DEBUG



}

void Enemy::End()
{
	MV1DeleteModel(m_modelHandle);
	m_modelHandle = -1;
}

//索敵範囲当たり判定
bool Enemy::Translation(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y  - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
	float delZ = (m_pos.z - m_pPlayer->GetPos().z) * (m_pos.z - m_pPlayer->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_discoveryRadius + m_pPlayer->GetRadius())
	{

		return true;
	}

	return false;
}

bool Enemy::IsAttackColliding(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
	float delZ = (m_pos.z - m_pPlayer->GetPos().z) * (m_pos.z - m_pPlayer->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_attackRadius + m_pPlayer->GetRadius())
	{

		return true;
	}

	return false;
}

bool Enemy::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.5f;	//アニメーション進める

	//現在再生中のアニメーションの総カウントを取得
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		now -= total;
		isLoop = true;
	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void Enemy::ChangeAnim(int animIndex)
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
}
