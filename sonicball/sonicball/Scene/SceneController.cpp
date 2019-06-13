#include "SceneController.h"
#include"../input/input.h"
#include"TitleScene.h"
#include<cassert>
using namespace std;


SceneController::SceneController()
{
	_scene.emplace_front(make_unique<TitleScene>(*this));
}


SceneController::~SceneController()
{
	_scene.clear();
}

void
SceneController::SceneUpdate(const Input& input) {
	if (_scene.empty())return;
	_scene.front()->Update(input);
	auto rit = _scene.rbegin();
	for (; rit != _scene.rend();++rit) {
		(*rit)->Draw();
	}
}


void
SceneController::ChangeScene(unique_ptr<Scene> scene) {
	_scene.pop_front();
	_scene.emplace_front(move(scene));
}

void
SceneController::PushScene(unique_ptr<Scene> scene) {
	_scene.emplace_front(move(scene));
}

void
SceneController::PopScene() {
	_scene.erase(_scene.begin());
	assert(!_scene.empty());
}

bool 
SceneController::IsMultipleScene() {
	return _scene.size() >= 2;
}