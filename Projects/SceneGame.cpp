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
    m_isEnemyTranslation(false),
    m_isBossEnemyTranslation(false),
    m_isEnemyDeath(false),
    m_isPlayerAttack(false),
    m_isEnemyAttack(false),
    m_isPlayerAttackCoolTime(false),
    m_isPlayerBossAttackCoolTime(false),
    m_isBossEnemyAttackCoolTime(false),
    m_isEnemyAttackCoolTime(false),
    m_playerFrame(0),
    m_enemyFrame(0),
    m_bossEnemyFrame(0),
    m_playerSkillHitFrame(0),
    m_enemySkillHitFrame(0),
    m_bossEnemySkillHitFrame(0),
    m_playerDeath(0),
    m_enemyDeath(0),
    m_bossEnemyDeath(0),
    m_playerAttackCoolTimeCounter(0), // �N�[���^�C���̏�����
    m_enemyAttackCoolTimeCounter(0),  // �N�[���^�C���̏�����
    m_bossEnemyAttackCoolTimeCounter(0) // �N�[���^�C���̏�����

{
    m_pEnemy = std::make_shared<Enemy>();
    m_pBossEnemy = std::make_shared<BossEnemy>();
    m_pPlayer = std::make_shared<Player>();
    m_pCamera = std::make_shared<Camera>();
    m_pStage = std::make_shared<Stage>();
    m_pSkyDome = std::make_shared<SkyDome>();
    m_pUi = std::make_shared<Ui>();

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

    m_pBossEnemy->Init();

    m_pUi->Init();

}


