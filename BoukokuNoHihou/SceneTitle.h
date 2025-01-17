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

	//ハンドル
	int m_handle;

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//カメラの座標
	VECTOR m_cameraPos;


};

