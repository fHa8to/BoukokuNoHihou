#include "SkyDome.h"
#include "Player.h"
namespace
{
	//��]���x
	constexpr float kRotSpeed = 0.0009f;
	//�t�@�C����
	const char* const kSkyDomeFileName = "data/skyDome/Domea.mv1";

	//���f���̃T�C�Y�ύX
	constexpr float kExpansion = 1.0f;
}

SkyDome::SkyDome():
	m_modelHandle(-1),
	m_pos(VGet(0.0f, 0.0f, 0.0f)),
	m_rot(0.0f)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Init()
{
	m_modelHandle = MV1LoadModel(kSkyDomeFileName);

	MV1SetScale(m_modelHandle, VGet(kExpansion, kExpansion, kExpansion));
}

void SkyDome::Update(std::shared_ptr<Player> m_pPlayer)
{
	//��]
	m_rot += kRotSpeed;

	m_pos = VGet(m_pPlayer->GetPos().x, 0.0f, m_pPlayer->GetPos().z);

	//1��]������l�����Z�b�g
	if (m_rot >= DX_PI_F) m_rot = 0.0f;


	//�X�J�C�h�[�����X�V
	MV1SetRotationXYZ(m_modelHandle, VGet(0, m_rot, 0));

}

void SkyDome::Draw()
{
	MV1SetPosition(m_modelHandle, m_pos);
	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG

	//DrawFormatString(0, 70, 0x000000, "SkyDome(x:%f,y:%f,z:%f)", m_pos.x, m_pos.y, m_pos.z);

#endif

}
