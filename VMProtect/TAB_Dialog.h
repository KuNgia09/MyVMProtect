#pragma once
#include "holder.h"

class TAB_Dialog
{
public:
	~TAB_Dialog() {};
	ULONG_PTR m_gdiplusToken;//��ʼ��GDIҪ�õ��ı���
private:
	TAB_Dialog() {};

	static void Show()
	{
		//������ģʽ�Ի���		
		DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_TAB_DIALOG), NULL, DialogProc);
	}
public:
	//����ģʽ
	static TAB_Dialog* GetInstance()
	{
		static TAB_Dialog* ptb = new TAB_Dialog();
		g_pTabDialog = ptb;
		Show();
		return ptb;
	}

public:
	//��ʼ��(����ͼ���)
	void OnInitDialog(HWND hwndlg,LPARAM lParam);
	//��ʼ��tab��ǩ
	BOOL Tab_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

	//������������Ϣѭ���Ļص�����
	static INT_PTR CALLBACK DialogProc(HWND hwndlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//��ͼ
	void On_Paint(HWND hwndlg);

public:
	HWND hDlg_intab[2];//����Ҫ���뵽TAB�ؼ��еĶԻ�����
	HWND htabctrl;//TAB�ؼ����

	bool isFile = false;//�Ƿ����ļ�

	//�ļ�·��
	TCHAR str_path[MAX_PATH] = { 0 };

	//��¼������ַ
	TCHAR str_vmaddr[BSTRINGLENGTH] = { 0 };

public:
	static BOOL WINAPI tab1_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//tab1�Ӵ��ڵĴ��ڴ�����̺�������
	static BOOL WINAPI tab2_dlg_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//tab2�Ӵ��ڵĴ��ڴ�����̺�������
	typedef BOOL (WINAPI* DIALOGPROC)(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); //����һ�� ����ָ��
	DIALOGPROC DlgProc[2] = { tab1_dlg_proc,tab2_dlg_proc };

	//��ʼ��
	void IniTialDlg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//�ӿؼ���Ϣ
	void OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	////��ק�ļ�������,�������ļ�·��
	TCHAR* _DropsFile(HWND hwnDlg, WPARAM wParam);

	//���ļ��������ļ�·��
	TCHAR* GetFilePath(HWND hwnDlg);

	//�����,���㺯���ĳ���
	void Disassembler_(HWND hwnd, DWORD virtualaddr, DWORD instruction);

};

