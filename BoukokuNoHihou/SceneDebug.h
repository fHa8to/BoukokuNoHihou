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
		Debug,		//�f�o�b�O
		Title,		//�^�C�g��
		Game,		//�v���C
		Clear,		//�N���A
		Over,		//�I�[�o�[

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
	int m_count;		// �J�[�\���̃J�E���g

	std::array<std::string, static_cast<int>(Scene::SceneNum)> m_sceneString;

	int m_fadeAlpha;

	bool m_isSceneEnd;
	bool m_isCommand;

	//�J�����̍��W
	VECTOR m_cameraPos;

};

