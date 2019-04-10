#include<DxLib.h>
#include<vector>
#include<list>
#include<algorithm>
#include<cassert>
#include"Geometry.h"

constexpr unsigned int scr_w = 640;
constexpr unsigned int scr_h = 480;
constexpr unsigned int play_areaX = 50;
constexpr unsigned int play_areaY = 50;

using namespace std;
//�c�Ɖ������Ȃ��������
struct Segment {
	Position2 a;//�K��������
	Position2 b;//�K���E����
	//Segment* lastSeg = nullptr;
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

///�h��Ԃ��͈͂�Ԃ�
///@param hSegs ������
///@param vSegs ������
///@remarks ���ꂼ��̃Z�O�����g�̓\�[�g�ς�(�と��)(�����E)�̂��̂Ƃ���B
void FillRange(std::list<Segment>& hSegs, std::list<Segment>& vSegs) {
	//std::sort(hSegs.begin(), hSegs.end(), [](const Segment& lval, const Segment& rval) {
	//	lval.a.y < rval.a.y;
	//});
	//std::sort(vSegs.begin(), vSegs.end(), [](const Segment& lval, const Segment& rval) {
	//	lval.a.x < rval.a.x;
	//});
	hSegs.sort([](const Segment& lval, const Segment& rval)->bool {
			return lval.a.y < rval.a.y;
	});
	vSegs.sort([](const Segment& lval, const Segment& rval)->bool {
			return lval.a.x < rval.a.x;
	});
	auto hit = hSegs.begin();
	auto top = hSegs.begin()->a.y;
	auto bottom = hSegs.back().a.y;
	for (int y = top; y < bottom;++y) {
		//++hit;
		//if (hit == hSegs.end())break;
		std::vector<Segment> xpoints;
		copy_if(vSegs.begin(), vSegs.end(), back_inserter(xpoints), [y](const Segment& vseg)->bool {
			return vseg.a.y <= y&&y <= vseg.b.y;
		});
		auto drawcount= xpoints.size()/2;
		for (int i = 0; i < drawcount; ++i) {
			DrawBox(xpoints[i*2].a.x,y, xpoints[i * 2+1].a.x,y+1,0xffaaaa,true);
		}
		//y = hit->a.y;
	}
}
//sort(hSegs.begin(), hSegs.end(), [](const Segment& lval, const Segment& rval) {
//	lval.a.y <
//});

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


	//�E�𔻒�B���Ȃ��͐��ɓ���Ȃ��B
	auto GetRight = [](const Segment& hSeg, const Segment& vSeg)->int {
		if (hSeg.b == vSeg.b || hSeg.a == vSeg.a || hSeg.b == vSeg.a || hSeg.a == vSeg.b)return -1;
		if (hSeg.a.x > vSeg.a.x)return -1;
		if (vSeg.a.y <= hSeg.a.y && hSeg.a.y <= vSeg.b.y) {
			//������܂����I�I
			return vSeg.a.x;
		}
		return -1;
	};
	auto SearchMinRight=[GetRight](const Segment& hseg, const std::list<Segment>& segments)->int {
		int minim = 450;
		for (auto& s : segments) {
			auto right = GetRight(hseg, s);
			if (right == -1)continue;
			minim =min(minim,right);
		}
		return minim;
	};
	std::list<Segment> hSegments;//������������
	std::list<Segment> vSegments;//������������
	
	Segment topseg(0,0,450,0);
	Segment bottomseg(0, 400, 450, 400);
	Segment leftseg(0, 0, 0, 400);
	Segment rightseg(450, 0, 450, 400);

	//hSegments.emplace_back(0, 0, 450, 0);//��
	//hSegments.emplace_back(0, 400, 450, 400);//��

	//vSegments.emplace_back(0, 0, 0, 400);//��
	//vSegments.emplace_back(450, 0, 450, 400);//�E


	bool onTheFrame = true;//�O�g�̏�ɂ��邩�H

	while (!ProcessMessage()) {
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]) {
			auto pposy = max(play_areaY,playerPos.y -2);
			
			if (firstUp) {
				startPos = playerPos-offset;
				hSegments.push_back(bottomseg);
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
				tmppos.y -= 1;
				keypoints.push_back(tmppos);
			}
			playerPos.y = pposy;
			onTheFrame = (playerPos.y == play_areaY);
			lastDirection = Direction::up;
		}else if (keystate[KEY_INPUT_DOWN]) {

			auto pposY = min(play_areaY+400,playerPos.y + 2);
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
				tmppos.y += 1;
				keypoints.push_back(tmppos);
				onTheFrame = false;
			}
			playerPos.y = pposY;
			onTheFrame = (playerPos.y == play_areaY + 400);

			lastDirection = Direction::down;
		}else if (keystate[KEY_INPUT_RIGHT]) {
			auto pposx = min(play_areaX+450,playerPos.x + 2);

			//�E�ǂɓ���������
			if (!onTheFrame&&pposx == play_areaX + 450) {
				//FillConvex
				auto tmppos = playerPos - offset;
				hSegments.emplace_back(keypoints.back(), tmppos);
				vSegments.push_back(rightseg);
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
				//auto hSegit = hSegments.rbegin();
				//for (; hSegit != hSegments.rend();++hSegit) {
				//	if (*hSegit == topseg || *hSegit == bottomseg)continue;
				//	int y = hSegit->a.y;
				//	auto it = find_if(hSegit, hSegments.rend(), [y](const Segment& seg)->bool {
				//		return y < seg.a.y;//�������Ⴂ�ʒu�ɂ��������T��
				//	});
				//	if (it != hSegments.rend()) {
				//		int right = SearchMinRight(*hSegit, vSegments);
				//		if (hSegit->a.x == it->a.x || hSegit->a.x == it->b.x) {
				//			DxLib::DrawBox(hSegit->a.x, hSegit->a.y - 1, right + 1, it->b.y + 1, 0xaaffaa, true);
				//		}
				//		else if((hSegit->b.x == it->a.x || hSegit->b.x == it->b.x)){
				//			DxLib::DrawBox(hSegit->b.x, hSegit->a.y - 1, right + 1, it->b.y + 1, 0xaaffaa, true);
				//		}
				//		else {
				//			DxLib::DrawBox(hSegit->a.x, hSegit->a.y - 1, right + 1, it->b.y + 1, 0xaaffaa, true);
				//		}
				//	}
				//	else {
				//		assert(0);
				//	}
				//}
				FillRange(hSegments, vSegments);
				onTheFrame = true;
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
			auto pposX = max(play_areaX , playerPos.x - 2);
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
			playerPos.x = pposX;
			lastDirection = Direction::left;
			onTheFrame = playerPos.x == play_areaX;
		}

		//�g�`��
		DxLib::DrawBox(offset.x-1, offset.y-1, offset.x + 451, offset.y + 401, 0xffffff, false);
		DxLib::DrawBox(offset.x, offset.y, offset.x + 450, offset.y+400 , 0xffffff, false);
		

		DxLib::SetDrawScreen(area);
		auto tmpPos = playerPos - Vector2(play_areaX, play_areaY);
		//�O�Չ��`��
		DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);
		/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		//�O�Օ`��
		DxLib::DrawGraph(play_areaX, play_areaY, area, true);
		//�����`��
		DxLib::DrawCircle(playerPos.x, playerPos.y, 3, 0xffff88, true);
		DxLib::ScreenFlip();
	}

	DxLib_End();


}