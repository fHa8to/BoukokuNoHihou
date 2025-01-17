#include "SceneTitle.h"
#include "DxLib.h"
#include "SceneGame.h"
#include "Game.h"
#include "Pad.h"

namespace
{
	//�t�H���g�̃T�C�Y
	constexpr int kFontSize = 32;

	//�����̈ʒu
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//���f���̏����ʒu
	constexpr float kPosX = 300.0f;

	constexpr float kPosY = 100.0f;

	constexpr float kPosZ = 0.0f;

	//���f���̃T�C�Y�ύX
	constexpr float kExpansion = 100.0f;

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
	constexpr float kCameraX = 0.0f;
	constexpr float kCameraY = 150.0f;
	constexpr float kCameraZ = -600.0f;

	//BGM�̃t�@�C����
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

}

SceneTitle::SceneTitle() :
	m_isCommand(false)
{

}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Init()
{
	m_handle = LoadGraph("data/image/GameTitle.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;


}

std::shared_ptr<SceneBase> SceneTitle::Update()
{

	if (!m_isCommand)
	{
		if (Pad::IsTrigger(PAD_INPUT_A))	// �p�b�h��1�{�^��or�L�[�{�[�h��Z�L�[
		{

			m_isSceneEnd = true;
			m_isCommand = true;

			PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);


		}
	}
	if (m_isSceneEnd && m_fadeAlpha >= kFadeValue)
	{
		return std::make_shared<SceneGame>();

	}



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

#ifdef _DEBUG

	DrawString(0, 0, "SceneTitle", GetColor(255, 255, 255));

#endif

	DrawGraph(0, 0, m_handle, true);

	DrawString(Game::kScreenWidth / 2 - 50, Game::kScreenHeight - 150, "�{�^��A �X�^�[�g", 0x000000);


	//�t�F�[�h�̕`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���

}

void SceneTitle::End()
{
}

