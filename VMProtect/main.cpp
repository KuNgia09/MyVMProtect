#include "TAB_Dialog.h"
#include "Pack.h"
#include "WProtectSDK.h"
//全局实例句柄
HINSTANCE g_hInstance;
//定义一个全局变量指针，用来存放MainDialog对象的this指针
LPVOID g_pTabDialog;

VMPStruct g_vmstruct;

DWORD dataencrypt = 0;//数据表加密的变量


//程序入口
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE pPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//WProtectEnd() 57 50 72 6f 74 65 63 74 20 42 65 67 69 6e
	//char* sz = ( char*)"WProtect Begin";

	//57 50 72 6f 74 65 63 74 20 45 6e 64 00
	//char* sz = ( char*)"WProtect End";
	_asm {
		_emit 0xeb
		_emit 0x0f
		_emit 0x57
		_emit 0x50
		_emit 0x72
		_emit 0x6f
		_emit 0x74
		_emit 0x65
		_emit 0x63
		_emit 0x74
		_emit 0x20
		_emit 0x42
		_emit 0x65
		_emit 0x67
		_emit 0x69
		_emit 0x6e
		_emit 0x00

	}

	
	g_hInstance = hInstance;
	
	TAB_Dialog* ptab = TAB_Dialog::GetInstance();
	//释放内存
	delete ptab;
	
	/*__asm {
		__emit__(0xEB, 0x0E, 0x56, 0x4D, 0x50, 0x72, 0x6F, 0x74, 0x65, 0x63, \
		0x74, 0x20, 0x65, 0x6E, 0x64, 0x00);
	}*/
	return 0;
	//57 50 72 6f 74 65 63 74 20 45 6e 64 00
	_asm {
		_emit 0xeb
		_emit 0x0d
		_emit 0x57
		_emit 0x50
		_emit 0x72
		_emit 0x6f
		_emit 0x74
		_emit 0x65
		_emit 0x63
		_emit 0x74
		_emit 0x20
		_emit 0x45
		_emit 0x6e
		_emit 0x64
		_emit 0x00

	}
}