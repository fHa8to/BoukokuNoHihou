#include "SceneGame.h"
#include "DxLib.h"
#include "Camera.h"
#include "SceneClear.h"
#include "SceneOver.h"
#include "Game.h"
#include "Pad.h"

namespace
{
	//�t�H���g�̃T�C�Y
	constexpr int kFontSize = 20;

	//�t�F�[�h�C���A�t�F�[�h�A�E�g�̐��l
	constexpr int kFadeValue = 255;

	//�t�F�[�h�l�̑���
	constexpr int kFadeUpDown = 8;

}

SceneGame::SceneGame() :
	m_isCommand(false),
	m_isTranslation(false),
	m_isEnemyDeath(false),
	m_isPlayerAttack(false),
	m_isEnemyAttack(false),
	m_isPlayerAttackCoolTime(false),
	m_isEnemyAttackCoolTime(false),
	m_playerFrame(0),
	m_enemyFrame(0),
	m_playerSkillHitFrame(0),
	m_enemySkillHitFrame(0),
	m_playerDeath(0),
	m_enemyDeath(0)
{

	m_pEnemy = std::make_shared<Enemy>();
	m_pPlayer = std::make_shared<Player>();
	m_pCamera = std::make_shared<Camera>();
	m_pStage = std::make_shared<Stage>();
	m_pSkyDome = std::make_shared<SkyDome>();

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	m_isSceneEnd = false;

	m_fadeAlpha = kFadeValue;

	m_pSkyDome->Init();

	m_pStage->Init();

	m_pCamera->Init();

	m_pPlayer->Load();
	m_pPlayer->Init();

	m_pEnemy->Init();

}

std::shared_ptr<SceneBase> SceneGame::Update()
{
	//hp���擾
	float playerHp = m_pPlayer->GetHp();
	float enemyHp = m_pEnemy->GetHp();


	VECTOR toEnemy = VSub(m_pEnemy->GetPos(), m_pPlayer->GetPos());
	float length = VSize(toEnemy);

	VECTOR posVec = VGet(0.0f,0.0f,0.0f);
	VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);




	m_playerPos = m_pPlayer->GetPos();

	m_pSkyDome->Update(m_pPlayer);

	m_pPlayer->SetCameraAngle(m_pCamera->GetAngle());


	m_pCamera->PlayerCameraUpdate(*m_pPlayer);

	m_pEnemy->Update(m_pPlayer);
	m_pPlayer->Update(*m_pStage);
	m_pStage->Update();

	m_isGimmickHit = m_pPlayer->IsCapsuleColliding(m_pEnemy);

	//�G�̍��G�͈͂̓����蔻��
	m_isTranslation = m_pEnemy->Translation(m_pPlayer);

	//�v���C���[�U���͈͂̓����蔻��
	m_isPlayerAttack = m_pPlayer->IsAttackColliding(m_pEnemy);

	m_isEnemyAttack = m_pEnemy->IsAttackColliding(m_pPlayer);
	//���ݒׂ������蔻��
	m_isStepOnAttock = m_pPlayer->IsStepOnAttockColliding(m_pEnemy);




	if (!m_isTranslation)
	{
		m_pEnemy->SetState(Enemy::kIdle);
	}
	if (m_isTranslation)
	{
		m_pEnemy->SetState(Enemy::kRun);
	}

	if (m_isGimmickHit)
	{
		m_pEnemy->SetState(Enemy::kAttack);
	}


	//�U�����������Ă��鎞
	if (m_pPlayer->GetUnderAttack())
	{
		if (!m_isPlayerAttackCoolTime)
		{
			m_playerFrame++;
			if (m_playerFrame > 20)
			{

				if (m_isPlayerAttack)
				{
					//HP�����炷
					enemyHp -= 1;
					m_pEnemy->SetHp(enemyHp);

					m_isPlayerAttackCoolTime = true;

					m_playerFrame = 0;
				}
			}
		}
		else
		{
			m_playerSkillHitFrame++;
			if (m_playerSkillHitFrame >= 50)
			{
				m_isPlayerAttackCoolTime = false;
				m_playerSkillHitFrame = 0;
			}
		}

	}

	if (m_pEnemy->GetUnderAttack())
	{
		if (!m_isEnemyAttackCoolTime)
		{
			m_enemyFrame++;
			if (m_enemyFrame > 20)
			{
				if (m_isEnemyAttack)
				{

					playerHp -= 1;
					m_pPlayer->SetHp(playerHp);

					m_isEnemyAttackCoolTime = true;

					m_enemyFrame = 0;
				}

			}
		}
		else
		{
			m_enemySkillHitFrame++;
			if (m_enemySkillHitFrame >= 50)
			{
				m_isEnemyAttackCoolTime = false;
				m_enemySkillHitFrame = 0;
			}
		}
	}



	if (m_pPlayer->GetHp() == 0)
	{
		m_playerDeath++;
		if (m_playerDeath >= 220)
		{
			return std::make_shared<SceneOver>();
		}
	}

	if (m_pEnemy->GetHp() == 0)
	{
		m_enemyDeath++;
		m_pEnemy->SetState(Enemy::kDeath);

		if (m_enemyDeath >= 220)
		{
			return std::make_shared<SceneClear>();
			m_enemyDeath = 0;
		}

	}



	//�t���[���C���A�A�E�g
	if (m_isSceneEnd)
	{
		m_fadeAlpha += kFadeUpDown;
		if (m_fadeAlpha > kFadeValue)
		{
			m_fadeAlpha = kFadeValue;
		}
	}
	else
	{
		m_fadeAlpha -= kFadeUpDown;
		if (m_fadeAlpha < 0)
		{
			m_fadeAlpha = 0;
		}
	}


	return shared_from_this();
}

