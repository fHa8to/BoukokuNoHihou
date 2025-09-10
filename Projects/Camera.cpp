#include "Camera.h"
#include "Player.h"
#include "Pad.h"
#include "DxLib.h"

#define D2R(deg) ((deg)*DX_PI_F/180.0f)

namespace
{
	constexpr float kCameraDist = 40;

}

Camera::Camera():
	m_pos(VGet(0.0f,0.0f,0.0f)),
	m_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	m_cameraAngle(VGet(0.0f, 0.0f, 0.0f)),
	m_angle(DX_PI_F / 2),
	m_pitch(0.0f)
{
	SetCameraNearFar(5, 1600);

	m_pos = VGet(0.0f, 2.5f, 10.0f);

	m_targetPos = VGet(0.0f, 1.0f, 0.0f);

	m_cameraAngle = VGet(0.0f, 0.0f, 0.0f);

}

Camera::~Camera()
{
}

void Camera::Init()
{
}


void Camera::PlayerCameraUpdate(Player& player)
{
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInputRight(&analogX, &analogZ, DX_INPUT_PAD1);
	Pad::Update();

	// カメラ角度の更新
	if (analogX >= 10)
	{
		m_angle += 0.05f;
	}
	else if (analogX <= -10)
	{
		m_angle -= 0.05f;
	}

	if (analogZ >= 10)
	{
		m_pitch -= 0.05f;
	}
	else if (analogZ <= -10)
	{
		m_pitch += 0.05f;
	}

	// 上下回転角度に制限
	const float pitchMin = 0;
	const float pitchMax = 85.0f * DX_PI_F / 180.0f;

	if (m_pitch < pitchMin) m_pitch = pitchMin;
	if (m_pitch > pitchMax) m_pitch = pitchMax;

	// 注視点の取得
	VECTOR playerAimPos;
	if (player.GetSkill())
	{
		playerAimPos = VGet(player.GetPos().x, player.GetPos().y + 5.0f, player.GetPos().z);
	}
	else
	{
		playerAimPos = VGet(player.GetPos().x, player.GetPos().y + 5.0f, player.GetPos().z);
	}

	m_targetPos = VAdd(player.GetPos(), VGet(0.0f, 10.0f, 0.0f));

	// カメラ距離の計算
	float dist = kCameraDist;
	if (player.GetSkill()) dist += 25;

	// カメラ位置の算出
	m_pos.x = playerAimPos.x + cosf(m_pitch) * cosf(m_angle) * dist;
	m_pos.y = playerAimPos.y + sinf(m_pitch) * dist;
	m_pos.z = playerAimPos.z + cosf(m_pitch) * sinf(m_angle) * dist;

	SetCameraPositionAndTarget_UpVecY(m_pos, m_targetPos);
}