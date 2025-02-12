#pragma once
#include "SceneBase.h"
#include "DxLib.h"

class SceneClear : public SceneBase
{
public:
	SceneClear();
	virtual ~SceneClear();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;

private:

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//�n���h��
	int m_handle;

	//�J�����̍��W
	VECTOR m_cameraPos;


};

