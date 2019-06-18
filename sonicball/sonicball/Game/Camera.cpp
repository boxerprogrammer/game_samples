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

///�Ď��v���C���[�̍폜
///@param player �폜�������v���C���[
void 
Camera::RemovePlayer(shared_ptr<Player> player) {
	_players.erase(std::remove_if(_players.begin(), _players.end(), [&player](shared_ptr<Player>& p) {
		return &(*player) == &(*p);
		}),_players.end());
}