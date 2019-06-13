#include "GameoverScene.h"
#include<DxLib.h>
#include"../input/Input.h"
#include"../Application.h"
#include"TitleScene.h"
#include"SceneController.h"
#include<cmath>

using namespace std;

GameoverScene::GameoverScene(SceneController& controller) :Scene(controller)
{
	_gameoverImg=DxLib::LoadGraph("img/gameover.png");
	_wait = 600;
}


GameoverScene::~GameoverScene()
{
	DxLib::DeleteGraph(_gameoverImg);
}

void 
GameoverScene::Update(const Input& input) {
	//for (int i = 0; i < 768; ++i) {
	//	SetDrawArea(i, 0, i + 1, 448);
	//	DxLib::DrawGraph(0, 30+sin((float)(_wait+i)/90.0f)*((float)_wait/10.0f), _gameoverImg, true);
	//}
	//if (p.IsTrigger(PAD_INPUT_8)||_wait==0) {
	//	SetDrawArea(0, 0, 768, 448);
	//	Application::Instance().ChangeScene(new TitleScene());
	//}
	if (input.IsTriggered(0, "decide")) {
		_controller.ChangeScene(make_unique< TitleScene>(_controller));
	}
	--_wait;
}

void 
GameoverScene::Draw() {
	DrawString(100, 100, "Gameover Scene", 0xffffff);
}