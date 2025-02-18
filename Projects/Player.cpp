#include "Player.h"
#include "DxLib.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include "Pad.h"
#include "Stage.h"
#include "Ui.h"
#include <cmath>
#include <cassert>


namespace
{
	//モデルのファイル名
	const char* const kModelFilename = "data/model/ArcticTextures/player.mv1";

	//モデルの向いてる位置の初期化
	constexpr float kInitAngle = 3.143059f;

	//モデルのサイズ変更
	constexpr float kExpansion = 0.1f;

	//アニメーション番号
	constexpr int kIdleAnimIndex = 1;
	constexpr int kWalkAnimIndex = 2;
	constexpr int kRnuAnimIndex = 3;
	constexpr int kJumpAnimIndex = 9;
	constexpr int kAttackAnimIndex = 5;
	constexpr int kFallingAnimIndex = 0;
	constexpr int kDeathAnimIndex = 10;
	constexpr int kDamageAnimIndex = 11;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//アナログスティックによる移動関連
	constexpr float kMaxSpeed = 0.4f;		//プレイヤーの最大移動速度
	constexpr float kAnalogRaneMin = 0.4f;	//アナログスティックの入力判定範囲
	constexpr float kAnalogRaneMax = 0.8f;
	constexpr float kAnglogInputMax = 1000.0f;	//アナログスティックの入力されるベクトルに

	//カプセルの座標
	constexpr int upperPart = 15;	//上部
	constexpr int bottom = 2;		//下部

}

Player::Player() :
	m_modelHandle(0),
	m_handle(0),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_attackPos(VGet(0.0f, 0.0f, 0.0f)),
	m_mapHitColl(VGet(0.0f, 0.0f, 0.0f)),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_animSpeed(0.0f),
	m_angle(kInitAngle),
	m_modelRadius(4.0f),
	m_radius(6.0f),
	m_attackRadius(5.0f),
	m_analogX(0.0f),
	m_analogZ(0.0f),
	m_runFrame(0),
	m_move(),
	m_nowState(State::kIdle),
	m_backState(State::kIdle),
	m_isAttack(false),
	m_isWalk(false),
	m_isRun(false),
	m_isJump(false),
	m_isIdle(false),
	m_isFloor(false),
	m_isDeath(false),
	m_isEnemyUnderAttack(false),
	m_isBossUnderAttack(false),
	m_isDamage(false),
	m_moveFlag(false),
	m_hitFlag(false),
	m_kabeNum(0),
	m_yukaNum(0)
{
	// 各ステートに対応するアニメーションの再生速度を設定
	m_animSpeedMap[State::kIdle] = 1.0f;
	m_animSpeedMap[State::kWalk] = 1.2f;
	m_animSpeedMap[State::kRun] = 1.3f;
	m_animSpeedMap[State::kJump] = 1.0f;
	m_animSpeedMap[State::kAttack] = 1.5f;
	m_animSpeedMap[State::kDamage] = 1.0f;
	m_animSpeedMap[State::kDeath] = 0.8f;
}

Player::~Player()
{
}

void Player::Load()
{
	m_modelHandle = MV1LoadModel(kModelFilename);
	m_handle = LoadGraph("data/image/GameUI.png");
}

void Player::Delete()
{
	MV1DeleteModel(m_modelHandle);
}

void Player::Init()
{

	//プレイヤースピード初期化
	m_speed = kMaxSpeed;

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;

	//プレイヤーの初期位置設定
	m_pos = VGet(0.0f, 5.0f, -20.0f);
	m_move = VGet(0.0f, 0.0f, 0.0f);


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

}

