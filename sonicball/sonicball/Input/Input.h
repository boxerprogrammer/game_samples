#pragma once

#include<vector>
#include<unordered_map>
#include<map>
#include<string>

enum class PeripheralType {
	keyboard,
	pad1, pad2, pad3, pad4
};

///���͏��
struct PeripheralInfo {
	PeripheralInfo() {};
	PeripheralInfo(PeripheralType pdno, int cd) :padno(pdno), code(cd) {}
	PeripheralType padno;//�����͋@��ԍ�
	int code;//�����̓R�[�h
	///unordered_map�p�n�b�V���֐�
	///��ӂȒl��Ԃ��΂��������Ȃ��
	///�p�b�h�ԍ��Ɠ��͒l��or���Z���Ă邾������
	struct HashFunc {
		size_t operator()(const PeripheralInfo& p)const {
			return (int)p.padno | (p.code << 4);
		}
	};
};
bool operator<(const PeripheralInfo & lval, const PeripheralInfo & rval);
bool operator==(const PeripheralInfo & lval, const PeripheralInfo & rval);
bool operator!=(const PeripheralInfo & lval, const PeripheralInfo & rval);

///���͎���N���X
class Input
{
private:
	std::vector<std::unordered_map<std::string, PeripheralInfo>> _inputTable;//�v���C���[�ԍ��Ɠ��͑Ή��e�[�u���̃Z�b�g
	std::unordered_map < PeripheralInfo, std::pair<unsigned short, std::string>, PeripheralInfo::HashFunc> _inputMap;//���͏��Ɨ��p���̃Z�b�g
	std::vector<std::unordered_map<std::string, bool>> _currentInputState;//���݂̉������
	std::vector<std::unordered_map<std::string, bool>> _lastInputState;//���݂̉������
public:
	Input();
	~Input();
	///�ڑ����p�b�h����Ԃ�
	int GetConnectedPadCount()const;

	///�v���C���[��(�p�b�h���Ƃ͊֌W�Ȃ���)��ݒ�
	void SetPlayerCount(int pcount);

	///�R�}���h�ݒ�
	///@param pno �v���C���[�ԍ�
	///@param cmd �R�}���h������
	///@param periNo ���͔ԍ�
	///@param code ���̓R�[�h
	void AddCommand(unsigned short pno, const char* cmd, PeripheralType periNo, unsigned int code);

	///���͏��X�V
	///@remarks ���t���[���Ăяo���Ă�������
	void Check();
	void UpdateHistory();

	///�����`�F�b�N
	///@param playerNo �v���C���[�ԍ�
	///@param cmd �R�}���h������
	 bool IsPressed(unsigned short playerNo, const char* cmd)const;

	///�g���K�[�`�F�b�N
	 bool IsTriggered(unsigned short playerNo, const char* cmd)const;

	///���݂̉����󋵂�Ԃ�
	const std::vector<std::unordered_map<std::string, bool>>& CurrentState()const { return _currentInputState; }
	///���O�̉����󋵂�Ԃ�
	const std::vector<std::unordered_map<std::string, bool>>& LastState()const { return _lastInputState; }


	

};

