#include <tchar.h>
#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include "StartUp.h"


StartUp::StartUp()
{
	success=true;
	num=1;
	line=16;
	column=30;
	random=true;
}

StartUp::StartUp(HINSTANCE hInstance)
{
	success=DialogBoxParam(hInstance,(TCHAR*)IDD_STARTUP,NULL,Dialog,(LPARAM)this);
}


StartUp::~StartUp()
{
}

INT_PTR StartUp::Dialog(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static StartUp *startup;
	static TCHAR path[MAX_PATH];
	switch (message)
	{
		case WM_INITDIALOG:
			startup=(StartUp*)lParam;
			startup->hRandom=GetDlgItem(hwnd,IDC_CHECK1);
			startup->hPath=GetDlgItem(hwnd,IDC_EDIT1);
			startup->hNum=GetDlgItem(hwnd,IDC_EDIT2);
			startup->hLine=GetDlgItem(hwnd,IDC_EDIT3);
			startup->hColumn=GetDlgItem(hwnd,IDC_EDIT4);
			_tcscpy_s(path,_T("C:\\Program Files\\Microsoft Games\\Minesweeper\\MineSweeper.exe"));
			SetWindowText(startup->hPath,path);
			SetWindowText(startup->hNum,_T("1"));
			SetWindowText(startup->hLine,_T("16"));
			SetWindowText(startup->hColumn,_T("30"));
			Button_SetCheck(startup->hRandom,true);
			return true;
		//???
		/*case WM_SETFOCUS:
			SetFocus(GetDlgItem(hwnd,IDC_BUTTON2));
			return true;*/
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON1:
				{
					OPENFILENAME FileName={ 0 };
					FileName.lStructSize=sizeof(OPENFILENAME);
					FileName.hwndOwner=hwnd;
					FileName.lpstrFilter=_T("可执行文件(*.exe)\0*.exe\0");
					FileName.lpstrFile=path;
					FileName.nMaxFile=MAX_PATH;
					if (GetOpenFileName(&FileName))
						SetWindowText(startup->hPath,path);
				}
				break;
				case IDC_CHECK1:
					EnableWindow(startup->hNum,Button_GetCheck(startup->hRandom) ? true : false);
					SetWindowText(startup->hNum,_T("1"));
					break;
				case IDC_BUTTON2:
				{
					TCHAR string[10];
					GetWindowText(startup->hNum,string,sizeof(string) / sizeof(string[0]));
					startup->num=_ttoi(string);
					GetWindowText(startup->hLine,string,sizeof(string) / sizeof(string[0]));
					startup->line=_ttoi(string);
					GetWindowText(startup->hColumn,string,sizeof(string) / sizeof(string[0]));
					startup->column=_ttoi(string);
					if (!startup->num || !startup->line || !startup->column)
					{
						MessageBox(hwnd,_T("无效参数!"),_T("错误"),MB_OK | MB_ICONHAND);
						return true;
					}
					STARTUPINFO StartUpInfo={ 0 };
					PROCESS_INFORMATION ProcessInfo;
					if (CreateProcess(path,NULL,NULL,NULL,false,0,NULL,NULL,&StartUpInfo,&ProcessInfo))
					{
						CloseHandle(ProcessInfo.hProcess);
						CloseHandle(ProcessInfo.hThread);
					}
					else
					{
						MessageBox(hwnd,_T("打开程序失败!"),_T("错误"),MB_OK | MB_ICONHAND);
						return true;
					}
					startup->random=Button_GetCheck(startup->hRandom);
					EndDialog(hwnd,true);
				}
				break;
			}
			return true;
		case WM_CLOSE:
			EndDialog(hwnd,false);
			return true;
	}
	return false;
}