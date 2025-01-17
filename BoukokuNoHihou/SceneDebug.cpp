#include "DxLib.h"
#include "SceneDebug.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneClear.h"
#include "SceneOver.h"
#include "Game.h"
#include "Pad.h"

namespace
{
	//�t�H���g�̃T�C�Y
	constexpr int kFontSize = 32;

	//�����̈ʒu
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//�t�F�[�h�C���A�t�F�[�h�A�E�g�̐��l
	constexpr int kFadeValue = 255;

	//�t�F�[�h�l�̑���
	constexpr int kFadeUpDown = 8;

	//�J�������
	constexpr float kCameraX = 0.0f;
	constexpr float kCameraY = 150.0f;
	constexpr float kCameraZ = -600.0f;


	constexpr float kCursorPosX = 500.0f;		// �����J�[�\���ʒuX
	constexpr float kCursorPosY = 300.0f;		// �����J�[�\���ʒuY

	constexpr float kCursorMove = 50.0f;		// �J�[�\���̈ړ���

	//SE�̃t�@�C����
	const char* const kBgmButton = "data/sound/Titledecide.mp3";


}

SceneDebug::SceneDebug():
	m_isCommand(false),
	m_count(0)
{
	//�ړ��p�����̐ݒ�
	m_sceneString[static_cast<int>(Scene::Debug)] = "Debug";
	m_sceneString[static_cast<int>(Scene::Title)] = "Title";
	m_sceneString[static_cast<int>(Scene::Game)] = "Game";
	m_sceneString[static_cast<int>(Scene::Clear)] = "Clear";
	m_sceneString[static_cast<int>(Scene::Over)] = "Over";

}

SceneDebug::~SceneDebug()
{
}

void SceneDebug::Init()
{
	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;
}

std::shared_ptr<SceneBase> SceneDebug::Update()
{

	if (!m_isCommand)
	{
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			m_isSceneEnd = true;
			m_isCommand = true;
			PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

			return UpdateNextScene();
		}
	}

	UpdateCursor();

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

void SceneDebug::Draw()
{
	// ������̕`��
	for (int i = 0; i < static_cast<int>(m_sceneString.size()); i++)
	{
		if (m_count == i)
		{
			// �I�΂�Ă��镶���͐Ԃ�����
			DrawString(static_cast<int>(kCursorPosX),
				static_cast<int>(kCursorPosY + (i * kCursorMove)),
				m_sceneString[i].c_str(),
				0xff0000);
		}
		else
		{
			// ���̑��̕����͔�
			DrawString(static_cast<int>(kCursorPosX),
				static_cast<int>(kCursorPosY + (i * kCursorMove)),
				m_sceneString[i].c_str(),
				0xffffff);
		}
	}

	DrawFormatString(0, 0, 0xffffff, "Debug");
	//DrawFormatString(0, 14, 0x00ffff, "�V�[����I�����Ă�������");

	//�t�F�[�h�̕`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���


}

void SceneDebug::End()
{
}

void SceneDebug::UpdateCursor()
{
	//�J�[�\���̈ړ�
	if (Pad::IsTrigger(PAD_INPUT_UP))
	{
		m_count--;
		//�J�[�\�������[�v����悤�ɂ���
		if (m_count < 0) m_count = static_cast<int>(m_sceneString.size() - 1);
	}
	else if (Pad::IsTrigger(PAD_INPUT_DOWN))
	{
		m_count++;
		//�J�[�\�������[�v����悤�ɂ���
		if (m_count >= static_cast<int>(m_sceneString.size())) m_count = 0;
	}
}

std::shared_ptr<SceneBase> SceneDebug::UpdateNextScene()
{
	std::shared_ptr<SceneBase> nextScene = nullptr;

	//�I�����ꂽ�V�[���ɑJ�ڂ���
	switch (m_count)
	{
	case static_cast<int>(Scene::Debug):
		nextScene = std::make_shared<SceneDebug>();
		break;
	case static_cast<int>(Scene::Title):
		nextScene = std::make_shared<SceneTitle>();
		break;
	case static_cast<int>(Scene::Game):
		nextScene = std::make_shared<SceneGame>();
		break;
	case static_cast<int>(Scene::Clear):
		nextScene = std::make_shared<SceneClear>();
		break;
	case static_cast<int>(Scene::Over):
		nextScene = std::make_shared<SceneOver>();
		break;
	default:
		return shared_from_this();
		break;
	}
	return nextScene;
}
