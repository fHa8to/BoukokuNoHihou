#pragma once
#include "DxLib.h"
#include <math.h>
#include <memory>
#include <unordered_map>

class Player;
class Ui;

class Explanation
{
public:
	Explanation();
	virtual ~Explanation();

	void Init();
	void Update(std::shared_ptr<Player> m_pPlayer);
	void Draw();
	void End();

	//座標を取得
	const VECTOR& GetPos() const { return m_pos; }

	//頭の座標を取得
	VECTOR& GetHeadPos() { return m_headPos; }

	//当たり判定の半径
	float GetRadius() { return m_modelRadius; }


	bool Translation(std::shared_ptr<Player> m_pPlayer);


private:
	//アニメーションの進行
//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);

	std::shared_ptr<Ui> m_pUi;


private:
	//モデルハンドル
	int m_modelHandle;


	float m_modelRadius;
	float m_discoveryRadius;


	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度

	bool m_isRange;
	bool m_isButton;

	float m_angle;

	//表示情報
	VECTOR m_pos;
	VECTOR m_headPos;

	int m_color = 0xffffff;


};

