#include "Input.h"
#include<cassert>
#include<DxLib.h>
#include<algorithm>

using namespace std;



bool operator<(const PeripheralInfo & lval, const PeripheralInfo & rval) {
	if (lval.padno == rval.padno) {
		return (lval.code < rval.code);
	}
	else if (lval.padno < rval.padno) {
		return true;
	}
	return false;
}

Input::Input()
{
	//とりあえずデフォルトで接続パッド数＋１(キーボード)にしとく
	SetPlayerCount(GetConnectedPadCount() + 1);
}


Input::~Input()
{
}

int
Input::GetConnectedPadCount()const {
	return DxLib::GetJoypadNum();
}
void
Input::SetPlayerCount(int pcount) {
	_inputTable.resize(pcount);
	_currentInputState.resize(pcount);
	_lastInputState.resize(pcount);
}

void
Input::AddCommand(unsigned short pno, const char* command, PeripheralType periNo, unsigned int code) {
	assert(pno < _inputTable.size());
	_inputTable[pno][command] = PeripheralInfo(periNo, code);
	_inputMap.emplace(PeripheralInfo(periNo, code), make_pair(pno, command));
	_currentInputState[pno][command] = false;
}

void
Input::Check() {
	auto padcount = GetConnectedPadCount();

	char keystate[256];
	GetHitKeyStateAll(keystate);

	std::vector<int> padinfo(padcount + 1);//+1はキーボード分
	for (int i = 1; i < padinfo.size(); ++i) {
		padinfo[i] = GetJoypadInputState(i);
	}
	for (auto& p : _currentInputState) {
		for (auto& s : p) {
			s.second = false;
		}
	}
	for (auto& inputinfo : _inputMap) {
		_currentInputState[inputinfo.second.first][inputinfo.second.second] =
			_currentInputState[inputinfo.second.first][inputinfo.second.second] ||
			(
				inputinfo.first.padno == PeripheralType::keyboard ? keystate[inputinfo.first.code] :
				padinfo[(int)inputinfo.first.padno] & inputinfo.first.code
				);
	}

}

void 
Input::UpdateHistory() {

	std::copy(_currentInputState.begin(), _currentInputState.end(), _lastInputState.begin());
	//for (int i = 0; i < _currentInputState.size(); ++i) {
	//	for (auto state : _currentInputState[i]) {
	//		_lastInputState[i][state.first] = state.second;
	//	}
	//}
}

 bool
Input::IsPressed(unsigned short playerNo, const char* command) const {
	auto state = _currentInputState[playerNo].find(command);
	return state == _currentInputState[playerNo].end() ? false : state->second;
}

 bool
Input::IsTriggered(unsigned short playerNo, const char* command) const {
	 auto cur= IsPressed(playerNo, command);
	 if (!cur)return false;
	 auto state = _lastInputState[playerNo].find(command);
	 return cur && !(state == _lastInputState[playerNo].end() ? false : state->second);
}


bool operator==(const PeripheralInfo & lval, const PeripheralInfo & rval) {
	return (lval.code == rval.code) && (lval.padno == rval.padno);
}
bool operator!=(const PeripheralInfo & lval, const PeripheralInfo & rval) {
	return  (lval.code != rval.code) || (lval.padno != rval.padno);
}