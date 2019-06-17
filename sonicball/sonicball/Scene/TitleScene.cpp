#include "TitleScene.h"
#include<DxLib.h>
#include"../Input/Input.h"
#include"../Application.h"

#include"GamePlayingScene.h"
#include"TopMenuScene.h"
#include"SceneController.h"
#include<cmath>
#include<iostream>

using namespace std;

const int title_offset_y = -120;
const int title_portal_offsetx = 40;
const int title_portal_offsety = -12;
const int portal_anim_num_y = 3;
const int portal_anim_num_x = 5;
//todo 40Çtitle_portal_offsetxÇ÷

void
TitleScene::ImgInfo::Load(const char* path) {
	handle = DxLib::LoadGraph(path);
	GetGraphSize(handle, &size.w, &size.h);
}
TitleScene::TitleScene(SceneController& controller):Scene(controller)
{
	_startSE = DxLib::LoadSoundMem("se/start.mp3");
	_titlepng=DxLib::LoadGraph("img/title/sonic_logo.png");
	_logoportalH= DxLib::LoadGraph("img/title/title_gate.png");
	_pressstartH = DxLib::LoadGraph("img/title/pressstart.png");
	_bgimgs.mostbackImg.Load("img/title/mostback.png");
	_bgimgs.farImg.Load("img/title/far.png");
	_bgimgs.middleImg.Load("img/title/middle.png");
	_bgimgs.foregroundImg.Load("img/title/foreground.png");
	_bgimgs.mostforeImg.Load("img/title/mostfore.png");

	GetGraphSize(_logoportalH, &_portalSize.w, &_portalSize.h);
	_portalSize.w = 48;//àÍÇ¬ìñÇΩÇËÇÃïù
	_portalSize.h = 96;//àÍÇ¬ìñÇΩÇËÇÃçÇ
	int w, h;
	DxLib::GetGraphSize(_titlepng, &w, &h);
	auto ssize = Application::Instance().GetConfig().GetScreenSize();
	_titlepos.x = (ssize.w-w) / 2;
	_titlepos.y = (ssize.h-h) / 2;
	_titlepos.y += title_offset_y;

	DxLib::GetGraphSize(_pressstartH, &_pressstartSize.w, &_pressstartSize.h);
	_wait = 60;
	_updater = &TitleScene::FadeinUpdate;
	_frameCounter = 0;
	auto a = []() {};
}

TitleScene::~TitleScene()
{
#ifdef _DEBUG
	cout << "Title Scene Deleted" << endl;
#endif
	DxLib::DeleteGraph(_titlepng);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0 );
	DxLib::DeleteSoundMem(_startSE);
}

void
TitleScene::Update(const Input& input) {

	(this->*_updater)(input);
}

