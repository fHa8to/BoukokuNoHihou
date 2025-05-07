#include "SceneGame.h"
#include "DxLib.h"
#include "Camera.h"
#include "SceneClear.h"
#include "SceneOver.h"
#include "Game.h"
#include "Pad.h"

namespace
{
    //フォントのサイズ
    constexpr int kFontSize = 20;

    //フェードイン、フェードアウトの数値
    constexpr int kFadeValue = 255;

    //フェード値の増減
    constexpr int kFadeUpDown = 8;


}

SceneGame::SceneGame() :
    m_modelHandle(-1),
    m_pos(VGet(0.0f, 0.0f, 0.0f)),
    m_isCommand(false),
    m_isEnemyTranslation(false),
    m_isBossEnemyTranslation(false),
    m_isEnemyDeath(false),
    m_isPlayerAttack(false),
	m_isSkillPlayerAttack(false),
	m_isEnemyHit(false),
	m_isBossEnemyHit(false),
	m_isSkillEnemyHit(false),
	m_isSkillBossEnemyHit(false),
	m_isStepOnAttock(false),
	m_isEnemyStop(false),
	m_isBossEnemyStop(false),
	m_isBossAttack(false),
	m_isSkillBossEnemyAttack(false),
    m_isEnemyAttack(false),
    m_isExplanaionAttack(false),
    m_isExplanaion(false),
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
    m_playerAttackCoolTimeCounter(0), // クールタイムの初期化
    m_enemyAttackCoolTimeCounter(0),  // クールタイムの初期化
    m_bossEnemyAttackCoolTimeCounter(0) // クールタイムの初期化

{
    m_pEnemy = std::make_shared<Enemy>();
    m_pBossEnemy = std::make_shared<BossEnemy>();
    m_pPlayer = std::make_shared<Player>();
    m_pCamera = std::make_shared<Camera>();
    m_pStage = std::make_shared<Stage>();
    m_pSkyDome = std::make_shared<SkyDome>();
    m_pUi = std::make_shared<Ui>();
    m_pSkill = std::make_shared<Skill>();
    m_pEffectManager = std::make_shared<EffectManager>();
    m_pExplanation = std::make_shared<Explanation>();

    m_textHandle = LoadGraph("data/image/text.png");

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

    m_pExplanation->Init();

    m_pPlayer->Load();
    m_pPlayer->Init();

    m_pEnemy->Init();

    m_pBossEnemy->Init();

    m_pUi->Init();

    m_pEffectManager->Init();

}


