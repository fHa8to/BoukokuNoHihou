#include "Camera.h"
#include "Player.h"
#include "Pad.h"
#include "DxLib.h"

#define D2R(deg) ((deg)*DX_PI_F/180.0f)

namespace
{
	constexpr float kCameraDist = 30;

}

Camera::Camera():
	m_pos(VGet(0.0f,0.0f,0.0f)),
	m_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraAngle(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(-DX_PI_F / 2)
{
	SetCameraNearFar(5, 1600);

	m_pos = VGet(0.0f, 1.0f, 10.0f);

	m_targetPos = VGet(0.0f, 0.5f, 0.0f);

	m_cameraAngle = VGet(D2R(-20.0f), 0.0f, 0.0f);

}

Camera::~Camera()
{
}

void Camera::Init()
{
}


void Camera::PlayerCameraUpdate(Player& player)
{
	//�A�i���O�X�e�B�b�N���g���Ĉړ�
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInputRight(&analogX, &analogZ, DX_INPUT_PAD1);	
	//GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	Pad::Update();



	//�J�����Ɉʒu�𔽉f
	//�����_�̍��W
	VECTOR playerAimPos = VGet(player.GetPos().x, player.GetPos().y - 5.0f, player.GetPos().z);
	//�x�N�g���̕���(�����_,�J�����̃|�W�V����)
	VECTOR posToAim = VSub(playerAimPos, m_pos);

	if (analogX >= 10)
	{
		m_angle -= 0.05f;
	}
	if (analogX <= -10)
	{
		m_angle += 0.05f;
	}




	//�����_�̍��W���v���C���[�̍��W�ɑ��
	m_targetPos = VAdd(player.GetPos(), VGet(0.0f, 10.0f, 0.0f));


	m_pos.x += cosf(m_angle) * kCameraDist;
	m_pos.y += kCameraDist;
	m_pos.z += sinf(m_angle) * kCameraDist;

	//���݈ʒu�Ƀ|�W�V�����𑫂�
	m_pos = VAdd(m_pos, posToAim);

	SetCameraPositionAndTarget_UpVecY(m_pos, m_targetPos);

}

