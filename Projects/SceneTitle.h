#pragma once
#include "DxLib.h"
#include "SceneBase.h"
#include <math.h>
#include <memory>

//ライントレースの点数
#define C_MAX_POINT 22

class Stage;
class Ui;

class SceneTitle : public SceneBase
{
public:
	SceneTitle();
	virtual ~SceneTitle();

	virtual void Init() override final;
	virtual std::shared_ptr<SceneBase> Update() override final;
	virtual void Draw() override final;
	virtual void End() override final;

private:
	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	void ChangeAnim(int animIndex);


	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<Ui> m_pUi;

private:

	//画像ハンドル
	int m_handle;
	int m_bottnHandle;

	//モデルハンドル
	int m_modelHandle;	

	float m_cameraAngle; // カメラの回転角度を管理する変数

	int m_fadeAlpha;

	int m_selectedMenuItem;

	bool m_isSceneEnd;
	bool m_isCommand;

	int TargetNumber;
	int m_arrowHandle; // ← 矢印画像
	int m_title; // ← 矢印画像

	//アニメーション情報
	int m_animIndex;
	int m_currentAnimNo;	//現在のアニメーション
	int m_prevAnimNo;		//変更前のアニメーション
	float m_animBlendRate;	//アニメーション合成割合
	float m_animSpeed;		//アニメーション速度
	bool m_isStopEnd;	// アニメーション最後で停止させる
	int m_runFrame;

	//カメラの座標
	VECTOR m_cameraPos;
	VECTOR m_pos;
	VECTOR m_move;
	VECTOR m_angle;


	//進む距離
	VECTOR m_distance;


	VECTOR PointPos[C_MAX_POINT];
	VECTOR PointPos1[C_MAX_POINT];


};

