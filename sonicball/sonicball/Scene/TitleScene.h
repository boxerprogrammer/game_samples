#pragma once
#include "Scene.h"
#include"../Geometry.h"
class Input;
class TitleScene final:
	public Scene
{
private:
	int _startSE;
	int _wait;
	int _titlepng;
	int _logoportalH;
	int _pressstartH;
	Size _portalSize;
	Size _pressstartSize;
	Position2 _titlepos;
	Position2 _pressstartpos;
	int _frameCounter;
	void (TitleScene::*_updater)(const Input& input);
	void FadeinUpdate(const Input& input);
	void NormalUpdate(const Input& input);
	void BlinkUpdate(const Input& input);
	void FadeoutUpdate(const Input& input);
public:
	TitleScene(SceneController& controller);
	~TitleScene();
	void Update(const Input& input) override;
	void Draw() override;
};

