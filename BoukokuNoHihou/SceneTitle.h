#pragma once
#include "DxLib.h"
#include "SceneBase.h"

class SceneTitle : public SceneBase
{
public:
	SceneTitle();
	virtual ~SceneTitle();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;

private:

	//�n���h��
	int m_handle;

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//�J�����̍��W
	VECTOR m_cameraPos;


};

