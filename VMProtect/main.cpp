#include "TAB_Dialog.h"
#include "Pack.h"
//ȫ��ʵ�����
HINSTANCE g_hInstance;
//����һ��ȫ�ֱ���ָ�룬�������MainDialog�����thisָ��
LPVOID g_pTabDialog;

VMPStruct g_vmstruct;

DWORD dataencrypt = 0;//���ݱ���ܵı���


//�������
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE pPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	TAB_Dialog* ptab = TAB_Dialog::GetInstance();
	//�ͷ��ڴ�
	delete ptab;
	return 0;
}