std::shared_ptr<SceneBase> SceneGame::Update()
{
    //hpを取得
    float playerHp = m_pUi->GetPlayerHp();
    float enemyHp = m_pEnemy->GetHp();
    float bossEnemyHp = m_pUi->GetBossHp();

    VECTOR toEnemy = VSub(m_pEnemy->GetPos(), m_pPlayer->GetPos());
    float length = VSize(toEnemy);

    VECTOR posVec = VGet(0.0f, 0.0f, 0.0f);
    VECTOR moveVec = VGet(0.0f, 0.0f, 0.0f);

    m_pSkyDome->Update(m_pPlayer);

    m_pExplanation->Update();

    m_pPlayer->SetCameraAngle(m_pCamera->GetAngle());

    m_pCamera->PlayerCameraUpdate(*m_pPlayer);

    m_pEnemy->Update(m_pPlayer, m_pUi, *m_pStage);

    m_pBossEnemy->Update(m_pPlayer, m_pUi,*m_pStage);
    m_pPlayer->Update(m_pEnemy, m_pBossEnemy, m_pUi, m_pExplanation, *m_pStage);
    m_pStage->Update();

    m_pUi->Update();

    m_pEffectManager->Update();

    //プレイヤーと敵の当たり判定
    m_isEnemyHit = m_pPlayer->IsEnemyCapsuleColliding(m_pEnemy);
    m_isBossEnemyHit = m_pPlayer->IsBossEnemyCapsuleColliding(m_pBossEnemy);

    //敵の索敵範囲の当たり判定
    m_isEnemyTranslation = m_pEnemy->Translation(m_pPlayer);

    //プレイヤー攻撃範囲の当たり判定
    m_isPlayerAttack = m_pPlayer->IsAttackColliding(m_pEnemy);
    m_isBossEnemyAttack = m_pPlayer->IsBossAttackColliding(m_pBossEnemy);
	//プレイヤーのスキル攻撃範囲の当たり判定
    m_isSkillPlayerAttack = m_pPlayer->IsSkillAttackColliding(m_pEnemy);
    m_isSkillBossEnemyAttack = m_pPlayer->IsSkillBossAttackColliding(m_pBossEnemy);

    //敵の攻撃範囲の当たり判定
    m_isEnemyAttack = m_pEnemy->IsAttackColliding(m_pPlayer);

    m_isBossAttack = m_pBossEnemy->IsAttackColliding(m_pPlayer);

    m_isBossEnemyTranslation = m_pBossEnemy->Translation(m_pPlayer);

    m_isExplanaionAttack = m_pPlayer->IsExplanationCapsuleColliding(m_pExplanation);

    //enemy止まる範囲
    m_isEnemyStop = m_pEnemy->IsStopColliding(m_pPlayer);

    m_isBossEnemyStop = m_pBossEnemy->IsStopColliding(m_pPlayer);


    int m_bossAttackWaitCounter = 0;  // 攻撃待機カウンター
    int m_bossNextAttackTime = 0;     // 次の攻撃までの時間（ランダムで決定）
    bool m_isBossWaitingToAttack = false;

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
            m_pEnemy->SetState(Enemy::kIdle); // クールタイム中かつ止まる範囲に当たっていて移動していない場合はIdleにする
        }
    }
    if (m_bossEnemyAttackCoolTimeCounter > 0)
    {
        m_bossEnemyAttackCoolTimeCounter--;
        if (m_isBossEnemyStop && !m_pPlayer->GetMove())
        {
            m_pBossEnemy->SetState(BossEnemy::kIdle); // クールタイム中かつ止まる範囲に当たっていて移動していない場合はIdleにする
        }
    }

    if (m_pPlayer->GetUnderExplanationAttack())
    {
        if (m_isExplanaionAttack && !m_pPlayer->IsAttackHit())
        {
            m_pPlayer->SetAttackHit(true);
            m_isExplanaion = true;
        }
    }




    if (m_pPlayer->GetSkill())
    {

        // スキルを初回発動時のみ再生するように
        if (!m_pPlayer->IsSkillEffectPlayed())
        {
            m_pEffectManager->DrawPlayerSkillEffect(m_pPlayer);
            m_pPlayer->SetSkillEffectPlayed(true); // 一度だけ再生されるようにする
        }


        m_pPlayer->SetAttackHit(false);

        //プレイヤーの攻撃がボスに当たっている時
        if (m_pPlayer->GetSkillUnderBossAttack())
        {

            if (m_isSkillBossEnemyAttack && !m_pPlayer->IsSkillAttackHit())
            {
                //HPを減らす
                bossEnemyHp -= 4;
                m_pUi->SetBossHp(bossEnemyHp);
                m_pPlayer->SetSkillAttackHit(true); // フラグを設定
                m_pEffectManager->DrawBossEnemyDamageEffect(m_pBossEnemy);

                if (m_pUi->GetBossHp() >= 1)
                {
                    m_pBossEnemy->SetState(BossEnemy::kDamage);
                }
            }
        }

        //プレイヤーの攻撃が敵に当たっている時
        if (m_pPlayer->GetSkillUnderAttack())
        {

            if (m_isSkillPlayerAttack && !m_pPlayer->IsSkillAttackHit())
                {
                    //HPを減らす
                    enemyHp -= 4;
                    m_pEnemy->SetHp(enemyHp);
                    m_pPlayer->SetSkillAttackHit(true); // フラグを設定
                    m_pEffectManager->DrawEnemyDamageEffect(m_pEnemy);

                    if (m_pEnemy->GetHp() >= 1)
                    {
                        m_pEnemy->SetState(Enemy::kDamage);
                    }
            }
        }
        
    }
    else
    {
        m_pPlayer->SetSkillEffectPlayed(false);  // 次回発動のためにフラグを戻す
        m_pPlayer->SetSkillAttackHit(false);     // 攻撃ヒットフラグも戻す

        //プレイヤーの攻撃がボスに当たっている時
        if (m_pPlayer->GetUnderBossAttack())
        {
                    if (m_isBossEnemyAttack && !m_pPlayer->IsAttackHit())
                    {
                        //HPを減らす
                        bossEnemyHp -= 1;
                        m_pUi->SetBossHp(bossEnemyHp);
                        m_pPlayer->SetAttackHit(true);
                        m_pEffectManager->DrawBossEnemyDamageEffect(m_pBossEnemy);

                        if (m_pUi->GetBossHp() >= 1)
                        {
                            m_pBossEnemy->SetState(BossEnemy::kDamage);
                        }
                    }
        }

        //プレイヤーの攻撃が敵に当たっている時
        if (m_pPlayer->GetUnderAttack())
        {
                    if (m_isPlayerAttack && !m_pPlayer->IsAttackHit())
                    {
                        //HPを減らす
                        enemyHp -= 1;
                        m_pEnemy->SetHp(enemyHp);
                        m_pPlayer->SetAttackHit(true);
                        m_pEffectManager->DrawEnemyDamageEffect(m_pEnemy);

                        if (m_pEnemy->GetHp() >= 1)
                        {
                            m_pEnemy->SetState(Enemy::kDamage);
                        }
                    }
        }



        //敵の攻撃が当たっている時
        if (m_pEnemy->GetUnderAttack())
        {
                    if (m_isEnemyAttack && !m_pEnemy->IsAttackHit())
                    {
                        playerHp -= 1;
                        m_pUi->SetPlayerHp(playerHp);
                        m_pEffectManager->DrawPlayerDamageEffect(m_pPlayer);

                        m_pEnemy->SetAttackHit(true);
                        m_pPlayer->SetDamage(true);
                    }

        }

        //Boss敵の攻撃が当たっている時
        if (m_pBossEnemy->GetUnderAttack())
        {
                    if (m_isBossAttack && !m_pBossEnemy->IsAttackHit())
                    {
                        playerHp -= 2;
                        m_pUi->SetPlayerHp(playerHp);
                        m_pEffectManager->DrawPlayerDamageEffect(m_pPlayer);

                        m_pBossEnemy->SetAttackHit(true);
                        m_pPlayer->SetDamage(true);

                    }
        }

    }

    m_pEffectManager->ResetFlags();

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

    //エネミーモデルの座標
    MV1SetPosition(m_modelHandle, m_pos);

    //フレームイン、アウト
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


    //エフェクトの描画
    m_pEffectManager->Draw();

    m_pBossEnemy->Draw(m_pPlayer);
    m_pEnemy->Draw(m_pPlayer);
    m_pPlayer->Draw();

    m_pExplanation->Draw();

    m_pUi->PlayerDraw(*m_pPlayer);

    if (m_isBossEnemyTranslation)
    {
        m_pUi->BossDraw();
    }


