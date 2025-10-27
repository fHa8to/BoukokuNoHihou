#include "SceneOver.h"
#include "DxLib.h"
#include "SceneTitle.h"
#include "Game.h"
#include "Pad.h"
#include "Stage.h"
#include "Ui.h"


namespace
{
	//EnemyModleモデルデータ
	const char* const kEnemyModel = "data/model/skeleton/skeleton1.mv1";
	//SEのファイル名
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

	//モデルのサイズ変更
	constexpr float kExpansion = 0.1f;

	//アニメーション番号
	constexpr int kIdleAnimIndex = 1;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 4.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;

	//フェードイン、フェードアウトの数値
	constexpr int kFadeValue = 255;

	//フェード値の増減
	constexpr int kFadeUpDown = 8;


}


SceneOver::SceneOver():
	m_isCommand(false),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	m_modelHandle(),
	m_fadeAlpha(0),
	m_currentAnimNo(-1),
	m_isSceneEnd(false),
	m_pos(VGet(0.0f, 0.0f, 0.0f))
{
	m_pStage = std::make_shared<Stage>();
	m_pUi = std::make_shared<Ui>();

}

SceneOver::~SceneOver()
{

}

void SceneOver::Init()
{
	m_modelHandle = MV1LoadModel(kEnemyModel);
	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

	// 待機アニメーション（kFallingAnimIndex）を設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	m_pStage->Init();

	m_pos = VGet(-15.0f, 0.0f, 80.0f);

	m_cameraPos = VGet(0.0f, 20.0f, 100.0f);

	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

	// カメラの初期位置と向きを設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, VGet(0, 0, 0));

	// カメラの近クリップ面と遠クリップ面を設定
	SetCameraNearFar(1.0f, 100000.0f); // 近クリップ面を1.0fに設定

}

std::shared_ptr<SceneBase> SceneOver::Update()
{
	m_pStage->Update();

	// アニメーションを進める（ループ再生）
	UpdateAnim(m_currentAnimNo);


	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, 180.0f, 0.0f));



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

	//エネミーモデルの座標
	MV1SetPosition(m_modelHandle, m_pos);
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	DrawString(0, 0, "SceneOver", GetColor(255, 255, 255));

#endif

	m_pUi->OverDraw();

	//フェードの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく


}

void SceneOver::End()
{
	m_pStage->End();
	MV1DeleteModel(m_modelHandle);
	m_pUi->End();

}

bool SceneOver::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.1f;	//アニメーション進める

	//現在再生中のアニメーションの総カウントを取得
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		now -= total;
		isLoop = true;

	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}
