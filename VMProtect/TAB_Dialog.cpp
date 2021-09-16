#include "TAB_Dialog.h"
#include "Common/header.h"
HWND g_hwnd;
HWND g_hwnd2;
//初始化
void TAB_Dialog::OnInitDialog(HWND hwndlg, LPARAM lParam)
{	
	// 初始化GDI+  
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//设置图标
	HICON hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_LT));
	SendMessage(hwndlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);//设置大图标
	SendMessage(hwndlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);//设置大图标

	//tab标签初始化
	Tab_OnInitDialog(hwndlg, hwndlg, lParam);
}

//主窗口循环
INT_PTR TAB_Dialog::DialogProc(HWND hwndlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TAB_Dialog* pt = (TAB_Dialog *)g_pTabDialog;
	switch (uMsg)
	{
	case WM_COMMAND:
		break;
	
	case WM_NOTIFY:
	{
		if (wParam==IDC_TAB1)
		{
			if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
			{
				int sel = TabCtrl_GetCurSel(pt->htabctrl);
				switch (sel)
				{
				case 0://选择TAB1标签时，显示tab1标签页面
				{
					ShowWindow(pt->hDlg_intab[0], TRUE);
					ShowWindow(pt->hDlg_intab[1], FALSE);
				}
					break;
				case 1://选择TAB2标签时，显示tab2标签页面
				{
					ShowWindow(pt->hDlg_intab[0],FALSE);
					ShowWindow(pt->hDlg_intab[1], TRUE);
				}
					break;
				default:
					break;
				}
			}
		}
	}
		break;
	case WM_INITDIALOG://初始化
		pt->OnInitDialog(hwndlg, lParam);
		break;

	case WM_CLOSE://退出
		EndDialog(hwndlg,0);
		break;
	default:
		break;
	}
    return FALSE;
}

//tab标签初始化
BOOL TAB_Dialog::Tab_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//显示tab标签
	htabctrl = GetDlgItem(hwnd, IDC_TAB1);
	TCITEM tie;//设置标签的属性
	const TCHAR* tabname[3] = { "项目","选项"};
	//具体设置tie的字段
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	for (int i = 0; i < 2; i++)
	{
		tie.pszText = (LPSTR)tabname[i];
		TabCtrl_InsertItem(htabctrl, i, &tie);
	}

	//显示别的控件
	RECT rect;//存放tab控件的区域位置
	GetClientRect(htabctrl, &rect);

	//将其他窗口往tab控件位置移动
	hDlg_intab[0] = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), htabctrl, DlgProc[0]);
	MoveWindow(hDlg_intab[0], -1, 21, rect.right - rect.left+2, rect.bottom - rect.top, FALSE);
	hDlg_intab[1] = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_OPT_DIALOG), htabctrl, DlgProc[1]);
	MoveWindow(hDlg_intab[1], -1, 21, rect.right - rect.left, rect.bottom - rect.top, FALSE);
	ShowWindow(hDlg_intab[0], SW_SHOW);
	return TRUE;
}

//绘图
void TAB_Dialog::On_Paint(HWND hwndlg)
{
	int width, height;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwndlg, &ps);

	//加载图像
	//Image image(L"beat.png");
	Image image(L"timg.jpg");

	/*if (image.GetLastStatus() != Status::Ok) {
		MessageBoxW(hwndlg, L"加载图片失败!", L"提示", MB_OK);
		return;
	}*/

	RECT rctA; //定义一个RECT结构体，存储窗口的长宽高
	::GetWindowRect(hwndlg, &rctA);//通过窗口句柄获得窗口的大小存储在rctA结构中

	//取得宽度和高度
	//width = rctA.right - rctA.left;   //窗口的宽度
	//height = rctA.bottom - rctA.top;  //窗口的高度		
	width = image.GetWidth();
	height = image.GetHeight();

	//绘图
	Graphics graphics(hdc);
	graphics.DrawImage(&image, 0, 0, width, height);
	EndPaint(hwndlg, &ps);
}

vector<DWORD>temp_addr;

