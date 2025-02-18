#pragma once
#include "DxLib.h"
#include <memory>

class Player;
class Camera
{
public:
	Camera();
	virtual ~Camera();

	void Init();

	void PlayerCameraUpdate(Player& player);

	//�J�����̃A���O�����擾
	const float GetAngle() const { return m_angle; }

	//�J�����̍��W���擾
	const VECTOR& GetPos() const { return m_pos; }

	//�����_�̍��W���擾
	const VECTOR& GetTargetPos() const { return m_targetPos; }


private:
	DINPUT_JOYSTATE input;

	std::shared_ptr<Player> m_pPlayer;


	VECTOR m_pos;			//�J�������W
	VECTOR m_targetPos;		//�����_���W
	VECTOR m_cameraAngle;	//�J�����̃A���O��


	float m_angle;			//�J�����p�x
};

