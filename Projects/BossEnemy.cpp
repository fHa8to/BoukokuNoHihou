#include "BossEnemy.h"
#include "Player.h"
#include "Ui.h"
#include "Stage.h"
#include <cmath>
#include <cassert>


#define D2R(deg) ((deg)* DX_PI_F/180.0f)

namespace
{
	//モデルのサイズ変更
	constexpr float kExpansion = 0.15f;

	//アニメーション番号
	constexpr int kIdleAnimIndex = 0;
	constexpr int kWalkAnimIndex = 1;
	constexpr int kRnuAnimIndex = 2;
	constexpr int kAttackAnimIndex = 3;
	constexpr int kDamageAnimIndex = 4;
	constexpr int kDeadAnimIndex = 5;


	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//敵の速さ
	constexpr float kSpeed = 0.7f;

	//カプセルの座標
	constexpr int upperPart = 15;	//上部
	constexpr int bottom = 2;		//下部

	constexpr int kAttackDelayDuration = 200; // 遅延フレーム数

}


BossEnemy::BossEnemy():
	m_modelHandle(MV1LoadModel("data/model/skeleton/skeleton1.mv1")),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_attackPos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(),
	m_move(VGet(0.0f, 0.0f, 0.0f)),
	m_modelRadius(4.0f),
	m_discoveryRadius(80.0f),
	m_attackRadius(13.0f),
	m_stopRadius(10.0f),
	m_direction(VGet(0, 0, 0)),
	m_isIdle(false),
	m_isAttack(false),
	m_isRnu(false),
	m_isDead(false),
	m_hp(0),
	m_state(kIdle),
	m_kabeNum(0),
	m_yukaNum(0),
	m_attackDelayCounter(0)
{
	m_pos = VGet(200.0f, 300.0f, -60.0f);
	m_move = VGet(0.0f, 0.0f, 0.0f);

	// 各ステートに対応するアニメーションの再生速度を設定
	m_animSpeedMap[State::kIdle] = 1.0f;
	m_animSpeedMap[State::kRun] = 1.0f;
	m_animSpeedMap[State::kAttack] = 1.5f;
	m_animSpeedMap[State::kDamage] = 1.0f;
	m_animSpeedMap[State::kDeath] = 0.8f;

}

BossEnemy::~BossEnemy()
{
}


void BossEnemy::Init()
{
	m_handle = LoadGraph("data/image/GameUI1.png");

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;

	m_attackPos = VGet(m_pos.x, m_pos.y, m_pos.z - 10);

	m_angle = VGet(0.0f, D2R(0.0f), 0.0f);


	m_hp = BOSS_ENEMY_HP_MAX;


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));
}

void BossEnemy::Update(std::shared_ptr<Player> m_pPlayer, std::shared_ptr<Ui> m_pUi, Stage& stage)
{
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

	//座標に移動量を足す
	m_pos = VAdd(m_pos, m_move);

	//重力
	m_move.y -= 0.1;

	if (m_attackDelayCounter > 0)
	{
		m_attackDelayCounter--;
	}
	
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
		if (m_state == kAttack && m_pUi->GetPlayerHp() > 0)
		{
			if (!m_isAttack && m_attackDelayCounter == 0)
			{
				// 攻撃の時
				if (m_state == kAttack && m_pUi->GetPlayerHp() > 0)
				{
					if (!m_isAttack)
					{
						ChangeAnim(kAttackAnimIndex);
					}
					m_isAttack = true;
					m_attackDelayCounter = kAttackDelayDuration; // 攻撃遅延カウンタをリセット
				}
			}
		}

		//待機の時
		if (m_state == kIdle)
		{
			if (!m_isIdle)
			{
				ChangeAnim(kIdleAnimIndex);
			}

			m_animIndex = kIdleAnimIndex;
			m_isIdle = true;

		}
		else
		{
			m_isIdle = false;
		}

		//追いかけている
		if (m_state == kRun)
		{

			//プレイヤーの座標
			VECTOR toTarget = VSub(m_pPlayer->GetPos(), m_pos);

			//プレイヤーの座標に移動
			toTarget = VNorm(toTarget);
			m_distance.x = toTarget.x * kSpeed;
			m_distance.y = 0.0f;
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
		else
		{
			m_isRnu = false;
		}

		if (m_state == kDamage)
		{
			ChangeAnim(kDamageAnimIndex);
			m_isDamage = true;
		}
		else
		{
			m_isDamage = false;
		}
	}
	else
	{
		//攻撃アニメーションが終了したら待機アニメーションを再生する
		if (isLoop)
		{
			m_isAttack = false;
		}
	}

	if (!m_isAttack)
	{

		VECTOR m_attackDir = VScale(m_direction, 50.0f);
		m_attackPos = VAdd(m_pos, m_attackDir);

	}



	//HPをマイナスにさせないため
	if (m_pUi->GetBossHp() <= 0)
	{
		m_pUi->SetBossHp(0);
	}

	if (m_isAttack)
	{
		m_isUnderAttack = true;
	}
	else
	{
		m_isUnderAttack = false;
	}

	CorrectPosition(stage);

	//エネミーモデルの座標
	MV1SetPosition(m_modelHandle, m_pos);
	//エネミーモデルの回転値
	MV1SetRotationXYZ(m_modelHandle, m_angle);
}

