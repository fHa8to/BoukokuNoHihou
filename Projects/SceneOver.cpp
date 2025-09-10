#include "SceneOver.h"
#include "DxLib.h"
#include "SceneTitle.h"
#include "Game.h"
#include "Pad.h"
#include "Stage.h"


namespace
{

	//フェードイン、フェードアウトの数値
	constexpr int kFadeValue = 255;

	//フェード値の増減
	constexpr int kFadeUpDown = 8;

	//SEのファイル名
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

}


SceneOver::SceneOver():
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f))
{
	m_pStage = std::make_shared<Stage>();
}

SceneOver::~SceneOver()
{

}

void SceneOver::Init()
{
	m_handle = LoadGraph("data/image/GameOver.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	m_pStage->Init();

	m_cameraPos = VGet(0.0f, 20.0f, 100.0f);

	// カメラの初期位置と向きを設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, VGet(0, 0, 0));

	// カメラの近クリップ面と遠クリップ面を設定
	SetCameraNearFar(1.0f, 100000.0f); // 近クリップ面を1.0fに設定

}

std::shared_ptr<SceneBase> SceneOver::Update()
{
	m_pStage->Update();

	if (Pad::IsTrigger(PAD_INPUT_1))	// パッドの1ボタンorキーボードのZキー
	{

		m_isSceneEnd = true;
		m_isCommand = true;

		PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

	}

	if (m_isSceneEnd && m_fadeAlpha >= kFadeValue)
	{
		return std::make_shared<SceneTitle>();

	}



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

void SceneOver::Draw()
{
	m_pStage->Draw();

#ifdef _DEBUG

	DrawString(0, 0, "SceneOver", GetColor(255, 255, 255));

#endif

	DrawGraph(Game::kScreenWidth / 2 - 270, Game::kScreenHeight / 3, m_handle, true);


	//フェードの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく


}

void SceneOver::End()
{
	m_pStage->End();
}
