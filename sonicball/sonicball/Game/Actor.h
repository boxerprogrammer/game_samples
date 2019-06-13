#pragma once

//プロトタイプ宣言
class Input;

///画面上で動く奴基底クラス
class Actor
{
public:
	Actor();
	virtual ~Actor();
	///状態や座標やアニメーションフレーム更新
	///@param input 入力
	virtual void Update(const Input& input) = 0;

	///描画
	///@remark 基本的に描画のみを実装するように
	virtual void Draw() = 0;
};

