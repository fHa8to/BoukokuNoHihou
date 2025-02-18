#include "DxLib.h"
#include "Game.h"
#include <memory>
#include "SceneManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"


// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


#ifdef _DEBUG

	ChangeWindowMode(true);

#endif

	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, 32);

	SetWindowText("");

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// ライトの設定
	ChangeLightTypeDir(VGet(-1.0f, -1.0f, -1.0f));
	CreateDirLightHandle(VGet(1.0f, 1.0f, 1.0f));



	// シーン管理
	std::shared_ptr<SceneManager> pScene = std::make_shared<SceneManager>();

	// 初期化
	pScene->Init();


	// ゲームループ
	while (ProcessMessage() != -1)
	{
		// このフレームの開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();

		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		pScene->Update();
		 //描画
		pScene->Draw();


		// 画面が切り替わるのを待つ
		ScreenFlip();

		// escキーでゲーム終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - start < 10000)
		{
			// 16.66ミリ秒(16667マイクロ秒)経過するまで待つ
		}
	}
	pScene->End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}