#pragma once
#include<deque>
#include<memory>
class Scene;
class Input;
///シーン管理クラス
class SceneController
{
private:
	std::deque<std::unique_ptr<Scene>> _scene;
	
public:
	SceneController();
	~SceneController();

	void SceneUpdate(const Input& input);
	void ChangeScene(std::unique_ptr<Scene>);
	void CleanChangeScene(Scene*);
	bool IsMultipleScene();
	void PushScene(std::unique_ptr<Scene>);
	void PopScene();
};