void Player::Update(std::shared_ptr<Enemy> m_pEnemy, std::shared_ptr<BossEnemy> m_pBossEnemy, std::shared_ptr<Ui> m_pUi, Stage& stage)
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
		if (!m_isStopEnd) ChangeAnim(m_animIndex);
	}
	UpdateAnim(m_prevAnimNo);

	//現在のステートを確認
	m_nowState = isGetState();

	m_prevPos = m_pos;

	//移動処理
	if(!m_isDamage && (m_nowState == State::kIdle) || (m_nowState == State::kWalk) || (m_nowState == State::kRun))
	{ 
		Move(); 
		m_isMove = true;
	}
	else
	{
		m_isMove = false;
	}


	//Jump
	if ((m_nowState != State::kJump) && (m_nowState != State::kDeath))
	{
		if (Pad::IsPress(PAD_INPUT_1))
		{
			Jump();
		}
	}
	
	//Attack
	if ((m_nowState != State::kAttack) && (m_nowState != State::kDamage))
	{
		if (Pad::IsPress(PAD_INPUT_3))
		{
			m_isAttack = true;
		}
	}


	// ダッシュアニメーションの制御
	if (Pad::IsPress(PAD_INPUT_2) && m_move.y == 0)
	{
		m_runFrame++;
		if (m_runFrame > 5)
		{
			m_isRun = true;

			//動くスピード
			m_move = VScale(m_move, 1.5f);

		}
	}
	else
	{
		m_isRun = false;
		m_runFrame = 0;
	}


	/*現在のアニメーション*/
	if (m_nowState == State::kIdle) { IdleAnim(); }		//待機
	if (m_nowState == State::kWalk) { WalkAnim(); }		//歩き
	if (m_nowState == State::kRun) { RunAnim(); }		//ダッシュ
	if (m_nowState == State::kJump) { JumpAnim(); }		//ジャンプ
	if (m_nowState == State::kAttack) { AttackAnim(); }	//攻撃
	if (m_nowState == State::kDamage) { DamageAnim(); }	//ダメージ
	if (m_nowState == State::kDeath) { DeathAnim(); }	//死


	//座標に移動量を足す
	m_pos = VAdd(m_pos, m_move);

	//重力
	m_move.y -= 0.1;

	// 敵の攻撃による吹っ飛びを防ぐための処理
	if (m_isDamage)
	{
		// ダメージを受けた際の移動量を制限
		m_move = VScale(m_move, 0.0f);
	}

	if (m_isAttack)
	{
		if (IsAttackColliding(m_pEnemy))
		{
			m_isEnemyUnderAttack = true;
		}
		if (IsBossAttackColliding(m_pBossEnemy))
		{
			m_isBossUnderAttack = true;
		}
	}
	else
	{
		m_isEnemyUnderAttack = false;
		m_isBossUnderAttack = false;
	}



	//HPをマイナスにさせないため
	if (m_pUi->GetPlayerHp() <= 0)
	{
		m_pUi->SetPlayerHp(0);
		m_isDeath = true;
	}

	CorrectPosition(stage);

	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetRotationXYZ(m_modelHandle, VGet(0, m_angle, 0));
}

void Player::Draw()
{
	MV1DrawModel(m_modelHandle);


#ifdef _DEBUG

	DrawFormatString(0, 20, 0xffffff, "Player(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);
	DrawFormatString(0, 60, 0x000000, "PlayerState%d", m_nowState);

	//当たり判定カプセル
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + upperPart, m_pos.z), VGet(m_pos.x, m_pos.y + bottom, m_pos.z), m_modelRadius, 8, m_color, m_color, false);

	DrawSphere3D(VAdd(m_attackPos, VGet(0, 10, 0)), m_attackRadius, 8, 0xffffff, 0xffffff, false);

#endif // _DEBUG


}

bool Player::UpdateAnim(int attachNo)
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
		m_isAttack = false;
		m_isDamage = false;
	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void Player::ChangeAnim(int animIndex)
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
	m_animSpeed = m_animSpeedMap[m_nowState];

}

Player::State Player::isGetState()
{
	//死
	if (m_isDeath == true)
	{
		return State::kDeath;
	}

	//ダメージ
	if (m_isDamage == true)
	{
		return State::kDamage;
	}

	//ジャンプ
	if (m_move.y != 0)
	{
		return State::kJump;
	}

	//攻撃
	if (m_isAttack == true )
	{
		return State::kAttack;
	}

	//ダッシュ
	if (m_isRun == true)
	{
		return State::kRun;
	}

	//待機
	if (VSize(m_move) == 0.0f && m_move.y == 0)
	{
		return State::kIdle;
	}


	//歩き
	if (VSize(m_move) != 0.0f && m_move.y == 0)
	{
		return State::kWalk;
	}

}

