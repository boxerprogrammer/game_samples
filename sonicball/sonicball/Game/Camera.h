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
};

