#include<DxLib.h>
#include<vector>
#include<list>
#include<algorithm>
#include<cassert>
#include<iostream>
#include<random>
#include<sstream>
#include"Geometry.h"
#include"Debug.h"

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
	Segment(const Segment& seg, Direction dir = Direction::none) :a(seg.a), b(seg.b), inner(dir) {
		if (b.x < a.x) {
			swap(a, b);
		}
		else if (b.y < a.y) {
			swap(a, b);
		}
	}

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
	bool IsOn(const Position2& pos)const {
		return (a.y == pos.y && a.x <= pos.x && pos.x <= b.x) ||
			(a.x == pos.x && a.y <= pos.y && pos.y <= b.y);
	}
};

bool operator==(const Segment& lval,const Segment& rval) {
	return lval.a == rval.a && lval.b == rval.b;
}
int rewardH;

Position2 _lastTmpPos=Vector2(0,0);
std::list<Segment> _hFixedSegs;
std::list<Segment> _vFixedSegs;

void RegisterFixedVerticalSegments(Segment & seg,Direction dir);

void RegisterFixedSegment(int y, std::vector<Segment> &xpoints, int i, bool reverseFlg)
{
	//�m��ӂ�o�^
	if ((y == xpoints[i].a.y && y == xpoints[i + 1].a.y) || (y == xpoints[i].b.y && y == xpoints[i + 1].b.y ) ||//|�Q|��|�P|��
		(y == xpoints[i].a.y && (xpoints[i + 1].a.y < y&&y < xpoints[i + 1].b.y)) ||//|�P�ǂ�
		(y == xpoints[i + 1].a.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y)) || //�ǁP|��
		(y == xpoints[i].b.y && (xpoints[i + 1].a.y < y&&y < xpoints[i + 1].b.y)) || //|_�ǂ�
		(y == xpoints[i + 1].b.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y))) {//��_|
		if ((y == xpoints[i].b.y && y == xpoints[i + 1].b.y)|| //|�Q|
			(y == xpoints[i].b.y && (xpoints[i + 1].a.y<y&&y< xpoints[i + 1].b.y))||//|_��
			(y == xpoints[i+1].b.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y))) {//��_|
			_hFixedSegs.emplace_back(Position2(xpoints[i].b.x, y), Position2(xpoints[i + 1].b.x, y), Direction::down);
		}
		else {
			_hFixedSegs.emplace_back(Position2(xpoints[i].a.x, y), Position2(xpoints[i + 1].a.x, y), Direction::up);
		}
		

		Direction d1 = Direction::left, d2 = Direction::right;
		if (reverseFlg) {
			swap(d1, d2);
		}
		if ((y == xpoints[i].a.y && y == xpoints[i + 1].a.y) ||
			(y == xpoints[i].b.y && y == xpoints[i + 1].b.y)) {//|�P|��|_|
			RegisterFixedVerticalSegments(xpoints[i], d1);
			RegisterFixedVerticalSegments(xpoints[i + 1], d2);
		}
		else if(y == xpoints[i].a.y){//|�P��
			RegisterFixedVerticalSegments(xpoints[i], d1);
		}
		else if (y == xpoints[i + 1].a.y) {//�ǁP|
			RegisterFixedVerticalSegments(xpoints[i+1], d2);
		}
		else if (y == xpoints[i].b.y) {//|_��
			RegisterFixedVerticalSegments(xpoints[i], d1);
		}
		else if (y == xpoints[i + 1].b.y) {//��_|
			RegisterFixedVerticalSegments(xpoints[i+1], d2);
		}
	}
}

void RegisterFixedVerticalSegments(Segment& seg, Direction dir){
	if (count(_vFixedSegs.begin(),_vFixedSegs.end(),seg)==0) {
		_vFixedSegs.emplace_back(seg, dir);
	}
}

bool 
OnTheFrame(const Position2& pos){
	//�c�Ӄ`�F�b�N
	auto vcount = count_if(_vFixedSegs.begin(), _vFixedSegs.end(), [pos](const Segment& seg)->bool {
		return seg.IsOn(pos);
	});
	if (vcount > 0)return true;
	//���Ӄ`�F�b�N
	auto hcount = count_if(_hFixedSegs.begin(), _hFixedSegs.end(), [pos](const Segment& seg)->bool {
		return seg.IsOn(pos);
	});
	if (hcount > 0)return true;
	return false;
}