void Player::Move()
{

	//アナログスティックを使って移動
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_KEY_PAD1);

	//アナログスティックの入力の10% ~ 80%の範囲を使用する

	//ベクトルの長さが最大で1000になる

	//プレイヤーの最大移動速度は 0.01f / frame

	move = VGet(analogX, 0.0f, -analogZ);	//ベクトルの長さ0~1000
	//0.00~0.01の長さにする


	//ベクトルの長さを取得する
	float len = VSize(move);
	//ベクトルの長さを0.0~1.0に割合に変換する
	float rate = len / kAnglogInputMax;

	//アナログスティック無効な範囲を除外する
	rate = (rate - kAnalogRaneMin) / (kAnalogRaneMax - kAnalogRaneMin);
	rate = min(rate, 1.0f);
	rate = max(rate, 0.0f);

	//速度が決定できるので移動ベクトルに反映する
	move = VNorm(move);
	float speed = m_speed * rate;
	move = VScale(move, speed);


	MATRIX mtx = MGetRotY(-m_cameraAngle - DX_PI_F / 2);
	move = VTransform(move, mtx);
	m_move = move;

	m_pos = VAdd(m_pos, move);


	if (VSquareSize(move) > 0.0f)
	{
		m_angle = atan2f(-move.z, move.x) - DX_PI_F / 2;
		m_attackDir = VNorm(move);
	}

	VECTOR attackMove = VScale(m_attackDir, 8.0f);
	m_attackPos = VAdd(m_pos, attackMove);

}

void Player::Jump()
{
	// ジャンプ中も走っている時の速度を維持する
	if (m_isRun)
	{
		m_move = VScale(m_move, 2.0f);
	}
	m_move.y = 4.0f;
	
}

/*アニメーション処理*/
void Player::IdleAnim()
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

void Player::WalkAnim()
{
	//前のステートWalkじゃないなら、アニメーション切り替え
	if (m_backState != State::kWalk)
	{
		ChangeAnim(kWalkAnimIndex);
		m_isStopEnd = false;
	}
	m_animIndex = kWalkAnimIndex;
	m_backState = State::kWalk;
}

void Player::RunAnim()
{
	//前のステートRunじゃないなら、アニメーション切り替え
	if (m_backState != State::kRun)
	{
		ChangeAnim(kRnuAnimIndex);
		m_isStopEnd = false;
	}

	m_animIndex = kRnuAnimIndex;
	m_backState = State::kRun;
}

void Player::JumpAnim()
{
	//前のステートJumpじゃないなら、アニメーション切り替え
	if (m_backState != State::kJump)
	{
		ChangeAnim(kJumpAnimIndex);
		m_isStopEnd = true;
	}
	m_animIndex = kJumpAnimIndex;
	m_backState = State::kJump;
}

void Player::FallAnim()
{

}

void Player::AttackAnim()
{
	//前のステートAttackじゃないなら、アニメーション切り替え
	if (m_backState != State::kAttack)
	{
		ChangeAnim(kAttackAnimIndex);
		m_isStopEnd = true;
	}
	m_animIndex = kAttackAnimIndex;
	m_backState = State::kAttack;
}

void Player::DamageAnim()
{
	//前のステートDamageじゃないなら、アニメーション切り替え
	if (m_backState != State::kDamage)
	{
		ChangeAnim(kDamageAnimIndex);
		m_isStopEnd = true;
	}
	m_animIndex = kDamageAnimIndex;
	m_backState = State::kDamage;

}

void Player::DeathAnim()
{
	//前のステートDeathじゃないなら、アニメーション切り替え
	if (m_backState != State::kDeath)
	{
		m_isMove = false;
		ChangeAnim(kDeathAnimIndex);
		m_isStopEnd = true;
	}
	m_animIndex = kDeathAnimIndex;
	m_backState = State::kDeath;

}


