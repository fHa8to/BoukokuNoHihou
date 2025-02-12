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
	//アナログスティックを使って移動
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInputRight(&analogX, &analogZ, DX_INPUT_PAD1);	
	//GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	Pad::Update();



	//カメラに位置を反映
	//注視点の座標
	VECTOR playerAimPos = VGet(player.GetPos().x, player.GetPos().y - 5.0f, player.GetPos().z);
	//ベクトルの方向(注視点,カメラのポジション)
	VECTOR posToAim = VSub(playerAimPos, m_pos);

	if (analogX >= 10)
	{
		m_angle -= 0.05f;
	}
	if (analogX <= -10)
	{
		m_angle += 0.05f;
	}




	//注視点の座標をプレイヤーの座標に代入
	m_targetPos = VAdd(player.GetPos(), VGet(0.0f, 10.0f, 0.0f));


	m_pos.x += cosf(m_angle) * kCameraDist;
	m_pos.y += kCameraDist;
	m_pos.z += sinf(m_angle) * kCameraDist;

	//現在位置にポジションを足す
	m_pos = VAdd(m_pos, posToAim);

	SetCameraPositionAndTarget_UpVecY(m_pos, m_targetPos);

}

