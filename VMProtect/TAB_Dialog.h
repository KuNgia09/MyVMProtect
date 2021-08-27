#pragma once
#include "holder.h"

class TAB_Dialog
{
public:
	~TAB_Dialog() {};
	ULONG_PTR m_gdiplusToken;//初始化GDI要用到的变量
private:
	TAB_Dialog() {};

	static void Show()
	{
		//创建非模式对话框		
		DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_TAB_DIALOG), NULL, DialogProc);
	}
public:
	//单例模式
	static TAB_Dialog* GetInstance()
	{
		static TAB_Dialog* ptb = new TAB_Dialog();
		g_pTabDialog = ptb;
		Show();
		return ptb;
	}

public:
	//初始化(设置图标等)
	void OnInitDialog(HWND hwndlg,LPARAM lParam);
	//初始化tab标签
	BOOL Tab_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

	//处理主窗口消息循环的回调函数
	static INT_PTR CALLBACK DialogProc(HWND hwndlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//绘图
	void On_Paint(HWND hwndlg);

public:
	HWND hDlg_intab[2];//两个要载入到TAB控件中的对话框句柄
	HWND htabctrl;//TAB控件句柄

	bool isFile = false;//是否有文件

	//文件路径
	TCHAR str_path[MAX_PATH] = { 0 };

	//记录函数地址
	TCHAR str_vmaddr[BSTRINGLENGTH] = { 0 };

public:
	static BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//tab1子窗口的窗口处理过程函数申明
	static BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//tab2子窗口的窗口处理过程函数申明
	typedef BOOL (WINAPI* DIALOGPROC)(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); //定义一个 函数指针
	DIALOGPROC DlgProc[2] = { tab1_dlg_proc,tab2_dlg_proc };

	//初始化
	void IniTialDlg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//子控件信息
	void OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	////拖拽文件进窗口,，返回文件路径
	TCHAR* _DropsFile(HWND hwnDlg, WPARAM wParam);

	//打开文件，返回文件路径
	TCHAR* GetFilePath(HWND hwnDlg);

	//反汇编,计算函数的长度
	void Disassembler_(HWND hwnd, DWORD virtualaddr, DWORD instruction);

};

