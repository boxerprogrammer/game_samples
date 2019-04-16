#include "Debug.h"
#include<DxLib.h>
#include<iostream>
#include<string>
#include<sstream>

using namespace std;

Debug::Debug()
{
}


Debug::~Debug()
{
}


void 
Debug::Out(const char* c) {
#ifdef _DEBUG
	::OutputDebugString(c);
	cout << c;
#endif
}

void 
Debug::WriteLine(const char* filename,const int lineno,const char* c) {
#ifdef _DEBUG
	ostringstream str;
	str << filename;
	str << "\t";
	str << lineno;
	str << "\t";
	str << c;
	str << endl;
	::OutputDebugString(str.str().c_str());
	cout << str.str() << endl;
#endif
}


void 
Debug::OutputFormatString(const char* c, ...) {
#ifdef _DEBUG
	char out[256] = {};
	va_list vl;
	va_start(vl, c);
	snprintf(out,256,c, vl);
	::OutputDebugString(out);
	cout << out << endl;
	va_end(vl);
#endif
}