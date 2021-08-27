#include "TAB_Dialog.h"
#include "Pack.h"
//全局实例句柄
HINSTANCE g_hInstance;
//定义一个全局变量指针，用来存放MainDialog对象的this指针
LPVOID g_pTabDialog;

VMPStruct g_vmstruct;

DWORD dataencrypt = 0;//数据表加密的变量


//程序入口
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE pPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	TAB_Dialog* ptab = TAB_Dialog::GetInstance();
	//释放内存
	delete ptab;
	return 0;
}