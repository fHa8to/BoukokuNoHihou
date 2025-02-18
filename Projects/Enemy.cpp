#include "Enemy.h"
#include "DxLib.h"
#include "Pad.h"
#include "Player.h"
#include "Stage.h"
#include "Ui.h"

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
	constexpr int kIdleAnimIndex = 0;
	constexpr int kWalkAnimIndex = 1;
	constexpr int kRnuAnimIndex = 2;
	constexpr int kAttackAnimIndex = 3;
	constexpr int kDamageAnimIndex = 4;
	constexpr int kDeadAnimIndex = 5;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;


	//�G�̑���
	constexpr float kSpeed = 0.7f;

	//�J�v�Z���̍��W
	constexpr int upperPart = 15;	//�㕔
	constexpr int bottom = 2;		//����

	constexpr int kAttackDelayDuration = 200; // �x���t���[����

}

Enemy::Enemy():
	m_modelHandle(MV1LoadModel("data/model/skeleton/skeleton1.mv1")),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_attackPos(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(),
	m_move(VGet(0.0f, 0.0f, 0.0f)),
	m_modelRadius(4.0f),
	m_discoveryRadius(50.0f),
	m_attackRadius(13.0f),
	m_stopRadius(10.0f),
	m_direction(VGet(0, 0, 0)),
	m_isIdle(false),
	m_isAttack(false),
	m_isRnu(false),
	m_isDead(false),
	m_isDamage(false),
	m_hp(0),
	m_state(kIdle),
	m_attackDelayCounter(0),
	m_kabeNum(0),
	m_yukaNum(0)
{
	m_pos = VGet(-111.0f, 170.0f, -117.0f);
	m_move = VGet(0.0f, 0.0f, 0.0f);

	// �e�X�e�[�g�ɑΉ�����A�j���[�V�����̍Đ����x��ݒ�
	m_animSpeedMap[State::kIdle] = 1.0f;
	m_animSpeedMap[State::kRun] = 1.0f;
	m_animSpeedMap[State::kAttack] = 1.5f;
	m_animSpeedMap[State::kDamage] = 1.0f;
	m_animSpeedMap[State::kDeath] = 0.8f;

}

Enemy::~Enemy()
{
}

void Enemy::Init()
{

	//�ҋ@�A�j���[�V������ݒ�
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;


	m_attackPos = VGet(m_pos.x, m_pos.y, m_pos.z);

	m_angle = VGet(0.0f, D2R(0.0f), 0.0f);

	
	TargetNumber = 1;

	m_hp = ENEMY_HP_MAX;


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

}

void Enemy::Update(std::shared_ptr<Player> m_pPlayer, std::shared_ptr<Ui> m_pUi, Stage& stage)
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
	
	//���W�Ɉړ��ʂ𑫂�
	m_pos = VAdd(m_pos, m_move);

	//�d��
	m_move.y -= 0.1;


	// �U���̒x������
	if (m_attackDelayCounter > 0)
	{
		m_attackDelayCounter--;
	}

	// ���񂾎�
	if (m_state == kDeath)
	{
		m_isAttack = false;
		if (!m_isDead)
		{
			m_isDead = true;
			ChangeAnim(kDeadAnimIndex);  // ���S�A�j���[�V����
		}
		else if (isLoop)  // ���S�A�j���[�V�������I��������
		{
			MV1DeleteModel(m_modelHandle);  // ���f�����폜
			m_modelHandle = -1;  // ���f���n���h���𖳌���
			return;  // �ȍ~�̏������X�L�b�v
		}
	}

	if (m_modelHandle == -1) return;  // ���f�����폜����Ă���ꍇ�͏������X�L�b�v



	if (!m_isAttack)
	{
		//�U���̎�
		if (m_state == kAttack && m_pUi->GetPlayerHp() > 0)
		{
			if (!m_isAttack && m_attackDelayCounter == 0)
			{
				// �U���̎�
				if (m_state == kAttack && m_pUi->GetPlayerHp() > 0)
				{
					if (!m_isAttack)
					{
						ChangeAnim(kAttackAnimIndex);
					}
					m_isAttack = true;
					m_attackDelayCounter = kAttackDelayDuration; // �U���x���J�E���^�����Z�b�g
				}
			}
		}

		//�ҋ@�̎�
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

		//�ǂ������Ă���
		if (m_state == kRun)
		{

			//�v���C���[�̍��W
			VECTOR toTarget = VSub(m_pPlayer->GetPos(), m_pos);

			//�v���C���[�̍��W�Ɉړ�
			toTarget = VNorm(toTarget);
			m_distance.x = toTarget.x * kSpeed;
			m_distance.y = 0.0f;
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
		//�U���A�j���[�V�������I��������ҋ@�A�j���[�V�������Đ�����
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

	CorrectPosition(stage);

	//�G�l�~�[���f���̍��W
	MV1SetPosition(m_modelHandle, m_pos);
	//�G�l�~�[���f���̉�]�l
	MV1SetRotationXYZ(m_modelHandle, m_angle);
}

void Enemy::Draw(std::shared_ptr<Player> m_pPlayer)
{	
	//�G�l�~�[���f���`��
	MV1DrawModel(m_modelHandle);

	


#ifdef _DEBUG

	if (Translation(m_pPlayer))
	{
		//HP�o�[
		DrawBox(696, 9, 704 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 44, 0x000000, true);
		DrawBox(697, 10, 703 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 43, 0xffffff, true);
		DrawBox(699, 12, 701 + ENEMY_HP_MAX * ENEMY_DRAW_SIZE, 41, 0x000000, true);
		DrawBox(700, 13, 700 + m_hp * ENEMY_DRAW_SIZE, 40, 0xff0000, true);
	}

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

	//�~�܂�͈͋�
	DrawSphere3D(VAdd(m_pos, VGet(0, 0, 0)), m_stopRadius, 10, 0xffffff, 0xffffff, false);

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

bool Enemy::IsStopColliding(std::shared_ptr<Player> m_pPlayer)
{
	float delX = (m_pos.x - m_pPlayer->GetPos().x) * (m_pos.x - m_pPlayer->GetPos().x);
	float delY = (m_pos.y - m_pPlayer->GetPos().y) * (m_pos.y - m_pPlayer->GetPos().y);
	float delZ = (m_pos.z - m_pPlayer->GetPos().z) * (m_pos.z - m_pPlayer->GetPos().z);

	//���Ƌ��̋���
	float Distance = sqrt(delX + delY + delZ);

	//���Ƌ��̋��������ƃG�l�~�[�̔��a�����������ꍇ
	if (Distance < m_stopRadius + m_pPlayer->GetRadius())
	{

		return true;
	}

	return false;
}

void Enemy::CorrectPosition(Stage& stage)
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

					//�������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
					m_pos = VAdd(m_pos, VScale(Poly->Normal, kSpeed));

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
		if (m_move.y > 0.0f)
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

bool Enemy::UpdateAnim(int attachNo)
{
	//�A�j���[�V�������ݒ肳��Ă��Ȃ��̂ŏI��
	if (attachNo == -1) return false;

	//�A�j���[�V������i�s������
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//���݂̍Đ��J�E���g���擾
	now += 0.5f * m_animSpeed;	//�A�j���[�V�����i�߂�

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

	m_animSpeed = m_animSpeedMap[m_state];
}
