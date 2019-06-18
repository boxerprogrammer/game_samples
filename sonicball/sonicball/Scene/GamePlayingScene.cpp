#include "GamePlayingScene.h"
#include<DxLib.h>
#include"../Input/Input.h"
#include"../Application.h"
#include"ContinueScene.h"
#include"SceneController.h"
#include"PauseScene.h"

using namespace std;
constexpr int GROUND_LINE = 500;

namespace{
	struct Fragment {
		DxLib::VERTEX* headVert=nullptr;//三角形ポリゴン頂点先頭アドレス
		Vector2f center;//重心
		Vector2f velocity;//速度
		Vector2f accel;//加速度(落下や風なら加速するはず)
		float angleVel;//角速度(回転させたい)
		void Init() {
			//重心を計算
			center.x = (headVert[0].x + headVert[1].x + headVert[2].x) / 3.f;
			center.y = (headVert[0].y + headVert[1].y + headVert[2].y) / 3.f;
			//初期速度を設定(多少のばらつきを加えている)
			velocity.x = 1.5f;
			velocity.y = -3.0f - 0.1f*static_cast<float>(rand() % 20 - 10);
			//加速度を設定(多少のばらつきを…)
			accel.x = 0.04f*static_cast<float>(rand() % 20 - 10) - 0.1f;
			accel.y = 0.3f;

			//角速度を設定(ばらつき…)
			angleVel = 0.015f*static_cast<float>(rand() % 20 - 10);
		}
		void Update() {
			//速度+=加速度
			velocity.x += accel.x;
			velocity.y += accel.y;
			//まず回転(重心中心)
			//平行移動に始まり平行移動に終わる()
			for (int i = 0; i<3; ++i) {
				auto& vert = headVert[i];

				//回転や拡縮のために重心を中心とする
				vert.x -= center.x;
				vert.y -= center.y;

				//コブラのマシンはサイコガン(回転)
				//わざわざ一時変数を使ってるのは両式にx,yが含まれているため
				float x = vert.x*cos(angleVel) - vert.y*sin(angleVel);
				float y = vert.x*sin(angleVel) + vert.y*cos(angleVel);
				vert.x = x;
				vert.y = y;

				//三角形を小さくする
				vert.x *= 0.985f;
				vert.y *= 0.985f;

				//一通りの計算のあとで中心座標分元に位置に戻す
				vert.x += center.x;
				vert.y += center.y;

				//落下
				vert.x += velocity.x;
				vert.y += velocity.y;

				//透明度UP
				vert.a -= 1;

			}
			//重心再計算
			center.x = (headVert[0].x + headVert[1].x + headVert[2].x) / 3.f;
			center.y = (headVert[0].y + headVert[1].y + headVert[2].y) / 3.f;
		}
	};

	std::vector<Fragment> _fragments;
	std::vector<DxLib::VERTEX> _vertices;
}

GamePlayingScene::GamePlayingScene(const char* fmf, const char* bgpath, const char* bgmpath, SceneController& controller) :Scene(controller)
{
	_updater = &GamePlayingScene::FadeinUpdate;
	_wait = 0;

	//ゲームプレイ中に必要な画像
	_playerH = LoadGraph("img/player.png");
	_raygunH = LoadGraph("img/raygun.png");
	_portalBulH = LoadGraph("img/portalBullet.png");
	_portalRH = LoadGraph("img/redportal.png");
	_portalBH = LoadGraph("img/blueportal.png");
	_scaffoldH = LoadGraph("img/scaffold.png");
	_backfireH = LoadGraph("img/backfire.png");

	_bgH = LoadGraph("img/bg.png");
	_beamSE = LoadSoundMem("se/beam.mp3");
	_gateSE = LoadSoundMem("se/pop.mp3");
}

void 
GamePlayingScene::ProgressStageData() {

}

GamePlayingScene::~GamePlayingScene()
{

}

Position2f RotatePosition(Position2f& p, float angle) {
	return p;
}

bool 
GamePlayingScene::IsStageEnd() {
	return false;
}

