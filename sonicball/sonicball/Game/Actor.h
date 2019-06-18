#pragma once

//プロトタイプ宣言
class Input;
class Camera;
///画面上で動く奴基底クラス
class Actor
{
protected:
	Camera& _camera;
public:
	Actor(Camera& cam);
	virtual ~Actor();
	///状態や座標やアニメーションフレーム更新
	///@param input 入力
	virtual void Update(const Input& input) = 0;

	///描画
	///@remark 基本的に描画のみを実装するように
	virtual void Draw() = 0;
};

