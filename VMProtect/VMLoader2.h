#pragma once
#include "holder.h"
//ָ���
#define STRUCTIONLENGTH 0x38
//����������
#define OPECODELENGTH 0x16


class VMLoader2
{
public://�ṹ����Ϣ

	//��¼�������λ��
	struct BASICBLOCKADDR
	{
		DWORD nmatch = 0;//����
		int index = -1;//����
		DWORD addr = 0;
		char instruction[STRUCTIONLENGTH] = { 0 };//ָ��
	};
	vector<BASICBLOCKADDR>m_basic;
	BASICBLOCKADDR temp_basic = { 0 };

	//��¼�м��ʾ�Ľṹ��
	struct MIDDLESTRUCT
	{
		DWORD originaddr = 0;//ԭʼ��ַ
		char vmfunc[OPECODELENGTH] = { 0 };//handler
		char param1[STRUCTIONLENGTH] = { 0 };//����һ
		char param2[STRUCTIONLENGTH] = { 0 };//������
	};
	vector<MIDDLESTRUCT>m_middle;

	//Ԥ������Ϣ�Ľṹ��
	struct PREINFORMATION
	{
		char vmfunc[OPECODELENGTH] = { 0 };//handler
		vector<JMPTABLE>temp_jmptbl;
	};
	vector<PREINFORMATION>m_preinformation;


	//��ת��
	vector<JMPTABLE>m_jmptbl;

	//���ݱ�
	vector<DATATABLE>m_datatable;


public:

	VMLoader2(VMPStruct vmps)
	{//��ʼ��		
		InitialVM(vmps);
	}
	VMLoader2() {};
	~VMLoader2() {};

public:
	//��ʼ��
	void InitialVM(VMPStruct vmps);

	//��vmtable32�ṹ������ĳ���
	int VMLength();

	//����ָ���ָ���ŵ�m_basic����
	void XEParseIns(char* str);
public:
	//������
	void Construct_Rub_Instruction(char* jmpdispatcher, int index,int nmatch=0);

	//JCCָ���
	void Construct_Jcc_Instruction(char* jccstr, int index);

	//������һ�ͻ������
	void Construct_BasicBlock_Instruction(char* instruction, char* jmpdispatcher, int nbool=3);

	//�������������Ŀ
	void Construct_Rub_Instruction_Num(char* jmpdispatcher, int index);

	//�޸�m_basic�����������
	void Modify_BasicData();

	//��m_basic��ŵ�ָ��ŵ�������
	void SaveInstructionToBuffer();

	
	//��������������ָ���
	void Const_BasicAndRub_Block(char* instruction, int index);

	//�ж�ָ���Ƿ��ܹ�����
	bool IsAnalysis(char* str);

	//���ָ����������
	void ColdTreatment(char* str);

	//����handler(Ԥ���غ���)
	void PreLoadHandler();

	//ɨ��Ҫ������Ӳ���룬����Ϊ���
	void DisassembleEngine();


	//��������ָ�����vm������ʼǰ
	void Out_Of_Order_RubInstruction();

	//��ͨhandler���������handler����
	void TransformToHandler_1(int index, bool isGeneral);

	//���м��ʾ����handler����������ָ��
	void TransformToHandler();

	//�޸����ݱ������
	void ModifyDataTbl();

public:
	//�ָ�ebp��esi��vm������ݵĵ�ַ�ĺ���
	void ResumeEsiAndEbp();

	//������
	void MainVMP();

	//�����������ָ��
	CString ProduceRubbishOpecode(char* reg03, char* reg04);

	//�����ڴ棬���ڴ淭��λ�м��ʾ
	void MemoryMiddle(DISASM disAsm,MEMORYTYPE memtype);

	//����Ҫ������ָ�����Ϊ�м��ʾ
	void MiddleRepresent(DISASM disAsm);

public:
	//��ȡ�����(��Χ��Min_��MAX_)
	DWORD SrandNum(int Min_, int Max_)
	{
		return rand() % (Max_ - Min_) + Min_;
	}
public:
	//vmtable32�ṹ������ĳ���
	int m_vmlength = 0;
	VMPStruct m_vmps = { 0 };

	//��ѡ�Ĵ���
	char* m_reg04;
	char* m_reg05;

	//��¼ָ���ŵ�������
	int m_szcalc=0;
	int m_szcalc_2 = 0;

	//����
	bool m_bool = true;

	//���ָ��Ŀ���,�����ǰ����ָ��ĵ�ַ������ת����
	bool cold_bool = true;
	//ָ��ԭʼ��ַ
	DWORD m_origin = 0;

	//��ʼָ��
	bool start_bool = true;
};

