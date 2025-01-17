#include "SceneOver.h"
#include "DxLib.h"
#include "SceneTitle.h"
#include "Game.h"
#include "Pad.h"


namespace
{

	//�t�F�[�h�C���A�t�F�[�h�A�E�g�̐��l
	constexpr int kFadeValue = 255;

	//�t�F�[�h�l�̑���
	constexpr int kFadeUpDown = 8;

	//SE�̃t�@�C����
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

}


SceneOver::SceneOver()
{
}

SceneOver::~SceneOver()
{

}

void SceneOver::Init()
{
	m_handle = LoadGraph("data/image/GameClear.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;


}

std::shared_ptr<SceneBase> SceneOver::Update()
{

	if (Pad::IsTrigger(PAD_INPUT_1))	// �p�b�h��1�{�^��or�L�[�{�[�h��Z�L�[
	{

		m_isSceneEnd = true;
		m_isCommand = true;

		PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

	}

	if (m_isSceneEnd && m_fadeAlpha >= kFadeValue)
	{
		return std::make_shared<SceneTitle>();

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

void SceneOver::Draw()
{

#ifdef _DEBUG

	DrawString(0, 0, "SceneOver", GetColor(255, 255, 255));

#endif
	DrawGraph(0, 0, m_handle, true);

	//�t�F�[�h�̕`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���


}

void SceneOver::End()
{
}
