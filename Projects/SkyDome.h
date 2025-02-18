#pragma once
#include "DxLib.h"
#include <memory>
#include <cmath>

class Player;

class SkyDome
{
public:
	SkyDome();
	virtual ~SkyDome();

	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer);
	void Draw();

private:
	std::shared_ptr<Player> m_pPlayer;

private:
	float m_rot;

	//ƒ‚ƒfƒ‹ƒnƒ“ƒhƒ‹
	int m_modelHandle;

	VECTOR m_pos;

};

