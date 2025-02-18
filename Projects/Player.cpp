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
	//���f���̃t�@�C����
	const char* const kModelFilename = "data/model/ArcticTextures/player.mv1";

	//���f���̌����Ă�ʒu�̏�����
	constexpr float kInitAngle = 3.143059f;

	//���f���̃T�C�Y�ύX
	constexpr float kExpansion = 0.1f;

	//�A�j���[�V�����ԍ�
	constexpr int kIdleAnimIndex = 1;
	constexpr int kWalkAnimIndex = 2;
	constexpr int kRnuAnimIndex = 3;
	constexpr int kJumpAnimIndex = 9;
	constexpr int kAttackAnimIndex = 5;
	constexpr int kFallingAnimIndex = 0;
	constexpr int kDeathAnimIndex = 10;
	constexpr int kDamageAnimIndex = 11;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//�A�i���O�X�e�B�b�N�ɂ��ړ��֘A
	constexpr float kMaxSpeed = 0.4f;		//�v���C���[�̍ő�ړ����x
	constexpr float kAnalogRaneMin = 0.4f;	//�A�i���O�X�e�B�b�N�̓��͔���͈�
	constexpr float kAnalogRaneMax = 0.8f;
	constexpr float kAnglogInputMax = 1000.0f;	//�A�i���O�X�e�B�b�N�̓��͂����x�N�g����

	//�J�v�Z���̍��W
	constexpr int upperPart = 15;	//�㕔
	constexpr int bottom = 2;		//����

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
	// �e�X�e�[�g�ɑΉ�����A�j���[�V�����̍Đ����x��ݒ�
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

	//�v���C���[�X�s�[�h������
	m_speed = kMaxSpeed;

	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;

	//�v���C���[�̏����ʒu�ݒ�
	m_pos = VGet(0.0f, 5.0f, -20.0f);
	m_move = VGet(0.0f, 0.0f, 0.0f);


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

}

void Player::Update(std::shared_ptr<Enemy> m_pEnemy, std::shared_ptr<BossEnemy> m_pBossEnemy, std::shared_ptr<Ui> m_pUi, Stage& stage)
{

	Pad::Update();

	//�A�j���[�V�����̐؂�ւ�
	if (m_prevAnimNo != -1)
	{
		m_animBlendRate += kAnimChangeRateSpeed;
		if (m_animBlendRate >= 1.0f)	m_animBlendRate = 1.0f;
		//�ύX�O�̃A�j���[�V����100%
		MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
		//�ύX��̃A�j���[�V����0%
		MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);

	}

	//�A�j���[�V������i�߂�
	bool isLoop = UpdateAnim(m_currentAnimNo);
	if (isLoop)
	{
		if (!m_isStopEnd) ChangeAnim(m_animIndex);
	}
	UpdateAnim(m_prevAnimNo);

	//���݂̃X�e�[�g���m�F
	m_nowState = isGetState();

	m_prevPos = m_pos;

	//�ړ�����
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


	// �_�b�V���A�j���[�V�����̐���
	if (Pad::IsPress(PAD_INPUT_2) && m_move.y == 0)
	{
		m_runFrame++;
		if (m_runFrame > 5)
		{
			m_isRun = true;

			//�����X�s�[�h
			m_move = VScale(m_move, 1.5f);

		}
	}
	else
	{
		m_isRun = false;
		m_runFrame = 0;
	}


	/*���݂̃A�j���[�V����*/
	if (m_nowState == State::kIdle) { IdleAnim(); }		//�ҋ@
	if (m_nowState == State::kWalk) { WalkAnim(); }		//����
	if (m_nowState == State::kRun) { RunAnim(); }		//�_�b�V��
	if (m_nowState == State::kJump) { JumpAnim(); }		//�W�����v
	if (m_nowState == State::kAttack) { AttackAnim(); }	//�U��
	if (m_nowState == State::kDamage) { DamageAnim(); }	//�_���[�W
	if (m_nowState == State::kDeath) { DeathAnim(); }	//��


	//���W�Ɉړ��ʂ𑫂�
	m_pos = VAdd(m_pos, m_move);

	//�d��
	m_move.y -= 0.1;

	// �G�̍U���ɂ�鐁����т�h�����߂̏���
	if (m_isDamage)
	{
		// �_���[�W���󂯂��ۂ̈ړ��ʂ𐧌�
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



	//HP���}�C�i�X�ɂ����Ȃ�����
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

	//�����蔻��J�v�Z��
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + upperPart, m_pos.z), VGet(m_pos.x, m_pos.y + bottom, m_pos.z), m_modelRadius, 8, m_color, m_color, false);

	DrawSphere3D(VAdd(m_attackPos, VGet(0, 10, 0)), m_attackRadius, 8, 0xffffff, 0xffffff, false);