///�h��Ԃ��͈͂�Ԃ�
///@param hSegs ������
///@param vSegs ������
///@remarks ���ꂼ��̃Z�O�����g�̓\�[�g�ς�(�と��)(�����E)�̂��̂Ƃ���B
void FillRange(std::list<Segment>& hSegs, std::list<Segment>& vSegs,Direction finalNormalDir , bool reverseFlg = false,bool debugDraw=false) {
	hSegs.sort([](const Segment& lval, const Segment& rval)->bool {
		return lval.a.y < rval.a.y;
	});
	vSegs.sort([](const Segment& lval, const Segment& rval)->bool {
		return lval.a.x < rval.a.x;
	});

	auto minVSeg = vSegs.front();
	auto maxVSeg = vSegs.back();
	//�ő�ŏ��̊ԂɊm��c�ӂ�����΁A�����ǉ�
	auto it = _vFixedSegs.begin();
	do{
		it = find_if(it, _vFixedSegs.end(), [finalNormalDir,minVSeg, maxVSeg](const Segment& seg)->bool {
			return finalNormalDir == seg.inner;
			//return (minVSeg.a.x < seg.a.x && seg.a.x < maxVSeg.a.x) && min(minVSeg.a.y, maxVSeg.a.y) <= seg.a.y && seg.b.y <= max(minVSeg.b.y, maxVSeg.b.y);
		}
		);
		if (it == _vFixedSegs.end()) {
			break;
		}
		vSegs.push_back(*it);
		++it;
	}while (it != _vFixedSegs.end());


	vSegs.sort([](const Segment& lval, const Segment& rval)->bool {
		return lval.a.x < rval.a.x;
	});
	if (reverseFlg) {
		vSegs.reverse();
	}

	auto hIt = hSegs.begin();
	auto top = hIt->a.y;
	auto bottom = hSegs.back().a.y;
	for (int y = top; y <= bottom;++y) {
		std::vector<Segment> xpoints;
		copy_if(vSegs.begin(), vSegs.end(), back_inserter(xpoints), [y](const Segment& vseg)->bool {
			return vseg.a.y <= y&&y <= vseg.b.y;
		});
		auto drawcount= xpoints.size();
		//assert(drawcount > 1);
		for (int i = 0; i < drawcount-1; i+=2) {
			//��ӂ������͉��ӂł���A���̌������ɂ������_������ꍇ(�������̌����_�܂ł�h��Ԃ�)
			//�� �Q|_ _ _ _|�����܂œh��Ԃ��Ώ�
			//  |�@        |
			//  �����������ꍇ
			if ((i + 2) < drawcount&&
				((y == xpoints[i].a.y &&	y == xpoints[i + 1].b.y ) || 
				(y == xpoints[i].b.y && y == xpoints[i + 1].a.y))) {
				//�m��ӂ�o�^
				if ((y == xpoints[i].a.y&&y == xpoints[i+1].b.y)||(y == xpoints[i].b.y&&y == xpoints[i + 1].a.y)) {
					Direction d1 = Direction::left, d2 = Direction::right;
					if (reverseFlg) {
						swap(d1, d2);
					}
					// |�� 
					//  �P|
					if (y == xpoints[i].b.y && y == xpoints[i + 1].a.y) {
						_hFixedSegs.emplace_back(Position2(xpoints[i].b.x, y), Position2(xpoints[i + 1].b.x, y), Direction::down);
						//��d�o�^�̖h�~
						RegisterFixedVerticalSegments(xpoints[i], d1);
						RegisterFixedVerticalSegments(xpoints[i + 1], d1);
					}
					//    |
					// |�P��
					else if (y == xpoints[i].a.y && y == xpoints[i + 1].b.y) {
						_hFixedSegs.emplace_back(xpoints[i].a, xpoints[i + 1].b, Direction::up);
						RegisterFixedVerticalSegments(xpoints[i], d1);
						RegisterFixedVerticalSegments(xpoints[i + 1], d1);
					}
				}

				auto w = abs(xpoints[i + 2].a.x - xpoints[i].a.x);
				//assert(w > 10);
				//DrawBox(xpoints[i].a.x, y, xpoints[i+2].a.x, y + 1, 0xffaaaa, true);
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i+2].a.x, y, xpoints[i+2].a.x, y, w, 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, w, 1, rewardH, false);
				}
				if (debugDraw) {
					DrawBox(xpoints[i].a.x, y, xpoints[i + 2].a.x, y + 1, 0xffaaaa, true);
				}
				++i;//����܂���(���̂ނ����܂œh��Ԃ��Ă���̂ŃJ�E���g��i�߂�)
			}
			//��̂�Ƌt�p�^�[��(�\�[�g���Ă邩��t�̏ꍇ����ŏI����Ă��܂��c)
			//|_ _ _ _|�Q���@
			//|�@        |
			//�����������ꍇ
			else if ((i + 2) < drawcount&&
				((y == xpoints[i + 1].a.y && y == xpoints[i + 2].b.y) || 
				 (y == xpoints[i + 1].b.y && y == xpoints[i + 2].a.y))) {
				
				//�m��ӂ�o�^
				if ((y == xpoints[i+1].a.y&&y == xpoints[i + 2].b.y) || (y == xpoints[i+1].b.y&&y == xpoints[i + 2].a.y)) {
					Direction d1 = Direction::left, d2 = Direction::right;
					if (reverseFlg) {
						swap(d1, d2);
					}
					//   |
					// |�P
					if (y == xpoints[i+1].a.y && y == xpoints[i + 2].b.y) {
						_hFixedSegs.emplace_back(Position2(xpoints[i+1].b.x, y), Position2(xpoints[i + 2].b.x, y), Direction::down);
						RegisterFixedVerticalSegments(xpoints[i + 1], d2);
						RegisterFixedVerticalSegments(xpoints[i + 2], d2);
					}
					//|  
					// �P|
					else if (y == xpoints[i+1].b.y && y == xpoints[i + 2].a.y) {
						_hFixedSegs.emplace_back(xpoints[i+1].b, xpoints[i + 2].a, Direction::up);
						RegisterFixedVerticalSegments(xpoints[i+1], d2);
						RegisterFixedVerticalSegments(xpoints[i + 2], d2);
					}
				}

				auto w = abs(xpoints[i + 2].a.x - xpoints[i].a.x);
				//assert(w > 10);
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i + 2].a.x, y, xpoints[i + 2].a.x, y, w, 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, w, 1, rewardH, false);
				}
				if (debugDraw) {
					if (reverseFlg) {
						DrawBox(xpoints[i + 2].a.x, y, xpoints[i].a.x, y + 1, 0xffaaaa, true);
					}
					else {
						DrawBox(xpoints[i].a.x, y, xpoints[i + 2].a.x, y + 1, 0xffaaaa, true);
					}
				}
				++i;//����܂���(���̂ނ����܂œh��Ԃ��Ă���̂ŃJ�E���g��i�߂�)
			}
			else {//�ʏ�h��
				RegisterFixedSegment(y, xpoints, i, reverseFlg);
				auto w = abs(xpoints[i + 1].a.x - xpoints[i].a.x);
				//assert(w > 10);
				
				if (reverseFlg) {
					DxLib::DrawRectGraph(xpoints[i+1].a.x, y, xpoints[i+1].a.x, y, w, 1, rewardH, false);
				}
				else {
					DxLib::DrawRectGraph(xpoints[i].a.x, y, xpoints[i].a.x, y, w, 1, rewardH, false);
				}

				if (debugDraw) {
					DrawBox(xpoints[i].a.x, y, xpoints[i + 1].a.x, y + 1, 0xffaaaa, true);
				}
			}
		}
	}
}


