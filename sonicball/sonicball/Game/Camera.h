#pragma once
#include<memory>
#include<vector>
#include"../Geometry.h"
class Player;
class Camera
{
private:
	Position2 _pos;
	std::vector<std::shared_ptr<Player>> _players;
public:
	Camera();
	~Camera();

	///�Ď��v���C���[�̒ǉ�
	///@param player �ǉ��������v���C���[
	void AddPlayer(std::shared_ptr<Player> player);

	///�Ď��v���C���[�̍폜
	///@param player �폜�������v���C���[
	void RemovePlayer(std::shared_ptr<Player> player);

	///�X�e�[�W�S�̂̃T�C�Y(�X�N���[�����E���W)
	///���Z�b�g����
	///@param stageSize �X�e�[�W�S�̂̑傫��
	///@remarks �J�����͌��E�l���z���Ȃ��悤�ɓ���
	void SetRange(const Size& stageSize);

	///�J�����ʒu�̍X�V
	void Update();

	///�J�����̌��ݍ��W��Ԃ�
	const Position2& GetPosition()const;

	///�J�����̎B�e�͈͂�Ԃ�
	const Rect& GetViewRange()const;

};

