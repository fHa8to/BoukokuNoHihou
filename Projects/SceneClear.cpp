#include "SceneClear.h"
#include "DxLib.h"
#include "SceneTitle.h"
#include "Game.h"
#include "Pad.h"
#include "Stage.h"

namespace
{
	//フォントのサイズ
	constexpr int kFontSize = 32;

	//文字の位置
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//モデルの初期位置
	//モデルの初期位置
	constexpr float kPosX = 300.0f;

	constexpr float kPosY = 100.0f;

	constexpr float kPosZ = 0.0f;

	//モデルのサイズ変更
	constexpr float kExpansion = 100.0f;

	//アニメモーションの番号
	//待機モーション
	constexpr int kStandByAnimIndex = 73;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 4.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//フェードイン、フェードアウトの数値
	constexpr int kFadeValue = 255;

	//フェード値の増減
	constexpr int kFadeUpDown = 8;
	//カメラ情報
	constexpr float kCameraX = 0.0f;
	constexpr float kCameraY = 150.0f;
	constexpr float kCameraZ = -600.0f;

	//BGMのファイル名
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

}

SceneClear::SceneClear() :
	m_isCommand(false),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f))
{
	m_pStage = std::make_shared<Stage>();

}

SceneClear::~SceneClear()
{
}

void SceneClear::Init()
{
	m_handle = LoadGraph("data/image/GameClear.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	m_pStage->Init();

	m_cameraPos = VGet(0.0f, 20.0f, 100.0f);

	// カメラの初期位置と向きを設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, VGet(0, 0, 0));

	// カメラの近クリップ面と遠クリップ面を設定
	SetCameraNearFar(1.0f, 100000.0f); // 近クリップ面を1.0fに設定

}

std::shared_ptr<SceneBase>  SceneClear::Update()
{
	m_pStage->Update();

	if (!m_isCommand)
	{
		if (Pad::IsTrigger(PAD_INPUT_1))	// パッドの1ボタンorキーボードのZキー
		{

			m_isSceneEnd = true;

			PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

		}
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

void SceneClear::Draw()
{
	m_pStage->Draw();

#ifdef _DEBUG

	DrawString(0, 0, "SceneClear", GetColor(255, 255, 255));

#endif

	DrawGraph(Game::kScreenWidth / 2 - 280, Game::kScreenHeight / 3, m_handle, true);


	//フェードの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく


}

void SceneClear::End()
{
	m_pStage->End();
}