void 
GamePlayingScene::FadeinUpdate(const Input& input) {
	if (--_wait <= 0) {
		_updater = &GamePlayingScene::NormalUpdate;
	}

}
void 
GamePlayingScene::NormalUpdate(const Input& input) {

	auto wsize = Application::Instance().GetConfig().GetScreenSize();

	if (input.IsTriggered(0, "decide")) {
		_controller.ChangeScene(make_unique< ContinueScene>(_controller));
	}
	if (input.IsTriggered(0, "pause")) {
		_controller.PushScene(make_unique< PauseScene>(_controller));
		_updater= &GamePlayingScene::PausingUpdate;
	}
	if (input.IsTriggered(0, "up") && _vy==0.0f) {
		_vy = -15.0f;
	}
	if (input.IsTriggered(0, "down")) {
		_vy = 2;
	}
	if (input.IsPressed(0, "right")) {
		_x += 2;
	}
	if (input.IsPressed(0, "left")) {
		_x -= 2;
	}

	if (_y >= GROUND_LINE) {
		_y = GROUND_LINE;
		_vy = 0;
	}
	else {
		_vy += _g;
		_y += _vy;
	}

	_gunx = _x + 11 - _cx;
	_guny = _y + 24 - _cy;
	DxLib::GetMousePoint(&_mx, &_my);


	_gunangle = std::atan2f(_my - _guny, _mx - _gunx);

	if (_shotable) {
		_bulX = _gunx + cosf(_gunangle) * 45.0f;
		_bulY = _guny + sinf(_gunangle) * 45.0f;

	}
	else {
		_bulX += _bulVx;
		_bulY += _bulVy;
		if (std::hypotf(_bulX - _tx, _bulY - _ty) < _epsilon) {
			_shotable = true;
			if (_currentPCol == PortalColor::red) {
				_redOn = true;
				_redX = _bulX;
				_redY = _bulY;
				_redAngle = std::atan2f(_bulVy, _bulVx) - DX_PI_F * 0.5f;
				_currentPCol = PortalColor::blue;
			}
			else {
				_blueOn = true;
				_blueX = _bulX;
				_blueY = _bulY;
				_blueAngle = std::atan2f(_bulVy, _bulVx) - DX_PI_F * 0.5f;
				_currentPCol = PortalColor::red;
			}
			PlaySoundMem(_gateSE, DX_PLAYTYPE_BACK);
		}
		if (_bulX > wsize.w || _bulY > wsize.h) {
			_shotable = true;
		}
		if (_bulX < -64 || _bulY < -64) {
			_shotable = true;
		}
	}

	if (_frame % 4 == 0) {
		auto tmpAng = static_cast<float>(rand() % 4)*(DX_PI_F / 2.0f);
		if (tmpAng != _bulAng) {
			_bulAng = tmpAng;
		}
		else {
			_bulAng = tmpAng + 0.5f;
		}
	}
	auto mInput = DxLib::GetMouseInput();
	if (mInput&MOUSE_INPUT_LEFT) {
		if (!_mousePress) {
			if (_shotable) {
				_shotable = false;
				_bulVx = cosf(_gunangle) * 16.0f;
				_bulVy = sinf(_gunangle) * 16.0f;
				_tx = _mx;
				_ty = _my;
				_angle = _gunangle;
				PlaySoundMem(_beamSE, DX_PLAYTYPE_BACK);
			}
		}
		_mousePress = true;
	}
	else {
		if (_shotable) {
			_angle = _gunangle;
		}
		_mousePress = false;
	}
	_frame = (_frame + 1) % (60 * 65535);
}

void
GamePlayingScene::PausingUpdate(const Input& input) {

	//ProgressStageData();
	if (!_controller.IsMultipleScene()) {
		_updater = &GamePlayingScene::NormalUpdate;
	}
	
}

void 
GamePlayingScene::FadeoutUpdate(const Input& input) {


}


void 
GamePlayingScene::NextStageFadeOutUpdate(const Input& input) {

}

void
GamePlayingScene::Update(const Input& input) {



	(this->*_updater)(input);

}

