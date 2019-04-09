#include<DxLib.h>
#include<vector>
#include<list>
#include"Geometry.h"

constexpr unsigned int scr_w = 640;
constexpr unsigned int scr_h = 480;
constexpr unsigned int play_areaX = 50;
constexpr unsigned int play_areaY = 50;

using namespace std;

int main() {
	ChangeWindowMode(true);
	DxLib::SetWindowText("QIX");
	DxLib::SetGraphMode(scr_w, scr_h, 32);
	DxLib_Init();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);

	


	DrawString(100, 100, "Hello World", 0xffffff); 
	Position2 playerPos(550 / 2,450);//
	auto offset=Vector2(play_areaX, play_areaY);
	char keystate[256];

	auto area = MakeScreen(450, 400,true);
	
	Position2 startPos = playerPos-offset;
	char field[225][200] = {};
	bool firstUp = true;
	enum class Direction {
		up,right,down,left
	};
	Direction lastDirection = Direction::up;
	std::list<Position2> keypoints;
	while (!ProcessMessage()) {
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]) {
			playerPos.y = max(play_areaY,playerPos.y -2);
			if (firstUp) {
				startPos = playerPos-offset;
				firstUp = false;
			}
			auto tmppos = playerPos - offset;
			if (lastDirection == Direction::right || lastDirection == Direction::left) {
				keypoints.push_back(tmppos);
			}
			lastDirection = Direction::up;
		}else if (keystate[KEY_INPUT_DOWN]) {
			playerPos.y  = min(play_areaY+400,playerPos.y + 2);
			auto tmppos = playerPos - offset;
			if (lastDirection == Direction::right || lastDirection == Direction::left) {
				keypoints.push_back(tmppos);
			}
			lastDirection = Direction::down;
		}else if (keystate[KEY_INPUT_RIGHT]) {
			playerPos.x = min(play_areaX+450,playerPos.x + 2);

			//‰E•Ç‚É“–‚½‚Á‚½Žž
			if (playerPos.x == play_areaX + 450) {
				//FillConvex
				auto tmppos = playerPos - offset;
				DxLib::SetDrawScreen(area);

				auto lastpos = startPos;
				unsigned int maxheight=0;
				for (auto& pos : keypoints) {
					if (lastpos.y != pos.y) {
						if (maxheight < abs(pos.y - lastpos.y)) {
							DxLib::DrawBox(pos.x - 1, pos.y - 1, pos.x + 1, startPos.y + 1, 0xaaffaa, true);
							continue;
						}
						maxheight = abs(pos.y - lastpos.y);
					}
					DxLib::DrawBox(pos.x - 1, pos.y - 1, tmppos.x + 1, lastpos.y + 1, 0xaaffaa, true);
					lastpos = pos;

				}
				keypoints.clear();
				firstUp = true;
				DxLib::SetDrawScreen(DX_SCREEN_BACK);

			}
			else {
				auto tmppos = playerPos - offset;
				if (lastDirection == Direction::up || lastDirection == Direction::down) {
					keypoints.push_back(tmppos);
				}
			}

			lastDirection = Direction::right;
		}else if (keystate[KEY_INPUT_LEFT]) {
			playerPos.x = max(play_areaX , playerPos.x - 2);
			auto tmppos = playerPos - offset;
			if (lastDirection == Direction::up || lastDirection == Direction::down) {
				keypoints.push_back(tmppos);
			}
			lastDirection = Direction::left;
		}

		//˜g•`‰æ
		DxLib::DrawBox(offset.x-1, offset.y-1, offset.x + 451, offset.y + 401, 0xffffff, false);
		DxLib::DrawBox(offset.x, offset.y, offset.x + 450, offset.y+400 , 0xffffff, false);
		

		DxLib::SetDrawScreen(area);
		auto tmpPos = playerPos - Vector2(play_areaX, play_areaY);
		//‹OÕ‰¼•`‰æ
		DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);
		/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		//‹OÕ•`‰æ
		DxLib::DrawGraph(play_areaX, play_areaY, area, true);
		//ŽžŠú•`‰æ
		DxLib::DrawCircle(playerPos.x, playerPos.y, 3, 0xffff88, true);
		DxLib::ScreenFlip();
	}

	DxLib_End();


}