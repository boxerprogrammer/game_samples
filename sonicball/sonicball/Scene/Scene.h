#pragma once

class Input;
class SceneController;
///�V�[���Ǘ��̂��߂̊��N���X
///(�������z�N���X)
class Scene
{
protected:
	SceneController& _controller;
public:
	Scene(SceneController& controller);
	virtual ~Scene();

	///�V�[���̍X�V���s��
	virtual void Update(const Input& input) = 0;

	///�`�悵�����`��
	virtual void Draw()= 0;
};