void FillVirtualScreen(int area, std::list<Segment> &hSegments, std::list<Segment> &vSegments,Direction finalNormalDir, bool &onTheFrame, std::list<Position2> &keypoints,bool reverseFlg=false);

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments);


bool IsIntersected(const Segment& lval, const Segment& rval) {
	if (lval.a == lval.b||rval.a==rval.b)return false;
	
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
	if (seg.a == seg.b)return false;
	for (auto& s : segments) {
		if (exclusiveSeg!=nullptr && s == *exclusiveSeg)continue;
		if (IsIntersected(seg, s)) {
			return true;
		}
	}
	return false;
}


bool IsIntersected(const Segment& seg, const std::list<Segment>& segments, std::list<Segment>::const_iterator& out, Segment* exclusiveSeg = nullptr) {
	if (seg.a == seg.b)return false;
	auto s = segments.begin();
	for (s; s != segments.end(); ++s) {
		if (exclusiveSeg != nullptr && *s == *exclusiveSeg)continue;
		if (IsIntersected(seg, *s)) {
			out = s;
			return true;
		}
	}
	out = segments.end();
	return false;
}

void DecideBaseSegmentToRightLeftDirection(Position2& pos, Direction dir,Segment& outerSegment,std::list<Segment>& floatingSegments, Segment*& baseSegment) {
	//�ǂ����̌Œ�ǂ���o�������̂��H
	if (pos.x == 0||pos.x==play_area_width) {//���E��
		floatingSegments.push_back(outerSegment);
		baseSegment = &outerSegment;
	}
	else {//�E�[�łȂ��Ȃ�A�ǂ����̊m��ӁH
		auto s = Segment(pos,_lastTmpPos);
		auto it = find_if(_vFixedSegs.begin(), _vFixedSegs.end(), [s,dir](const Segment& seg) {
			return seg.inner == dir && IsIntersected(s, seg); //&&  seg.a.x == pos.x && seg.a.y <= pos.y && pos.y <= seg.b.y;
		});
		if (it != _vFixedSegs.end()) {
			floatingSegments.push_back(*it);
			baseSegment = &(*it);
		}
	}
}