#endif // _DEBUG


}

bool Player::UpdateAnim(int attachNo)
{
	//�A�j���[�V�������ݒ肳��Ă��Ȃ��̂ŏI��
	if (attachNo == -1) return false;

	//�A�j���[�V������i�s������
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//���݂̍Đ��J�E���g���擾
	now += 0.5f * m_animSpeed; // �A�j���[�V�������Đ����x�ɉ����Đi�߂�

	//���ݍĐ����̃A�j���[�V�����̑��J�E���g���擾
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

	//�i�߂����Ԃ̐ݒ�
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void Player::ChangeAnim(int animIndex)
{
	//����ɌÂ��A�j���[�V�������A�^�b�`����Ă���ꍇ�͂��̎��_�ō폜���Ă���
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//���ݍĐ����̑ҋ@�A�j���[�V�����͕ύX�O�̃A�j���[�V����������
	m_prevAnimNo = m_currentAnimNo;

	//�ύX��̃A�j���[�V�����Ƃ��čU���A�j���[�V���������߂Đݒ肷��
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex, -1, false);

	//�؂�ւ��̏u�Ԃ͕ύX��̃A�j���[�V�������Đ������
	m_animBlendRate = 0.0f;

	//�ύX�O�̃A�j���[�V����100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//�ύX��̃A�j���[�V����0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);

	// ���݂̃X�e�[�g�ɉ������A�j���[�V�����̍Đ����x��ݒ�
	m_animSpeed = m_animSpeedMap[m_nowState];

}

Player::State Player::isGetState()
{
	//��
	if (m_isDeath == true)
	{
		return State::kDeath;
	}

	//�_���[�W
	if (m_isDamage == true)
	{
		return State::kDamage;
	}

	//�W�����v
	if (m_move.y != 0)
	{
		return State::kJump;
	}

	//�U��
	if (m_isAttack == true )
	{
		return State::kAttack;
	}

	//�_�b�V��
	if (m_isRun == true)
	{
		return State::kRun;
	}

	//�ҋ@
	if (VSize(m_move) == 0.0f && m_move.y == 0)
	{
		return State::kIdle;
	}


	//����
	if (VSize(m_move) != 0.0f && m_move.y == 0)
	{
		return State::kWalk;
	}

}

void Player::Move()
{

	//�A�i���O�X�e�B�b�N���g���Ĉړ�
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_KEY_PAD1);

	//�A�i���O�X�e�B�b�N�̓��͂�10% ~ 80%�͈̔͂��g�p����

	//�x�N�g���̒������ő��1000�ɂȂ�

	//�v���C���[�̍ő�ړ����x�� 0.01f / frame

	move = VGet(analogX, 0.0f, -analogZ);	//�x�N�g���̒���0~1000
	//0.00~0.01�̒����ɂ���


	//�x�N�g���̒������擾����
	float len = VSize(move);
	//�x�N�g���̒�����0.0~1.0�Ɋ����ɕϊ�����
	float rate = len / kAnglogInputMax;

	//�A�i���O�X�e�B�b�N�����Ȕ͈͂����O����
	rate = (rate - kAnalogRaneMin) / (kAnalogRaneMax - kAnalogRaneMin);
	rate = min(rate, 1.0f);
	rate = max(rate, 0.0f);

	//���x������ł���̂ňړ��x�N�g���ɔ��f����
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
	// �W�����v���������Ă��鎞�̑��x���ێ�����
	if (m_isRun)
	{
		m_move = VScale(m_move, 2.0f);
	}
	m_move.y = 4.0f;
	
}

