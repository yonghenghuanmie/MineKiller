#include <functional>
#include <tchar.h>
#include <windows.h>
#include "StartUp.h"
#include "Control.h"
#include "..\Hook\Hook.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	StartUp *startup;
	if (!_tcsicmp(lpCmdLine,_T("/no interface")))
		startup=new StartUp();
	else
		startup=new StartUp(hInstance);
	if (startup->success)
	{
		for (int i=0;i < startup->num;i++)
		{
			Control *control=new Control(startup->line,startup->column);
			control->wait();
			bool success=control->dispatch(startup->random);
			if (escape)
				break;
			if (success || (!success&&startup->random))
				if (i + 1 < startup->num)
					control->click('P',0,0);
			delete control;
		}
	}
	delete startup;
	return 0;
}