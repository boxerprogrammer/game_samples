#include "Camera.h"
#include<algorithm>

using namespace std;

Camera::Camera()
{
}


Camera::~Camera()
{
}

void
Camera::AddPlayer(shared_ptr<Player> player) {
	_players.emplace_back(player);
}

///監視プレイヤーの削除
///@param player 削除したいプレイヤー
void 
Camera::RemovePlayer(shared_ptr<Player> player) {
	_players.erase(std::remove_if(_players.begin(), _players.end(), [&player](shared_ptr<Player>& p) {
		return &(*player) == &(*p);
		}),_players.end());
}