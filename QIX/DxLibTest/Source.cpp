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

enum class Direction{
	none,//��������
	up,//��
	down,//��
	left,//��
	right//�E
};

//�c�Ɖ������Ȃ��������
struct Segment {
	Position2 a;//�K��������
	Position2 b;//�K���E����
	Direction inner;//�Z�O�����g�̓�������
	//Segment* lastSeg = nullptr;
	Segment(const Position2& lval, const Position2& rval,Direction dir=Direction::none) :a(lval), b(rval),inner(dir) {
		if (rval.x < lval.x) {
			a = rval;
			b = lval;
		}
		else if (rval.y < lval.y) {
			a = rval;
			b = lval;
		}
	}
	Segment(int x1,int y1,int x2, int y2, Direction dir = Direction::none) : a(x1,y1),b(x2,y2),inner(dir){
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

std::list<Segment> _hFixedSegs;
std::list<Segment> _vFixedSegs;

///�h��Ԃ��͈͂�Ԃ�
///@param hSegs ������
///@param vSegs ������
///@remarks ���ꂼ��̃Z�O�����g�̓\�[�g�ς�(�と��)(�����E)�̂��̂Ƃ���B
void FillRange(std::list<Segment>& hSegs, std::list<Segment>& vSegs,bool reverseFlg=false) {
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
	for (int y = top; y <= bottom;++y) {
		std::vector<Segment> xpoints;
		copy_if(vSegs.begin(), vSegs.end(), back_inserter(xpoints), [y](const Segment& vseg)->bool {
			return vseg.a.y <= y&&y <= vseg.b.y;
		});
		auto drawcount= xpoints.size();
		for (int i = 0; i < drawcount-1; i+=2) {
			//��ӂ������͉��ӂł���A���̌������ɂ������_������ꍇ(�������̌����_�܂ł�h��Ԃ�)
			// �Q|       |�����܂œh��Ԃ��Ώ�
			//|�@        |
			//�����������ꍇ
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
				++i;//����܂���(���̂ނ����܂œh��Ԃ��Ă���̂ŃJ�E���g��i�߂�)
			}
			//��̂�Ƌt�p�^�[��(�\�[�g���Ă邩��t�̏ꍇ����ŏI����Ă��܂��c)
			//|       |�Q�@
			//|�@        |
			//�����������ꍇ
			else if ((i + 2) < drawcount&&
				((y == xpoints[i + 1].a.y &&
				y == xpoints[i + 2].b.y) || (y == xpoints[i + 1].b.y &&
					y == xpoints[i + 2].a.y))) {
				
				//�m��ӂ�o�^
				if (y==xpoints[i+1].a.y&&y==xpoints[i + 2].a.y) {
					_hFixedSegs.emplace_back(xpoints[i+1].a, xpoints[i + 2].a, Direction::up);
					_vFixedSegs.emplace_back(xpoints[i+1].a, xpoints[i+1].b, Direction::left);
					_vFixedSegs.emplace_back(xpoints[i + 2].a, xpoints[i + 2].b, Direction::right);
				}

				if (reverseFlg) {
					DrawBox(xpoints[i + 2].a.x, y, xpoints[i].a.x, y + 1, 0xffaaaa, true);
					DxLib::DrawRectGraph(xpoints[i + 2].a.x, y, xpoints[i + 2].a.x, y, abs(xpoints[i + 2].a.x - xpoints[i].a.x), 1, rewardH, false);
					
				}
				else {
					DrawBox(xpoints[i].a.x, y, xpoints[i + 2].a.x, y + 1, 0xffaaaa, true);
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, abs(xpoints[i + 2].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
				++i;//����܂���(���̂ނ����܂œh��Ԃ��Ă���̂ŃJ�E���g��i�߂�)
			}
			else {//�ʏ�h��
				if (y == xpoints[i].b.y || y == xpoints[i + 1].b.y) {
					_hFixedSegs.emplace_back(Position2(xpoints[i].b.x, y), Position2(xpoints[i + 1].b.x, y), Direction::down);
				}
				//�m��ӂ�o�^
				if (y==xpoints[i].a.y || y==xpoints[i + 1].a.y) {
					_hFixedSegs.emplace_back(Position2(xpoints[i].a.x,y), Position2(xpoints[i+1].a.x, y), Direction::up);
					Direction d1 = Direction::left, d2 = Direction::right;
					if (reverseFlg) {
						swap(d1, d2);
					}
					if (xpoints[i].a.y < y) {
						_vFixedSegs.emplace_back(Position2(xpoints[i].a.x, y), Position2(xpoints[i].a.x, xpoints[i].b.y), d1);
					}
					else {
						_vFixedSegs.emplace_back(xpoints[i].a,xpoints[i].b, d1);
					}
					if (xpoints[i + 1].a.y < y) {
						_vFixedSegs.emplace_back(Position2(xpoints[i+1].a.x, y), Position2(xpoints[i + 1].a.x, xpoints[i + 1].b.y), d2);
					}
					else {
						_vFixedSegs.emplace_back(xpoints[i+1].a,xpoints[i+1].b, d2);
					}
				}
				DrawBox(xpoints[i].a.x, y, xpoints[i+1].a.x, y + 1, 0xffaaaa, true);
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i+1].a.x, y, xpoints[i+1].a.x, y, abs(xpoints[i + 1].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, abs(xpoints[i + 1].a.x - xpoints[i].a.x), 1, rewardH, false);
				}
			}
		}
	}
}


void FillVirtualScreen(int area, std::list<Segment> &hSegments, std::list<Segment> &vSegments, bool &onTheFrame, std::list<Position2> &keypoints,bool reverseFlg=false);

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments);


