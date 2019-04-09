#include<DxLib.h>
#include<vector>
#include<list>
#include<algorithm>
#include"Geometry.h"

constexpr unsigned int scr_w = 640;
constexpr unsigned int scr_h = 480;
constexpr unsigned int play_areaX = 50;
constexpr unsigned int play_areaY = 50;

using namespace std;
//ècÇ∆â°ÇµÇ©Ç»Ç¢ê¸ï™èÓïÒ
struct Segment {
	Position2 a;//ïKÇ∏ç∂Ç©è„
	Position2 b;//ïKÇ∏âEÇ©â∫
	Segment(const Position2& lval, const Position2& rval) :a(lval), b(rval) {
		if (rval.x < lval.x) {
			a = rval;
			b = lval;
		}
		else if (rval.y < lval.y) {
			a = rval;
			b = lval;
		}
	}
	Segment(int x1,int y1,int x2, int y2) : a(x1,y1),b(x2,y2){
		if (x1> x2) {
			swap(a, b);
		}
		else if (y1>y2) {
			swap(a, b);
		}
	}
};

bool operator==(const Segment& lval,const Segment& rval) {
	return lval.a == rval.a && lval.b == rval.b;
}
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


	//âEÇîªíËÅBíºÇ¬Ç»Ç¨ÇÕêîÇ…ì¸ÇÍÇ»Ç¢ÅB
	auto GetRight = [](const Segment& hSeg, const Segment& vSeg)->int {
		if (hSeg.b == vSeg.b || hSeg.a == vSeg.a)return -1;
		if (hSeg.a.x > vSeg.a.x)return -1;
		if (vSeg.a.y <= hSeg.a.y && hSeg.a.y <= vSeg.b.y) {
			//Ç†ÇΩÇËÇ‹ÇµÇΩÅIÅI
			return vSeg.a.x;
		}
		return -1;
	};
	auto SearchMinRight=[GetRight](const Segment& hseg, const std::list<Segment>& segments)->int {
		int minim = 400;
		for (auto& s : segments) {
			auto right = GetRight(hseg, s);
			if (right == -1)continue;
			minim =min(minim,right);
		}
		return minim;
	};
	std::list<Segment> hSegments;//êÖïΩï˚å¸ê¸ï™
	std::list<Segment> vSegments;//êÇíºï˚å¸ê¸ï™
	
	Segment topseg(0,0,450,0);
	Segment bottomseg(0, 400, 450, 400);
	Segment leftseg(0, 0, 0, 400);
	Segment rightseg(450, 0, 450, 400);

	hSegments.emplace_back(0, 0, 450, 0);//è„
	hSegments.emplace_back(0, 400, 450, 400);//â∫

	vSegments.emplace_back(0, 0, 0, 400);//ç∂
	vSegments.emplace_back(450, 0, 450, 400);//âE


	bool onTheFrame = true;//äOògÇÃè„Ç…Ç¢ÇÈÇ©ÅH

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
			if (onTheFrame || lastDirection == Direction::right || lastDirection == Direction::left) {
				if (!keypoints.empty()) {
					if (keypoints.back().x == tmppos.x) {
						vSegments.emplace_back(keypoints.back(), tmppos);
					}
					else if (keypoints.back().y == tmppos.y) {
						hSegments.emplace_back(keypoints.back(), tmppos);
					}
				}
				keypoints.push_back(tmppos);
			}
			onTheFrame = (playerPos.y == play_areaY);
			lastDirection = Direction::up;
		}else if (keystate[KEY_INPUT_DOWN]) {
			playerPos.y  = min(play_areaY+400,playerPos.y + 2);
			auto tmppos = playerPos - offset;
			if (onTheFrame || lastDirection == Direction::right || lastDirection == Direction::left) {
				if (!keypoints.empty()) {
					if (keypoints.back().x == tmppos.x) {
						vSegments.emplace_back(keypoints.back(), tmppos);
					}
					else if (keypoints.back().y == tmppos.y) {
						hSegments.emplace_back(keypoints.back(), tmppos);
					}
				}
				keypoints.push_back(tmppos);
				onTheFrame = false;
			}
			onTheFrame = (playerPos.y == play_areaY + 400);

			lastDirection = Direction::down;
		}else if (keystate[KEY_INPUT_RIGHT]) {
			auto pposx = min(play_areaX+450,playerPos.x + 2);

			//âEï«Ç…ìñÇΩÇ¡ÇΩéû
			if (pposx == play_areaX + 450) {
				//FillConvex
				auto tmppos = playerPos - offset;
				hSegments.emplace_back(keypoints.back(), tmppos);
				DxLib::SetDrawScreen(area);

				auto lastpos = startPos;
				unsigned int maxheight=0;
				//for (auto& pos : keypoints) {
				//	if (lastpos.y != pos.y) {
				//		if (maxheight > abs(pos.y - startPos.y)) {
				//			DxLib::DrawBox(pos.x - 1, pos.y - 1, pos.x + 1, startPos.y + 1, 0xaaffaa, true);
				//			continue;
				//		}
				//		maxheight = abs(pos.y - startPos.y);
				//	}
				//	DxLib::DrawBox(pos.x - 1, pos.y - 1, tmppos.x + 1, lastpos.y + 1, 0xaaffaa, true);
				//	lastpos = pos;
				//}
				auto hSegit = hSegments.rbegin();
				for (; hSegit != hSegments.rend();++hSegit) {
					if (*hSegit == topseg || *hSegit == bottomseg)continue;
					int y = hSegit->a.y;
					auto it = find_if(hSegit, hSegments.rend(), [y](const Segment& seg)->bool {
						return y < seg.a.y;
					});
					if (it != hSegments.rend()) {
						int right = SearchMinRight(*hSegit, vSegments);
						DxLib::DrawBox(hSegit->a.x, hSegit->a.y-1, right+ 1, it->b.y + 1, 0xaaffaa, true);
					}
				}

				keypoints.clear();
				vSegments.clear();
				hSegments.clear();
				firstUp = true;
				DxLib::SetDrawScreen(DX_SCREEN_BACK);

			}
			else {
				auto tmppos = playerPos - offset;
				if (onTheFrame || lastDirection == Direction::up || lastDirection == Direction::down) {
					if (!keypoints.empty()) {
						if (keypoints.back().x == tmppos.x) {
							vSegments.emplace_back(keypoints.back(), tmppos);
						}
						else if (keypoints.back().y == tmppos.y) {
							hSegments.emplace_back(keypoints.back(), tmppos);
						}
					}
					
					keypoints.push_back(tmppos);
					onTheFrame = false;
				}
			}
			playerPos.x=pposx;
			lastDirection = Direction::right;
			onTheFrame=playerPos.x == play_areaX + 450;
		}else if (keystate[KEY_INPUT_LEFT]) {
			playerPos.x = max(play_areaX , playerPos.x - 2);
			auto tmppos = playerPos - offset;
			if (onTheFrame || lastDirection == Direction::up || lastDirection == Direction::down) {

				if (!keypoints.empty()) {
					if (keypoints.back().x == tmppos.x) {
						vSegments.emplace_back(keypoints.back(), tmppos);
					}
					else if (keypoints.back().y == tmppos.y) {
						hSegments.emplace_back(keypoints.back(), tmppos);
					}
				}

				keypoints.push_back(tmppos);
			}
			lastDirection = Direction::left;
			onTheFrame = playerPos.x == play_areaX;
		}

		//ògï`âÊ
		DxLib::DrawBox(offset.x-1, offset.y-1, offset.x + 451, offset.y + 401, 0xffffff, false);
		DxLib::DrawBox(offset.x, offset.y, offset.x + 450, offset.y+400 , 0xffffff, false);
		

		DxLib::SetDrawScreen(area);
		auto tmpPos = playerPos - Vector2(play_areaX, play_areaY);
		//ãOê’âºï`âÊ
		DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);
		/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		//ãOê’ï`âÊ
		DxLib::DrawGraph(play_areaX, play_areaY, area, true);
		//éûä˙ï`âÊ
		DxLib::DrawCircle(playerPos.x, playerPos.y, 3, 0xffff88, true);
		DxLib::ScreenFlip();
	}

	DxLib_End();


}