void BossEnemy::Draw(std::shared_ptr<Player> m_pPlayer)
{
	//エネミーモデル描画
	MV1DrawModel(m_modelHandle);


#ifdef _DEBUG

	DrawFormatString(0, 40, 0x000000, "Enemy(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

	//カプセル
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + upperPart, m_pos.z), VGet(m_pos.x, m_pos.y + bottom, m_pos.z), m_modelRadius, 8, 0xffffff, 0xffffff, false);

	//攻撃球
	DrawSphere3D(VAdd(m_attackPos, VGet(0, 10, 0)), m_attackRadius, 8, 0xffffff, 0xffffff, false);

	//索敵範囲球
	DrawSphere3D(VAdd(m_pos, VGet(0, 0, 0)), m_discoveryRadius, 10, m_color, m_color, false);

#endif // _DEBUG



}

void BossEnemy::End()
{
	MV1DeleteModel(m_modelHandle);
	m_modelHandle = -1;

}

bool BossEnemy::Translation(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
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

bool BossEnemy::IsAttackColliding(std::shared_ptr<Player> m_pPlayer)
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

bool BossEnemy::IsStopColliding(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
	float delZ = (m_pos.z - m_pPlayer->GetPos().z) * (m_pos.z - m_pPlayer->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_stopRadius + m_pPlayer->GetRadius())
	{

		return true;
	}
	return false;
}

void BossEnemy::CorrectPosition(Stage& stage)
{
	int j;

	OldPos = m_pos;

	m_mapHitColl = VAdd(OldPos, m_move);


	//プレイヤーの周囲にあるステージポリゴンを取得する
	HitDim = MV1CollCheck_Sphere(stage.GetCollisionMap(), -1, stage.GetVectorMapPos(), 1500.0f);

	for (int i = 0; i < HitDim.HitNum; i++)
	{
		if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.0000001f)
		{
			//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
			if (HitDim.Dim[i].Position[0].y > m_pos.y + 1.0f ||
				HitDim.Dim[i].Position[1].y > m_pos.y + 1.0f ||
				HitDim.Dim[i].Position[2].y > m_pos.y + 1.0f)
			{
				//ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (m_kabeNum < PLAYER_MAX_HITCOLL)
				{
					//ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
					Kabe[m_kabeNum] = &HitDim.Dim[i];

					//壁ポリゴンの数を加算する
					m_kabeNum++;
				}
			}
		}
		else
		{
			//ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
			if (m_yukaNum < PLAYER_MAX_HITCOLL)
			{
				//ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
				Yuka[m_yukaNum] = &HitDim.Dim[i];

				//壁ポリゴンの数を加算する
				m_yukaNum++;
			}

		}
	}

	//壁ポリゴンとの当たり判定処理
	if (m_kabeNum != 0)
	{
		m_hitFlag = false;

		if (m_moveFlag == true)
		{
			for (int i = 0; i < m_kabeNum; i++)
			{
				Poly = Kabe[i];

				//ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]) == false) continue;

				//ポリゴンに当たったフラグを立てる
				m_hitFlag = true;

				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < m_kabeNum; j++)
				{
					//j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					Poly = Kabe[j];

					//当たっていたらループから抜ける
					if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
						Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true) break;
				}

				//j が m_kabeNum だった場合はどのポリゴンとも当たらなかったということなので
				//壁に当たったフラグを倒した上でループから抜ける
				if (j == m_kabeNum)
				{
					m_hitFlag = false;
					break;
				}
			}
		}
		else
		{
			//移動していない場合の処理

			//壁ポリゴンの数だけ繰り返し
			for (int i = 0; i < m_kabeNum; i++)
			{
				//i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				Poly = Kabe[i];

				//ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
				if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true)
				{
					m_hitFlag = true;
					break;
				}
			}
		}

		//壁に当たっていたら壁から押し出す処理を行う
		if (m_hitFlag == true)
		{
			for (int i = 0; i < 16; i++)
			{

				//壁ポリゴンの数だけ繰り返し
				for (int k = 0; k < m_kabeNum; k++)
				{
					Poly = Kabe[k];

					//プレイヤーと当たっているかを判定
					if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
						Poly->Position[0], Poly->Position[1], Poly->Position[2]) == false) continue;

					//当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
					m_pos = VAdd(m_pos, VScale(Poly->Normal, kSpeed));

					//移動した上で壁ポリゴンと接触しているかどうかを判定
					for (j = 0; j < m_kabeNum; j++)
					{
						Poly = Kabe[j];
						if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
							Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true) break;
					}

					//全てのポリゴンと当たっていなかったらここでループ終了
					if (j == m_kabeNum) break;

				}
				if (i != m_kabeNum) break;

			}
		}
	}

	if (m_yukaNum != 0)
	{
		if (m_move.y > 0.0f)
		{
			float MinY;

			//天井に頭をぶつける処理を行う

			//一番低い天井にぶつける為の判定用変数を初期化
			MinY = 0.0f;

			//当たったかどうかのフラグを当たっていないを意味する０にしておく
			m_hitFlag = false;

			//床ポリゴンの数だけ繰り返し
			for (int i = 0; i < m_yukaNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				Poly = Yuka[i];

				//足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
				LineRes = HitCheck_Line_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)),
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				//接触していなかったら何もしない
				if (!LineRes.HitFlag) continue;

				//既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
				if (m_hitFlag == 1 && MinY < LineRes.Position.y) continue;

				//ポリゴンに当たったフラグを立てる
				m_hitFlag = true;

				//接触したＹ座標を保存する
				MinY = LineRes.Position.y;
			}

			//接触したポリゴンがあったかどうかで処理を分岐
			if (m_hitFlag)
			{
				//接触した場合はプレイヤーのＹ座標を接触座標を元に更新
				m_mapHitColl.y = MinY - upperPart;

				//Ｙ軸方向の速度は反転
				m_move.y = 0.0f;
			}
		}
		else
		{
			float MaxY;

			//下降中かジャンプ中ではない場合の処理

			//床ポリゴンに当たったかどうかのフラグを倒しておく
			m_hitFlag = false;

			//一番高い床ポリゴンにぶつける為の判定用変数を初期化
			MaxY = 0.0f;

			//床ポリゴンの数だけ繰り返し
			for (int i = 0; i < m_yukaNum; i++)
			{
				//i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				Poly = Yuka[i];

				//ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
				LineRes = HitCheck_Line_Triangle(VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_mapHitColl,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				//当たっていなかったら何もしない
				if (!LineRes.HitFlag) continue;

				//既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (m_hitFlag && MaxY > LineRes.Position.y) continue;

				//ポリゴンに当たったフラグを立てる
				m_hitFlag = true;

				// 接触したＹ座標を保存する
				MaxY = LineRes.Position.y;
			}

			//床ポリゴンに当たったかどうかで処理を分岐
			if (m_hitFlag)
			{
				//当たった場合

				//接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
				m_mapHitColl.y = MaxY;

				//Ｙ軸方向の移動速度は０に
				m_move.y = 0.0f;

			}
		}
	}


	//検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(HitDim);
}

bool BossEnemy::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.5f * m_animSpeed;	//アニメーション進める

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

void BossEnemy::ChangeAnim(int animIndex)
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

	m_animSpeed = m_animSpeedMap[m_state];

}
