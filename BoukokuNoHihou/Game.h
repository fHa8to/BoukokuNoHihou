#pragma once
#include "DxLib.h"

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