/*当たり判定*/
//カプセル同士の当たり判定
bool Player::IsEnemyCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	// プレイヤーと敵のカプセルの端点を計算
	m_topA = { m_pos.x, m_pos.y + upperPart, m_pos.z };
	m_bottomA = { m_pos.x, m_pos.y + bottom, m_pos.z };
	m_topB = { m_pEnemy->GetPos().x, m_pEnemy->GetPos().y + upperPart, m_pEnemy->GetPos().z };
	m_bottomB = { m_pEnemy->GetPos().x, m_pEnemy->GetPos().y + bottom, m_pEnemy->GetPos().z };

	// カプセルの中心点
	VECTOR centerA = { (m_bottomA.x + m_topA.x) / 2, (m_bottomA.y + m_topA.y) / 2, (m_bottomA.z + m_topA.z) / 2 };
	VECTOR centerB = { (m_bottomB.x + m_topB.x) / 2, (m_bottomB.y + m_topB.y) / 2, (m_bottomB.z + m_topB.z) / 2 };

	// 各カプセルの端点間の最短距離を計算する
	auto capsuleSegmentDistance = [](VECTOR bottom1, VECTOR top1, VECTOR bottom2, VECTOR top2) {
		// 直線の距離の最短を求める
		// ここで、2つの線分の最短距離を計算します
		VECTOR dir1 = { top1.x - bottom1.x, top1.y - bottom1.y, top1.z - bottom1.z };
		VECTOR dir2 = { top2.x - bottom2.x, top2.y - bottom2.y, top2.z - bottom2.z };
		VECTOR diff = { bottom1.x - bottom2.x, bottom1.y - bottom2.y, bottom1.z - bottom2.z };

		float a = dir1.x * dir1.x + dir1.y * dir1.y + dir1.z * dir1.z;
		float b = dir1.x * dir2.x + dir1.y * dir2.y + dir1.z * dir2.z;
		float c = dir2.x * dir2.x + dir2.y * dir2.y + dir2.z * dir2.z;
		float d = dir1.x * diff.x + dir1.y * diff.y + dir1.z * diff.z;
		float e = dir2.x * diff.x + dir2.y * diff.y + dir2.z * diff.z;

		float det = a * c - b * b;
		if (det == 0.0f) {
			return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		}

		float s = (b * e - c * d) / det;
		float t = (a * e - b * d) / det;

		if (s < 0.0f) s = 0.0f;
		else if (s > 1.0f) s = 1.0f;
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;

		VECTOR closestA = { bottom1.x + s * dir1.x, bottom1.y + s * dir1.y, bottom1.z + s * dir1.z };
		VECTOR closestB = { bottom2.x + t * dir2.x, bottom2.y + t * dir2.y, bottom2.z + t * dir2.z };

		VECTOR delta = { closestA.x - closestB.x, closestA.y - closestB.y, closestA.z - closestB.z };
		return std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	};

	// プレイヤーのカプセルと敵のカプセルの端点間の最短距離を計算
	float distance = capsuleSegmentDistance(m_bottomA, m_topA, m_bottomB, m_topB);

	// 衝突判定
	if (distance < m_modelRadius + m_pEnemy->GetRadius()) {
		// 衝突が発生した場合、反発ベクトルを計算してめり込みを解消
		float overlap = m_modelRadius + m_pEnemy->GetRadius() - distance;

		// 衝突方向の単位ベクトル
		VECTOR delta = { centerA.x - centerB.x, centerA.y - centerB.y, centerA.z - centerB.z };
		float norm = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		delta = { delta.x / norm, delta.y / norm, delta.z / norm };

		// プレイヤーの位置を修正
		m_pos.x += delta.x * overlap * 0.5f;
		m_pos.z += delta.z * overlap * 0.5f;

		return true; // 衝突が発生した
	}

	return false; // 衝突は発生していない
}

bool Player::IsBossEnemyCapsuleColliding(std::shared_ptr<BossEnemy> m_pBossEnemy)
{
	// プレイヤーと敵のカプセルの端点を計算
	m_topA = { m_pos.x, m_pos.y + upperPart, m_pos.z };
	m_bottomA = { m_pos.x, m_pos.y + bottom, m_pos.z };
	m_topB = { m_pBossEnemy->GetPos().x, m_pBossEnemy->GetPos().y + upperPart, m_pBossEnemy->GetPos().z };
	m_bottomB = { m_pBossEnemy->GetPos().x, m_pBossEnemy->GetPos().y + bottom, m_pBossEnemy->GetPos().z };

	// カプセルの中心点
	VECTOR centerA = { (m_bottomA.x + m_topA.x) / 2, (m_bottomA.y + m_topA.y) / 2, (m_bottomA.z + m_topA.z) / 2 };
	VECTOR centerB = { (m_bottomB.x + m_topB.x) / 2, (m_bottomB.y + m_topB.y) / 2, (m_bottomB.z + m_topB.z) / 2 };

	// 各カプセルの端点間の最短距離を計算する
	auto capsuleSegmentDistance = [](VECTOR bottom1, VECTOR top1, VECTOR bottom2, VECTOR top2) {
		// 直線の距離の最短を求める
		// ここで、2つの線分の最短距離を計算します
		VECTOR dir1 = { top1.x - bottom1.x, top1.y - bottom1.y, top1.z - bottom1.z };
		VECTOR dir2 = { top2.x - bottom2.x, top2.y - bottom2.y, top2.z - bottom2.z };
		VECTOR diff = { bottom1.x - bottom2.x, bottom1.y - bottom2.y, bottom1.z - bottom2.z };

		float a = dir1.x * dir1.x + dir1.y * dir1.y + dir1.z * dir1.z;
		float b = dir1.x * dir2.x + dir1.y * dir2.y + dir1.z * dir2.z;
		float c = dir2.x * dir2.x + dir2.y * dir2.y + dir2.z * dir2.z;
		float d = dir1.x * diff.x + dir1.y * diff.y + dir1.z * diff.z;
		float e = dir2.x * diff.x + dir2.y * diff.y + dir2.z * diff.z;

		float det = a * c - b * b;
		if (det == 0.0f) {
			return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		}

		float s = (b * e - c * d) / det;
		float t = (a * e - b * d) / det;

		if (s < 0.0f) s = 0.0f;
		else if (s > 1.0f) s = 1.0f;
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;

		VECTOR closestA = { bottom1.x + s * dir1.x, bottom1.y + s * dir1.y, bottom1.z + s * dir1.z };
		VECTOR closestB = { bottom2.x + t * dir2.x, bottom2.y + t * dir2.y, bottom2.z + t * dir2.z };

		VECTOR delta = { closestA.x - closestB.x, closestA.y - closestB.y, closestA.z - closestB.z };
		return std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		};

	// プレイヤーのカプセルと敵のカプセルの端点間の最短距離を計算
	float distance = capsuleSegmentDistance(m_bottomA, m_topA, m_bottomB, m_topB);

	// 衝突判定
	if (distance < m_modelRadius + m_pBossEnemy->GetRadius()) {
		// 衝突が発生した場合、反発ベクトルを計算してめり込みを解消
		float overlap = m_modelRadius + m_pBossEnemy->GetRadius() - distance;

		// 衝突方向の単位ベクトル
		VECTOR delta = { centerA.x - centerB.x, centerA.y - centerB.y, centerA.z - centerB.z };
		float norm = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		delta = { delta.x / norm, delta.y / norm, delta.z / norm };

		// プレイヤーの位置を修正
		m_pos.x += delta.x * overlap * 0.5f;
		m_pos.z += delta.z * overlap * 0.5f;

		return true; // 衝突が発生した
	}

	return false; // 衝突は発生していない
}

