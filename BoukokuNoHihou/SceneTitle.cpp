#include "SceneTitle.h"
#include "DxLib.h"
#include "SceneGame.h"
#include "Game.h"
#include "Pad.h"


//�X�s�[�h
#define MOVE_SPEED 0.5f

//���̋����܂ŋ߂Â����玟�̃|�C���g��
#define CHECK_LENGTH 2.0f

namespace
{
	//�t�H���g�̃T�C�Y
	constexpr int kFontSize = 32;

	//�����̈ʒu
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//���f���̏����ʒu
	constexpr float kPosX = 0.0f;
	constexpr float kPosY = 0.0f;
	constexpr float kPosZ = 0.0f;

	//���f���̃T�C�Y�ύX
	constexpr float kExpansion = 0.1f;

	//�A�j�����[�V�����̔ԍ�
	//�ҋ@���[�V����
	constexpr int kStandByAnimIndex = 73;

	//�A�j���[�V�����̐؂�ւ��ɂ�����t���[����
	constexpr float kAnimChangeFrame = 4.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//�t�F�[�h�C���A�t�F�[�h�A�E�g�̐��l
	constexpr int kFadeValue = 255;

	//�t�F�[�h�l�̑���
	constexpr int kFadeUpDown = 8;

	//�J�������
	constexpr int kCameraPosY = 20;

	//BGM�̃t�@�C����
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

	//���j���[���ڐ�
	constexpr int kMenuItemCount = 2;
}

SceneTitle::SceneTitle() :
	m_isCommand(false),
	m_handle(0),
	m_bottnHandle(0),
	m_modelHandle(MV1LoadModel("data/model/skeleton/skeleton1.mv1")),
	m_fadeAlpha(0),
	m_isSceneEnd(false),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraAngle(0.0f),
	m_selectedMenuItem(0)
{
	m_pStage = std::make_shared<Stage>();
	m_pos = VGet(-121.0f, 170.0f, -137.0f);
	m_move = VGet(0.0f, 0.0f, 0.0f);

	// �`�悷�镶����̃T�C�Y��ݒ�
	SetFontSize(kFontSize);
}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Init()
{
	m_pStage->Init();

	m_handle = LoadGraph("data/image/GameTitle.png");

	m_bottnHandle = LoadGraph("data/image/GameBottn1.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	//�ړ�������W
	PointPos[0] = VGet(0.0f, 0.0f + kCameraPosY, 100.0f);
	PointPos[1] = VGet(36.0f, 0.0f + kCameraPosY, -175.0f);
	PointPos[2] = VGet(42.0f, 57.0f + kCameraPosY, -229.0f);
	PointPos[3] = VGet(44.0f, 57.0f + kCameraPosY, -251.0f);
	PointPos[4] = VGet(33.0f, 57.0f + kCameraPosY, -258.0f);
	PointPos[5] = VGet(-3.0f, 93.0f + kCameraPosY, -270.0f);
	PointPos[6] = VGet(-35.0f, 93.0f + kCameraPosY, -262.0f);
	PointPos[7] = VGet(-84.0f, 114.0f + kCameraPosY, -229.0f);
	PointPos[8] = VGet(-108.0f, 114.0f + kCameraPosY, -239.0f);
	PointPos[9] = VGet(-129.0f, 136.0f + kCameraPosY, -242.0f);
	PointPos[10] = VGet(-166.0f, 161.0f + kCameraPosY, -219.0f);
	PointPos[11] = VGet(-187.0f, 161.0f + kCameraPosY, -173.0f);
	PointPos[12] = VGet(-121.0f, 165.0f + kCameraPosY, -137.0f);
	PointPos[13] = VGet(-44.0f, 165.0f + kCameraPosY, -79.0f);
	PointPos[14] = VGet(-10.0f, 158.0f + kCameraPosY, -9.0f);
	PointPos[15] = VGet(-31.0f, 158.0f + kCameraPosY, 32.0f);
	PointPos[16] = VGet(-69.0f, 211.0f + kCameraPosY, 67.0f);
	PointPos[17] = VGet(-168.0f, 211.0f + kCameraPosY, 76.0f);
	PointPos[18] = VGet(-205.0f, 290.0f + kCameraPosY, 17.0f);
	PointPos[19] = VGet(-226.0f, 290.0f + kCameraPosY, -36.0f);
	PointPos[20] = VGet(32.0f, 290.0f + kCameraPosY, -37.0f);
	PointPos[21] = VGet(109.0f, 290.0f + kCameraPosY, -38.0f);


	m_cameraPos = PointPos[0];

	TargetNumber = 1;


	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

	// �J�����̏����ʒu�ƌ�����ݒ�
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, VGet(0, 50, 0));

	 // �J�����̋߃N���b�v�ʂƉ��N���b�v�ʂ�ݒ�
    SetCameraNearFar(1.0f, 100000.0f); // �߃N���b�v�ʂ�1.0f�ɐݒ�



}

