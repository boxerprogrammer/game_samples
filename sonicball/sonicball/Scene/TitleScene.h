#pragma once
#include "Scene.h"
#include"../Geometry.h"
class Input;
class TitleScene final:
	public Scene
{
private:
	struct ImgInfo {
		int handle;
		Size size;
		void Load(const char*);
	};
	struct BgImgs{
		ImgInfo mostbackImg;
		ImgInfo farImg;
		ImgInfo middleImg;
		ImgInfo foregroundImg;
		ImgInfo mostforeImg;
	};
	BgImgs _bgimgs;
	int _bgFrame = 0;


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
	void DrawTitleBackground(int w, int h);
	void NormalUpdate(const Input& input);
	void BlinkUpdate(const Input& input);
	void FadeoutUpdate(const Input& input);
public:
	TitleScene(SceneController& controller);
	~TitleScene();
	void Update(const Input& input) override;
	void Draw() override;
};

