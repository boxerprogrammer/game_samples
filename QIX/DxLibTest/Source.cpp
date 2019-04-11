#include<DxLib.h>
#include<vector>
#include<list>
#include<algorithm>
#include<cassert>
#include"Geometry.h"

constexpr unsigned int scr_w = 640;
constexpr unsigned int scr_h = 480;
constexpr unsigned int play_area_left = 50;
constexpr unsigned int play_area_top = 50;
constexpr unsigned int play_area_right = 500;
constexpr unsigned int play_area_bottom = 450;
constexpr unsigned int play_area_width = play_area_right-play_area_left;
constexpr unsigned int play_area_height = play_area_bottom - play_area_top;


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
int rewardH;

///�h��Ԃ��͈͂�Ԃ�
///@param hSegs ������
///@param vSegs ������
///@remarks ���ꂼ��̃Z�O�����g�̓\�[�g�ς�(�と��)(�����E)�̂��̂Ƃ���B
void FillRange(std::list<Segment>& hSegs, std::list<Segment>& vSegs,bool reverseFlg=false) {
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
	if (reverseFlg) {
		vSegs.reverse();
	}
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
		auto drawcount= xpoints.size();
		for (int i = 0; i < drawcount-1; i+=2) {
			if ((i + 2) < drawcount&&
				(y == xpoints[i].a.y &&
				y == xpoints[i + 1].b.y ) || 
				(y == xpoints[i].b.y &&
					y == xpoints[i + 1].a.y)) {
				DrawBox(xpoints[i].a.x, y, xpoints[i+2].a.x, y + 1, 0xffaaaa, true);
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i+2].a.x, y, xpoints[i+2].a.x, y, abs(xpoints[i+2].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, abs(xpoints[i + 2].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
				++i;//����܂���
			}
			else if ((i + 2) < drawcount&&
				((y == xpoints[i + 1].a.y &&
				y == xpoints[i + 2].b.y) || (y == xpoints[i + 1].b.y &&
					y == xpoints[i + 2].a.y))) {
				if (reverseFlg) {
					DrawBox(xpoints[i].a.x, y, xpoints[i + 2].a.x, y + 1, 0xffaaaa, true);
				}
				else {
					DrawBox(xpoints[i+2].a.x, y, xpoints[i].a.x, y + 1, 0xffaaaa, true);
				}
				DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, abs(xpoints[i + 2].a.x - xpoints[i].a.x), 1, rewardH, false);
				++i;//����܂���
			}
			else {
				DrawBox(xpoints[i].a.x, y, xpoints[i+1].a.x, y + 1, 0xffaaaa, true);
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i+1].a.x, y, xpoints[i+1].a.x, y, abs(xpoints[i + 1].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, abs(xpoints[i + 1].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
			}
		}
		//y = hit->a.y;
	}
}
//sort(hSegs.begin(), hSegs.end(), [](const Segment& lval, const Segment& rval) {
//	lval.a.y <
//});

void FillVirtualScreen(int area, std::list<Segment> &hSegments, std::list<Segment> &vSegments, bool &onTheFrame, std::list<Position2> &keypoints,bool reverseFlg=false);

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments);

