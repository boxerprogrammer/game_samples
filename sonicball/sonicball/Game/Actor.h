#pragma once

//�v���g�^�C�v�錾
class Input;
class Camera;
///��ʏ�œ����z���N���X
class Actor
{
protected:
	Camera& _camera;
public:
	Actor(Camera& cam);
	virtual ~Actor();
	///��Ԃ���W��A�j���[�V�����t���[���X�V
	///@param input ����
	virtual void Update(const Input& input) = 0;

	///�`��
	///@remark ��{�I�ɕ`��݂̂���������悤��
	virtual void Draw() = 0;
};

