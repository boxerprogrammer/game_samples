#include "Application.h"
#include"Scene/SceneController.h"
#include"Scene/TitleScene.h"
#include"Input\Input.h"
#include<DxLib.h>
#include<cmath>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;


Size
Application::Configure::GetScreenSize()const {
	return Size(SCREEN_WIDTH, SCREEN_HEIGHT);
}

Application::Application()
{
}


Application::~Application()
{
}

void
Application::Initialize() {
	DxLib::SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	DxLib::ChangeWindowMode(true);
	DxLib_Init();
}
void 
Application::Run() {

	SceneController sceneController;
	
	DxLib::SetMainWindowText("Portal2d");

	//メニュー設定
	DxLib::AddMenuItem_Name(nullptr, "mode");
	DxLib::AddMenuItem_Name("mode", "play");
	DxLib::AddMenuItem_Name("mode", "edit");
	DxLib::SetMenuItemMark_Name("play", MENUITEM_MARK_RADIO);
	DxLib::SetMenuItemMark_Name("edit", MENUITEM_MARK_NONE);

	DxLib::SetDrawScreen(DX_SCREEN_BACK);
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	//入力の設定
	Input input;
	input.AddCommand(0, "decide", PeripheralType::keyboard, KEY_INPUT_RETURN);
	input.AddCommand(0, "pause", PeripheralType::keyboard, KEY_INPUT_P);

	input.AddCommand(0, "up", PeripheralType::keyboard, KEY_INPUT_UP);
	input.AddCommand(0, "down", PeripheralType::keyboard, KEY_INPUT_DOWN);
	input.AddCommand(0, "right", PeripheralType::keyboard, KEY_INPUT_RIGHT);
	input.AddCommand(0, "left", PeripheralType::keyboard, KEY_INPUT_LEFT);

	while (ProcessMessage() == 0)
	{
		//メニュー用
		if (DxLib::CheckMenuItemSelect_Name("edit")) {
			DxLib::SetMenuItemMark_Name("edit", MENUITEM_MARK_RADIO);
			DxLib::SetMenuItemMark_Name("play", MENUITEM_MARK_NONE);
		}
		if (DxLib::CheckMenuItemSelect_Name("play")) {
			DxLib::SetMenuItemMark_Name("play", MENUITEM_MARK_RADIO);
			DxLib::SetMenuItemMark_Name("edit", MENUITEM_MARK_NONE);
		}

		//こっからゲーム用
		ClearDrawScreen();
		input.Check();
		sceneController.SceneUpdate(input);
		input.UpdateHistory();
		ScreenFlip();
	}
}
void 
Application::Terminate() {
	DxLib_End();
}