#pragma once


#include"Geometry.h"
class Application
{
public:
	class Configure {
	public:
		Size GetScreenSize()const;
	};
private:
	Application();
	Application(const Application&);
	void operator=(const Application&);
	Configure _config;
public:
	static Application& Instance() {
		static Application instance;
		return instance;
	}
	~Application();
	void Initialize();
	void Run();
	void Terminate();

	
	Configure& GetConfig() { return _config; }
};

