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

	///ステージ全体のサイズ(スクロール限界座標)
	///をセットする
	///@param stageSize ステージ全体の大きさ
	///@remarks カメラは限界値を越えないように動く
	void SetRange(const Size& stageSize);

	///カメラ位置の更新
	void Update();

	///カメラの現在座標を返す
	const Position2& GetPosition()const;

	///カメラの撮影範囲を返す
	const Rect& GetViewRange()const;

};