void DecideBaseSegmentToUpDownDirection(Position2& pos, Direction dir, Segment& outerSegment, std::list<Segment>& floatingSegments, Segment*& baseSegment) {
	//�ǂ����̌Œ�ǂ���o�������̂��H
	if (pos.y == 0 || pos.y == play_area_height) {//�㉺��
		floatingSegments.push_back(outerSegment);
		baseSegment = &outerSegment;
	}
	else {//�E�[�łȂ��Ȃ�A�ǂ����̊m��ӁH
		auto it = find_if(_hFixedSegs.begin(), _hFixedSegs.end(), [pos, dir](const Segment& seg) {
			return seg.inner == dir && seg.a.y == pos.y && seg.a.x <= pos.x && pos.x <= seg.b.x;
		});
		if (it != _hFixedSegs.end()) {
			floatingSegments.push_back(*it);
			baseSegment = &(*it);
		}
	}
}

void DrawDebugLines() {
	//����
	for (auto& s : _hFixedSegs) {
		DrawLine(s.a.x + play_area_left,
			s.a.y + play_area_top,
			s.b.x + play_area_left,
			s.b.y + play_area_top, 0xaaffaa, 1);
		if (s.inner != Direction::none) {
			DrawLine(s.a.x + play_area_left,
				s.a.y + play_area_top + (s.inner == Direction::up ? 1 : -1),
				s.b.x + play_area_left,
				s.b.y + play_area_top + (s.inner == Direction::up ? 1 : -1),
				0xff0000, 1);
		}

		//����
		auto m = (s.a.x + s.b.x) / 2 + play_area_left;
		auto y1 = s.a.y + play_area_top;
		auto y2 = s.a.y + play_area_top + (s.inner == Direction::up ? -10 : 10);
		DrawLine(m, y1,
			m,
			y2, 0xff0000, 1);
	}
	//�c��
	for (auto& s : _vFixedSegs) {
		DrawLine(s.a.x + play_area_left,
			s.a.y + play_area_top,
			s.b.x + play_area_left,
			s.b.y + play_area_top, 0xaaffaa, 1);
		if (s.inner != Direction::none) {
			DrawLine(s.a.x + play_area_left + (s.inner == Direction::left ? 1 : -1),
				s.a.y + play_area_top,
				s.b.x + play_area_left + (s.inner == Direction::left ? 1 : -1),
				s.b.y + play_area_top, 0xff0000, 1);
		}

		//����
		auto m = (s.a.y + s.b.y) / 2 + play_area_top;
		auto x1 = s.a.x + play_area_left;
		auto x2 = s.a.x + play_area_left + (s.inner == Direction::left ? -10 : 10);
		DrawLine(x1, m,
			x2,
			m, 0xff0000, 1);
	}

}