//线程回调函数
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	TAB_Dialog* pt = (TAB_Dialog * )lpParameter;
	

	VMStart(pt->str_path,temp_addr);

	CString str_notify = "";
	TCHAR str_1[BSTRINGLENGTH] = { 0 };
	GetDlgItemTextA(g_hwnd, IDC_EDIT_ADDR, str_1, BSTRINGLENGTH);
	str_notify = str_notify + str_1 + "\r\n" + "加壳完成!!!";
	SetDlgItemTextA(g_hwnd, IDC_SHOW_EDIT, str_notify.GetString());
	return 0;
}

//tab1标签回调函数
BOOL __stdcall TAB_Dialog:: tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TAB_Dialog* pt = (TAB_Dialog*)g_pTabDialog;
	g_hwnd = hwnd;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{		
		////设置进度条
		//HWND Proc = GetDlgItem(hwnd, IDC_PROGRESS1);
		//SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));// 设置默认的范围值	
		//SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_SETPOS, 500, 0);// 设置当前的位
		//int x = SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_GETPOS, 0, 0);//获取进度条的值
	}
		break;
	case WM_DROPFILES://拖拽进窗口
	{
		pt->_DropsFile(hwnd, wParam);
		
	}
	break;
	case WM_COMMAND:
	{		
		switch (wParam)
		{
		case IDC_PRO_BUTTON://编译
		{
			memcpy(pt->str_path, "D:\\project\\MyVMProtect\\Debug\\VMProtect - 副本.exe",strlen("D:\\project\\MyVMProtect\\Debug\\VMProtect - 副本.exe") );
			if (pt->str_path[0] == 0)
			{
				MessageBoxA(NULL, "请添加PE格式文件!", "错误", MB_OK);
				return 0;
			}
			CString str = pt->str_vmaddr;
			str = "004414a0\r\n";
			char linestr[32] = { 0 };
			int nlen = str.GetCount();
			char* cstr = str.GetString();
			DWORD dwTimer = 0;
			for (int i = 0; i < nlen;)
			{
				int ncalc = str.LeftFindChar(cstr + i, '\n');
				memcpy(linestr, cstr + i, ncalc - 1);
								
				//字符转数字：
				sscanf(linestr, "%X", &dwTimer);
				if (dwTimer==0)
				{
					break;
				}
				temp_addr.push_back(dwTimer);
				dwTimer = 0;
				memset(linestr, 0, 32);
				i += ncalc;
			}
			//创建线程
			HANDLE hTread = CreateThread(NULL, 0, ThreadProc, (LPVOID)pt, 0, NULL);
			
			CString str_notify = "正在加壳，请耐心等待......";
			SetDlgItemTextA(hwnd, IDC_SHOW_EDIT, str_notify.GetString());
			//MessageBoxA(NULL, "确定编译？", "提示", MB_OK);

			temp_addr.clear();
		}

			break;
		case IDC_OPEN_BUTTON://打开文件
			pt->GetFilePath(hwnd);
			break;
		default:
			break;
		}
	}
		break;
	default:
		break;
	}
	return 0;
}
#define ID_ADD 2
//tab2标签回调函数
BOOL __stdcall TAB_Dialog:: tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	g_hwnd2 = hwnd;
	TAB_Dialog* pt = (TAB_Dialog*)g_pTabDialog;
	switch (uMsg)
	{
	case WM_PAINT:
		pt->On_Paint(hwnd);
		break;

	case WM_COMMAND:
	{
		if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_EDIT_ADDR)
		{
			//MessageBoxA(NULL, "编辑框改变了", "提示", MB_OK);
			
			TCHAR str_1[BSTRINGLENGTH] = { 0 };			
			GetDlgItemTextA(hwnd, IDC_EDIT_ADDR, str_1, BSTRINGLENGTH);	
			CString str = "";
			str = str + str_1 + "\r\n";
			memcpy(pt->str_vmaddr, str.GetString(), str.GetCount());
			SetDlgItemTextA(hwnd, IDC_EDIT_ADDR, str.GetString());
		}
		pt->OnCommand(hwnd, uMsg, wParam, lParam);
	}
		break;
	case WM_INITDIALOG:
	{
		pt->IniTialDlg( hwnd,  uMsg,  wParam,  lParam);

	}
		break;
	
	
	default:
		break;
	}
	return 0;
}

