#include "Explanation.h"
#include "DxLib.h"
#include <cmath>
#include <cassert>




namespace
{
    const char* const kModelCharacterFilename = "data/model/skeleton/skeleton.mv1";

    //モデルのサイズ変更
    constexpr float kExpansion = 7.0f;

    constexpr int kFallingAnimIndex = 0;

	//アニメーションの切り替えにかかるフレーム数
	constexpr float kAnimChangeFrame = 8.0f;
	constexpr float kAnimChangeRateSpeed = 1.0f / kAnimChangeFrame;


	constexpr int kModelRadius = 4.0f;
	constexpr int kModelAngle = 180.0f;

	//カプセルの座標
	constexpr int upperPart = 20;	//上部
	constexpr int bottom = 2;		//下部

}


Explanation::Explanation():
	m_modelHandle(-1),
	m_currentAnimNo(-1),
	m_prevAnimNo(-1),
	m_animBlendRate(0.0f),
	m_angle(kModelAngle),
	m_modelRadius(kModelRadius),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_headPos(VGet(0.0f, 0.0f, 0.0f))
{
	m_pos = VGet(-20.0f, 1.0f, -40.0f);


}

Explanation::~Explanation()
{
}

void Explanation::Init()
{
	m_modelHandle = MV1LoadModel(kModelCharacterFilename);
    MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));

    // 待機アニメーション（kFallingAnimIndex）を設定
    m_currentAnimNo = MV1AttachAnim(m_modelHandle, kFallingAnimIndex, -1, false);
    m_prevAnimNo = -1;
    m_animBlendRate = 1.0f;


}

void Explanation::Update()
{
	// アニメーションを進める（ループ再生）
	UpdateAnim(m_currentAnimNo);


	m_headPos = MV1GetFramePosition(m_modelHandle, 7);

	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angle, 0.0f));

}

void Explanation::Draw()
{
	//エネミーモデル描画
	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG

	//当たり判定カプセル
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + upperPart, m_pos.z), VGet(m_pos.x, m_pos.y + bottom, m_pos.z), m_modelRadius, 10, m_color, m_color, false);

#endif

}

void Explanation::End()
{
	MV1DeleteModel(m_modelHandle);
	m_modelHandle = -1;
}


bool Explanation::UpdateAnim(int attachNo)
{
	//アニメーションが設定されていないので終了
	if (attachNo == -1) return false;

	//アニメーションを進行させる
	float now = MV1GetAttachAnimTime(m_modelHandle, attachNo);	//現在の再生カウントを取得
	now += 0.1f;	//アニメーション進める

	//現在再生中のアニメーションの総カウントを取得
	float total = MV1GetAttachAnimTotalTime(m_modelHandle, attachNo);
	bool isLoop = false;
	if (now >= total)
	{
		now -= total;
		isLoop = true;

	}

	//進めた時間の設定
	MV1SetAttachAnimTime(m_modelHandle, attachNo, now);

	return isLoop;
}