void 
DrawDebugInfo(list<Position2> &keypoints, 
	list<Segment> &hSegments, 
	list<Segment> &vSegments, 
	const DWORD &count) {
	//�m����`��
	DrawDebugLines();
	DrawDebugStatus(keypoints, hSegments, vSegments);
	DrawFormatString(512, 350, 0xffffff, "fps=%f", 1000.0f / static_cast<float>(GetTickCount() - count));

}

void DrawPlayer(bool onTheFrame, 
	Segment * baseSegment, 
	Position2 &playerPos, 
	int frame) {
	if (!onTheFrame && baseSegment == nullptr) {//�����t���[����ɂȂ��x�[�X�����Ȃ��Ȃ�o�O
		DxLib::DrawCircle(playerPos.x, playerPos.y,
			3 + 3.0*float(abs(frame - 30) / 60.0f),
			0xff0000,
			true);
	}
	else {
		DxLib::DrawCircle(playerPos.x, playerPos.y,
			3 + 3.0*float(abs(frame - 30) / 60.0f),
			onTheFrame ? 0xaaffaa : 0xffff88,
			true);
	}
}

void 
DrawOuterFrame(Vector2 &offset) {
	DxLib::DrawBox(offset.x - 1, offset.y - 1, offset.x + 451, offset.y + 401, 0xffffff, false);
	DxLib::DrawBox(offset.x, offset.y, offset.x + play_area_width, offset.y + play_area_height, 0xffffff, false);
}

void 
DrawPlayArea(int area, 
	Position2 &playerPos) {
	DxLib::SetDrawScreen(area);
	auto tmpPos = playerPos - Vector2(play_area_left, play_area_top);
	//�O�Չ��`��
	DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);

	/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
	DxLib::SetDrawScreen(DX_SCREEN_BACK);


}

void 
DrawGame(Vector2 &offset, int area, Position2 &playerPos, 
	Segment * baseSegment, 	int frame, 
	list<Position2> &keypoints, list<Segment> &hSegments, 
	list<Segment> &vSegments, DWORD &count) {
	//�g�`��
	DrawOuterFrame(offset);

	//�h��Ԃ��̈�̕`��
	DrawPlayArea(area, playerPos);

	//�O�Օ`��
	DxLib::DrawGraph(play_area_left, play_area_top, area, true);

	//���@�`��
	DrawPlayer(OnTheFrame, baseSegment, playerPos, frame);

	//�f�o�b�O�p
	DrawDebugInfo(keypoints, hSegments, vSegments, count);

}