int main() {
	ChangeWindowMode(true);
	DxLib::SetWindowText("QIX");
	DxLib::SetGraphMode(scr_w, scr_h, 32);
	DxLib_Init();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
	rewardH = DxLib::LoadGraph("img/reward.jpg");

	DrawString(100, 100, "Hello World", 0xffffff); 
	Position2 playerPos(play_area_left + (play_area_right-play_area_left) / 2,play_area_bottom);//
	auto offset=Vector2(play_area_left, play_area_top);
	char keystate[256];

	auto area = MakeScreen(play_area_right-play_area_left,
		play_area_bottom-play_area_top, 
		true);
	
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
		int minim = play_area_right-play_area_left;
		for (auto& s : segments) {
			auto right = GetRight(hseg, s);
			if (right == -1)continue;
			minim =min(minim,right);
		}
		return minim;
	};
	std::list<Segment> hSegments;//������������
	std::list<Segment> vSegments;//������������
	
	//�㉺���E�Z�O�����g
	Segment topseg(0,0,play_area_width,0);
	Segment bottomseg(0, play_area_height, play_area_width, play_area_height);
	Segment leftseg(0, 0, 0, play_area_height);
	Segment rightseg(play_area_width, 0, play_area_width, play_area_height);




	bool onTheFrame = true;//�O�g�̏�ɂ��邩�H

	auto count = GetTickCount();
	while (!ProcessMessage()) {
		
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]&&playerPos.y > play_area_top) {
			auto pposy = max(play_area_top,playerPos.y -2);
			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				if (playerPos.y == play_area_bottom) {
					hSegments.push_back(bottomseg);
				}

				auto tmppos = playerPos - offset;
				//��ǂɓ���������
				if (!onTheFrame&&pposy == play_area_top) {
					//FillConvex����
					hSegments.push_back(topseg);
					vSegments.emplace_back(keypoints.back(), tmppos);
					if (hSegments.back()==topseg && hSegments.front()==bottomseg) {
						vSegments.push_back(rightseg);//�Ƃ肠�����E��
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments, onTheFrame, keypoints);
				}
				else {
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
				}
			}
			playerPos.y = pposy;
			onTheFrame = (playerPos.y == play_area_top);
			lastDirection = Direction::up;
			
		}
		else if (keystate[KEY_INPUT_DOWN] && playerPos.y < play_area_bottom) {

			auto pposy = min(play_area_bottom, playerPos.y + 2);
			auto tmppos = playerPos - offset;

			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				if (playerPos.y == play_area_top) {
					hSegments.push_back(topseg);
				}
				//���ǂɓ���������
				if (!onTheFrame&&pposy == play_area_bottom) {
					//FillConvex����
					hSegments.push_back(bottomseg);
					vSegments.emplace_back(keypoints.back(), tmppos);
					if (hSegments.back() == bottomseg && hSegments.front() == topseg) {
						vSegments.push_back(leftseg);//�Ƃ肠��������
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments, onTheFrame, keypoints);
				}
				else {

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
				}
			}
			playerPos.y = pposy;
			onTheFrame = (playerPos.y == play_area_bottom);

			lastDirection = Direction::down;
		}else if (keystate[KEY_INPUT_RIGHT] && playerPos.x < play_area_right) {
			auto pposx = min(play_area_right,playerPos.x + 2);
			auto tmppos = playerPos - offset;
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				if (playerPos.x == play_area_left) {
					vSegments.push_back(leftseg);
				}
				//�E�ǂɓ���������
				if (!onTheFrame&&pposx == play_area_right) {
					//FillConvex����
					vSegments.push_back(rightseg);
					hSegments.emplace_back(keypoints.back(), tmppos);
					if (vSegments.back() == rightseg && vSegments.front() == leftseg) {
						//�Ƃ肠������
						hSegments.push_back(topseg);
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments, onTheFrame, keypoints);
				}
				else {
					if (onTheFrame || lastDirection == Direction::up || lastDirection == Direction::down) {
						if (!keypoints.empty()) {
							if (keypoints.back().x == tmppos.x) {
								vSegments.emplace_back(keypoints.back(), tmppos);
							}
							else if (keypoints.back().y == tmppos.y) {
								hSegments.emplace_back(keypoints.back(), tmppos);
							}
						}
						tmppos.x++;
						keypoints.push_back(tmppos);
						onTheFrame = false;
					}
				}
			}
			playerPos.x=pposx;
			lastDirection = Direction::right;
			onTheFrame=playerPos.x == play_area_right;
		}else if (keystate[KEY_INPUT_LEFT] && playerPos.x > play_area_left) {
			
			auto pposx = max(play_area_left , playerPos.x - 2);
			auto tmppos = playerPos - offset;
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				if (playerPos.x == play_area_right) {
					vSegments.push_back(rightseg);
				}
				//���ǂɓ���������
				if (!onTheFrame && pposx == play_area_left) {
					//FillConvex����
					if (vSegments.front() == leftseg) {
					}
					else {
						vSegments.push_back(leftseg);
					}
					hSegments.emplace_back(keypoints.back(), tmppos);
					if (vSegments.back() == leftseg && vSegments.front() == rightseg) {
						//�Ƃ肠������
						hSegments.push_back(bottomseg);
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments, onTheFrame, keypoints,true);
				}
				else {

					if (onTheFrame || lastDirection == Direction::up || lastDirection == Direction::down) {

						if (!keypoints.empty()) {
							if (keypoints.back().x == tmppos.x) {
								vSegments.emplace_back(keypoints.back(), tmppos);
							}
							else if (keypoints.back().y == tmppos.y) {
								hSegments.emplace_back(keypoints.back(), tmppos);
							}
						}
						onTheFrame = false;
						tmppos.x--;
						keypoints.push_back(tmppos);
					}
				}
			}
			playerPos.x = pposx;
			lastDirection = Direction::left;
			onTheFrame = playerPos.x == play_area_left;
		}

		//�g�`��
		DxLib::DrawBox(offset.x-1, offset.y-1, offset.x + 451, offset.y + 401, 0xffffff, false);
		DxLib::DrawBox(offset.x, offset.y, offset.x + play_area_width, offset.y+play_area_height, 0xffffff, false);
		

		DxLib::SetDrawScreen(area);
		auto tmpPos = playerPos - Vector2(play_area_left, play_area_top);
		//�O�Չ��`��
		DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);

		/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
		DxLib::SetDrawScreen(DX_SCREEN_BACK);
		//�O�Օ`��
		DxLib::DrawGraph(play_area_left, play_area_top, area, true);
		//�����`��
		DxLib::DrawCircle(playerPos.x, playerPos.y, 3, 0xffff88, true);

		DrawDebugStatus(keypoints, hSegments, vSegments);
		DrawFormatString(512, 300, 0xffffff, "fps=%f", 1000.0f/static_cast<float>(GetTickCount() - count));
		count = GetTickCount();

		DxLib::ScreenFlip();
	}

	DxLib_End();


}

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments)
{
	//�f�o�b�O�\��
	DrawFormatString(512, 50, 0xffffff, "points=%d", keypoints.size());
	DrawFormatString(512, 150, 0xffffff, "hSegs=%d", hSegments.size());
	DrawFormatString(512, 250, 0xffffff, "vSegs=%d", vSegments.size());
}

void FillVirtualScreen(int area, list<Segment> &hSegments, list<Segment> &vSegments, bool &onTheFrame, list<Position2> &keypoints, bool reverseFlg )
{
	//����ʂ�h��Ԃ�
	DxLib::SetDrawScreen(area);
	FillRange(hSegments, vSegments,reverseFlg);
	onTheFrame = true;
	//�㏈��
	keypoints.clear();
	vSegments.clear();
	hSegments.clear();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
}
