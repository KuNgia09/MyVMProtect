#include "TAB_Dialog.h"
#include "Common/header.h"
HWND g_hwnd;
HWND g_hwnd2;
//��ʼ��
void TAB_Dialog::OnInitDialog(HWND hwndlg, LPARAM lParam)
{	
	// ��ʼ��GDI+  
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//����ͼ��
	HICON hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_LT));
	SendMessage(hwndlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);//���ô�ͼ��
	SendMessage(hwndlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);//���ô�ͼ��

	//tab��ǩ��ʼ��
	Tab_OnInitDialog(hwndlg, hwndlg, lParam);
}

//������ѭ��
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
				case 0://ѡ��TAB1��ǩʱ����ʾtab1��ǩҳ��
				{
					ShowWindow(pt->hDlg_intab[0], TRUE);
					ShowWindow(pt->hDlg_intab[1], FALSE);
				}
					break;
				case 1://ѡ��TAB2��ǩʱ����ʾtab2��ǩҳ��
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
	case WM_INITDIALOG://��ʼ��
		pt->OnInitDialog(hwndlg, lParam);
		break;

	case WM_CLOSE://�˳�
		EndDialog(hwndlg,0);
		break;
	default:
		break;
	}
    return FALSE;
}

//tab��ǩ��ʼ��
BOOL TAB_Dialog::Tab_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//��ʾtab��ǩ
	htabctrl = GetDlgItem(hwnd, IDC_TAB1);
	TCITEM tie;//���ñ�ǩ������
	const TCHAR* tabname[3] = { "��Ŀ","ѡ��"};
	//��������tie���ֶ�
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	for (int i = 0; i < 2; i++)
	{
		tie.pszText = (LPSTR)tabname[i];
		TabCtrl_InsertItem(htabctrl, i, &tie);
	}

	//��ʾ��Ŀؼ�
	RECT rect;//���tab�ؼ�������λ��
	GetClientRect(htabctrl, &rect);

	//������������tab�ؼ�λ���ƶ�
	hDlg_intab[0] = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), htabctrl, DlgProc[0]);
	MoveWindow(hDlg_intab[0], -1, 21, rect.right - rect.left+2, rect.bottom - rect.top, FALSE);
	hDlg_intab[1] = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_OPT_DIALOG), htabctrl, DlgProc[1]);
	MoveWindow(hDlg_intab[1], -1, 21, rect.right - rect.left, rect.bottom - rect.top, FALSE);
	ShowWindow(hDlg_intab[0], SW_SHOW);
	return TRUE;
}

//��ͼ
void TAB_Dialog::On_Paint(HWND hwndlg)
{
	int width, height;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwndlg, &ps);

	//����ͼ��
	//Image image(L"beat.png");
	Image image(L"timg.jpg");

	/*if (image.GetLastStatus() != Status::Ok) {
		MessageBoxW(hwndlg, L"����ͼƬʧ��!", L"��ʾ", MB_OK);
		return;
	}*/

	RECT rctA; //����һ��RECT�ṹ�壬�洢���ڵĳ����
	::GetWindowRect(hwndlg, &rctA);//ͨ�����ھ����ô��ڵĴ�С�洢��rctA�ṹ��

	//ȡ�ÿ�Ⱥ͸߶�
	//width = rctA.right - rctA.left;   //���ڵĿ��
	//height = rctA.bottom - rctA.top;  //���ڵĸ߶�		
	width = image.GetWidth();
	height = image.GetHeight();

	//��ͼ
	Graphics graphics(hdc);
	graphics.DrawImage(&image, 0, 0, width, height);
	EndPaint(hwndlg, &ps);
}

vector<DWORD>temp_addr;

//�̻߳ص�����
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	TAB_Dialog* pt = (TAB_Dialog * )lpParameter;
	

	VMStart(pt->str_path,temp_addr);

	CString str_notify = "";
	TCHAR str_1[BSTRINGLENGTH] = { 0 };
	GetDlgItemTextA(g_hwnd, IDC_EDIT_ADDR, str_1, BSTRINGLENGTH);
	str_notify = str_notify + str_1 + "\r\n" + "�ӿ����!!!";
	SetDlgItemTextA(g_hwnd, IDC_SHOW_EDIT, str_notify.GetString());
	return 0;
}

