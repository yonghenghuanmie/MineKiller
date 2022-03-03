#include <cstdlib>
#include <functional>
#include <tchar.h>
#include <windows.h>
#include "Control.h"
#include "..\Hook\Hook.h"


Control::Control(int line,int column)
{
	this->line=line;
	this->column=column;
	array=(char*)calloc(line*column,sizeof(*array));
	memset(array,-1,line*column * sizeof(*array));
	for (int i=0;i < sizeof(ColorArray) / sizeof(ColorArray[0]);i++)
	{
		ColorArray[i].num=i - 1;
		ColorArray[i].deviation=5;
	}
	ColorArray[0].used=false;
	ColorArray[1].used=false;
	ColorArray[2].used=true;
	ColorArray[2].r=66;
	ColorArray[2].g=82;
	ColorArray[2].b=191;
	ColorArray[3].used=true;
	ColorArray[3].r=31;
	ColorArray[3].g=106;
	ColorArray[3].b=5;
	ColorArray[4].used=true;
	ColorArray[4].r=173;
	ColorArray[4].g=5;
	ColorArray[4].b=7;
	ColorArray[5].used=true;
	ColorArray[5].r=3;
	ColorArray[5].g=1;
	ColorArray[5].b=128;
	ColorArray[6].used=true;
	ColorArray[6].r=123;
	ColorArray[6].g=0;
	ColorArray[6].b=1;
	ColorArray[7].used=true;
	ColorArray[7].r=5;
	ColorArray[7].g=122;
	ColorArray[7].b=127;
	//Ã»ÕÒµ½...
	ColorArray[8].used=false;
	ColorArray[9].used=false;
	hHook=SetHook();
}


Control::~Control()
{
	free(array);
	if (hHook)
		Unhook(hHook);
}

void Control::wait()
{
	do
	{
		Sleep(500);
		hMine=FindWindow(NULL,_T("É¨À×"));
	} while (hMine == NULL);
	SetForegroundWindow(hMine);
	hMine=FindWindowEx(hMine,NULL,_T("Static"),NULL);
	RECT rect;
	GetWindowRect(hMine,&rect);
	cx=rect.right - rect.left + 1;
	cy=rect.bottom - rect.top + 1;
	const double proportion=0.1738;
	blocksize=(1.0 - proportion)*cy / line;
	startx=(cx - blocksize*column) / 2;
	starty=proportion / 2 * cy;
}

bool Control::dispatch(bool random)
{
	bool success=false;
	click(14,7,1);
	Sleep(100);
	do
	{
		Sleep(50);
		scan();
		//out();
		if (!calculate())
		{
			DWORD error=GetLastError();
			if (error == ERROR_SUCCESS)
			{
				success=true;
				Sleep(150);
				break;
			}
			else if (error == ERROR_ACCESS_DENIED)
			{
				if (random)
				{
					FILETIME FileTime;
					GetSystemTimeAsFileTime(&FileTime);
					srand(FileTime.dwLowDateTime);
					int ordinal;
					do
					{
						ordinal=rand() % (line*column);
						if (array[ordinal] == -1)
							break;
					} while (true);
					click(ordinal%column,ordinal / column,1);
					Sleep(150);
					if (FindWindow(NULL,_T("ÓÎÏ·Ê§°Ü")))
						break;
				}
				else
					break;
			}
		}
	} while (!escape);
	return success;
}

void Control::scan()
{
	HDC hdc=GetDC(hMine);
	HDC hdcm=CreateCompatibleDC(hdc);
	HBITMAP hBitmap=CreateCompatibleBitmap(hdc,cx,cy);
	SelectObject(hdcm,hBitmap);
	BitBlt(hdcm,0,0,cx,cy,hdc,0,0,SRCCOPY);
	ReleaseDC(hMine,hdc);
	//???
	BITMAPINFO *BitmapInfo=(BITMAPINFO*)calloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD),1);
	BitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	GetDIBits(hdcm,hBitmap,0,cy,NULL,BitmapInfo,DIB_RGB_COLORS);
	BitmapInfo->bmiHeader.biHeight*=-1;
	DWORD *bits=(DWORD*)malloc(BitmapInfo->bmiHeader.biSizeImage);
	GetDIBits(hdcm,hBitmap,0,cy,bits,BitmapInfo,DIB_RGB_COLORS);
	DeleteObject(hBitmap);
	DeleteDC(hdcm);
	free(BitmapInfo);
	analysis(bits);
	free(bits);
}

byte Control::AddAbsolute(byte a,byte b)
{
	if ((DWORD)a + b > 0xFF)
		return 0xFF;
	return a + b;
}

byte Control::SubAbsolute(byte a,byte b)
{
	if ((int)a - b < 0)
		return 0;
	return a - b;
}

