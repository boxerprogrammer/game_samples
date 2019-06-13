#include "TopMenuScene.h"
#include<DxLib.h>
#include"SceneController.h"
#include"../Input/Input.h"
#include"GamePlayingScene.h"

using namespace std;

TopMenuScene::TopMenuScene(SceneController& controller):Scene(controller)
{
}


TopMenuScene::~TopMenuScene()
{
}

void
TopMenuScene::Update(const Input& input) {
	if (input.IsTriggered(0, "decide")) {
		_controller.ChangeScene(make_unique< GamePlayingScene>("stagedata/stage1.fmf", "img/stage1.png", "bgm/stage1.mp3", _controller));
	}
}

void
TopMenuScene::Draw() {
	DrawString(100, 100, "TopMenu Scene", 0xffffff);
}