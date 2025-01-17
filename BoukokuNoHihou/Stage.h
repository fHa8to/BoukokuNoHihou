#pragma once
#include "DxLib.h"
#include <math.h>
#include <memory>


class Player;
class Stage
{
public:
	Stage();
	virtual ~Stage();

	void Init();
	void Update();
	void Draw();
	void End();

	int GetCollisionMap() { return m_collisionHandle; }
	VECTOR GetVectorMapPos() { return m_pos; }

private:
	int m_modelHandle;	//���f���n���h��
	int m_collisionHandle;   //�}�b�v�̃R���W�����̃f�[�^������ϐ�

	VECTOR m_pos;
	VECTOR m_collisionMapPosition;   //�R���W�����̃|�W�V�����ݒ�
	VECTOR m_playerPos;

};