#ifdef _DEBUG

    DrawString(0, 0, "SceneGame", 0x000000);


  

    if (m_isEnemyHit)
    {

        DrawString(0, Game::kScreenHeight / 2 + 105, "カプセルは衝突しています。", 0xff0000);

        m_pPlayer->SetColor(0x00ff00);

    }
    else
    {

        DrawString(0, Game::kScreenHeight / 2 + 105, "カプセルは衝突していません。", 0x00ff00);

        m_pPlayer->SetColor(0xffffff);

    }


    if (m_isStepOnAttock)
    {

        DrawString(0, Game::kScreenHeight / 2 + 120, "踏み潰す球は衝突しています。", 0xff0000);

    }
    else
    {

        DrawString(0, Game::kScreenHeight / 2 + 120, "踏み潰す球は衝突していません。", 0x00ff00);


    }

    if (m_isPlayerAttack)
    {

        DrawString(0, Game::kScreenHeight / 2 + 135, "攻撃の球は衝突しています。", 0xff0000);

    }
    else
    {

        DrawString(0, Game::kScreenHeight / 2 + 135, "攻撃の球は衝突していません。", 0x00ff00);


    }

    if (m_isEnemyTranslation)
    {
        DrawString(0, Game::kScreenHeight / 2 + 150, "Enemy発見", 0x00ff00);

        m_pEnemy->SetColor(0x00ff00);
    }
    else
    {
        DrawString(0, Game::kScreenHeight / 2 + 150, "Enemy警戒", 0xff0000);

        m_pEnemy->SetColor(0xffffff);
    }

    if (m_isBossEnemyTranslation)
    {
        DrawString(0, Game::kScreenHeight / 2 + 170, "Boss発見", 0x00ff00);

        m_pBossEnemy->SetColor(0x00ff00);
    }
    else
    {
        DrawString(0, Game::kScreenHeight / 2 + 170, "Boss警戒", 0xff0000);

        m_pBossEnemy->SetColor(0xffffff);
    }

    if (m_isSkillPlayerAttack)
    {

        DrawString(0, Game::kScreenHeight / 2 + 190, "Enemyとカプセルの衝突しています。", 0xff0000);

        m_pSkill->SetColor(0x00ff00);

    }
    else
    {

        DrawString(0, Game::kScreenHeight / 2 + 190, "Enemyとカプセルの衝突していません。", 0x00ff00);

        m_pSkill->SetColor(0xffffff);

    }

    if (m_isSkillBossEnemyAttack)
    {

        DrawString(0, Game::kScreenHeight / 2 + 210, "Bossとカプセルの衝突しています。", 0xff0000);

        m_pSkill->SetColor(0x00ff00);

    }
    else
    {

        DrawString(0, Game::kScreenHeight / 2 + 210, "Bossとカプセルの衝突していません。", 0x00ff00);

        m_pSkill->SetColor(0xffffff);

    }

#endif // _DEBUG


    //フェードの描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha); //半透明で表示
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, GetColor(0, 0, 0), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); //不透明に戻しておく



}

void SceneGame::End()
{

    m_pStage->End();
    m_pExplanation->End();
    m_pPlayer->Delete();
    m_pBossEnemy->End();
    m_pEnemy->End();
    m_pEffectManager->End();
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

    // X+-,Z+-の方向が分かりやすいように表示を追加する
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