void
GamePlayingScene::CreateFragileVertices(int divx, int divy, Size size) {
	_fragments.resize(divx*divy * 2);
	_vertices.resize(divx*divy * 6);
	for (int j = 0; j<divy; ++j) {
		float top = (float)(size.h / divy)*j;
		float bottom = (float)(size.h / divy)*(j + 1);
		float topv = (1.f / divy)*static_cast<float>(j);
		float bottomv = (1.f / divy)*static_cast<float>(j + 1);
		for (int i = 0; i<divx; ++i) {
			float left = (size.w / divx)*static_cast<float>(i);
			float right = (size.w / divx)*static_cast<float>(i + 1);
			float leftu = (1.f / divx)*static_cast<float>(i);
			float rightu = (1.f / divx)*static_cast<float>(i + 1);

			int headidx = (i + divx * j) * 6;

			//上半分
			_vertices[headidx + 0].r = _vertices[headidx + 0].g = _vertices[headidx + 0].b = _vertices[headidx + 0].a = 255;
			_vertices[headidx + 0].x = left; _vertices[headidx + 0].y = top;
			_vertices[headidx + 0].u = leftu; _vertices[headidx + 0].v = topv;
			_vertices[headidx + 1].r = _vertices[headidx + 1].g = _vertices[headidx + 1].b = _vertices[headidx + 1].a = 255;
			_vertices[headidx + 1].x = right; _vertices[headidx + 1].y = top;
			_vertices[headidx + 1].u = rightu; _vertices[headidx + 1].v = topv;
			_vertices[headidx + 2].r = _vertices[headidx + 2].g = _vertices[headidx + 2].b = _vertices[headidx + 2].a = 255;
			_vertices[headidx + 2].x = left; _vertices[headidx + 2].y = bottom;
			_vertices[headidx + 2].u = leftu; _vertices[headidx + 2].v = bottomv;
			int fragHeadIdx = (i + divx * j) * 2;
			_fragments[fragHeadIdx].headVert = &_vertices[headidx + 0];
			_fragments[fragHeadIdx].Init();

			//下半分
			_vertices[headidx + 3].r = _vertices[headidx + 3].g = _vertices[headidx + 3].b = _vertices[headidx + 3].a = 255;
			_vertices[headidx + 3].x = right; _vertices[headidx + 3].y = top;
			_vertices[headidx + 3].u = rightu; _vertices[headidx + 3].v = topv;
			_vertices[headidx + 4].r = _vertices[headidx + 4].g = _vertices[headidx + 4].b = _vertices[headidx + 4].a = 255;
			_vertices[headidx + 4].x = left; _vertices[headidx + 4].y = bottom;
			_vertices[headidx + 4].u = leftu; _vertices[headidx + 4].v = bottomv;
			_vertices[headidx + 5].r = _vertices[headidx + 5].g = _vertices[headidx + 5].b = _vertices[headidx + 5].a = 255;
			_vertices[headidx + 5].x = right; _vertices[headidx + 5].y = bottom;
			_vertices[headidx + 5].u = rightu; _vertices[headidx + 5].v = bottomv;
			_fragments[fragHeadIdx + 1].headVert = &_vertices[headidx + 3];
			_fragments[fragHeadIdx + 1].Init();
		}
	}
}

void
GamePlayingScene::Draw() {

	for (int n = 0; n < 2;  ++n) {
		for (int m = 0; m < 3; ++m) {
			DrawGraph(m*512, n*512, _bgH, false);
		}
	}
	

	auto& config = Application::Instance().GetConfig();
	auto wsize = config.GetScreenSize();
	DrawModiGraph(0, GROUND_LINE - 32,
		wsize.w, GROUND_LINE - 32,
		wsize.w, GROUND_LINE + 32, 
		0, GROUND_LINE + 32, _scaffoldH, true);

	DrawLine(0, GROUND_LINE, wsize.w, GROUND_LINE, 0xffaaaa);

	DrawFormatString(0, 0, 0x000000, "x=%d,y=%d", _mx, _my);
	DrawRotaGraph2(_x, _y, _cx, _cy, 1.0f, 0.0f, _playerH, true);
	DrawRotaGraph2(_gunx, _guny, 18, 18, 1.0f, _gunangle, _raygunH, true);
	DrawPixel(_gunx, _guny, 0xffffff);

	DrawString(100, 100, "GamePlaying Scene", 0xffffff);


	if (_redOn) {
		if ((_frame / 5) % 2 == 0) {
			DrawRotaGraph2(_redX, _redY, 64, 108, 1.0f, _redAngle, _portalRH, true);
		}
		else {
			DrawRotaGraph2(_redX, _redY, 64, 108, 1.0f, _redAngle, _portalRH, true, true);
		}
	}
	if (_blueOn) {
		if ((_frame / 5) % 2 == 0) {
			DrawRotaGraph2(_blueX, _blueY, 64, 108, 1.0f, _blueAngle, _portalBH, true);
		}
		else {
			DrawRotaGraph2(_blueX, _blueY, 64, 108, 1.0f, _blueAngle, _portalBH, true, true);
		}
	}
	auto idx = (_frame % 18) / 6;
	DrawRectRotaGraph2(_bulX, _bulY, idx * 64, 0, 64, 64, 51, 32, 1.2f, _angle, _backfireH, true);
	DrawRotaGraph2(_bulX, _bulY, 22, 22, 1.0f, _bulAng, _portalBulH, true);

	
}