#pragma once
#include "Common/header.h"

/*
00C4EB31 56                   push        esi
00C4EB32 4D                   dec         ebp
00C4EB33 45                   inc         ebp
00C4EB34 4E                   dec         esi
00C4EB35 44                   inc         esp
*/
//"VMEND"�������
#define VM_END		_asm	push	esi\
					_asm	dec		ebp\
					_asm	inc		ebp\
					_asm	dec		esi\
					_asm	inc		esp


//�������ݱ���ܵı���
extern DWORD dataencrypt;

typedef void (*FUNC)(void);
typedef CString(*HANDLERFUN)(char* , char*);

//MOV��Add��XOR��SUB�ȵ�ָ��
CString vMOV4(char* VR0, char* VR1);
CString vAdd4(char* VR0, char* VR1);
CString vXOR4(char* VR0, char* VR1);
CString vSUB4(char* VR0, char* VR1);

CString vINC(char* VR0, char* VR1);
CString vPUSH(char* VR0, char* VR1);
CString vPOP4(char* VR0, char* VR1);
CString vPOP2(char* VR0, char* VR1);
//�Ƚ�ָ��
CString vTEST(char* VR0, char* VR1);
CString vCMP(char* VR0, char* VR1);

CString vLEA(char* VR0, char* VR1);

//JCCָ���JMPָ��
CString vJGE(char* VR0, char* VR1);
CString vJAE(char* VR0, char* VR1);
CString vJBE(char* VR0, char* VR1);
CString vJB(char* VR0, char* VR1);
CString vJA(char* VR0, char* VR1);
CString vJG(char* VR0, char* VR1);
CString vJL(char* VR0, char* VR1);
CString vJLE(char* VR0, char* VR1);
CString vJZ(char* VR0, char* VR1);
CString vJNZ(char* VR0, char* VR1);
CString vJO(char* VR0, char* VR1);
CString vJNO(char* VR0, char* VR1);
CString vJS(char* VR0, char* VR1);
CString vJNS(char* VR0, char* VR1);
CString vJPE(char* VR0, char* VR1);
CString vJPO(char* VR0, char* VR1);
CString vJCXZ(char* VR0, char* VR1);
CString vJECXZ(char* VR0, char* VR1);
CString vJMP(char* VR0, char* VR1);

//push popָ��
CString vPushReg1_low(char* VR0, char* VR1);//8λ ��λ
CString vPushReg1_above(char* VR0, char* VR1);//8λ ��λ
CString vPushReg2(char* VR0, char* VR1);
CString vPushReg4(char* VR0, char* VR1);
CString vPopReg1_low(char* VR0, char* VR1);
CString vPopReg1_above(char* VR0, char* VR1);
CString vPopReg2(char* VR0, char* VR1);
CString vPopReg4(char* VR0, char* VR1);
CString vPushImm4(char* VR0, char* VR1);

//������ջ
CString vAddEsp(char* VR0, char* VR1);


/*----------------------------------------------------------------------------------*/
/*	��д�ڴ����	      				     */
/*----------------------------------------------------------------------------------*/
//��д�ڴ�ר�ó˷�handler
CString vMUL_MEM(char* VR0, char* VR1);
//���ڴ� ������8λ
CString vReadMemDs1(char* constn, char* dst);
//���ڴ� ������16λ
CString vReadMemDs2(char* constn, char* dst);
//���ڴ� ������32λ
CString vReadMemDs4(char* constn, char* dst);
//д�ڴ� ������8λ
CString vWriteMemDs1(char* constn, char* dst);
//д�ڴ� ������16λ
CString vWriteMemDs2(char* constn, char* dst);
//д�ڴ� ������32λ
CString vWriteMemDs4(char* constn, char* dst);

//���pop�ǲ����ĸ��ֽڵ��ڴ�
CString vPopWirteMem4(char* constn, char* dst);

//���CLLָ���Ǽ����ת(����call dword ptr[eax])
CString vCallMem(char* constn, char* dst);
//���CALLָ������ǼĴ���(���� call eax)
CString vCallReg(char* constn, char* dst);

//����ģ��ָ���
CString vNotSimulate(char* instruction, char* vresumestartaddr);

//call������ģ��ָ��
CString vCALL(char* esistateaddr, char* vresumestartaddr);

//�˳������
CString vRETN(char* VR0, char* VR1);

//������
void  VMDispatcher();

//��ú�������
int GetFunSize(FUNC Func);

//VStartVM ���������
void VStartVM();

//���������1
CString VMStartVM(char* instruction, char* vresumestartaddr);
//���������2
CString VMStartVM_2(char* VR0, char* VR1);

//��ת�����������ʼ��ַ
void JmpToVMStart();

//���VMContext�Ƿ񱻸���
void  VCheckESP();

//vRetnNOT �˳��������ִ�в���ģ��ָ��
CString vRetnNOT_(char* VR0, char* VR1);
//ִ�в���ģ��ָ���,�ٴν��������
CString vResumeStart_2(char* VR0, char* VR1);

//ִ�в���ģ��ָ���,�ٴν��������1
CString vResumeStart_1(char* instruction, char* vresumestartaddr);

//ִ�в���ģ��ָ���,�ٴν��������
void vResumeStart();