void SceneGame::Draw()
{
	m_pSkyDome->Draw();
	m_pStage->Draw();

#ifdef _DEBUG

	DrawGrid();

#endif 


	m_pEnemy->Draw();
	m_pPlayer->Draw();


#ifdef _DEBUG

	DrawString(0, 0, "SceneGame", 0x000000);


	if (m_isGimmickHit)
	{

		DrawString(0, 105, "�J�v�Z���͏Փ˂��Ă��܂��B", 0xff0000);

		m_pPlayer->SetColor(0x00ff00);

	}
	else
	{

		DrawString(0, 105, "�J�v�Z���͏Փ˂��Ă��܂���B", 0x00ff00);

		m_pPlayer->SetColor(0xffffff);

	}

	if (m_isStepOnAttock)
	{

		DrawString(0, 120, "���ݒׂ����͏Փ˂��Ă��܂��B", 0xff0000);

	}
	else
	{

		DrawString(0, 120, "���ݒׂ����͏Փ˂��Ă��܂���B", 0x00ff00);


	}

	if (m_isPlayerAttack)
	{

		DrawString(0, 135, "�U���̋��͏Փ˂��Ă��܂��B", 0xff0000);

	}
	else
	{

		DrawString(0, 135, "�U���̋��͏Փ˂��Ă��܂���B", 0x00ff00);


	}

	if (m_isTranslation)
	{
		DrawString(0, 150, "����", 0x00ff00);

		m_pEnemy->SetColor(0x00ff00);

	}
	else
	{

		DrawString(0, 150, "�x��", 0xff0000);

		m_pEnemy->SetColor(0xffffff);

	}

#endif // _DEBUG


	DrawString(Game::kScreenWidth - 150, Game::kScreenHeight - 150, "�{�^��A�F�W�����v", 0x000000);
	DrawString(Game::kScreenWidth - 150, Game::kScreenHeight - 130, "�{�^��X�F�U��", 0x000000);
	DrawString(Game::kScreenWidth - 150, Game::kScreenHeight - 110, "�{�^��B�F�_�b�V��", 0x000000);

	//�t�F�[�h�̕`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���



}

void SceneGame::End()
{
	m_pStage->End();
	m_pPlayer->Delete();
	m_pEnemy->End();
}

void SceneGame::DrawGrid()
{
	for (int x = -150; x <= 150; x += 10)
	{
		DrawLine3D(VGet(static_cast<float>(x), 0, -150), VGet(static_cast<float>(x), 0, 150), 0xffff00);
	}
	for (int z = -150; z <= 150; z += 10)
	{
		DrawLine3D(VGet(-150, 0, static_cast<float>(z)), VGet(150, 0, static_cast<float>(z)), 0xff0000);
	}

	// X+-,Z+-�̕�����������₷���悤�ɕ\����ǉ�����
	VECTOR dispPos = ConvWorldPosToScreenPos(VGet(500, 0, 0));
	if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
	{
		DrawStringF(dispPos.x, dispPos.y, "X+", 0xffffff);
	}

	dispPos = ConvWorldPosToScreenPos(VGet(-500, 0, 0));
	if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
	{
		DrawStringF(dispPos.x, dispPos.y, "X-", 0xffffff);
	}

	dispPos = ConvWorldPosToScreenPos(VGet(0, 0, 500));
	if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
	{
		DrawStringF(dispPos.x, dispPos.y, "Z+", 0xffffff);
	}

	dispPos = ConvWorldPosToScreenPos(VGet(0, 0, -500));
	if (dispPos.z >= 0.0f && dispPos.z <= 1.0f)
	{
		DrawStringF(dispPos.x, dispPos.y, "Z-", 0xffffff);
	}
}

