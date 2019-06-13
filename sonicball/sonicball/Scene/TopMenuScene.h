#pragma once
#include "Scene.h"
class TopMenuScene final :
	public Scene
{
public:
	TopMenuScene(SceneController& controller);
	~TopMenuScene();
	void Update(const Input& input)override;
	void Draw()override;
};