void TAB_Dialog::IniTialDlg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CString str = "1、输入编辑框格式 401000或者0x401000或者401000h  十六进制。\r\n";
	str = str + "2、函数添加错误，或者有重复，直接用鼠标在左边的编辑框清除即可。\r\n";
	str = str + "3、暂时只支持Release版，不支持Debug版!\n";
	SetDlgItemTextA(hwnd, IDC_EDIT_ASSEM, str.GetString());
}

//处理子控件信息
void TAB_Dialog::OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDC_BUTTON_ADD://添加
	{
		/*if (isFile==false)
		{
			MessageBoxA(NULL, "请添加PE格式文件，再来添加函数!", "提示", MB_OK);
			return;
		}*/
		TCHAR str[STRINGLENGTH] = { 0 };
		TCHAR str_1[BSTRINGLENGTH] = { 0 };
		GetDlgItemTextA(hwnd, IDC_EDIT_INPUT, str, STRINGLENGTH);
	


		GetDlgItemTextA(hwnd, IDC_EDIT_ADDR, str_1, BSTRINGLENGTH);

		CString cstr = str_1;		
		/*if (cstr.GetCount())
		{
			cstr = cstr + "\r\n";
		}*/
		cstr = cstr + str;
		if (cstr.GetCount())
		{
			cstr = cstr + "\r\n";
		}
		memcpy(str_vmaddr, cstr.GetString(), cstr.GetCount() + 1);
		SetDlgItemTextA(hwnd, IDC_EDIT_ADDR, cstr.GetString());
	}
		break;
	case IDC_BUTTON_ASSEM://查看反汇编
	{
		if (isFile == false)
		{
			MessageBoxA(NULL, "请添加PE格式文件!", "提示", MB_OK);
			return;
		}

		PEInfo peinfo;
		PE pe;
		pe.GetPEInformation_(str_path, &peinfo);

		//拉伸文件
		ULONG_PTR StretchAddr = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);
		//更新peinfo信息
		pe.GetPEInformation_1((char*)StretchAddr, &peinfo, peinfo.FileSize);

		
		TCHAR str_addr[STRINGLENGTH] = { 0 };
		GetDlgItemTextA(g_hwnd2, IDC_EDIT_INPUT, str_addr, STRINGLENGTH);
		CString cstr = "";
		DWORD funaddr = cstr.Str_To_Num(str_addr);
		//函数的起始位置
		ULONG_PTR proaddr = funaddr - peinfo.ImageBase + StretchAddr;


		SetDlgItemTextA(hwnd, IDC_EDIT_ASSEM, "");
		Disassembler_(hwnd, funaddr, proaddr);
	}
		break;
	default:
		break;
	}
}

TCHAR* TAB_Dialog::_DropsFile(HWND hwnd, WPARAM wParam)
{
	int  n, Num;
	TCHAR File[MAX_PATH];
	memset(File, 0, sizeof(File));
	HDROP hDrop;

	hDrop = (HDROP)wParam;  //wParam为消息的第一个参数
	Num = DragQueryFile(hDrop, -1, File, 256); //拖拉的文件个数
	for (n = 0; n < Num; n++) {
		DragQueryFile(hDrop, n, File, MAX_PATH); //取得每个文件的文件名

	}
	DragFinish(hDrop);

	if (!File)
	{
		return File;
	}
	SetDlgItemTextA(hwnd, IDC_PATH_PATH, File);
	memcpy(str_path, File, MAX_PATH);
	//SetDlgItemTextA(g_hwnd2, IDC_EDIT_ASSEM, "");
	SetDlgItemTextA(g_hwnd2, IDC_EDIT_ADDR, "");
	SetDlgItemTextA(g_hwnd2, IDC_EDIT_INPUT, "");
	SetDlgItemTextA(hwnd, IDC_SHOW_EDIT, "");
	temp_addr.clear();
	isFile = true;
	return nullptr;
}

