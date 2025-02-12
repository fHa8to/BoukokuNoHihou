#pragma once
#include "DxLib.h"
#include "SceneBase.h"
#include <math.h>
#include <memory>

//���C���g���[�X�̓_��
#define C_MAX_POINT 22

class Stage;

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
	std::shared_ptr<Stage> m_pStage;

private:

	//�摜�n���h��
	int m_handle;
	int m_bottnHandle;

	//���f���n���h��
	int m_modelHandle;	

	float m_cameraAngle; // �J�����̉�]�p�x���Ǘ�����ϐ�

	int m_fadeAlpha;

	int m_selectedMenuItem;

	bool m_isSceneEnd;
	bool m_isCommand;

	int TargetNumber;

	//�J�����̍��W
	VECTOR m_cameraPos;
	VECTOR m_pos;
	VECTOR m_move;
	VECTOR m_angle;


	//�i�ދ���
	VECTOR m_distance;


	VECTOR PointPos[C_MAX_POINT];
	VECTOR PointPos1[C_MAX_POINT];


};