void Control::analysis(DWORD *bits)
{
	for (int y=0;y < line;y++)
		for (int x=0;x < column;x++)
		{
			int minR=0xFF,maxR=0,maxB=0;
			if (array[y*column + x] != 9)
				for (int j=0;j < blocksize / 2;j++)
					for (int i=0;i < blocksize / 2;i++)
					{
						int pointx=startx + x*blocksize + blocksize / 4 + i,
							pointy=starty + y*blocksize + blocksize / 4 + j,
							ordinal=pointy*cx + pointx;

						int value=GetRValue(bits[ordinal]);
						if (value > maxR)
							maxR=value;
						if (value < minR)
							minR=value;
						value=GetBValue(bits[ordinal]);
						if (value > maxB)
							maxB=value;

						for (int t=0;t < sizeof(ColorArray) / sizeof(ColorArray[0]);t++)
							if (ColorArray[t].used == true &&
								GetRValue(bits[ordinal]) <= AddAbsolute(ColorArray[t].r,ColorArray[t].deviation) &&
								GetRValue(bits[ordinal]) >= SubAbsolute(ColorArray[t].r,ColorArray[t].deviation) &&
								GetGValue(bits[ordinal]) <= AddAbsolute(ColorArray[t].g,ColorArray[t].deviation) &&
								GetGValue(bits[ordinal]) >= SubAbsolute(ColorArray[t].g,ColorArray[t].deviation) &&
								GetBValue(bits[ordinal]) <= AddAbsolute(ColorArray[t].b,ColorArray[t].deviation) &&
								GetBValue(bits[ordinal]) >= SubAbsolute(ColorArray[t].b,ColorArray[t].deviation))
							{
								array[y*column + x]=ColorArray[t].num;
								if (ColorArray[t].num != 1)
								{
									i=blocksize / 2 - 1;
									j=i;
								}
								break;
							}
					}
			if (array[y*column + x] == 1 && maxR - minR < 100)
				array[y*column + x]=-1;
			if (array[y*column + x] == -1 && maxB - minR < 55)
				array[y*column + x]=0;
		}
}

void Control::out()
{
	int size=line*(column + 2);
	char *buffer=(char*)malloc(size);
	for (int y=0;y < line;y++)
	{
		int x;
		for (x=0;x < column;x++)
			buffer[y*(column + 2) + x]=array[y*column + x] == -1 ? '.' : array[y*column + x] + '0';
		buffer[y*(column + 2) + x]='\r';
		buffer[y*(column + 2) + x + 1]='\n';
	}
	HANDLE hFile=CreateFile(_T("out.txt"),GENERIC_ALL,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile,buffer,size,(DWORD*)&size,NULL);
	CloseHandle(hFile);
	free(buffer);
}

bool Control::calculate()
{
	bool solved=false;
	int closed=0;
	for (int i=0;i < line;i++)
		for (int j=0;j < column;j++)
		{
			int n=i*column + j;
			if (array[n] == -1)
				closed++;
			else if (array[n] != 0 && array[n] != 9)
				array[n]-=ReturnCount(j,i,9,0);
		}
	if (closed == 0)
	{
		SetLastError(ERROR_SUCCESS);
		return false;
	}

	for (int i=0;i < line;i++)
	{
		for (int j=0;j < column;j++)
		{
			int n=i*column + j;
			if (array[n] != -1 && array[n] != 9)
			{
				if (array[n] == 0)
				{
					if (ReturnCount(j,i,-1,0) != 0)
					{
						ReturnCount(j,i,-1,1);
						solved=true;
					}
				}
				else
				{
					if (array[n] == ReturnCount(j,i,-1,0))
					{
						ReturnCount(j,i,-1,2);
						solved=true;
					}
					if (array[n] == 1)
					{
						if (algorithms(j,i,std::bind(&Control::algorithms1_1,this,std::placeholders::_1,
							std::placeholders::_2,std::placeholders::_3,std::placeholders::_4)))
							solved=true;
					}
					else if (array[n] == 2)
					{
						if (algorithms(j,i,std::bind(&Control::algorithms2_1,this,std::placeholders::_1,
							std::placeholders::_2,std::placeholders::_3,std::placeholders::_4)))
							solved=true;
					}
				}
			}
		}
	}
	if (!solved)
		SetLastError(ERROR_ACCESS_DENIED);
	return solved;
}

void Control::click(int x,int y,char state)
{
	if (escape)
		return;
	INPUT input={ 0 };
	if (state)
	{
		input.type=INPUT_MOUSE;
		POINT point={ startx + x*blocksize + blocksize / 2,starty + y*blocksize + blocksize / 2 };
		ClientToScreen(hMine,&point);
		input.mi.dx=(double)point.x * 65536 / GetSystemMetrics(SM_CXSCREEN);
		input.mi.dy=(double)point.y * 65536 / GetSystemMetrics(SM_CYSCREEN);
		input.mi.dwFlags=MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
		SendInput(1,&input,sizeof(input));
		Sleep(45);
		input.mi.dwFlags=state == 1 ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
		SendInput(1,&input,sizeof(input));
		Sleep(25);
		input.mi.dwFlags=state == 1 ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
		SendInput(1,&input,sizeof(input));

		if (state == 2)
		{
			array[y*column + x]=9;
			for (int i=-1;i < 2;i++)
				for (int j=-1;j < 2;j++)
					if (boundary(x + i,y + j))
					{
						int n=(y + j)*column + x + i;
						if (array[n] != -1 &&
							array[n] != 0 &&
							array[n] != 9)
							array[n]--;
					}
		}
	}
	else
	{
		input.type=INPUT_KEYBOARD;
		input.ki.wVk=x;
		SendInput(1,&input,sizeof(input));
		Sleep(25);
		input.ki.dwFlags=KEYEVENTF_KEYUP;
		SendInput(1,&input,sizeof(input));
	}
}

