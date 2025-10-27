#pragma once
#include "SceneBase.h"
#include "DxLib.h"
#include <math.h>
#include <memory>


class Stage;
class Ui;

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
	bool UpdateAnim(int attachNo);

	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<Ui> m_pUi;


private:
	//モデルハンドル
	int m_modelHandle;

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション


	VECTOR m_pos;
	VECTOR m_cameraPos;


};

