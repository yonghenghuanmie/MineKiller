#pragma once

#ifdef _WIN64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'		\
version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else if WIN32
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls'		\
version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif // _WIN64

class StartUp
{
public:
	StartUp();
	StartUp(HINSTANCE hInstance);
	~StartUp();
	static INT_PTR CALLBACK Dialog(HWND,UINT,WPARAM,LPARAM);
	bool success=false;
	bool random;
	int num;
	int line=16,column=30;
private:
	HWND hRandom;
	HWND hNum;
	HWND hPath;
	HWND hLine;
	HWND hColumn;
};

