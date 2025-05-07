#pragma once
#include "DxLib.h"
#include <math.h>
#include <memory>
#include <unordered_map>


class Explanation
{
public:
	Explanation();
	virtual ~Explanation();

	void Init();
	void Update();
	void Draw();
	void End();

	//座標を取得
	const VECTOR& GetPos() const { return m_pos; }

	//頭の座標を取得
	VECTOR& GetHeadPos() { return m_headPos; }

	//当たり判定の半径
	float GetRadius() { return m_modelRadius; }


private:
	//アニメーションの進行
//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);

private:
	//モデルハンドル
	int m_modelHandle;

	float m_modelRadius;

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度

	float m_angle;

	//表示情報
	VECTOR m_pos;
	VECTOR m_headPos;

};

