#pragma once
#include"actor.h"

///プレイヤークラス
///@note 
///入力への反応以外は基本的にActorの
///仕様に準拠する
class Player : public Actor
{
public:
	Player();
	~Player();
	///更新
	void Update(const Input& input) override final;
	
	///描画
	void Draw() override final;
};