void LoopEndProcess(Vector2 &offset, int area, 
	Position2 &playerPos, Segment * baseSegment, 
	int &frame, list<Position2> &keypoints, list<Segment> &hSegments, 
	list<Segment> &vSegments, DWORD &count) {
	DrawGame(offset, area, playerPos, baseSegment, frame, keypoints, hSegments, vSegments, count);
	frame = (frame + 1) % 60;
	count = GetTickCount();
	DxLib::ScreenFlip();

}

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR,int){ 
#endif
	cout << "Application Start" << endl;

	
	ChangeWindowMode(true);
	DxLib::SetWindowText("QIX 000000_��엳��");
	DxLib::SetGraphMode(scr_w+200, scr_h, 32);//�f�o�b�O�p+200
	DxLib_Init();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
	ostringstream oss;
	mt19937 rnd;
	rnd.seed(GetTickCount());
	oss << "img/reward" << rnd() % 8 + 1 << ".jpg";
	rewardH = DxLib::LoadGraph(oss.str().c_str());

	DrawString(100, 100, "Hello World", 0xffffff); 
	Position2 playerPos(play_area_left + (play_area_right-play_area_left) / 2,play_area_bottom);//
	auto offset=Vector2(play_area_left, play_area_top);
	char keystate[256];

	auto area = MakeScreen(play_area_right-play_area_left,
		play_area_bottom-play_area_top, 
		true);
	
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
	Segment topseg(0,0,play_area_width,0,Direction::down);
	Segment bottomseg(0, play_area_height, play_area_width, play_area_height,Direction::up);
	Segment leftseg(0, 0, 0, play_area_height,Direction::right);
	Segment rightseg(play_area_width, 0, play_area_width, play_area_height,Direction::left);

	_vFixedSegs.emplace_back(rightseg,rightseg.inner);
	_vFixedSegs.emplace_back(leftseg,leftseg.inner);
	_hFixedSegs.emplace_back(topseg,topseg.inner);
	_hFixedSegs.emplace_back(bottomseg,bottomseg.inner);

	bool onTheFrame = true;//�O�g�̏�ɂ��邩�H

	auto count = GetTickCount();

	Segment* baseSegment=nullptr;//�o���n�_�̃Z�O�����g
	bool _lastOnTheFrame = false;
	int frame = 30;
	auto tmppos = playerPos - offset;
	while (!ProcessMessage()) {
		_lastTmpPos = tmppos;
		tmppos = playerPos - offset;
		_lastOnTheFrame = onTheFrame;
		onTheFrame = OnTheFrame(tmppos);
		ClearDrawScreen();
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]&&playerPos.y > play_area_top ) {
			auto pposy = max(play_area_top,playerPos.y -2);
			
			auto tmpy = pposy - play_area_top;
			if (IsIntersected(Segment(tmppos, Position2(tmppos.x, tmpy)),hSegments, baseSegment)) {
				pposy = playerPos.y;
			}
			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				//�o���n�_���Ǐ���������c�܂�
				DecideBaseSegmentToUpDownDirection(tmppos,Direction::up,bottomseg,hSegments,baseSegment );
				
				//��ǂɓ���������
				if (!onTheFrame&&pposy == play_area_top) {
					//FillConvex����
					hSegments.push_back(topseg);
					vSegments.emplace_back(keypoints.back(), tmppos);
					if (hSegments.back()==topseg && hSegments.front()==bottomseg) {
						vSegments.push_back(rightseg);//�Ƃ肠�����E��
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments,Direction::down ,onTheFrame, keypoints);
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
			//auto tmppos = playerPos - offset;
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
					vSegments.emplace_back(keypoints.back(), Position2(tmppos.x,tmpy));
					if (hSegments.back() == bottomseg && hSegments.front() == topseg) {
						vSegments.push_back(leftseg);//�Ƃ肠��������
					}
					//�̈�h��Ԃ�
					FillVirtualScreen(area, hSegments, vSegments,  Direction::up, onTheFrame, keypoints);
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
			auto tmpx = pposx - play_area_left;
			auto diff = keypoints.empty()?Vector2(0,0):(tmppos - keypoints.back());
			if (diff.y == 0 && diff.x < 0) {//�����Ԃ��s��
				LoopEndProcess(offset, area, playerPos, baseSegment, frame,
					keypoints, hSegments, vSegments, count);
				continue;
			}
			if (IsIntersected(Segment(tmppos, Position2(tmpx,tmppos.y)), vSegments, baseSegment)) {
				pposx = playerPos.x;
			}
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				//�����Œ�ǂ���o�����Ă���baseSegment���m�肷��
				DecideBaseSegmentToRightLeftDirection(tmppos,Direction::right, leftseg, vSegments, baseSegment);


				//�E��(�������͌Œ��)�ɓ���������
				std::list<Segment>::const_iterator cit = _vFixedSegs.end();
				if ( IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)),
						_vFixedSegs,cit/*,baseSegment*/)) {
					assert(cit != _vFixedSegs.end());
					if (cit->inner == Direction::left) {
						pposx = cit->a.x+play_area_left;//�z��
						if (!onTheFrame) {
							vSegments.push_back(*cit);
						}
					}
					if (!onTheFrame) {
						onTheFrame = true;
						hSegments.emplace_back(keypoints.back(), tmppos);
						if (baseSegment == nullptr) {
							Debug::WriteLine(__FILE__, __LINE__, "baseSegment is nullptr");
							LoopEndProcess(offset, area, playerPos, baseSegment, frame,
								keypoints, hSegments, vSegments, count);
							continue;
						}
						//�����E�ƍ����Ȃ��悤�Ȑ��������Ă���ꍇ�A�x�[�X���ǂ��ɂ���̂����m�肳������
						auto baseIt = find_if(_vFixedSegs.begin(), _vFixedSegs.end(), [baseSegment](const Segment& seg)->bool {
							return seg == *baseSegment;
						});

						if (vSegments.back() == rightseg && (vSegments.front() == leftseg || (baseIt!=_vFixedSegs.end() &&vSegments.front() == *baseIt))) {
							//�Ƃ肠������
							hSegments.push_back(topseg);
						}
						//�̈�h��Ԃ�
						FillVirtualScreen(area, hSegments, vSegments,Direction::left, onTheFrame, keypoints);
						baseSegment = nullptr;
					}
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
		}else if (keystate[KEY_INPUT_LEFT] && playerPos.x > play_area_left) {
			
			auto pposx = max(play_area_left , playerPos.x - 2);
			//auto tmppos = playerPos - offset;
			auto tmpx = pposx - play_area_left;
			if (IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)), vSegments, baseSegment)) {
				pposx = playerPos.x;
			}
			if (!onTheFrame && playerPos.y > play_area_top && playerPos.y < play_area_bottom) {

				DecideBaseSegmentToRightLeftDirection(tmppos, Direction::left, rightseg, vSegments, baseSegment);

				std::list<Segment>::const_iterator cit = _vFixedSegs.end();
				//����(�������͌Œ��)�ɓ���������
				if (pposx == play_area_left || 
					IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)),
						_vFixedSegs, cit, baseSegment)) {
					if (cit == _vFixedSegs.end()) {
						vSegments.push_back(leftseg);
					}
					else {
						if (cit->inner == Direction::right) {
							pposx = cit->a.x + play_area_left;//�z��
							if (!onTheFrame) {
								vSegments.push_back(*cit);
							}
						}
					}
					if (!onTheFrame) {
						onTheFrame = true;
						hSegments.emplace_back(keypoints.back(), tmppos);


						//�����E�ƍ����Ȃ��悤�Ȑ��������Ă���ꍇ�A�x�[�X���ǂ��ɂ���̂����m�肳������
						auto baseIt = find_if(_vFixedSegs.begin(), _vFixedSegs.end(), [baseSegment](const Segment& seg)->bool {
							return seg == *baseSegment;
						});

						if (vSegments.back() == leftseg && (vSegments.front() == rightseg || ((baseIt != _vFixedSegs.end() && vSegments.front() == *baseIt)))) {
							//�Ƃ肠������
							hSegments.push_back(bottomseg);
						}
						//�̈�h��Ԃ�
						FillVirtualScreen(area, hSegments, vSegments, Direction::right, onTheFrame, keypoints, true);
						baseSegment = nullptr;
					}
				}
				else {

					if ((!onTheFrame&&_lastOnTheFrame) || lastDirection == Direction::up || lastDirection == Direction::down) {

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
		}
		LoopEndProcess(offset,area,playerPos,baseSegment,frame,
			keypoints,hSegments,vSegments,count);

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

void FillVirtualScreen(int area, list<Segment> &hSegments, list<Segment> &vSegments,Direction finalNormalDir, bool &onTheFrame, list<Position2> &keypoints, bool reverseFlg )
{
	//����ʂ�h��Ԃ�
	DxLib::SetDrawScreen(area);
	bool debugDraw = false;
	if (CheckHitKey(KEY_INPUT_M)) {//�f�o�b�O�p
		debugDraw = true;
	}
	FillRange(hSegments, vSegments,finalNormalDir,reverseFlg,debugDraw);
	onTheFrame = true;
	//�㏈��
	keypoints.clear();
	vSegments.clear();
	hSegments.clear();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
}
