#pragma once
#include <memory>

class SceneBase : public std::enable_shared_from_this<SceneBase>
{

public:
	SceneBase() {}
	virtual ~SceneBase() {}

	virtual void Init() = 0;

	virtual std::shared_ptr<SceneBase> Update() = 0;	//シーンを変更する場合は遷移先のシーンのポインタ

	virtual void Draw() = 0;	//毎フレーム行う描画処理
	virtual void End() = 0;		//シーンを抜けるときの処理

};


