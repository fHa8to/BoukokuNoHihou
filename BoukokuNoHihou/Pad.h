#pragma once

namespace Pad
{
	//パッドの入力状態取得
	void Update();

	//押し下げ判定
	bool IsPress(int key);
	//トリガー判定
	bool IsTrigger(int key);
	//離した判定
	bool IsRelase(int key);

}
