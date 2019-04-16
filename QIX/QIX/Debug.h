#pragma once

class Debug
{
public:
	Debug();
	~Debug();
	static void Out(const char* );
	static void WriteLine(const char* filename, int lineno, const char* str);
	static void OutputFormatString(const char* c, ...);
};

