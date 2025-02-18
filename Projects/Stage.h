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
	int m_modelHandle;	//モデルハンドル
	int m_collisionHandle;   //マップのコリジョンのデータを入れる変数

	VECTOR m_pos;
	VECTOR m_collisionMapPosition;   //コリジョンのポジション設定
	VECTOR m_playerPos;

};