bool IsIntersected(const Segment& lval, const Segment& rval) {
	bool lvertical = (lval.a.x == lval.b.x);
	bool rvertical = (rval.a.x == rval.b.x);
	if (lvertical == rvertical)return false;//���s�Ȃ̂ŃN���X���Ȃ�
	if (lvertical) {
		return (rval.a.x <= lval.a.x && lval.a.x <= rval.b.x) && 
			(lval.a.y <= rval.a.y && rval.a.y <= lval.b.y);
	}
	else {
		return (lval.a.x <= rval.a.x && rval.a.x <= lval.b.x) &&
			(rval.a.y <= lval.a.y && lval.a.y <= rval.b.y);
	}
}

bool IsIntersected(const Segment& seg, const std::list<Segment>& segments,Segment* exclusiveSeg=nullptr) {
	for (auto& s : segments) {
		if (exclusiveSeg!=nullptr && s == *exclusiveSeg)continue;
		if (IsIntersected(seg, s)) {
			return true;
		}
	}
	return false;
}

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

	Segment* baseSegment=nullptr;//�o���n�_�̃Z�O�����g

	int frame = 30;

	while (!ProcessMessage()) {
		
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]&&playerPos.y > play_area_top ) {
			auto pposy = max(play_area_top,playerPos.y -2);
			auto tmppos = playerPos - offset;
			auto tmpy = pposy - play_area_top;
			if (IsIntersected(Segment(tmppos, Position2(tmppos.x, tmpy)),hSegments, baseSegment)) {
				pposy = playerPos.y;
			}
			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				if (playerPos.y == play_area_bottom) {
					hSegments.push_back(bottomseg);
					baseSegment = &bottomseg;
				}

				
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
			auto tmpy = pposy - play_area_top;
			if (IsIntersected(Segment(tmppos, Position2(tmppos.x, tmpy)), hSegments, baseSegment)) {
				pposy = playerPos.y;
			}
			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				if (playerPos.y == play_area_top) {
					hSegments.push_back(topseg);
					baseSegment = &topseg;
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
					baseSegment = nullptr;
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
			auto tmpx = pposx - play_area_left;
			if (IsIntersected(Segment(tmppos, Position2(tmpx,tmppos.y)), vSegments, baseSegment)) {
				pposx = playerPos.x;
			}
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				if (playerPos.x == play_area_left) {
					vSegments.push_back(leftseg);
					baseSegment = &leftseg;
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
					baseSegment = nullptr;
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
			auto tmpx = pposx - play_area_left;
			if (IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)), vSegments, baseSegment)) {
				pposx = playerPos.x;
			}
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				if (playerPos.x == play_area_right) {
					vSegments.push_back(rightseg);
					baseSegment = &rightseg;
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
					baseSegment = nullptr;
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
		
		//���@�`��
		DxLib::DrawCircle(playerPos.x, playerPos.y, 3+3.0*float(abs(frame-30)/60.0f), 0xffff88, true);
		frame = (frame + 1) % 60;
		//�f�o�b�O�p
		//�m����`��
		for (auto& s : _hFixedSegs) {
			DrawLine(s.a.x+play_area_left, 
				s.a.y+ play_area_top, 
				s.b.x+play_area_left, 
				s.b.y+ play_area_top, 0xaaffaa, 1);
			DrawLine(s.a.x + play_area_left, 
				s.a.y + play_area_top+1,
				s.b.x + play_area_left,
				s.b.y + play_area_top+1, 0xff0000, 1);

			//����
			auto m = (s.a.x + s.b.x) / 2 + play_area_left;
			auto y1 = s.a.y + play_area_top;
			auto y2 = s.a.y + play_area_top+(s.inner==Direction::up?-10:10);
			DrawLine(m, y1,
				m,
				y2, 0xff0000, 1);
		}
		for (auto& s : _vFixedSegs) {
			DrawLine(s.a.x + play_area_left, 
				s.a.y + play_area_top,
				s.b.x + play_area_left,
				s.b.y + play_area_top, 0xaaffaa, 1);
			DrawLine(s.a.x + play_area_left, 
				s.a.y + play_area_top,
				s.b.x + play_area_left + (s.inner == Direction::left ? 1 : -1),
				s.b.y + play_area_top , 0xff0000, 1);

			//����
			auto m = (s.a.y + s.b.y) / 2 + play_area_top;
			auto x1 = s.a.x + play_area_left;
			auto x2 = s.a.x + play_area_left + (s.inner == Direction::left ? -10 : 10);
			DrawLine(x1, m,
				x2,
				m, 0xff0000, 1);
		}

		DrawDebugStatus(keypoints, hSegments, vSegments);
		DrawFormatString(512, 350, 0xffffff, "fps=%f", 1000.0f/static_cast<float>(GetTickCount() - count));
		count = GetTickCount();

		DxLib::ScreenFlip();
	}

	DxLib_End();


}

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments)
{
	//�f�o�b�O�\��
	DrawFormatString(512, 50, 0xffffff, "�_�̐�=%d", keypoints.size());
	DrawFormatString(512, 100, 0xffffff, "���m�艡��=%d", hSegments.size());
	DrawFormatString(512, 150, 0xffffff, "���m��c��=%d", vSegments.size());

	DrawFormatString(512, 200, 0xffffff, "�m�艡��=%d", _hFixedSegs.size());
	DrawFormatString(512, 250, 0xffffff, "�m��c��=%d", _vFixedSegs.size());

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