TCHAR* TAB_Dialog::GetFilePath(HWND hwnd)
{
	OPENFILENAME stOpenFile;
	TCHAR szPeFileExt[100] = _T("*.exe;*.dll;*.scr;*.drv;*.sys");//打开的文件类型
	TCHAR szFileName[256];
	DWORD start_filename = 0;//文件名在字符串中的开始位置
	memset(szFileName, 0, 256);
	memset(&stOpenFile, 0, sizeof(OPENFILENAME));
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile.hwndOwner = hwnd;
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName;
	stOpenFile.nMaxFile = MAX_PATH;
	GetOpenFileName(&stOpenFile);

	start_filename = stOpenFile.nFileOffset;
	if (!start_filename)
	{
		return szFileName;
	}
	SetDlgItemTextA(hwnd, IDC_PATH_PATH, szFileName);
	memcpy(str_path, szFileName, MAX_PATH);
	//SetDlgItemTextA(g_hwnd2, IDC_EDIT_ASSEM, "");
	SetDlgItemTextA(g_hwnd2, IDC_EDIT_ADDR, "");
	SetDlgItemTextA(g_hwnd2, IDC_EDIT_INPUT, "");
	SetDlgItemTextA(hwnd, IDC_SHOW_EDIT, "");
	temp_addr.clear();
	isFile = true;
	return nullptr;
}

//JCC指令字符串
#define JCCNUMSS 30
const char* JCCSTR[JCCNUMSS] = {
	"JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ",
	"JPO ","JO ","JNO ","JC","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
	"JNBE ","JA ","JL","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG "
};

void TAB_Dialog::Disassembler_(HWND hwnd,DWORD virtualaddr, DWORD instruction)
{
	int inslength = 0;//记录指令的条数

	vector<DWORD>JccAddr;//记录JCC后面的地址

	DISASM disAsm = { 0 };

	// 3. 配置结构体，初始化反汇编的opcode
	disAsm.EIP = (UIntPtr)instruction;
	disAsm.VirtualAddr = virtualaddr; // opcode 指令的地址
	disAsm.Archi = 0; // 0 => 32 , 1 => 64
	disAsm.Options = 0x000; // masm 汇编指令格式

	int nCount = 0;// 用于记录在循环当中，反汇编了多少个字节
	int nLen = 0; // 用于记录当前的汇编指令的字节数
	CString str = "";
	int x = 0;
	while (true)
	{
		nLen = Disasm(&disAsm); // 每次只反汇编一条汇编指令， 并且返回当前得到的汇编指令的长度
		unsigned int uAddr = disAsm.VirtualAddr;

		//printf("%08X | ", uAddr); // 打印地址
		TCHAR addr08[32] = { 0 };
		sprintf(addr08, "%08X", uAddr);

		TCHAR tempstr[0x20000] = { 0 };
		GetDlgItemTextA(hwnd, IDC_EDIT_ASSEM, tempstr, 0x20000);
		str =  tempstr;
		if (x)
		{
			str = str + "\r\n";
		}
		x++;

		str = str + addr08 + "\t" + disAsm.CompleteInstr;
		SetDlgItemTextA(hwnd, IDC_EDIT_ASSEM, str.GetString());
		//printf(" | %s\n", disAsm.CompleteInstr); // 打印反汇编指令

		if (inslength == 0x1000)
		{//防止死循环
			MessageBoxA(NULL, "未知错误，读取指令失败!!", "提示", MB_OK);
			return;
		}

		++inslength;//记录指令的条数，为后面申请内存提供依据
		nCount += nLen; // 累加已经反汇编的字节数
		disAsm.EIP += nLen; // 定位到下一条汇编指令
		disAsm.VirtualAddr += nLen; // 设置到下一条汇编指令的地址

		//如果遇到jcc指令或者jmp指令，记录后面的地址
		for (int i = 0; i < JCCNUMSS; i++)
		{
			if (0 == _stricmp(disAsm.Instruction.Mnemonic, JCCSTR[i]))
			{
				DWORD tempaddr = 0;
				sscanf_s(disAsm.Argument1.ArgMnemonic, "%X", &tempaddr);
				JccAddr.push_back(tempaddr);
				break;
			}
		}

		if (
			(0 == _stricmp(disAsm.Instruction.Mnemonic, "ret ")) ||
			(0 == _stricmp(disAsm.Instruction.Mnemonic, "retn ")) ||
			(0 == _stricmp(disAsm.Instruction.Mnemonic, "int3 "))
			)
		{
			int isretn = 1;
			for (int i = 0; i < JccAddr.size(); i++)
			{
				if ((*(JccAddr.begin() + i)) > disAsm.VirtualAddr - nLen)
				{
					isretn = 0;
					break;
				}
			}
			if (isretn || (0 == _stricmp(disAsm.Instruction.Mnemonic, "int3 ")))
			{
				return ;
			}
		}
	}
}

