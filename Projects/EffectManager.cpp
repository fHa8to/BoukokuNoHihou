#include "EffectManager.h"
#include "Player.h"
#include "Enemy.h"
#include "BossEnemy.h"

#include "EffekseerForDXLib.h"


EffectManager::EffectManager():
	m_pos(),
	m_bossEnemyDamageEffectHandle(-1),
	m_bossEnemySkillEffectHandle(-1),
	m_enemyDamageEffectHandle(-1),
	m_playerDamageEffectHandle(-1),
	m_playerSkillEffectHandle(-1),
	m_arrowEffecthandle(-1),
	m_bossEnemyDamageEffectTime(0),
	m_bossEnemySkillEffectTime(0),
	m_enemyDamageEffectTime(0),
	m_playerDamageEffectTime(0),
	m_playerSkillEffectTime(0),
	m_arrowEffectTime(0),
	m_arrowTime(0),
	m_isBossEnemyDamageEffect(false),
	m_isBossEnemySkillEffect(false),
	m_isEnemyDamageEffect(false),
	m_isPlayerDamageEffect(false),
	m_isPlayerSkillEffect(false),
	m_isArrowEffect(false)
{
	// プレイヤーダメージエフェクトをロード
	m_playerDamageEffectHandle = LoadEffekseerEffect("data/effect/damage.efkefc");

	m_playerSkillEffectHandle = LoadEffekseerEffect("data/effect/hani.efkefc");

	// プレイヤーダメージエフェクトをロード
	m_enemyDamageEffectHandle = LoadEffekseerEffect("data/effect/damage.efkefc");

	// プレイヤーダメージエフェクトをロード
	m_bossEnemyDamageEffectHandle = LoadEffekseerEffect("data/effect/damage.efkefc");

	m_bossEnemySkillEffectHandle = LoadEffekseerEffect("data/effect/hani.efkefc");

	//矢印のエフェクトをロード
	m_arrowEffecthandle = LoadEffekseerEffect("data/effect/arrow.efkefc");
}

EffectManager::~EffectManager()
{
}

void EffectManager::Init()
{
	m_pos = VGet(25.0f, 1.1f, -180.0f);
}

void EffectManager::Update()
{
	UpdateEffekseer3D();
	Effekseer_Sync3DSetting();


	if (m_isArrowEffect)
	{
		m_isArrowEffect = false;
	}

	if (m_isPlayerDamageEffect)
	{
		m_isPlayerDamageEffect = false;
	}

	if (m_isPlayerSkillEffect)
	{
		m_isPlayerSkillEffect = false;
	}

	if (m_isEnemyDamageEffect)
	{
		m_isEnemyDamageEffect = false;
	}

	if (m_isBossEnemyDamageEffect)
	{
		m_isBossEnemyDamageEffect = false;
	}

}

void EffectManager::Draw()
{
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();


}

void EffectManager::End()
{
	DeleteEffekseerEffect(m_playerDamageEffectHandle);
	DeleteEffekseerEffect(m_bossEnemyDamageEffectHandle);
	DeleteEffekseerEffect(m_bossEnemySkillEffectHandle);
	DeleteEffekseerEffect(m_enemyDamageEffectHandle);
	DeleteEffekseerEffect(m_playerSkillEffectHandle);
	DeleteEffekseerEffect(m_arrowEffecthandle);
}

void EffectManager::ResetFlags()
{
	m_isPlayerSkillEffect = false;
	m_isPlayerDamageEffect = false;
	m_isEnemyDamageEffect = false;
	m_isBossEnemyDamageEffect = false;
	m_isArrowEffect = false;
}

void EffectManager::DrawPlayerAttackEffect()
{

}

void EffectManager::DrawPlayerSkillEffect(std::shared_ptr<Player> m_pPlayer)
{
	if (!m_isPlayerSkillEffect)
	{
		VECTOR playerPos = m_pPlayer->GetPos();

		m_playerSkillEffectTime = PlayEffekseer3DEffect(m_playerSkillEffectHandle);

		// 再生位置設定
		SetPosPlayingEffekseer3DEffect(m_playerSkillEffectTime, playerPos.x, playerPos.y, playerPos.z);

		m_isPlayerSkillEffect = true;
	}

}

void EffectManager::DrawPlayerDamageEffect(std::shared_ptr<Player> m_pPlayer)
{
	if (!m_isPlayerDamageEffect)
	{
		VECTOR playerPos = m_pPlayer->GetDamgePos();

		m_playerDamageEffectTime = PlayEffekseer3DEffect(m_playerDamageEffectHandle);

		// 再生位置設定
		SetPosPlayingEffekseer3DEffect(m_playerDamageEffectTime, playerPos.x, playerPos.y, playerPos.z);

		m_isPlayerDamageEffect = true;
	}
}

void EffectManager::DrawEnemyDamageEffect(std::shared_ptr<Enemy> m_pEnemy)
{
	if (!m_isEnemyDamageEffect)
	{
		VECTOR enemyPos = m_pEnemy->GetDamgePos();

		m_enemyDamageEffectTime = PlayEffekseer3DEffect(m_enemyDamageEffectHandle);

		// 再生位置設定
		SetPosPlayingEffekseer3DEffect(m_enemyDamageEffectTime, enemyPos.x, enemyPos.y, enemyPos.z);

		m_isEnemyDamageEffect = true;
	}

}

void EffectManager::DrawBossEnemyAttackEffect(std::shared_ptr<BossEnemy> m_pBossEnemy)
{
	if (!m_isBossEnemySkillEffect)
	{
		VECTOR bossEnemyPos = m_pBossEnemy->GetPos();

		m_bossEnemySkillEffectTime = PlayEffekseer3DEffect(m_bossEnemySkillEffectHandle);

		// 再生位置設定
		SetPosPlayingEffekseer3DEffect(m_bossEnemySkillEffectTime, bossEnemyPos.x, bossEnemyPos.y, bossEnemyPos.z);

		m_isBossEnemySkillEffect = true;
	}

}

void EffectManager::DrawBossEnemyDamageEffect(std::shared_ptr<BossEnemy> m_pBossEnemy)
{
	if (!m_isBossEnemyDamageEffect)
	{
		VECTOR bossEnemyPos = m_pBossEnemy->GetDamgePos();

		m_bossEnemyDamageEffectTime = PlayEffekseer3DEffect(m_bossEnemyDamageEffectHandle);

		// 再生位置設定
		SetPosPlayingEffekseer3DEffect(m_bossEnemyDamageEffectTime, bossEnemyPos.x, bossEnemyPos.y, bossEnemyPos.z);

		m_isBossEnemyDamageEffect = true;
	}

}

void EffectManager::DrawArrowEffect()
{

	if (m_arrowTime > 0)
	{
		m_arrowTime--;

	}

	if (!m_isArrowEffect && m_arrowTime == 0)
	{
		m_arrowTime = 190;

		// 再生して戻り値（インスタンスID）を保持する
		m_arrowEffectTime = PlayEffekseer3DEffect(m_arrowEffecthandle);
		
		SetPosPlayingEffekseer3DEffect(m_arrowEffectTime, m_pos.x, m_pos.y, m_pos.z);
	
		m_isArrowEffect = true;
	}

}
