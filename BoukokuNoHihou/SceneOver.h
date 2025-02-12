#pragma once
#include "SceneBase.h"
#include "DxLib.h"

class SceneOver : public SceneBase
{
public:
	SceneOver();
	virtual ~SceneOver();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;


private:
	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//ハンドル
	int m_handle;

	//カメラの座標
	VECTOR m_cameraPos;


};

