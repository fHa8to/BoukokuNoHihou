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
	//フォントのサイズ
	constexpr int kFontSize = 32;

	//文字の位置
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//フェードイン、フェードアウトの数値
	constexpr int kFadeValue = 255;

	//フェード値の増減
	constexpr int kFadeUpDown = 8;

	//カメラ情報
	constexpr float kCameraX = 0.0f;
	constexpr float kCameraY = 150.0f;
	constexpr float kCameraZ = -600.0f;


	constexpr float kCursorPosX = 500.0f;		// 初期カーソル位置X
	constexpr float kCursorPosY = 300.0f;		// 初期カーソル位置Y

	constexpr float kCursorMove = 50.0f;		// カーソルの移動量

	//SEのファイル名
	const char* const kBgmButton = "data/sound/Titledecide.mp3";


}

SceneDebug::SceneDebug():
	m_isCommand(false),
	m_count(0)
{
	//移動用文字の設定
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

	//フレームイン、アウト
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
	// 文字列の描画
	for (int i = 0; i < static_cast<int>(m_sceneString.size()); i++)
	{
		if (m_count == i)
		{
			// 選ばれている文字は赤くする
			DrawString(static_cast<int>(kCursorPosX),
				static_cast<int>(kCursorPosY + (i * kCursorMove)),
				m_sceneString[i].c_str(),
				0xff0000);
		}
		else
		{
			// その他の文字は白
			DrawString(static_cast<int>(kCursorPosX),
				static_cast<int>(kCursorPosY + (i * kCursorMove)),
				m_sceneString[i].c_str(),
				0xffffff);
		}
	}

	DrawFormatString(0, 0, 0xffffff, "Debug");
	//DrawFormatString(0, 14, 0x00ffff, "シーンを選択してください");

	//フェードの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく


}

void SceneDebug::End()
{
}

void SceneDebug::UpdateCursor()
{
	//カーソルの移動
	if (Pad::IsTrigger(PAD_INPUT_UP))
	{
		m_count--;
		//カーソルがループするようにする
		if (m_count < 0) m_count = static_cast<int>(m_sceneString.size() - 1);
	}
	else if (Pad::IsTrigger(PAD_INPUT_DOWN))
	{
		m_count++;
		//カーソルがループするようにする
		if (m_count >= static_cast<int>(m_sceneString.size())) m_count = 0;
	}
}

std::shared_ptr<SceneBase> SceneDebug::UpdateNextScene()
{
	std::shared_ptr<SceneBase> nextScene = nullptr;

	//選択されたシーンに遷移する
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
