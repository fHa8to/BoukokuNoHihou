#pragma once
#include "DxLib.h"

#define PLAYER_MAX_HITCOLL 21836 //処理するコリジョンポリゴンの最大数

// 最大ＨＰ
#define PLAYER_HP_MAX		25
// ＨＰ１でどれだけバーを伸ばすか
#define PLAYER_DRAW_SIZE	20

// 最大ＨＰ
#define BOSS_ENEMY_HP_MAX		16
// ＨＰ１でどれだけバーを伸ばすか
#define BOSS_ENEMY_DRAW_SIZE	51

namespace Game
{

	// 画面幅
	constexpr int kScreenWidth = 1280;      //画面の横幅
	constexpr int kScreenHeight = 720;     //画面の縦幅

	// 最大当たり判定ポリゴン数
	constexpr int kMaxColHitPolyNum = 2000;
	// 壁押し出し処理の最大試行回数
	constexpr int kMaxColHitTryNum = 16;
	// 壁押し出し時にスライドさせる距離
	constexpr float kColHitSlideLength = 1.0f;

}