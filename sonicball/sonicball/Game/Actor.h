#pragma once

//�v���g�^�C�v�錾
class Input;

///��ʏ�œ����z���N���X
class Actor
{
public:
	Actor();
	virtual ~Actor();
	///��Ԃ���W��A�j���[�V�����t���[���X�V
	///@param input ����
	virtual void Update(const Input& input) = 0;

	///�`��
	///@remark ��{�I�ɕ`��݂̂���������悤��
	virtual void Draw() = 0;
};