int Control::ReturnCount(int x,int y,char value,char click)
{
	int count=0;
	for (int i=-1;i < 2;i++)
		for (int j=-1;j < 2;j++)
			if (boundary(x + i,y + j))
				if (array[(y + j)*column + x + i] == value)
				{
					if (click)
						this->click(x + i,y + j,click);
					else
						count++;
				}
	return count;
}

bool Control::boundary(int x,int y)
{
	if (x >= 0 && x < column&&y >= 0 && y < line)
		return true;
	return false;
}

bool Control::algorithms(int x,int y,std::function<bool(int,int,DIRECTION,POINT)> call)
{
	bool solved=false;
	DIRECTION direction=(DIRECTION)0;
	POINT point={ 0 };
	if (array[(y - 1)*column + x] == 1)
	{
		direction=up;
		point.x=x;
		point.y=y - 1;
		if (call(x,y,direction,point))
			solved=true;
	}
	if (array[(y + 1)*column + x] == 1)
	{
		direction=down;
		point.x=x;
		point.y=y + 1;
		if (call(x,y,direction,point))
			solved=true;
	}
	if (array[y*column + x - 1] == 1)
	{
		direction=left;
		point.x=x - 1;
		point.y=y;
		if (call(x,y,direction,point))
			solved=true;
	}
	if (array[y*column + x + 1] == 1)
	{
		direction=right;
		point.x=x + 1;
		point.y=y;
		if (call(x,y,direction,point))
			solved=true;
	}
	return solved;
}

COMMON * Control::GetCommon(int x,int y,DIRECTION direction)
{
	COMMON *common=(COMMON*)calloc(1,sizeof(COMMON));
	switch (direction)
	{
		case up:
			if (array[y*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y;
				common->common++;
			}
			if (array[y*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y;
				common->common++;
			}
			if (array[(y - 1)*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			if (array[(y - 1)*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			break;
		case down:
			if (array[y*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y;
				common->common++;
			}
			if (array[y*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y;
				common->common++;
			}
			if (array[(y + 1)*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			if (array[(y + 1)*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			break;
		case left:
			if (array[(y - 1)*column + x] == -1)
			{
				common->point[common->common].x=x;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			if (array[(y + 1)*column + x] == -1)
			{
				common->point[common->common].x=x;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			if (array[(y - 1)*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			if (array[(y + 1)*column + x - 1] == -1)
			{
				common->point[common->common].x=x - 1;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			break;
		case right:
			if (array[(y - 1)*column + x] == -1)
			{
				common->point[common->common].x=x;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			if (array[(y + 1)*column + x] == -1)
			{
				common->point[common->common].x=x;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			if (array[(y - 1)*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y - 1;
				common->common++;
			}
			if (array[(y + 1)*column + x + 1] == -1)
			{
				common->point[common->common].x=x + 1;
				common->point[common->common].y=y + 1;
				common->common++;
			}
			break;
	}
	common->notcommon=ReturnCount(x,y,-1,0) - common->common;
	return common;
}

bool Control::algorithms1_1(int x,int y,DIRECTION direction,POINT point)
{
	bool solved=false;
	COMMON *common=GetCommon(x,y,direction);
	if (common->notcommon == 0 && common->common > 1 && ReturnCount(point.x,point.y,-1,0) > common->common)
	{
		ExceptClick(point,common,1);
		solved=true;
	}
	free(common);
	return solved;
}

bool Control::algorithms2_1(int x,int y,DIRECTION direction,POINT point)
{
	bool solved=false;
	COMMON *common=GetCommon(x,y,direction);
	if (common->notcommon == 1 && common->common > 1)
	{
		if (ReturnCount(point.x,point.y,-1,0) > common->common)
			ExceptClick(point,common,1);
		point.x=x;
		point.y=y;
		ExceptClick(point,common,2);
		solved=true;
	}
	free(common);
	return solved;
}

void Control::ExceptClick(POINT point,COMMON *common,char state)
{
	for (int i=-1;i < 2;i++)
		for (int j=-1;j < 2;j++)
			if (boundary(point.x + i,point.y + j))
				if (array[(point.y + j)*column + point.x + i] == -1)
				{
					bool not=true;
					for (int k=0;k < common->common;k++)
						if (point.x + i == common->point[k].x&&point.y + j == common->point[k].y)
							not=false;
					if (not)
						click(point.x + i,point.y + j,state);
				}
}
