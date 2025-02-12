#include "Enemy.h"
#include "DxLib.h"
#include "Pad.h"
#include "Player.h"

#define D2R(deg) ((deg)* DX_PI_F/180.0f)

//�X�s�[�h
#define MOVE_SPEED 0.3f

//���̋����܂ŋ߂Â����玟�̃|�C���g��
#define CHECK_LENGTH 2.0f

namespace
{
	//���f���̃T�C�Y�ύX
	constexpr float kExpansion = 0.1f;

	//�A�j���[�V�����ԍ�
	constexpr int kIdleAnimIndex = 5;
	constexpr int kWalkAnimIndex = 0;
	constexpr int kRnuAnimIndex = 1;
	constexpr int kAttackAnimIndex = 2;
	constexpr int kDeadAnimIndex = 3;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;


	//�G�̑���
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

	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kWalkAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;


	m_attackPos = VGet(m_pos.x, m_pos.y, m_pos.z - 10);

	m_angle = VGet(0.0f, D2R(0.0f), 0.0f);

	//�ړ�������W
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
		ChangeAnim(m_animIndex);
	}
	UpdateAnim(m_prevAnimNo);
	


	//���񂾎�
	if (m_state == kDeath)
	{

		if (!m_isDead)
		{
			m_isDead = true;
			ChangeAnim(kDeadAnimIndex);  // ���S�A�j���[�V����

		}


	}

	if (!m_isAttack)
	{
		//�U���̎�
		if (m_state == kAttack)
		{
			m_isAttack = true;
			ChangeAnim(kAttackAnimIndex);
		}

		//�ҋ@�̎�
		if (m_state == kIdle)
		{

			//�|�C���g���W
			VECTOR TargetPos = PointPos[TargetNumber];

			//�^�[�Q�b�g�̍��W�܂ł̋���
			VECTOR Vec = VSub(TargetPos, m_pos);
			float Length = VSize(Vec);

			//�ړ��x�N�g��
			VECTOR Move = VScale(VNorm(Vec), MOVE_SPEED);

			//�ړ��x�N�g���ɍ��킹�Č�����ύX
			m_angle.y = atan2f(-Move.x, -Move.z);

			//���W��i�߂�
			m_pos = VAdd(m_pos, Move);

			//�ړ���̍��W
			Vec = VSub(TargetPos, m_pos);


			//�|�C���g���W�̋������߂Â�����
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

		//�ǂ������Ă���
		if (m_state == kRun)
		{

			//�v���C���[�̍��W
			VECTOR toTarget = VSub(m_pPlayer->GetPos(), m_pos);

			//�v���C���[�̍��W�Ɉړ�
			toTarget = VNorm(toTarget);
			m_distance.x = toTarget.x * kSpeed;
			m_distance.y = m_pos.y;
			m_distance.z = toTarget.z * kSpeed;


			m_pos = VAdd(m_pos, m_distance);


			//���f���̌�����ς���
			VECTOR SubVector = VSub(m_pPlayer->GetPos(), m_pos);

			// atan ���g�p���Ċp�x���擾
			m_angle.y = atan2f(-SubVector.x, -SubVector.z);


			m_attackPos = VAdd(m_pos, m_distance);

			//�v���C���[�̕���������
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
		//�U���A�j���[�V�������I��������ҋ@�A�j���[�V�������Đ�����
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


	//HP���}�C�i�X�ɂ����Ȃ�����
	if (m_hp <= 0) m_hp = 0;

	if (m_isAttack)
	{
		m_isUnderAttack = true;
	}
	else
	{
		m_isUnderAttack = false;
	}


	//�G�l�~�[���f���̍��W
	MV1SetPosition(m_modelHandle, m_pos);
	//�G�l�~�[���f���̉�]�l
	MV1SetRotationXYZ(m_modelHandle, m_angle);
}

void Enemy::Draw()
{	
	//�G�l�~�[���f���`��
	MV1DrawModel(m_modelHandle);

	//HP�o�[
	DrawBox(696, 9, 704 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 44, 0x000000, true);
	DrawBox(697, 10, 703 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 43, 0xffffff, true);
	DrawBox(699, 12, 701 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 41, 0x000000, true);
	DrawBox(700, 13, 700 + m_hp * ENEMY_DRAW_SIZE, 40, 0xff0000, true);


#ifdef _DEBUG

	//�G�l�~�[�̈ړ���
	for (int i = 0; i < MAX_POINT; i++)
	{
		int n1 = i;
		int n2 = (i + 1) % MAX_POINT;
		DrawLine3D(PointPos[n1], PointPos[n2], GetColor(0, 255, 0));
	}

	DrawFormatString(0, 40, 0x000000, "Enemy(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

	//�J�v�Z��
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 15, m_pos.z), VGet(m_pos.x, m_pos.y + 2, m_pos.z), m_modelRadius, 8, 0xffffff, 0xffffff, false);

	//�U����
	DrawSphere3D(VAdd(m_attackPos, VGet(0, 10, 0)), m_attackRadius, 8, 0xffffff, 0xffffff, false);
	
	//���G�͈͋�
	DrawSphere3D(VAdd(m_pos, VGet(0, 0, 0)), m_discoveryRadius, 10, m_color, m_color, false);

#endif // _DEBUG



}

void Enemy::End()
{
	MV1DeleteModel(m_modelHandle);
	m_modelHandle = -1;
}

//���G�͈͓����蔻��
bool Enemy::Translation(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y  - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
	float delZ = (m_pos.z - m_pPlayer->GetPos().z) * (m_pos.z - m_pPlayer->GetPos().z);

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
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

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
	if (Distance < m_attackRadius + m_pPlayer->GetRadius())
	{

		return true;
	}

	return false;
}

bool Enemy::UpdateAnim(int attachNo)
{
	//�A�j���[�V�������ݒ肳��Ă��Ȃ��̂ŏI��
	if (attachNo == -1) return false;

	//�A�j���[�V������i�s������
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//���݂̍Đ��J�E���g���擾
	now += 0.5f;	//�A�j���[�V�����i�߂�

	//���ݍĐ����̃A�j���[�V�����̑��J�E���g���擾
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		now -= total;
		isLoop = true;
	}

	//�i�߂����Ԃ̐ݒ�
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void Enemy::ChangeAnim(int animIndex)
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
}