/*�A�j���[�V��������*/
void Player::IdleAnim()
{
	//�O�̃X�e�[�gIdle����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gWalk����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gRun����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gJump����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gAttack����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gDamage����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
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
	//�O�̃X�e�[�gDeath����Ȃ��Ȃ�A�A�j���[�V�����؂�ւ�
	if (m_backState != State::kDeath)
	{
		m_isMove = false;
		ChangeAnim(kDeathAnimIndex);
		m_isStopEnd = true;
	}
	m_animIndex = kDeathAnimIndex;
	m_backState = State::kDeath;

}


/*�����蔻��*/
//�J�v�Z�����m�̓����蔻��
bool Player::IsEnemyCapsuleColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	// �v���C���[�ƓG�̃J�v�Z���̒[�_���v�Z
	m_topA = { m_pos.x, m_pos.y + upperPart, m_pos.z };
	m_bottomA = { m_pos.x, m_pos.y + bottom, m_pos.z };
	m_topB = { m_pEnemy->GetPos().x, m_pEnemy->GetPos().y + upperPart, m_pEnemy->GetPos().z };
	m_bottomB = { m_pEnemy->GetPos().x, m_pEnemy->GetPos().y + bottom, m_pEnemy->GetPos().z };

	// �J�v�Z���̒��S�_
	VECTOR centerA = { (m_bottomA.x + m_topA.x) / 2, (m_bottomA.y + m_topA.y) / 2, (m_bottomA.z + m_topA.z) / 2 };
	VECTOR centerB = { (m_bottomB.x + m_topB.x) / 2, (m_bottomB.y + m_topB.y) / 2, (m_bottomB.z + m_topB.z) / 2 };

	// �e�J�v�Z���̒[�_�Ԃ̍ŒZ�������v�Z����
	auto capsuleSegmentDistance = [](VECTOR bottom1, VECTOR top1, VECTOR bottom2, VECTOR top2) {
		// �����̋����̍ŒZ�����߂�
		// �����ŁA2�̐����̍ŒZ�������v�Z���܂�
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

	// �v���C���[�̃J�v�Z���ƓG�̃J�v�Z���̒[�_�Ԃ̍ŒZ�������v�Z
	float distance = capsuleSegmentDistance(m_bottomA, m_topA, m_bottomB, m_topB);

	// �Փ˔���
	if (distance < m_modelRadius + m_pEnemy->GetRadius()) {
		// �Փ˂����������ꍇ�A�����x�N�g�����v�Z���Ă߂荞�݂�����
		float overlap = m_modelRadius + m_pEnemy->GetRadius() - distance;

		// �Փ˕����̒P�ʃx�N�g��
		VECTOR delta = { centerA.x - centerB.x, centerA.y - centerB.y, centerA.z - centerB.z };
		float norm = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		delta = { delta.x / norm, delta.y / norm, delta.z / norm };

		// �v���C���[�̈ʒu���C��
		m_pos.x += delta.x * overlap * 0.5f;
		m_pos.z += delta.z * overlap * 0.5f;

		return true; // �Փ˂���������
	}

	return false; // �Փ˂͔������Ă��Ȃ�
}

