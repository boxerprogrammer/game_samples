#pragma once
#include"actor.h"

///�v���C���[�N���X
///@note 
///���͂ւ̔����ȊO�͊�{�I��Actor��
///�d�l�ɏ�������
class Player : public Actor
{
public:
	Player();
	~Player();
	///�X�V
	void Update(const Input& input) override final;
	
	///�`��
	void Draw() override final;
};

