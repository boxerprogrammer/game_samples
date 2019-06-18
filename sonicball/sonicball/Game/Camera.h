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

	///監視プレイヤーの追加
	///@param player 追加したいプレイヤー
	void AddPlayer(std::shared_ptr<Player> player);

	///監視プレイヤーの削除
	///@param player 削除したいプレイヤー
	void RemovePlayer(std::shared_ptr<Player> player);
};

