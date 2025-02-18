#pragma once
#include "DxLib.h"

#define PLAYER_MAX_HITCOLL 21836 //��������R���W�����|���S���̍ő吔

// �ő�g�o
#define PLAYER_HP_MAX		25
// �g�o�P�łǂꂾ���o�[��L�΂���
#define PLAYER_DRAW_SIZE	20

// �ő�g�o
#define BOSS_ENEMY_HP_MAX		16
// �g�o�P�łǂꂾ���o�[��L�΂���
#define BOSS_ENEMY_DRAW_SIZE	51

namespace Game
{

	// ��ʕ�
	constexpr int kScreenWidth = 1280;      //��ʂ̉���
	constexpr int kScreenHeight = 720;     //��ʂ̏c��

	// �ő哖���蔻��|���S����
	constexpr int kMaxColHitPolyNum = 2000;
	// �ǉ����o�������̍ő厎�s��
	constexpr int kMaxColHitTryNum = 16;
	// �ǉ����o�����ɃX���C�h�����鋗��
	constexpr float kColHitSlideLength = 1.0f;

}