bool Player::IsBossEnemyCapsuleColliding(std::shared_ptr<BossEnemy> m_pBossEnemy)
{
	// �v���C���[�ƓG�̃J�v�Z���̒[�_���v�Z
	m_topA = { m_pos.x, m_pos.y + upperPart, m_pos.z };
	m_bottomA = { m_pos.x, m_pos.y + bottom, m_pos.z };
	m_topB = { m_pBossEnemy->GetPos().x, m_pBossEnemy->GetPos().y + upperPart, m_pBossEnemy->GetPos().z };
	m_bottomB = { m_pBossEnemy->GetPos().x, m_pBossEnemy->GetPos().y + bottom, m_pBossEnemy->GetPos().z };

	// �J�v�Z���̒��S�_
	VECTOR centerA = { (m_bottomA.x + m_topA.x) / 2, (m_bottomA.y + m_topA.y) / 2, (m_bottomA.z + m_topA.z) / 2 };
	VECTOR centerB = { (m_bottomB.x + m_topB.x) / 2, (m_bottomB.y + m_topB.y) / 2, (m_bottomB.z + m_topB.z) / 2 };

	// �e�J�v�Z���̒[�_�Ԃ̍ŒZ�������v�Z����
	auto capsuleSegmentDistance = [](VECTOR bottom1, VECTOR top1, VECTOR bottom2, VECTOR top2) {
		// �����̋����̍ŒZ�����߂�
		// �����ŁA2�̐����̍ŒZ�������v�Z���܂�
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

	// �v���C���[�̃J�v�Z���ƓG�̃J�v�Z���̒[�_�Ԃ̍ŒZ�������v�Z
	float distance = capsuleSegmentDistance(m_bottomA, m_topA, m_bottomB, m_topB);

	// �Փ˔���
	if (distance < m_modelRadius + m_pBossEnemy->GetRadius()) {
		// �Փ˂����������ꍇ�A�����x�N�g�����v�Z���Ă߂荞�݂�����
		float overlap = m_modelRadius + m_pBossEnemy->GetRadius() - distance;

		// �Փ˕����̒P�ʃx�N�g��
		VECTOR delta = { centerA.x - centerB.x, centerA.y - centerB.y, centerA.z - centerB.z };
		float norm = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
		delta = { delta.x / norm, delta.y / norm, delta.z / norm };

		// �v���C���[�̈ʒu���C��
		m_pos.x += delta.x * overlap * 0.5f;
		m_pos.z += delta.z * overlap * 0.5f;

		return true; // �Փ˂���������
	}

	return false; // �Փ˂͔������Ă��Ȃ�
}

//�U���̓����蔻��
bool Player::IsAttackColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	float delX = (m_attackPos.x - m_pEnemy->GetPos().x) * (m_attackPos.x - m_pEnemy->GetPos().x);
	float delY = (m_attackPos.y - m_pEnemy->GetPos().y + (upperPart / 2)) * (m_attackPos.y - m_pEnemy->GetPos().y + (upperPart / 2));
	float delZ = (m_attackPos.z - m_pEnemy->GetPos().z) * (m_attackPos.z - m_pEnemy->GetPos().z);

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
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

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
	if (Distance < m_radius + m_pBossEnemy->GetRadius())
	{

		return true;
	}

	return false;

}


