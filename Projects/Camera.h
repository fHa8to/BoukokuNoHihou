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

	//カメラのアングルを取得
	const float GetAngle() const { return m_angle; }

	//カメラの座標を取得
	const VECTOR& GetPos() const { return m_pos; }

	//注視点の座標を取得
	const VECTOR& GetTargetPos() const { return m_targetPos; }


private:
	DINPUT_JOYSTATE input;

	std::shared_ptr<Player> m_pPlayer;


	VECTOR m_pos;			//カメラ座標
	VECTOR m_targetPos;		//注視点座標
	VECTOR m_cameraAngle;	//カメラのアングル


	float m_angle;			//カメラ角度
};

