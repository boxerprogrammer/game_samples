#include "ContinueScene.h"
#include"../Application.h"
#include"../input/input.h"
#include<DxLib.h>
#include"GameoverScene.h"
#include"GamePlayingScene.h"
#include"SceneController.h"

using namespace std;

ContinueScene::ContinueScene(SceneController& controller):Scene(controller)
{
	_countImg = DxLib::LoadGraph("img/count.png");
	_frameCount = 0;
}


ContinueScene::~ContinueScene()
{
	DxLib::DeleteGraph(_countImg);

}


void 
ContinueScene::Update(const Input& input) {


	DxLib::DrawRectRotaGraph2(360, 200, 
		117*((_frameCount/90)%5), 
		159* ((_frameCount / 90) / 5),
		117, 159,
		59,80,
		(float)(10+_frameCount%90)/100.0f,0.0f,_countImg, true,false);


	++_frameCount;
	//if (p.IsTrigger(PAD_INPUT_1) || p.IsTrigger(PAD_INPUT_2)) {
	//	_frameCount += 90 - (_frameCount % 90);
	//}

	if (input.IsTriggered(0,"decide")) {
		_controller.ChangeScene(make_unique<GameoverScene>(_controller));
		//	//Game::Instance().ChangeScene(new GamePlayingScene("",""));
		//	//Game::Instance().ChangeScene(new GamePlayingScene("stagedata/stage1_b.fmf", "img/stage1_b.png", "bgm/boss.mp3"));
	}
	
	//if (p.IsTrigger(PAD_INPUT_8)){
	//	//Game::Instance().ChangeScene(new GamePlayingScene("",""));
	//	//Game::Instance().ChangeScene(new GamePlayingScene("stagedata/stage1_b.fmf", "img/stage1_b.png", "bgm/boss.mp3"));
	//}
	//else if (_frameCount > 900) {
	//	//Game::Instance().ChangeScene(new GameoverScene());
	//}

}


void
ContinueScene::Draw() {
	DrawString(100, 100, "Continue Scene", 0xffffff);
}