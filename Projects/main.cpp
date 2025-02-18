#include "DxLib.h"
#include "Game.h"
#include <memory>
#include "SceneManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"


// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{


#ifdef _DEBUG

	ChangeWindowMode(true);

#endif

	// ��ʂ̃T�C�Y�ύX
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, 32);

	SetWindowText("");

	if (DxLib_Init() == -1)		// �c�w���C�u��������������
	{
		return -1;			// �G���[���N�����璼���ɏI��
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// ���C�g�̐ݒ�
	ChangeLightTypeDir(VGet(-1.0f, -1.0f, -1.0f));
	CreateDirLightHandle(VGet(1.0f, 1.0f, 1.0f));



	// �V�[���Ǘ�
	std::shared_ptr<SceneManager> pScene = std::make_shared<SceneManager>();

	// ������
	pScene->Init();


	// �Q�[�����[�v
	while (ProcessMessage() != -1)
	{
		// ���̃t���[���̊J�n�������o���Ă���
		LONGLONG start = GetNowHiPerformanceCount();

		// �`����s���O�ɉ�ʂ��N���A����
		ClearDrawScreen();

		pScene->Update();
		 //�`��
		pScene->Draw();


		// ��ʂ��؂�ւ��̂�҂�
		ScreenFlip();

		// esc�L�[�ŃQ�[���I��
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPS60�ɌŒ肷��
		while (GetNowHiPerformanceCount() - start < 10000)
		{
			// 16.66�~���b(16667�}�C�N���b)�o�߂���܂ő҂�
		}
	}
	pScene->End();
	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}