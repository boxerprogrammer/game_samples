#pragma once
#include "Scene.h"
#include"../Geometry.h"
#include<memory>
#include<vector>
class Background;
class Player;
class Ground;
class EnemyFactory;
class EventObjectFactory;
class HUD;
class Camera;
class Stage;
class Goal;

class GamePlayingScene :
	public Scene
{
private:
	friend Goal;

	
	//ゲームプレイ用パラメータ
	
	//ゲームプレイ中に必要な画像
	int _playerH;
	int _raygunH;
	int _portalBulH;
	int _portalRH;
	int _portalBH;
	int _scaffoldH;
	int _backfireH;

	int _bgH;

	int _x = 50, _y = 50;
	int _cx = 16, _cy = 65;
	float _vy = 0.0f;
	float _g = 0.5f;
	int _mx, _my;
	int _tx = 0, _ty = 0;//ターゲット
	float _bulVx = 0, _bulVy = 0;
	float _bulAng = 0;
	unsigned int _frame = 0;
	bool _mousePress = false;
	bool _shotable = true;
	float _bulX = 0, _bulY = 0;
	float _epsilon = 8.f;
	bool _blueOn = false;
	bool _redOn = false;
	float _angle = 0.0f;
	enum class PortalColor {
		red,
		blue
	};
	PortalColor _currentPCol = PortalColor::red;
	int _blueX = 0, _blueY = 0;
	int _redX = 0, _redY = 0;
	float _blueAngle = 0;
	float _redAngle = 0;

	int _gunx;
	int _guny;
	float _gunangle;

	int _pauseH;
	bool _isPause;
	int _captureH;
	int _preCaptureH;
	int _blendH;


	int _beamSE;
	int _gateSE;

	int _wait;
	int _readX;
	int _hitStopTimer;
	std::shared_ptr<Stage> _stage;
	std::shared_ptr<Camera> _camera;
	std::shared_ptr<Background> _bg;
	std::shared_ptr<Player> _player;
	std::shared_ptr<Ground> _ground;
	std::shared_ptr<EnemyFactory> _efactory;
	std::shared_ptr<EventObjectFactory> _eventFactory;
	std::shared_ptr<HUD> _hud;
	int _stageBGM;
	void CreateFragileVertices(int divx, int divy, Size size);

	void (GamePlayingScene::*_updater)(const Input& input);
	void FadeinUpdate(const Input& input);
	void NormalUpdate(const Input& input);
	void PausingUpdate(const Input& input);
	void FadeoutUpdate(const Input& input);

	void NextStageFadeOutUpdate(const Input& input);

	void ProgressStageData();

	void GameUpdate(const Input& input);

	bool IsStageEnd();
public:
	GamePlayingScene(const char* fmf, const char* bgpath, const char* bgmpath, SceneController& controller);
	~GamePlayingScene();
	void Update(const Input& )override;
	void Draw()override;
};

