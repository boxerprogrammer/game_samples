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
	none,//方向未定
	up,//上
	down,//下
	left,//左
	right//右
};

//縦と横しかない線分情報
struct Segment {
	Position2 a;//必ず左か上
	Position2 b;//必ず右か下
	Direction inner;//セグメントの内側方向
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
	//確定辺を登録
	if ((y == xpoints[i].a.y && y == xpoints[i + 1].a.y) || (y == xpoints[i].b.y && y == xpoints[i + 1].b.y ) ||//|＿|か|￣|か
		(y == xpoints[i].a.y && (xpoints[i + 1].a.y < y&&y < xpoints[i + 1].b.y)) ||//|￣壁か
		(y == xpoints[i + 1].a.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y)) || //壁￣|か
		(y == xpoints[i].b.y && (xpoints[i + 1].a.y < y&&y < xpoints[i + 1].b.y)) || //|_壁か
		(y == xpoints[i + 1].b.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y))) {//壁_|
		if ((y == xpoints[i].b.y && y == xpoints[i + 1].b.y)|| //|＿|
			(y == xpoints[i].b.y && (xpoints[i + 1].a.y<y&&y< xpoints[i + 1].b.y))||//|_壁
			(y == xpoints[i+1].b.y && (xpoints[i].a.y < y&&y < xpoints[i].b.y))) {//壁_|
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
			(y == xpoints[i].b.y && y == xpoints[i + 1].b.y)) {//|￣|か|_|
			RegisterFixedVerticalSegments(xpoints[i], d1);
			RegisterFixedVerticalSegments(xpoints[i + 1], d2);
		}
		else if(y == xpoints[i].a.y){//|￣壁
			RegisterFixedVerticalSegments(xpoints[i], d1);
		}
		else if (y == xpoints[i + 1].a.y) {//壁￣|
			RegisterFixedVerticalSegments(xpoints[i+1], d2);
		}
		else if (y == xpoints[i].b.y) {//|_壁
			RegisterFixedVerticalSegments(xpoints[i], d1);
		}
		else if (y == xpoints[i + 1].b.y) {//壁_|
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
	//縦辺チェック
	auto vcount = count_if(_vFixedSegs.begin(), _vFixedSegs.end(), [pos](const Segment& seg)->bool {
		return seg.IsOn(pos);
	});
	if (vcount > 0)return true;
	//横辺チェック
	auto hcount = count_if(_hFixedSegs.begin(), _hFixedSegs.end(), [pos](const Segment& seg)->bool {
		return seg.IsOn(pos);
	});
	if (hcount > 0)return true;
	return false;
}

///塗りつぶし範囲を返す
///@param hSegs 水平辺
///@param vSegs 垂直辺
///@remarks それぞれのセグメントはソート済み(上→下)(左→右)のものとする。
void FillRange(std::list<Segment>& hSegs, std::list<Segment>& vSegs,Direction finalNormalDir , bool reverseFlg = false,bool debugDraw=false) {
	hSegs.sort([](const Segment& lval, const Segment& rval)->bool {
		return lval.a.y < rval.a.y;
	});
	vSegs.sort([](const Segment& lval, const Segment& rval)->bool {
		return lval.a.x < rval.a.x;
	});

	auto minVSeg = vSegs.front();
	auto maxVSeg = vSegs.back();
	//最大最小の間に確定縦辺があれば、それを追加
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
			//上辺もしくは下辺であり、その向こうにも交差点がある場合(向こうの交差点までを塗りつぶす)
			//→ ＿|_ _ _ _|ここまで塗りつぶし対象
			//  |　        |
			//  ↑こういう場合
			if ((i + 2) < drawcount&&
				((y == xpoints[i].a.y &&	y == xpoints[i + 1].b.y ) || 
				(y == xpoints[i].b.y && y == xpoints[i + 1].a.y))) {
				//確定辺を登録
				if ((y == xpoints[i].a.y&&y == xpoints[i+1].b.y)||(y == xpoints[i].b.y&&y == xpoints[i + 1].a.y)) {
					Direction d1 = Direction::left, d2 = Direction::right;
					if (reverseFlg) {
						swap(d1, d2);
					}
					// |← 
					//  ￣|
					if (y == xpoints[i].b.y && y == xpoints[i + 1].a.y) {
						_hFixedSegs.emplace_back(Position2(xpoints[i].b.x, y), Position2(xpoints[i + 1].b.x, y), Direction::down);
						//二重登録の防止
						RegisterFixedVerticalSegments(xpoints[i], d1);
						RegisterFixedVerticalSegments(xpoints[i + 1], d1);
					}
					//    |
					// |￣←
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
				++i;//消費しました(そのむこうまで塗りつぶしているのでカウントを進める)
			}
			//上のやつと逆パターン(ソートしてるから逆の場合左二つで終わってしまう…)
			//|_ _ _ _|＿←　
			//|　        |
			//↑こういう場合
			else if ((i + 2) < drawcount&&
				((y == xpoints[i + 1].a.y && y == xpoints[i + 2].b.y) || 
				 (y == xpoints[i + 1].b.y && y == xpoints[i + 2].a.y))) {
				
				//確定辺を登録
				if ((y == xpoints[i+1].a.y&&y == xpoints[i + 2].b.y) || (y == xpoints[i+1].b.y&&y == xpoints[i + 2].a.y)) {
					Direction d1 = Direction::left, d2 = Direction::right;
					if (reverseFlg) {
						swap(d1, d2);
					}
					//   |
					// |￣
					if (y == xpoints[i+1].a.y && y == xpoints[i + 2].b.y) {
						_hFixedSegs.emplace_back(Position2(xpoints[i+1].b.x, y), Position2(xpoints[i + 2].b.x, y), Direction::down);
						RegisterFixedVerticalSegments(xpoints[i + 1], d2);
						RegisterFixedVerticalSegments(xpoints[i + 2], d2);
					}
					//|  
					// ￣|
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
				++i;//消費しました(そのむこうまで塗りつぶしているのでカウントを進める)
			}
			else {//通常塗り
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
	if (lvertical == rvertical)return false;//平行なのでクロスしない
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
	//どこかの固定壁から出発したのか？
	if (pos.x == 0||pos.x==play_area_width) {//左右壁
		floatingSegments.push_back(outerSegment);
		baseSegment = &outerSegment;
	}
	else {//右端でないなら、どこかの確定辺？
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
	//どこかの固定壁から出発したのか？
	if (pos.y == 0 || pos.y == play_area_height) {//上下壁
		floatingSegments.push_back(outerSegment);
		baseSegment = &outerSegment;
	}
	else {//右端でないなら、どこかの確定辺？
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
	//横線
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

		//方向
		auto m = (s.a.x + s.b.x) / 2 + play_area_left;
		auto y1 = s.a.y + play_area_top;
		auto y2 = s.a.y + play_area_top + (s.inner == Direction::up ? -10 : 10);
		DrawLine(m, y1,
			m,
			y2, 0xff0000, 1);
	}
	//縦線
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

		//方向
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
	//確定線描画
	DrawDebugLines();
	DrawDebugStatus(keypoints, hSegments, vSegments);
	DrawFormatString(512, 350, 0xffffff, "fps=%f", 1000.0f / static_cast<float>(GetTickCount() - count));

}

void DrawPlayer(bool onTheFrame, 
	Segment * baseSegment, 
	Position2 &playerPos, 
	int frame) {
	if (!onTheFrame && baseSegment == nullptr) {//もしフレーム上になくベース線もないならバグ
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
	//軌跡仮描画
	DxLib::DrawBox(tmpPos.x - 1, tmpPos.y - 1, tmpPos.x + 1, tmpPos.y + 1, 0xffffff, true);

	/*DrawCircle(playerPos.x- play_areaX, playerPos.y- play_areaY, 1, 0xffffff, true);*/
	DxLib::SetDrawScreen(DX_SCREEN_BACK);


}

void 
DrawGame(Vector2 &offset, int area, Position2 &playerPos, 
	Segment * baseSegment, 	int frame, 
	list<Position2> &keypoints, list<Segment> &hSegments, 
	list<Segment> &vSegments, DWORD &count) {
	//枠描画
	DrawOuterFrame(offset);

	//塗りつぶし領域の描画
	DrawPlayArea(area, playerPos);

	//軌跡描画
	DxLib::DrawGraph(play_area_left, play_area_top, area, true);

	//自機描画
	DrawPlayer(OnTheFrame, baseSegment, playerPos, frame);

	//デバッグ用
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
	DxLib::SetWindowText("QIX 000000_川野竜一");
	DxLib::SetGraphMode(scr_w, scr_h, 32);//デバッグ用
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
	
	
	Direction lastDirection = Direction::up;
	std::list<Position2> keypoints;

	std::list<Segment> hSegments;//水平方向線分
	std::list<Segment> vSegments;//垂直方向線分
	
	//上下左右セグメント
	Segment topseg(0,0,play_area_width,0,Direction::down);
	Segment bottomseg(0, play_area_height, play_area_width, play_area_height,Direction::up);
	Segment leftseg(0, 0, 0, play_area_height,Direction::right);
	Segment rightseg(play_area_width, 0, play_area_width, play_area_height,Direction::left);

	_vFixedSegs.emplace_back(rightseg,rightseg.inner);
	_vFixedSegs.emplace_back(leftseg,leftseg.inner);
	_hFixedSegs.emplace_back(topseg,topseg.inner);
	_hFixedSegs.emplace_back(bottomseg,bottomseg.inner);

	bool onTheFrame = true;//外枠の上にいるか？

	auto count = GetTickCount();

	Segment* baseSegment=nullptr;//出発地点のセグメント
	bool _lastOnTheFrame = false;
	int frame = 30;
	auto tmppos = playerPos - offset;
	char lastKeystate[256] = {};
	while (!ProcessMessage()) {
		_lastTmpPos = tmppos;
		tmppos = playerPos - offset;
		_lastOnTheFrame = onTheFrame;
		onTheFrame = OnTheFrame(tmppos);
		ClearDrawScreen();
		copy(begin(keystate),end(keystate),begin(lastKeystate));
		DxLib::GetHitKeyStateAll(keystate);
		if (keystate[KEY_INPUT_UP]&&playerPos.y > play_area_top ) {
			auto pposy = max(play_area_top,playerPos.y -2);
			
			auto tmpy = pposy - play_area_top;
			if (IsIntersected(Segment(tmppos, Position2(tmppos.x, tmpy)),hSegments, baseSegment)) {
				pposy = playerPos.y;
			}
			if (playerPos.x > play_area_left && playerPos.x < play_area_right) {
				//出発地点が壁床だったら…つまり
				DecideBaseSegmentToUpDownDirection(tmppos,Direction::up,bottomseg,hSegments,baseSegment );
				
				//上壁に当たった時
				if (!onTheFrame&&pposy == play_area_top) {
					//FillConvex準備
					hSegments.push_back(topseg);
					vSegments.emplace_back(keypoints.back(), tmppos);
					if (hSegments.back()==topseg && hSegments.front()==bottomseg) {
						vSegments.push_back(rightseg);//とりあえず右を
					}
					//領域塗りつぶし
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
				//下壁に当たった時
				if (!onTheFrame&&pposy == play_area_bottom) {
					//FillConvex準備
					hSegments.push_back(bottomseg);
					vSegments.emplace_back(keypoints.back(), Position2(tmppos.x,tmpy));
					if (hSegments.back() == bottomseg && hSegments.front() == topseg) {
						vSegments.push_back(leftseg);//とりあえず左を
					}
					//領域塗りつぶし
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
			if (diff.y == 0 && diff.x < 0) {//引き返し不可
				LoopEndProcess(offset, area, playerPos, baseSegment, frame,
					keypoints, hSegments, vSegments, count);
				continue;
			}
			if (IsIntersected(Segment(tmppos, Position2(tmpx,tmppos.y)), vSegments, baseSegment)) {
				pposx = playerPos.x;
			}
			if (playerPos.y > play_area_top && playerPos.y < play_area_bottom) {
				//もし固定壁から出発してたらbaseSegmentを確定する
				DecideBaseSegmentToRightLeftDirection(tmppos,Direction::right, leftseg, vSegments, baseSegment);


				//右壁(もしくは固定辺)に当たった時
				std::list<Segment>::const_iterator cit = _vFixedSegs.end();
				if ( IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)),
						_vFixedSegs,cit/*,baseSegment*/)) {
					assert(cit != _vFixedSegs.end());
					if (cit->inner == Direction::left) {
						pposx = cit->a.x+play_area_left;//吸着
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
						//もし右と左をつなぐような線を引いている場合、ベースがどこにあるのかを確定させたい
						auto baseIt = find_if(_vFixedSegs.begin(), _vFixedSegs.end(), [baseSegment](const Segment& seg)->bool {
							return seg == *baseSegment;
						});

						if (vSegments.back() == rightseg && (vSegments.front() == leftseg || (baseIt!=_vFixedSegs.end() &&vSegments.front() == *baseIt))) {
							//とりあえず上
							hSegments.push_back(topseg);
						}
						//領域塗りつぶし
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
				//左壁(もしくは固定辺)に当たった時
				if (pposx == play_area_left || 
					IsIntersected(Segment(tmppos, Position2(tmpx, tmppos.y)),
						_vFixedSegs, cit, baseSegment)) {
					if (cit == _vFixedSegs.end()) {
						vSegments.push_back(leftseg);
					}
					else {
						if (cit->inner == Direction::right) {
							pposx = cit->a.x + play_area_left;//吸着
							if (!onTheFrame) {
								vSegments.push_back(*cit);
							}
						}
					}
					if (!onTheFrame) {
						onTheFrame = true;
						hSegments.emplace_back(keypoints.back(), tmppos);


						//もし右と左をつなぐような線を引いている場合、ベースがどこにあるのかを確定させたい
						auto baseIt = find_if(_vFixedSegs.begin(), _vFixedSegs.end(), [baseSegment](const Segment& seg)->bool {
							return seg == *baseSegment;
						});

						if (vSegments.back() == leftseg && (vSegments.front() == rightseg || ((baseIt != _vFixedSegs.end() && vSegments.front() == *baseIt)))) {
							//とりあえず下
							hSegments.push_back(bottomseg);
						}
						//領域塗りつぶし
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

		if (!lastKeystate[KEY_INPUT_R] && keystate[KEY_INPUT_R]) {
			playerPos.x = play_area_left + (play_area_right - play_area_left) / 2;
			playerPos.y = play_area_bottom;//
			onTheFrame = true;
			baseSegment = nullptr;//出発地点のセグメント
			_lastOnTheFrame = false;
			_vFixedSegs.clear();
			_hFixedSegs.clear();
			vSegments.clear();
			hSegments.clear();
			keypoints.clear();

			_vFixedSegs.emplace_back(rightseg, rightseg.inner);
			_vFixedSegs.emplace_back(leftseg, leftseg.inner);
			_hFixedSegs.emplace_back(topseg, topseg.inner);
			_hFixedSegs.emplace_back(bottomseg, bottomseg.inner);

			SetDrawScreen(area);
			ClearDrawScreen();
			SetDrawScreen(DX_SCREEN_BACK);
		}

		LoopEndProcess(offset,area,playerPos,baseSegment,frame,
			keypoints,hSegments,vSegments,count);

	}

	DxLib_End();


}

void DrawDebugStatus(std::list<Position2> &keypoints, std::list<Segment> &hSegments, std::list<Segment> &vSegments)
{
	//デバッグ表示
	DrawFormatString(512, 50, 0xffffff, "点の数=%d", keypoints.size());
	DrawFormatString(512, 100, 0xffffff, "未確定横辺=%d", hSegments.size());
	DrawFormatString(512, 150, 0xffffff, "未確定縦辺=%d", vSegments.size());

	DrawFormatString(512, 200, 0xffffff, "確定横辺=%d", _hFixedSegs.size());
	DrawFormatString(512, 250, 0xffffff, "確定縦辺=%d", _vFixedSegs.size());

}

void FillVirtualScreen(int area, list<Segment> &hSegments, list<Segment> &vSegments,Direction finalNormalDir, bool &onTheFrame, list<Position2> &keypoints, bool reverseFlg )
{
	//仮画面を塗りつぶす
	DxLib::SetDrawScreen(area);
	bool debugDraw = false;
	if (CheckHitKey(KEY_INPUT_M)) {//デバッグ用
		debugDraw = true;
	}
	FillRange(hSegments, vSegments,finalNormalDir,reverseFlg,debugDraw);
	onTheFrame = true;
	//後処理
	keypoints.clear();
	vSegments.clear();
	hSegments.clear();
	DxLib::SetDrawScreen(DX_SCREEN_BACK);
}
