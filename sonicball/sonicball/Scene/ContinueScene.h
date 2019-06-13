#pragma once
#include "Scene.h"
class ContinueScene :
	public Scene
{
private:
	int _countImg;
	int _frameCount;
public:
	ContinueScene(SceneController& controller);
	~ContinueScene();
	void Update(const Input& input)override;
	void Draw()override;
};

