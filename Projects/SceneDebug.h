#pragma once
#include "DxLib.h"
#include "SceneBase.h"

#include <string>
#include <array>

class SceneDebug : public SceneBase
{
public:
	enum class Scene
	{
		Debug,		//デバッグ
		Title,		//タイトル
		Game,		//プレイ
		Clear,		//クリア
		Over,		//オーバー

		SceneNum,
	};

public:
	SceneDebug();
	virtual ~SceneDebug();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;

private:
	void UpdateCursor();

	std::shared_ptr<SceneBase> UpdateNextScene();

private:
	int m_count;		// カーソルのカウント

	std::array<std::string, static_cast<int>(Scene::SceneNum)> m_sceneString;

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//カメラの座標
	VECTOR m_cameraPos;

};

