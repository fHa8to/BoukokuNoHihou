#include "Stage.h"
#include "DxLib.h"
#include "Player.h"

#define D2R(deg) ((deg)* DX_PI_F/180.0f)

namespace
{
	//モデルのファイル名
	const char* const kModelFilename = "data/model/stage/stage.mv1";

	//モデルのサイズ変更
	constexpr float kExpansion = 3.0f;
}

Stage::Stage() :
	m_modelHandle(0),
	m_collisionHandle(0)
{
	m_pos = VGet(0.0f, 0.0f, 0.0f);
	m_collisionMapPosition = VGet(0.0f, 0.0f, 0.0f);

}

Stage::~Stage()
{
}

void Stage::Init()
{
	m_modelHandle = MV1LoadModel(kModelFilename);

	m_collisionHandle = MV1LoadModel(kModelFilename);



	//モデルのサイズ変更
	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));
	MV1SetScale(m_collisionHandle, VGet(kExpansion, kExpansion, kExpansion ));

	//マップのポジション設定
	m_pos = VGet(0.0f, 0.0f, 0.0f);
	m_collisionMapPosition = VGet(0.0f, 0.0f, 0.0f);

}

void Stage::Update()
{

}

void Stage::Draw()
{
	MV1SetPosition(m_modelHandle, m_pos);
	MV1SetPosition(m_collisionHandle, m_collisionMapPosition);

	MV1DrawModel(m_modelHandle);  // ステージモデルの描画

#ifdef _DEBUG

	DrawFormatString(0, 90, 0x000000, "Stage(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

#endif

}

void Stage::End()
{
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_collisionHandle);
}