std::shared_ptr<SceneBase> SceneTitle::Update()
{
	m_pStage->Update();

	if (!m_isCommand)
	{
		if (Pad::IsTrigger(PAD_INPUT_DOWN))
		{
			m_selectedMenuItem = (m_selectedMenuItem + 1) % kMenuItemCount;
		}
		else if (Pad::IsTrigger(PAD_INPUT_UP))
		{
			m_selectedMenuItem = (m_selectedMenuItem - 1 + kMenuItemCount) % kMenuItemCount;
		}
		else if (Pad::IsTrigger(PAD_INPUT_A))	// �p�b�h��1�{�^��or�L�[�{�[�h��Z�L�[
		{
			m_isCommand = true;
			PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

			switch (m_selectedMenuItem)
			{
			case 0: // �J�n
				m_isSceneEnd = true;
				break;
			//case 1: // �I�v�V����
			//	break;
			case 1: // �I��
				DxLib_End(); // �Q�[�����I��
				break;
			}
		}
	}

	if (m_isSceneEnd && m_fadeAlpha >= kFadeValue)
	{
		return std::make_shared<SceneGame>();
	}

	//�|�C���g���W
	VECTOR TargetPos = PointPos[TargetNumber];

	//�^�[�Q�b�g�̍��W�܂ł̋���
	VECTOR Vec = VSub(TargetPos, m_cameraPos);
	float Length = VSize(Vec);

	//�ړ��x�N�g��
	VECTOR Move = VScale(VNorm(Vec), MOVE_SPEED);

	//���W��i�߂�
	m_cameraPos = VAdd(m_cameraPos, Move);

	//�ړ���̍��W
	Vec = VSub(TargetPos, m_cameraPos);

	//�|�C���g���W�̋������߂Â�����
	if (VSize(Vec) < CHECK_LENGTH)
	{
		TargetNumber++;
		if (TargetNumber >= C_MAX_POINT)
		{
			TargetNumber = 0;
		}
	}

	//���f���̌�����ς���
	VECTOR SubVector = VSub(m_cameraPos, m_pos);

	// atan ���g�p���Ċp�x���擾
	m_angle.y = atan2f(SubVector.x, SubVector.z);

	//�v���C���[�̕���������
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angle.y + DX_PI_F, 0.0f));

	// �J�����̈ʒu�ƌ����𖈃t���[���ݒ�
	VECTOR cameraTarget = VAdd(m_cameraPos, VNorm(Vec));
	cameraTarget.y = m_cameraPos.y; // �J�����̍����͌Œ�
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, cameraTarget);

	//�t���[���C���A�A�E�g
	if (m_isSceneEnd)
	{
		m_fadeAlpha += kFadeUpDown;
		if (m_fadeAlpha > kFadeValue)
		{
			m_fadeAlpha = kFadeValue;
		}
	}
	else
	{
		m_fadeAlpha -= kFadeUpDown;
		if (m_fadeAlpha < 0)
		{
			m_fadeAlpha = 0;
		}
	}

	return shared_from_this();
}


void SceneTitle::Draw()
{

	m_pStage->Draw();

	//�G�l�~�[���f���̍��W
	MV1SetPosition(m_modelHandle, m_pos);
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	DrawString(0, 0, "SceneTitle", GetColor(255, 255, 255));

		//�G�l�~�[�̈ړ���
	for (int i = 0; i < C_MAX_POINT; i++)
	{
		int n1 = i;
		int n2 = (i + 1) % C_MAX_POINT;
		DrawLine3D(PointPos[n1], PointPos[n2], 0x00ff00);
	}



#endif
	


	DrawGraph(Game::kScreenWidth / 4, Game::kScreenHeight / 6, m_handle, true);


	// ���j���[���ڂ̕`��
	const int menuX = Game::kScreenWidth / 2 - 5;
	const int menuY = Game::kScreenHeight - 220;
	const int menuSpacing = 40;


	// �`�悷�镶����̃T�C�Y��ݒ�
	SetFontSize(kFontSize);

	const char* menuItems[kMenuItemCount] = { "�J�n", "�I��" };

	for (int i = 0; i < kMenuItemCount; ++i)
	{
		int color = (i == m_selectedMenuItem) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		DrawString(menuX, menuY + i * menuSpacing, menuItems[i], color);
	}
	
	//�t�F�[�h�̕`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���

}

void SceneTitle::End()
{
	m_pStage->End();
	DeleteGraph(m_handle);
	DeleteGraph(m_bottnHandle);
	MV1DeleteModel(m_modelHandle);
}

