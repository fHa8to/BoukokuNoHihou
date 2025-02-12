#include "Stage.h"
#include "DxLib.h"
#include "Player.h"

namespace
{
	//���f���̃t�@�C����
	const char* const kModelFilename1 = "data/model/stage/stage.mv1";
	const char* const kModelFilename2 = "data/model/stage/stage1.mv1";

	//���f���̃T�C�Y�ύX
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
	m_modelHandle = MV1LoadModel(kModelFilename1);

	m_collisionHandle = MV1LoadModel(kModelFilename2);


	//���f���̃T�C�Y�ύX
	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));
	MV1SetScale(m_collisionHandle, VGet(kExpansion, kExpansion, kExpansion ));

	//�}�b�v�̃|�W�V�����ݒ�
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

	MV1DrawModel(m_modelHandle);  // �X�e�[�W���f���̕`��

#ifdef _DEBUG

	DrawFormatString(0, 90, 0x000000, "Stage(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

#endif

}

void Stage::End()
{
	MV1DeleteModel(m_modelHandle);
	MV1DeleteModel(m_collisionHandle);
}

