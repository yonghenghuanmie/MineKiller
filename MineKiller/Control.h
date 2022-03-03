#pragma once

typedef struct _COLOR
{
	byte r,g,b;
	char num;
	char deviation;
	bool used;
}COLOR;

typedef enum _DIRECTION
{
	up=1,down,left,right
}DIRECTION;

typedef struct _COMMON
{
	int notcommon;
	int common;
	POINT point[4];
}COMMON;

class Control
{
public:
	Control(int,int);
	~Control();
	void wait();
	bool dispatch(bool);
	void scan();
	virtual void analysis(DWORD*);
	void out();
	inline byte AddAbsolute(byte,byte);
	inline byte SubAbsolute(byte,byte);
	bool calculate();
	//0¼üÅÌ,1×ó¼ü,2ÓÒ¼ü
	void click(int,int,char);
	int ReturnCount(int,int,char,char);
	inline bool boundary(int,int);
	bool algorithms(int,int,std::function<bool(int,int,DIRECTION,POINT)>);
	COMMON* GetCommon(int,int,DIRECTION);
	bool algorithms1_1(int,int,DIRECTION,POINT);
	bool algorithms2_1(int,int,DIRECTION,POINT);
	inline void ExceptClick(POINT,COMMON*,char);
	HHOOK hHook;
private:
	HWND hMine=NULL;
	int line,column;
	char *array=NULL;
	COLOR ColorArray[10];
	int cx,cy,blocksize,startx,starty;
};

#undef GetRValue
#define GetRValue(rgb) (LOBYTE((rgb)>>16))
#undef GetBValue
#define GetBValue(rgb) (LOBYTE(rgb))