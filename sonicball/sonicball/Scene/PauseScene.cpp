#include "PauseScene.h"
#include"SceneController.h"
#include"../Input/Input.h"
#include"../Application.h"
#include<DxLib.h>

constexpr int expand_time = 20;
constexpr int scale_time = 60;
constexpr int menu_id_count = 3;

using namespace std;

PauseScene::PauseScene(SceneController& controller):Scene(controller),
lambda_fadein ( [this](const Input& input) {
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, _frame);
	DxLib::DrawBox(0, 0, 640, 480, 0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	--_frame;
}),
lambda_fadeout([this](const Input& input) {
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, _frame);
	DxLib::DrawBox(0, 0, 640, 480, 0x000000, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	++_frame;
})

{
	_inframeH=DxLib::MakeScreen(500, 500);
	_frame = expand_time;
	_updater = &PauseScene::UpdatePreOpener;
	_drawer = &PauseScene::DrawExpand;
	_openSE = DxLib::LoadSoundMem("se/menuopen.wav");
	_closeSE = DxLib::LoadSoundMem("se/menuclose.wav");
}


PauseScene::~PauseScene()
{
	DxLib::DeleteSoundMem(_openSE);
	DxLib::DeleteSoundMem(_closeSE);
}

void
PauseScene::UpdatePreOpener(const Input& input) {
	if (_frame > 0) {
		_frame = (_frame - 1);
	}
	else {
		_frame = scale_time;
		DxLib::PlaySoundMem(_openSE, DX_PLAYTYPE_BACK);
		_updater = &PauseScene::UpdateOpener;
		_drawer = &PauseScene::DrawMain;
	}
}
void
PauseScene::UpdateOpener(const Input& input) {
	if (_frame > 0) {
		_frame = (_frame - 1);
	}
	else {
		
		_updater = &PauseScene::UpdateMain;
		
	}
}

void
PauseScene::UpdateMain(const Input& input) {
	if (input.IsTriggered(0, "pause")) {
		_frame = 0;
		DxLib::PlaySoundMem(_closeSE, DX_PLAYTYPE_BACK);
		_updater = &PauseScene::UpdateCloser;
	}
	if (input.IsTriggered(0, "up")) {
		_currentCursorId=(_currentCursorId + menu_id_count - 1) % menu_id_count;
	}
	if (input.IsTriggered(0, "down")) {
		_currentCursorId = (_currentCursorId +1) % menu_id_count;
	}
}

void
PauseScene::UpdateCloser(const Input& input) {
	if (_frame <= scale_time) {
		_frame = (_frame + 1);
	}
	else {
		_frame = 0;
		_updater = &PauseScene::UpdatePostCloser;
		_drawer = &PauseScene::DrawExpand;
	}
	
	//_lambda(input);
}



void
PauseScene::UpdatePostCloser(const Input& input) {
	if (_frame <= expand_time) {
		_frame = (_frame + 1);
	}
	else {
		_controller.PopScene();
	}
}

void 
PauseScene::Update(const Input& input) {
	(this->*_updater)(input);
}
void 
PauseScene::Draw() {
	(this->*_drawer)();
}


void 
PauseScene::DrawMain() {
	Application& app = Application::Instance();
	auto wsize = app.Instance().GetConfig().GetScreenSize();
	SetDrawScreen(_inframeH);
	ClearDrawScreen();
	DrawString(200, 10, "Pause", 0xffffff);
	DrawString(150, 110, "Command List", 0xffffff);
	DrawString(150, 210, "Back to Title", 0xffffff);
	DrawString(150, 310, "Resume GamePlay", 0xffffff);
	DrawCircle(125, 112 + _currentCursorId * 100 + 5, 10, 0xff8888, false, 3);
	DrawCircle(125, 112 + _currentCursorId * 100 + 5, 5, 0xff8888, true);
	SetDrawScreen(DX_SCREEN_BACK);

	int h = (scale_time - _frame)*(500.0f / static_cast<float>(scale_time));
	DrawRectGraph((wsize.w - 500) / 2, (wsize.h - h) / 2, 0, (500 - h) / 2, 500, h, _inframeH, true);
	DrawBox((wsize.w - 500) / 2, (wsize.h - h) / 2, (wsize.w + 500) / 2, (wsize.h + h) / 2, 0xffffff, false);
}
void 
PauseScene::DrawExpand() {
	Application& app = Application::Instance();
	auto wsize = app.Instance().GetConfig().GetScreenSize();
	int w = (expand_time - _frame)*(500.0f / static_cast<float>(expand_time));
	DrawBox((wsize.w - w) / 2, (wsize.h - 1) / 2, (wsize.w + w) / 2, (wsize.h + 1) / 2, 0xffffff, false);
}