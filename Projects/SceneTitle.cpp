#include "SceneTitle.h"
#include "DxLib.h"
#include "SceneGame.h"
#include "Game.h"
#include "Pad.h"
#include "Ui.h"


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

	//アニメーション番号
	constexpr int kIdleAnimIndex = 0;

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
	m_bottnHandle(0),
	m_modelHandle(MV1LoadModel("data/model/skeleton/skeleton1.mv1")),
	m_fadeAlpha(0),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_animSpeed(0.0f),
	m_isSceneEnd(false),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraAngle(0.0f),
	m_selectedMenuItem(0)
{
	m_pStage = std::make_shared<Stage>();
	m_pUi = std::make_shared<Ui>();

	m_pos = VGet(-121.0f, 120.0f, -137.0f);
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

	//待機アニメーションを設定
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, kIdleAnimIndex, -1, false);
	m_prevAnimNo = -1;
	m_animBlendRate = 1.0f;


	m_bottnHandle = LoadGraph("data/image/GameBottn1.png");
	m_arrowHandle = LoadGraph("data/image/sankaku.png");
	m_title = LoadGraph("data/image/title.png");

	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	//移動する座標
	PointPos[0] = VGet(0.0f, 0.0f + kCameraPosY, 100.0f);
	PointPos[1] = VGet(23.0f, 0.0f + kCameraPosY, -178.0f);
	PointPos[2] = VGet(34.0f, 32.0f + kCameraPosY, -244.0f);
	PointPos[3] = VGet(29.0f, 32.0f + kCameraPosY, -252.0f);
	PointPos[4] = VGet(-26.0f, 51.0f + kCameraPosY, -265.0f);
	PointPos[5] = VGet(-90.0f, 62.0f + kCameraPosY, -215.0f);
	PointPos[6] = VGet(-121.0f, 93.0f + kCameraPosY, -242.0f);
	PointPos[7] = VGet(-159.0f, 100.0f + kCameraPosY, -216.0f);
	PointPos[8] = VGet(-184.0f, 100.0f + kCameraPosY, -190.0f);
	PointPos[9] = VGet(-123.0f, 119.0f + kCameraPosY, -151.0f);
	PointPos[10] = VGet(-50.0f, 119.0f + kCameraPosY, -59.0f);
	PointPos[11] = VGet(-22.0f, 137.0f + kCameraPosY, -6.0f);
	PointPos[12] = VGet(71.0f, 152.0f + kCameraPosY, 110.0f);
	PointPos[13] = VGet(-10.0f, 166.0f + kCameraPosY, 230.0f);
	PointPos[14] = VGet(-97.0f, 182.0f + kCameraPosY, 198.0f);
	PointPos[15] = VGet(-138.0f, 200.0f + kCameraPosY, 174.0f);
	PointPos[16] = VGet(-182.0f, 232.0f + kCameraPosY, 129.0f);
	PointPos[17] = VGet(-197.0f, 232.0f + kCameraPosY, 113.0f);
	PointPos[18] = VGet(-205.0f, 270.0f + kCameraPosY, 17.0f);
	PointPos[19] = VGet(-226.0f, 270.0f + kCameraPosY, -36.0f);
	PointPos[20] = VGet(32.0f, 270.0f + kCameraPosY, -37.0f);
	PointPos[21] = VGet(109.0f, 270.0f + kCameraPosY, -38.0f);


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
	

	m_pUi->TitleDraw();
	DrawGraph(Game::kScreenWidth / 2 - 20 , Game::kScreenHeight/ 2 + 150, m_title, true);


	// メニュー項目の描画
	const int menuX = Game::kScreenWidth / 2 - 40;
	const int menuY = Game::kScreenHeight - 220;
	const int menuSpacing = 40;


	// 描画する文字列のサイズを設定
	SetFontSize(kFontSize);

	const char* menuItems[kMenuItemCount] = { "",""};

	for (int i = 0; i < kMenuItemCount; ++i)
	{
		int color = (i == m_selectedMenuItem) ? GetColor(255, 0, 0) : GetColor(255, 255, 255);

		// 選択中の項目の左に矢印画像を描画
		if (i == m_selectedMenuItem)
		{
			int arrowX = menuX - 40; // 画像の位置（左に寄せる）
			int arrowY = menuY + i * menuSpacing;

			DrawGraph(arrowX, arrowY, m_arrowHandle, true); // true = 半透明対応
		}

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
	m_pUi->End();
	MV1DeleteModel(m_modelHandle);
}

bool SceneTitle::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.5f * m_animSpeed; // アニメーションを再生速度に応じて進める

	//現在再生中のアニメーションの総カウントを取得
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		if (m_isStopEnd)
		{
			now = total;
		}
		else
		{
			now -= total;
		}
		isLoop = true;
	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

void SceneTitle::ChangeAnim(int animIndex)
{
	//さらに古いアニメーションがアタッチされている場合はこの時点で削除しておく
	if (m_prevAnimNo != -1)
	{
		MV1DetachAnim(m_modelHandle, m_prevAnimNo);
	}

	//現在再生中の待機アニメーションは変更前のアニメーション扱いに
	m_prevAnimNo = m_currentAnimNo;

	//変更後のアニメーションとして攻撃アニメーションを改めて設定する
	m_currentAnimNo = MV1AttachAnim(m_modelHandle, animIndex, -1, false);

	//切り替えの瞬間は変更後のアニメーションが再生される
	m_animBlendRate = 0.0f;

	//変更前のアニメーション100%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_prevAnimNo, 1.0f - m_animBlendRate);
	//変更後のアニメーション0%
	MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAnimNo, m_animBlendRate);

	//// 現在のステートに応じたアニメーションの再生速度を設定
	//m_animSpeed = m_animSpeedMap[m_nowState];

}

