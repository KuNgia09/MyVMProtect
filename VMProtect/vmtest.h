#pragma once
#include "Common/header.h"

/*
00C4EB31 56                   push        esi
00C4EB32 4D                   dec         ebp
00C4EB33 45                   inc         ebp
00C4EB34 4E                   dec         esi
00C4EB35 44                   inc         esp
*/
//"VMEND"结束标记
#define VM_END		_asm	push	esi\
					_asm	dec		ebp\
					_asm	inc		ebp\
					_asm	dec		esi\
					_asm	inc		esp


//声明数据表加密的变量
extern DWORD dataencrypt;

typedef void (*FUNC)(void);
typedef CString(*HANDLERFUN)(char* , char*);

//MOV、Add、XOR、SUB等等指令
CString vMOV4(char* VR0, char* VR1);
CString vAdd4(char* VR0, char* VR1);
CString vXOR4(char* VR0, char* VR1);
CString vSUB4(char* VR0, char* VR1);

CString vINC(char* VR0, char* VR1);
CString vPUSH(char* VR0, char* VR1);
CString vPOP4(char* VR0, char* VR1);
CString vPOP2(char* VR0, char* VR1);
//比较指令
CString vTEST(char* VR0, char* VR1);
CString vCMP(char* VR0, char* VR1);

CString vLEA(char* VR0, char* VR1);

//JCC指令和JMP指令
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

//push pop指令
CString vPushReg1_low(char* VR0, char* VR1);//8位 低位
CString vPushReg1_above(char* VR0, char* VR1);//8位 高位
CString vPushReg2(char* VR0, char* VR1);
CString vPushReg4(char* VR0, char* VR1);
CString vPopReg1_low(char* VR0, char* VR1);
CString vPopReg1_above(char* VR0, char* VR1);
CString vPopReg2(char* VR0, char* VR1);
CString vPopReg4(char* VR0, char* VR1);
CString vPushImm4(char* VR0, char* VR1);

//提升堆栈
CString vAddEsp(char* VR0, char* VR1);


/*----------------------------------------------------------------------------------*/
/*	读写内存操作	      				     */
/*----------------------------------------------------------------------------------*/
//读写内存专用乘法handler
CString vMUL_MEM(char* VR0, char* VR1);
//读内存 操作数8位
CString vReadMemDs1(char* constn, char* dst);
//读内存 操作数16位
CString vReadMemDs2(char* constn, char* dst);
//读内存 操作数32位
CString vReadMemDs4(char* constn, char* dst);
//写内存 操作数8位
CString vWriteMemDs1(char* constn, char* dst);
//写内存 操作数16位
CString vWriteMemDs2(char* constn, char* dst);
//写内存 操作数32位
CString vWriteMemDs4(char* constn, char* dst);

//如果pop是操作四个字节的内存
CString vPopWirteMem4(char* constn, char* dst);

//如果CLL指令是间接跳转(比如call dword ptr[eax])
CString vCallMem(char* constn, char* dst);
//如果CALL指令后面是寄存器(比如 call eax)
CString vCallReg(char* constn, char* dst);

//不可模拟指令函数
CString vNotSimulate(char* instruction, char* vresumestartaddr);

//call，不可模拟指令
CString vCALL(char* esistateaddr, char* vresumestartaddr);

//退出虚拟机
CString vRETN(char* VR0, char* VR1);

//调度器
void  VMDispatcher();

//获得函数长度
int GetFunSize(FUNC Func);

//VStartVM 进入虚拟机
void VStartVM();

//进入虚拟机1
CString VMStartVM(char* instruction, char* vresumestartaddr);
//进入虚拟机2
CString VMStartVM_2(char* VR0, char* VR1);

//跳转到虚拟机的起始地址
void JmpToVMStart();

//检查VMContext是否被覆盖
void  VCheckESP();

//vRetnNOT 退出虚拟机，执行不可模拟指令
CString vRetnNOT_(char* VR0, char* VR1);
//执行不可模拟指令后,再次进入虚拟机
CString vResumeStart_2(char* VR0, char* VR1);

//执行不可模拟指令后,再次进入虚拟机1
CString vResumeStart_1(char* instruction, char* vresumestartaddr);

//执行不可模拟指令后,再次进入虚拟机
void vResumeStart();



