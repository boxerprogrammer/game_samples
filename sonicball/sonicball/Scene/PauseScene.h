#pragma once
#include "Scene.h"
#include<functional>
class SceneController;
class PauseScene final : public Scene
{
private:
	int _inframeH;//枠用レンダーターゲット
	int _frame;
	int _openSE;
	int _closeSE;
	int _currentCursorId = 0;
	typedef void (PauseScene::*UpdateFunc_t)(const Input& input);
	typedef void (PauseScene::*DrawFunc_t)();
	UpdateFunc_t _updater;
	DrawFunc_t _drawer;
	
	void (PauseScene::*_u)(const Input& input);

	void DrawMain();
	void DrawExpand();

	void UpdatePreOpener(const Input& input);
	void UpdateOpener(const Input& input);
	void UpdateMain(const Input& input);
	void UpdateCloser(const Input& input);
	void UpdatePostCloser(const Input& input);
	const std::function<void(const Input& input)> lambda_fadein;
	const std::function<void(const Input& input)> lambda_fadeout;
	std::function<void(const Input& input)> _lambda;
public:
	PauseScene(SceneController& controller);
	~PauseScene();
	void Update(const Input& input)override;
	void Draw()override;
};