//攻撃の当たり判定
bool Player::IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	float delX = (m_attackPos.x - m_pEnemy->GetPos().x) * (m_attackPos.x - m_pEnemy->GetPos().x);
	float delY = (m_attackPos.y - m_pEnemy->GetPos().y + (upperPart / 2)) * (m_attackPos.y - m_pEnemy->GetPos().y + (upperPart / 2));
	float delZ = (m_attackPos.z - m_pEnemy->GetPos().z) * (m_attackPos.z - m_pEnemy->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_radius + m_pEnemy->GetRadius())
	{

		return true;
	}

	return false;
}

bool Player::IsBossAttackColliding(std::shared_ptr<BossEnemy> m_pBossEnemy)
{

	float delX = (m_attackPos.x - m_pBossEnemy->GetPos().x) * (m_attackPos.x - m_pBossEnemy->GetPos().x);
	float delY = (m_attackPos.y - m_pBossEnemy->GetPos().y + (upperPart / 2)) * (m_attackPos.y - m_pBossEnemy->GetPos().y + (upperPart / 2));
	float delZ = (m_attackPos.z - m_pBossEnemy->GetPos().z) * (m_attackPos.z - m_pBossEnemy->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_radius + m_pBossEnemy->GetRadius())
	{

		return true;
	}

	return false;

}


//踏み攻撃の当たり判定
bool Player::IsStepOnAttockColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	float delX = (m_pos.x - m_pEnemy->GetPos().x) * (m_pos.x - m_pEnemy->GetPos().x);
	float delY = ((m_pos.y + bottom) - (m_pEnemy->GetPos().y + upperPart)) * ((m_pos.y + bottom) - (m_pEnemy->GetPos().y + upperPart));
	float delZ = (m_pos.z - m_pEnemy->GetPos().z) * (m_pos.z - m_pEnemy->GetPos().z);

	//球と球の距離
	float Distance = sqrt(delX + delY + delZ);

	//球と球の距離が剣とエネミーの半径よりも小さい場合
	if (Distance < m_radius + m_pEnemy->GetRadius())
	{

		return true;
	}

	return false;
}


void Player::CorrectPosition(Stage& stage)
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

					if (m_nowState != State::kRun)
					{
						//当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						m_pos = VAdd(m_pos, VScale(Poly->Normal, kMaxSpeed));

					}
					else if (m_nowState == State::kRun)
					{
						//当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						m_pos = VAdd(m_pos, VScale(Poly->Normal, kMaxSpeed * 2));

					}

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
		if (m_nowState == State::kJump && m_JumpPower > 0.0f)
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
