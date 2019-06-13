#include<iostream>
#include<cassert>
#include"Application.h"

#ifdef _DEBUG
//���A�T�[�V����
#define oreassert(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#else
#define oreassert(expression) 
#endif

#ifdef _DEBUG
int main() {
#else
#include<Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	std::cout << "�͂��A��[���X�^�[�g" << std::endl;
	auto& app=Application::Instance();
	app.Initialize();
	app.Run();
	app.Terminate();
}