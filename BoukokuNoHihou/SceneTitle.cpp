#include "SceneTitle.h"
#include "DxLib.h"
#include "SceneGame.h"
#include "Game.h"
#include "Pad.h"


//スピード
#define MOVE_SPEED 0.5f

//この距離まで近づいたら次のポイントへ
#define CHECK_LENGTH 2.0f

namespace
{
	//フォントのサイズ
	constexpr int kFontSize = 32;

	//文字の位置
	constexpr int kFontPosX = 500;
	constexpr int kFontPosY = 500;


	//モデルの初期位置
	constexpr float kPosX = 0.0f;
	constexpr float kPosY = 0.0f;
	constexpr float kPosZ = 0.0f;

	//モデルのサイズ変更
	constexpr float kExpansion = 0.1f;

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
	constexpr int kCameraPosY = 20;

	//BGMのファイル名
	const char* const kBgmButton = "data/sound/Titledecide.mp3";

	//メニュー項目数
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

	// 描画する文字列のサイズを設定
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

	//移動する座標
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

	// カメラの初期位置と向きを設定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, VGet(0, 50, 0));

	 // カメラの近クリップ面と遠クリップ面を設定
    SetCameraNearFar(1.0f, 100000.0f); // 近クリップ面を1.0fに設定



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
		else if (Pad::IsTrigger(PAD_INPUT_A))	// パッドの1ボタンorキーボードのZキー
		{
			m_isCommand = true;
			PlaySoundFile(kBgmButton, DX_PLAYTYPE_BACK);

			switch (m_selectedMenuItem)
			{
			case 0: // 開始
				m_isSceneEnd = true;
				break;
			//case 1: // オプション
			//	break;
			case 1: // 終了
				DxLib_End(); // ゲームを終了
				break;
			}
		}
	}

	if (m_isSceneEnd && m_fadeAlpha >= kFadeValue)
	{
		return std::make_shared<SceneGame>();
	}

	//ポイント座標
	VECTOR TargetPos = PointPos[TargetNumber];

	//ターゲットの座標までの距離
	VECTOR Vec = VSub(TargetPos, m_cameraPos);
	float Length = VSize(Vec);

	//移動ベクトル
	VECTOR Move = VScale(VNorm(Vec), MOVE_SPEED);

	//座標を進める
	m_cameraPos = VAdd(m_cameraPos, Move);

	//移動後の座標
	Vec = VSub(TargetPos, m_cameraPos);

	//ポイント座標の距離が近づいたら
	if (VSize(Vec) < CHECK_LENGTH)
	{
		TargetNumber++;
		if (TargetNumber >= C_MAX_POINT)
		{
			TargetNumber = 0;
		}
	}

	//モデルの向きを変える
	VECTOR SubVector = VSub(m_cameraPos, m_pos);

	// atan を使用して角度を取得
	m_angle.y = atan2f(SubVector.x, SubVector.z);

	//プレイヤーの方向を向く
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angle.y + DX_PI_F, 0.0f));

	// カメラの位置と向きを毎フレーム設定
	VECTOR cameraTarget = VAdd(m_cameraPos, VNorm(Vec));
	cameraTarget.y = m_cameraPos.y; // カメラの高さは固定
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, cameraTarget);

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


void SceneTitle::Draw()
{

	m_pStage->Draw();

	//エネミーモデルの座標
	MV1SetPosition(m_modelHandle, m_pos);
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	DrawString(0, 0, "SceneTitle", GetColor(255, 255, 255));

		//エネミーの移動線
	for (int i = 0; i < C_MAX_POINT; i++)
	{
		int n1 = i;
		int n2 = (i + 1) % C_MAX_POINT;
		DrawLine3D(PointPos[n1], PointPos[n2], 0x00ff00);
	}



#endif
	


	DrawGraph(Game::kScreenWidth / 4, Game::kScreenHeight / 6, m_handle, true);


	// メニュー項目の描画
	const int menuX = Game::kScreenWidth / 2 - 5;
	const int menuY = Game::kScreenHeight - 220;
	const int menuSpacing = 40;


	// 描画する文字列のサイズを設定
	SetFontSize(kFontSize);

	const char* menuItems[kMenuItemCount] = { "開始", "終了" };

	for (int i = 0; i < kMenuItemCount; ++i)
	{
		int color = (i == m_selectedMenuItem) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);
		DrawString(menuX, menuY + i * menuSpacing, menuItems[i], color);
	}
	
	//フェードの描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく

}

void SceneTitle::End()
{
	m_pStage->End();
	DeleteGraph(m_handle);
	DeleteGraph(m_bottnHandle);
	MV1DeleteModel(m_modelHandle);
}

