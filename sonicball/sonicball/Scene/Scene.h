#pragma once

class Input;
class SceneController;
///シーン管理のための基底クラス
///(純粋仮想クラス)
class Scene
{
protected:
	SceneController& _controller;
public:
	Scene(SceneController& controller);
	virtual ~Scene();

	///シーンの更新を行う
	virtual void Update(const Input& input) = 0;

	///描画しもぁ～す
	virtual void Draw()= 0;
};

