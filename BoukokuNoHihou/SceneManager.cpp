#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneDebug.h"
#include "Pad.h"

SceneManager::SceneManager() :
	m_pScene(nullptr)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	//�ŏ��̃V�[���̃��������m�ۂ���
#ifdef _DEBUG


	m_pScene = std::make_shared<SceneDebug>();

#else

	m_pScene = std::make_shared<SceneTitle>();

#endif

	m_pScene->Init();

}

void SceneManager::Update()
{
	Pad::Update();

	std::shared_ptr<SceneBase> pNext = m_pScene->Update();

	if (!pNext)
	{
		// ���ݏ������̃V�[���̏I������
		m_pScene->End();
	}

	if (pNext != m_pScene)
	{
		//���ݏ������̃V�[���̏I������
		m_pScene->End();

		//Update���Ԃ����V�����V�[���̊J�n�������s��
		m_pScene = pNext;
		m_pScene->Init();
	}

}

void SceneManager::Draw()
{
	m_pScene->Draw();
}

void SceneManager::End()
{
	m_pScene->End();
}

