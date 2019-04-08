#include<DxLib.h>
#include"Geometry.h"

constexpr unsigned int scr_w = 640;
constexpr unsigned int scr_h = 480;
constexpr unsigned int play_areaX = 50;
constexpr unsigned int play_areaY = 50;

int main() {
	ChangeWindowMode(true);
	DxLib::SetWindowText("QIX");
	DxLib::SetGraphMode(scr_w, scr_h, 32);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	DrawString(100, 100, "Hello World", 0xffffff); 
	Position2 playerPos(550 / 2,450);//
	auto offset=Vector2(play_areaX, play_areaY);
	char keystate[256];

	auto area = MakeScreen(450, 400,true);
	
	Position2 startPos = playerPos-offset;
	char field[225][200] = {};
	bool firstUp = true;
	while (!ProcessMessage()) {
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]) {
			playerPos.y = max(play_areaY,playerPos.y -2);
			if (firstUp) {
				startPos = playerPos-offset;
				firstUp = false;
			}
		}else if (keystate[KEY_INPUT_DOWN]) {
			playerPos.y  = min(play_areaY+400,playerPos.y + 2);
		}else if (keystate[KEY_INPUT_RIGHT]) {
			playerPos.x = min(play_areaX+450,playerPos.x + 2);
			if (playerPos.x == play_areaX + 450) {
				//FillConvex
				auto tmppos = playerPos - offset;
				SetDrawScreen(area);
				DrawBox(startPos.x, tmppos.y, tmppos.x, startPos.y, 0xaaffaa, true);
				SetDrawScreen(DX_SCREEN_BACK);
			}
		}else if (keystate[KEY_INPUT_LEFT]) {
			playerPos.x = max(play_areaX , playerPos.x - 2);
		}

		//ògï`âÊ
		DrawBox(offset.x-1, offset.y-1, offset.x + 451, offset.y + 401, 0xffffff, false);
		DrawBox(offset.x, offset.y, offset.x + 450, offset.y+400 , 0xffffff, false);
		

		SetDrawScreen(area);
		auto tmpPos = playerPos - Vector2(play_areaX, play_areaY);
		//ãOê’âºï`âÊ
		DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);
		/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
		SetDrawScreen(DX_SCREEN_BACK);
		//ãOê’ï`âÊ
		DrawGraph(play_areaX, play_areaY, area, true);
		//éûä˙ï`âÊ
		DrawCircle(playerPos.x, playerPos.y, 3, 0xffff88, true);
		ScreenFlip();
	}

	DxLib_End();


}