std::shared_ptr<SceneBase> SceneGame::Update()
{
    //hp���擾
    float playerHp = m_pUi->GetPlayerHp();
    float enemyHp = m_pEnemy->GetHp();
    float bossEnemyHp = m_pUi->GetBossHp();

    VECTOR toEnemy = VSub(m_pEnemy->GetPos(), m_pPlayer->GetPos());
    float length = VSize(toEnemy);

    VECTOR posVec = VGet(0.0f, 0.0f, 0.0f);
    VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);

    m_pSkyDome->Update(m_pPlayer);

    m_pPlayer->SetCameraAngle(m_pCamera->GetAngle());

    m_pCamera->PlayerCameraUpdate(*m_pPlayer);

    m_pEnemy->Update(m_pPlayer, m_pUi, *m_pStage);

    m_pBossEnemy->Update(m_pPlayer, m_pUi,*m_pStage);
    m_pPlayer->Update(m_pEnemy, m_pBossEnemy, m_pUi, *m_pStage);
    m_pStage->Update();

    m_pUi->Update();

    //�v���C���[�ƓG�̓����蔻��
    m_isGimmickHit = m_pPlayer->IsEnemyCapsuleColliding(m_pEnemy);
    m_isGimmickHit1 = m_pPlayer->IsBossEnemyCapsuleColliding(m_pBossEnemy);

    //�G�̍��G�͈͂̓����蔻��
    m_isEnemyTranslation = m_pEnemy->Translation(m_pPlayer);

    //�v���C���[�U���͈͂̓����蔻��
    m_isPlayerAttack = m_pPlayer->IsAttackColliding(m_pEnemy);

    //�v���C���[�U���͈͂̓����蔻��
    m_isBossEnemyAttack = m_pPlayer->IsBossAttackColliding(m_pBossEnemy);

    //�G�̍U���͈͂̓����蔻��
    m_isEnemyAttack = m_pEnemy->IsAttackColliding(m_pPlayer);

    m_isBossAttack = m_pBossEnemy->IsAttackColliding(m_pPlayer);

    //���ݒׂ������蔻��
    m_isStepOnAttock = m_pPlayer->IsStepOnAttockColliding(m_pEnemy);

    m_isBossEnemyTranslation = m_pBossEnemy->Translation(m_pPlayer);

    //enemy�~�܂�͈�
    m_isEnemyStop = m_pEnemy->IsStopColliding(m_pPlayer);

    m_isBossEnemyStop = m_pBossEnemy->IsStopColliding(m_pPlayer);


    if (!m_isEnemyStop)
    {
        if (m_isEnemyTranslation)
        {
            m_pEnemy->SetState(Enemy::kRun);
        }
        else
        {
            m_pEnemy->SetState(Enemy::kIdle);
        }
    }
    else
    {
        m_pEnemy->SetState(Enemy::kAttack);
    }


    if (!m_isBossEnemyStop)
    {
        if (m_isBossEnemyTranslation)
        {
            m_pBossEnemy->SetState(BossEnemy::kRun);
        }
        else
        {
            m_pBossEnemy->SetState(BossEnemy::kIdle);
        }
    }
    else
    {
        m_pBossEnemy->SetState(BossEnemy::kAttack);
    }

    if (m_enemyAttackCoolTimeCounter > 0)
    {
        m_enemyAttackCoolTimeCounter--;
        if (m_isEnemyStop && !m_pPlayer->GetMove())
        {
            m_pEnemy->SetState(Enemy::kIdle); // �N�[���^�C�������~�܂�͈͂ɓ������Ă��Ĉړ����Ă��Ȃ��ꍇ��Idle�ɂ���
        }
    }
    if (m_bossEnemyAttackCoolTimeCounter > 0)
    {
        m_bossEnemyAttackCoolTimeCounter--;
        if (m_isBossEnemyStop && !m_pPlayer->GetMove())
        {
            m_pBossEnemy->SetState(BossEnemy::kIdle); // �N�[���^�C�������~�܂�͈͂ɓ������Ă��Ĉړ����Ă��Ȃ��ꍇ��Idle�ɂ���
        }
    }

    //�v���C���[�̍U�����{�X�ɓ������Ă��鎞
    if (m_pPlayer->GetUnderBossAttack())
    {
        if (!m_isPlayerBossAttackCoolTime)
        {
            m_playerFrame++;
            if (m_playerFrame > 15)
            {
                if (m_isBossEnemyAttack)
                {
                    //HP�����炷
                    bossEnemyHp -= 1;
                    m_pUi->SetBossHp(bossEnemyHp);
                    m_isPlayerBossAttackCoolTime = true;
                    if (m_pUi->GetBossHp() >= 1)
                    {
                        m_pBossEnemy->SetState(BossEnemy::kDamage);
                    }
                    m_playerFrame = 0;
                }
            }
        }
        else
        {
            m_bossEnemySkillHitFrame++;
            if (m_bossEnemySkillHitFrame >= 50)
            {
                m_isPlayerBossAttackCoolTime = false;
                m_bossEnemySkillHitFrame = 0;
            }
        }
    }

    //�v���C���[�̍U�����G�ɓ������Ă��鎞
    if (m_pPlayer->GetUnderAttack())
    {
        if (!m_isPlayerAttackCoolTime)
        {
            m_playerFrame++;
            if (m_playerFrame > 15)
            {
                if (m_isPlayerAttack)
                {
                    //HP�����炷
                    enemyHp -= 1;
                    m_pEnemy->SetHp(enemyHp);
                    m_isPlayerAttackCoolTime = true;
                    if (m_pEnemy->GetHp() >= 1)
                    {
                        m_pEnemy->SetState(Enemy::kDamage);
                    }
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

    //�G�̍U�����������Ă��鎞
    if (m_pEnemy->GetUnderAttack())
    {
        if (m_enemyAttackCoolTimeCounter == 0)
        {
            m_enemyFrame++;
            if (m_enemyFrame > 15)
            {
                if (m_isEnemyAttack)
                {
                    playerHp -= 1;
                    m_pUi->SetPlayerHp(playerHp);
                    m_enemyAttackCoolTimeCounter = 50; // �N�[���^�C����ݒ�
                    m_pPlayer->SetDamage(true);
                    m_enemyFrame = 0;
                }
            }
        }

    }

    //Boss�G�̍U�����������Ă��鎞
    if (m_pBossEnemy->GetUnderAttack())
    {
        if (m_bossEnemyAttackCoolTimeCounter == 0)
        {
            m_bossEnemyFrame++;
            if (m_bossEnemyFrame > 15)
            {
                if (m_isBossAttack)
                {
                    playerHp -= 2;
                    m_pUi->SetPlayerHp(playerHp);
                    m_bossEnemyAttackCoolTimeCounter = 50; // �N�[���^�C����ݒ�
                    m_pPlayer->SetDamage(true);
                    m_bossEnemyFrame = 0;

                }
            }
        }
    }


    if (m_pUi->GetPlayerHp() == 0)
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
            m_enemyDeath = 0;
        }
    }

    if (m_pUi->GetBossHp() == 0)
    {
        m_bossEnemyDeath++;
        m_pBossEnemy->SetState(BossEnemy::kDeath);

        if (m_bossEnemyDeath >= 220)
        {
            return std::make_shared<SceneClear>();
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


    m_pBossEnemy->Draw(m_pPlayer);
    m_pEnemy->Draw(m_pPlayer);
    m_pPlayer->Draw();

    m_pUi->PlayerDraw();

    if (m_isBossEnemyTranslation)
    {
        m_pUi->BossDraw();
    }


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

    if (m_isEnemyTranslation)
    {
        DrawString(0, 150, "Enemy����", 0x00ff00);

        m_pEnemy->SetColor(0x00ff00);
    }
    else
    {
        DrawString(0, 150, "Enemy�x��", 0xff0000);

        m_pEnemy->SetColor(0xffffff);
    }

    if (m_isBossEnemyTranslation)
    {
        DrawString(0, 170, "Boss����", 0x00ff00);

        m_pBossEnemy->SetColor(0x00ff00);
    }
    else
    {
        DrawString(0, 170, "Boss�x��", 0xff0000);

        m_pBossEnemy->SetColor(0xffffff);
    }

#endif // _DEBUG


    //�t�F�[�h�̕`��
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //�������ŕ\��
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //�s�����ɖ߂��Ă���



}

void SceneGame::End()
{
    m_pStage->End();
    m_pPlayer->Delete();
    m_pBossEnemy->End();
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