void 
TitleScene::FadeinUpdate(const Input& input) {
	if (--_wait ==0) {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		_updater = &TitleScene::NormalUpdate;
	}
	int w, h;
	GetWindowSize(&w, &h);
	
	
	//É^ÉCÉgÉã
	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - 255 * ((float)_wait) / 60.0f);
	//îwåiÇÃï\é¶
	DrawTitleBackground(w, h);


	DxLib::SetDrawBlendMode(DX_BLENDMODE_MUL, 64);
	DrawBox(0, 0, w, h, 0x444444, true);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - 255 * ((float)_wait) / 60.0f);
	DxLib::DrawGraph(_titlepos.x, _titlepos.y, _titlepng, true);
	DxLib::DrawRectGraph(_titlepos.x + title_portal_offsetx, _titlepos.y+ title_portal_offsety,
		0, 0, _portalSize.w, _portalSize.h, _logoportalH, true);




	//DxLib::DrawGraph((w - _pressstartSize.w) / 2,( h-_pressstartSize.h)/2, _pressstartH, true);
}
void TitleScene::DrawTitleBackground(int w, int h)
{
	//ç≈îwñ 
	DrawExtendGraph(0, 0, w, h, _bgimgs.mostbackImg.handle, false);

	//îwñ 
	DrawExtendGraph(-(_bgFrame/6%w), h - ((float)w / (float)_bgimgs.farImg.size.w) *_bgimgs.farImg.size.h,
		w - (_bgFrame/6%w), h, _bgimgs.farImg.handle, true);
	DrawExtendGraph(w-(_bgFrame / 6 % w), h - ((float)w / (float)_bgimgs.farImg.size.w) *_bgimgs.farImg.size.h,
		w*2 - (_bgFrame / 6 % w), h, _bgimgs.farImg.handle, true);
	
	//íÜä‘
	DrawExtendGraph(-(_bgFrame/2%w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.middleImg.size.h,
		w - (_bgFrame/2%w), h, _bgimgs.middleImg.handle, true);
	DrawExtendGraph(w-(_bgFrame / 2 % w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.middleImg.size.h,
		w*2 - (_bgFrame / 2 % w), h, _bgimgs.middleImg.handle, true);
	
	//éËëO
	DrawExtendGraph(-(_bgFrame%w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.foregroundImg.size.h,
		w - (_bgFrame%w), h, _bgimgs.foregroundImg.handle, true);
	DrawExtendGraph(2-(_bgFrame%w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.foregroundImg.size.h,
		w*2 - (_bgFrame%w), h, _bgimgs.foregroundImg.handle, true);


	//ç≈ëOñ 
	DrawExtendGraph(-(_bgFrame*2%w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.mostforeImg.size.h,
		w-(_bgFrame*2%w), h, _bgimgs.mostforeImg.handle, true);
	DrawExtendGraph(w-(_bgFrame * 2 % w), h - ((float)w / (float)_bgimgs.mostbackImg.size.w) *_bgimgs.mostforeImg.size.h,
		w*2 - (_bgFrame * 2 % w), h, _bgimgs.mostforeImg.handle, true);

	++_bgFrame;
}
void 
TitleScene::NormalUpdate(const Input& input) {
	
	if (input.IsTriggered(0,"decide")) {
		_startSE = DxLib::PlaySoundMem(_startSE, DX_PLAYTYPE_BACK);
		_updater = &TitleScene::BlinkUpdate;
		_wait = 40;
	}
	const auto portal_anim_frame = _frameCounter / 4;
	const auto portal_anim_num = portal_anim_num_x * portal_anim_num_y;
	Position2 portalSrc;
	portalSrc.x = ((portal_anim_frame%portal_anim_num) % portal_anim_num_x) * _portalSize.w;
	portalSrc.y = ((portal_anim_frame%portal_anim_num) / portal_anim_num_x) * _portalSize.h;

	int w, h;
	GetWindowSize(&w, &h);


	//îwåiÇÃï\é¶
	DrawTitleBackground(w, h);

	DxLib::SetDrawBlendMode(DX_BLENDMODE_MUL, 64);
	DrawBox(0, 0, w, h, 0x444444, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//ÉçÉSï\é¶
	DxLib::DrawGraph(_titlepos.x, _titlepos.y, _titlepng, true);
	DxLib::DrawRectGraph(_titlepos.x + title_portal_offsetx, _titlepos.y + title_portal_offsety,
		portalSrc.x, portalSrc.y, _portalSize.w, _portalSize.h, _logoportalH, true);
	if ((_frameCounter / 40) % 2 == 1) {
		SetDrawBlendMode(DX_BLENDMODE_ADD, abs((_frameCounter/13 % 3)-1) * 64 + 192);
		DxLib::DrawGraph((w - _pressstartSize.w) / 2, (h - _pressstartSize.h) / 2, _pressstartH, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	_frameCounter = (_frameCounter + 1)%320 ;

	
}
void 
TitleScene::BlinkUpdate(const Input& input) {
	int w, h;
	GetWindowSize(&w, &h);
	//îwåiÇÃï\é¶
	DrawTitleBackground(w, h);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_MUL, 64);
	DrawBox(0, 0, w, h, 0x444444, true);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DxLib::DrawGraph(_titlepos.x, _titlepos.y, _titlepng, true);

	const auto portal_anim_num = portal_anim_num_x * portal_anim_num_y;
	const auto portal_anim_frame = _frameCounter / 4;
	Position2 portalSrc;
	portalSrc.x = ((portal_anim_frame%portal_anim_num) % portal_anim_num_x) * _portalSize.w;
	portalSrc.y = ((portal_anim_frame%portal_anim_num) / portal_anim_num_x) * _portalSize.h;

	DxLib::DrawRectGraph(_titlepos.x + title_portal_offsetx, _titlepos.y + title_portal_offsety,
		portalSrc.x, portalSrc.y, _portalSize.w, _portalSize.h, _logoportalH, true);


	//push start button
	if ((_frameCounter / 8) % 2 == 1) {
		SetDrawBlendMode(DX_BLENDMODE_ADD, abs((_frameCounter / 13 % 3) - 1) * 64 + 192);
		DxLib::DrawGraph((w - _pressstartSize.w) / 2, (h - _pressstartSize.h) / 2, _pressstartH, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	_frameCounter = (_frameCounter + 1) % 320;
	if (--_wait == 0) {
		_controller.ChangeScene(make_unique< TopMenuScene>(_controller));

		_updater = &TitleScene::FadeoutUpdate;
		_wait = 90;
	}
}
void 
TitleScene::FadeoutUpdate(const Input& input) {
	if (--_wait == 0) {
		_controller.ChangeScene(make_unique< TopMenuScene>(_controller));
	}
	else {
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * ((float)_wait) / 60.0f);
		DxLib::DrawGraph(_titlepos.x, _titlepos.y, _titlepng, true);
		DxLib::DrawRectGraph(_titlepos.x + title_portal_offsetx, _titlepos.y,
			0, 0, _portalSize.w, _portalSize.h, _logoportalH, true);
		DxLib::DrawGraph((768 - _pressstartSize.w) / 2, 300, _pressstartH, true);
	}
}

void 
TitleScene::Draw() {
	DrawString(100, 100, "Title Scene", 0xffffff);
}