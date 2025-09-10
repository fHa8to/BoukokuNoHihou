#pragma once
#include "SceneBase.h"
#include "DxLib.h"
#include <math.h>
#include <memory>

class Stage;


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
	std::shared_ptr<Stage> m_pStage;

private:
	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//ƒnƒ“ƒhƒ‹
	int m_handle;

	VECTOR m_cameraPos;


};