//���ݍU���̓����蔻��
bool Player::IsStepOnAttockColliding(std::shared_ptr<Enemy> m_pEnemy)
{
	float delX = (m_pos.x - m_pEnemy->GetPos().x) * (m_pos.x - m_pEnemy->GetPos().x);
	float delY = ((m_pos.y + bottom) - (m_pEnemy->GetPos().y + upperPart)) * ((m_pos.y + bottom) - (m_pEnemy->GetPos().y + upperPart));
	float delZ = (m_pos.z - m_pEnemy->GetPos().z) * (m_pos.z - m_pEnemy->GetPos().z);

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
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


	//�v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
	HitDim = MV1CollCheck_Sphere(stage.GetCollisionMap(), -1, stage.GetVectorMapPos(), 1500.0f);

	for (int i = 0; i < HitDim.HitNum; i++)
	{
		if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.0000001f)
		{
			//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
			if (HitDim.Dim[i].Position[0].y > m_pos.y + 1.0f ||
				HitDim.Dim[i].Position[1].y > m_pos.y + 1.0f ||
				HitDim.Dim[i].Position[2].y > m_pos.y + 1.0f)
			{
				//�|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (m_kabeNum < PLAYER_MAX_HITCOLL)
				{
					//�|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					Kabe[m_kabeNum] = &HitDim.Dim[i];

					//�ǃ|���S���̐������Z����
					m_kabeNum++;
				}
			}
		}
		else
		{
			//�|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
			if (m_yukaNum < PLAYER_MAX_HITCOLL)
			{
				//�|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
				Yuka[m_yukaNum] = &HitDim.Dim[i];

				//�ǃ|���S���̐������Z����
				m_yukaNum++;
			}

		}
	}

	//�ǃ|���S���Ƃ̓����蔻�菈��
	if (m_kabeNum != 0)
	{
		m_hitFlag = false;

		if (m_moveFlag == true)
		{
			for (int i = 0; i < m_kabeNum; i++)
			{
				Poly = Kabe[i];

				//�|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]) == false) continue;

				//�|���S���ɓ��������t���O�𗧂Ă�
				m_hitFlag = true;

				// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
				for (j = 0; j < m_kabeNum; j++)
				{
					//j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					Poly = Kabe[j];

					//�������Ă����烋�[�v���甲����
					if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
						Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true) break;
				}

				//j �� m_kabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				//�ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == m_kabeNum)
				{
					m_hitFlag = false;
					break;
				}
			}
		}
		else
		{
			//�ړ����Ă��Ȃ��ꍇ�̏���

			//�ǃ|���S���̐������J��Ԃ�
			for (int i = 0; i < m_kabeNum; i++)
			{
				//i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				Poly = Kabe[i];

				//�|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
				if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true)
				{
					m_hitFlag = true;
					break;
				}
			}
		}

		//�ǂɓ������Ă�����ǂ��牟���o���������s��
		if (m_hitFlag == true)
		{
			for (int i = 0; i < 16; i++)
			{

				//�ǃ|���S���̐������J��Ԃ�
				for (int k = 0; k < m_kabeNum; k++)
				{
					Poly = Kabe[k];

					//�v���C���[�Ɠ������Ă��邩�𔻒�
					if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
						Poly->Position[0], Poly->Position[1], Poly->Position[2]) == false) continue;

					if (m_nowState != State::kRun)
					{
						//�������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						m_pos = VAdd(m_pos, VScale(Poly->Normal, kMaxSpeed));

					}
					else if (m_nowState == State::kRun)
					{
						//�������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						m_pos = VAdd(m_pos, VScale(Poly->Normal, kMaxSpeed * 2));

					}

					//�ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					for (j = 0; j < m_kabeNum; j++)
					{
						Poly = Kabe[j];
						if (HitCheck_Capsule_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_modelRadius,
							Poly->Position[0], Poly->Position[1], Poly->Position[2]) == true) break;
					}

					//�S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
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

			//�V��ɓ����Ԃ��鏈�����s��

			//��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
			MinY = 0.0f;

			//�����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���
			m_hitFlag = false;

			//���|���S���̐������J��Ԃ�
			for (int i = 0; i < m_yukaNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				Poly = Yuka[i];

				//���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				LineRes = HitCheck_Line_Triangle(m_mapHitColl, VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)),
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				//�ڐG���Ă��Ȃ������牽�����Ȃ�
				if (!LineRes.HitFlag) continue;

				//���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
				if (m_hitFlag == 1 && MinY < LineRes.Position.y) continue;

				//�|���S���ɓ��������t���O�𗧂Ă�
				m_hitFlag = true;

				//�ڐG�����x���W��ۑ�����
				MinY = LineRes.Position.y;
			}

			//�ڐG�����|���S�������������ǂ����ŏ����𕪊�
			if (m_hitFlag)
			{
				//�ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
				m_mapHitColl.y = MinY - upperPart;

				//�x�������̑��x�͔��]
				m_move.y = 0.0f;
			}
		}
		else
		{
			float MaxY;

			//���~�����W�����v���ł͂Ȃ��ꍇ�̏���

			//���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			m_hitFlag = false;

			//��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			MaxY = 0.0f;

			//���|���S���̐������J��Ԃ�
			for (int i = 0; i < m_yukaNum; i++)
			{
				//i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				Poly = Yuka[i];

				//�W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
				LineRes = HitCheck_Line_Triangle(VAdd(m_mapHitColl, VGet(0.0f, upperPart, 0.0f)), m_mapHitColl,
					Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				//�������Ă��Ȃ������牽�����Ȃ�
				if (!LineRes.HitFlag) continue;

				//���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (m_hitFlag && MaxY > LineRes.Position.y) continue;

				//�|���S���ɓ��������t���O�𗧂Ă�
				m_hitFlag = true;

				// �ڐG�����x���W��ۑ�����
				MaxY = LineRes.Position.y;
			}

			//���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (m_hitFlag)
			{
				//���������ꍇ

				//�ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				m_mapHitColl.y = MaxY;

				//�x�������̈ړ����x�͂O��
				m_move.y = 0.0f;

			}
		}
	}


	//���o�����v���C���[�̎��͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(HitDim);
}