//tab1��ǩ�ص�����
BOOL __stdcall TAB_Dialog:: tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TAB_Dialog* pt = (TAB_Dialog*)g_pTabDialog;
	g_hwnd = hwnd;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{		
		////���ý�����
		//HWND Proc = GetDlgItem(hwnd, IDC_PROGRESS1);
		//SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));// ����Ĭ�ϵķ�Χֵ	
		//SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_SETPOS, 500, 0);// ���õ�ǰ��λ
		//int x = SendDlgItemMessage(hwnd, IDC_PROGRESS1, PBM_GETPOS, 0, 0);//��ȡ��������ֵ
	}
		break;
	case WM_DROPFILES://��ק������
	{
		pt->_DropsFile(hwnd, wParam);
		
	}
	break;
	case WM_COMMAND:
	{		
		switch (wParam)
		{
		case IDC_PRO_BUTTON://����
		{
			memcpy(pt->str_path, "D:\\project\\MyVMProtect\\Debug\\VMProtect - ����.exe",strlen("D:\\project\\MyVMProtect\\Debug\\VMProtect - ����.exe") );
			if (pt->str_path[0] == 0)
			{
				MessageBoxA(NULL, "�����PE��ʽ�ļ�!", "����", MB_OK);
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
								
				//�ַ�ת���֣�
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
			//�����߳�
			HANDLE hTread = CreateThread(NULL, 0, ThreadProc, (LPVOID)pt, 0, NULL);
			
			CString str_notify = "���ڼӿǣ������ĵȴ�......";
			SetDlgItemTextA(hwnd, IDC_SHOW_EDIT, str_notify.GetString());
			//MessageBoxA(NULL, "ȷ�����룿", "��ʾ", MB_OK);

			temp_addr.clear();
		}

			break;
		case IDC_OPEN_BUTTON://���ļ�
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
//tab2��ǩ�ص�����
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
			//MessageBoxA(NULL, "�༭��ı���", "��ʾ", MB_OK);
			
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
	CString str = "1������༭���ʽ 401000����0x401000����401000h  ʮ�����ơ�\r\n";
	str = str + "2��������Ӵ��󣬻������ظ���ֱ�����������ߵı༭��������ɡ�\r\n";
	str = str + "3����ʱֻ֧��Release�棬��֧��Debug��!\n";
	SetDlgItemTextA(hwnd, IDC_EDIT_ASSEM, str.GetString());
}

//�����ӿؼ���Ϣ
void TAB_Dialog::OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDC_BUTTON_ADD://���
	{
		/*if (isFile==false)
		{
			MessageBoxA(NULL, "�����PE��ʽ�ļ���������Ӻ���!", "��ʾ", MB_OK);
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
	case IDC_BUTTON_ASSEM://�鿴�����
	{
		if (isFile == false)
		{
			MessageBoxA(NULL, "�����PE��ʽ�ļ�!", "��ʾ", MB_OK);
			return;
		}

		PEInfo peinfo;
		PE pe;
		pe.GetPEInformation_(str_path, &peinfo);

		//�����ļ�
		ULONG_PTR StretchAddr = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);
		//����peinfo��Ϣ
		pe.GetPEInformation_1((char*)StretchAddr, &peinfo, peinfo.FileSize);

		
		TCHAR str_addr[STRINGLENGTH] = { 0 };
		GetDlgItemTextA(g_hwnd2, IDC_EDIT_INPUT, str_addr, STRINGLENGTH);
		CString cstr = "";
		DWORD funaddr = cstr.Str_To_Num(str_addr);
		//��������ʼλ��
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

	hDrop = (HDROP)wParam;  //wParamΪ��Ϣ�ĵ�һ������
	Num = DragQueryFile(hDrop, -1, File, 256); //�������ļ�����
	for (n = 0; n < Num; n++) {
		DragQueryFile(hDrop, n, File, MAX_PATH); //ȡ��ÿ���ļ����ļ���

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
	TCHAR szPeFileExt[100] = _T("*.exe;*.dll;*.scr;*.drv;*.sys");//�򿪵��ļ�����
	TCHAR szFileName[256];
	DWORD start_filename = 0;//�ļ������ַ����еĿ�ʼλ��
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

//JCCָ���ַ���
#define JCCNUMSS 30
const char* JCCSTR[JCCNUMSS] = {
	"JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ",
	"JPO ","JO ","JNO ","JC","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
	"JNBE ","JA ","JL","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG "
};

void TAB_Dialog::Disassembler_(HWND hwnd,DWORD virtualaddr, DWORD instruction)
{
	int inslength = 0;//��¼ָ�������

	vector<DWORD>JccAddr;//��¼JCC����ĵ�ַ

	DISASM disAsm = { 0 };

	// 3. ���ýṹ�壬��ʼ��������opcode
	disAsm.EIP = (UIntPtr)instruction;
	disAsm.VirtualAddr = virtualaddr; // opcode ָ��ĵ�ַ
	disAsm.Archi = 0; // 0 => 32 , 1 => 64
	disAsm.Options = 0x000; // masm ���ָ���ʽ

	int nCount = 0;// ���ڼ�¼��ѭ�����У�������˶��ٸ��ֽ�
	int nLen = 0; // ���ڼ�¼��ǰ�Ļ��ָ����ֽ���
	CString str = "";
	int x = 0;
	while (true)
	{
		nLen = Disasm(&disAsm); // ÿ��ֻ�����һ�����ָ� ���ҷ��ص�ǰ�õ��Ļ��ָ��ĳ���
		unsigned int uAddr = disAsm.VirtualAddr;

		//printf("%08X | ", uAddr); // ��ӡ��ַ
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
		//printf(" | %s\n", disAsm.CompleteInstr); // ��ӡ�����ָ��

		if (inslength == 0x1000)
		{//��ֹ��ѭ��
			MessageBoxA(NULL, "δ֪���󣬶�ȡָ��ʧ��!!", "��ʾ", MB_OK);
			return;
		}

		++inslength;//��¼ָ���������Ϊ���������ڴ��ṩ����
		nCount += nLen; // �ۼ��Ѿ��������ֽ���
		disAsm.EIP += nLen; // ��λ����һ�����ָ��
		disAsm.VirtualAddr += nLen; // ���õ���һ�����ָ��ĵ�ַ

		//�������jccָ�����jmpָ���¼����ĵ�ַ
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

