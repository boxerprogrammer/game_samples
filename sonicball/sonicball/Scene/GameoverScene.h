#pragma once
#include "Scene.h"
class Peripheral;
class GameoverScene :
	public Scene
{
private:
	int _gameoverImg;
	int _wait;
public:
	GameoverScene(SceneController& controller);
	~GameoverScene();
	void Update(const Input& input)override;
	void Draw()override;
};

