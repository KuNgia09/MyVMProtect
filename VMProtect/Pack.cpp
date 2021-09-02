#include "Pack.h"

//点击保护文件按钮，被加壳的程序会在这个函数里处理
/*
		1、加载被加壳程序，处理IAT表和反调试。
		2、获取Stub文件的PE信息，将必要的信息设置到Stub中。
		3、合并目标PE和Stub.dll以及把处理好的文件存盘
*/
void Pack::StartProtect(HWND hwndDlg, TCHAR* strPath, TCHAR* NewFileBuff, PEInfo peinfo, BYTE byXor, DWORD vminformation)
{
	/*----------------------------------------------------------------------------------*/
	/*	1、加载被加壳程序，处理IAT表和反调试											*/
	/*----------------------------------------------------------------------------------*/
	//1.0 获取目标文件PE信息
	PE pe;

	//1.1 拉伸目标文件
	ULONG_PTR TempBuff = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);

	//1.2 判断IAT加密复选框是否被选中
	if (true)
	{
		//1.2.0 保存IAT表
		SaveImportTab((char*)TempBuff);
		//1.2.1 清理IAT表
		ClearImportTab((char*)TempBuff);
	}

	DealTLS(TempBuff);


	//1.3 判断反调试复选框是否被选中
	if (false)
	{
		PEInfo peinfo_debug;
		//更新peinfo_debug信息
		pe.GetPEInformation_1((char*)TempBuff, &peinfo_debug);
		//绑定机器码
		XorMachineCode(GetCPUID(), peinfo_debug);
		//更多反调试方法请参看Stub项目的PreventDebug类
	}

	//1.4 获取IAT表信息和重定位信息
	getinfo((char*)TempBuff);



	DWORD strip_length;
	//1.5 把拉伸的文件还原成磁盘大小
	ULONG_PTR TempBuff_1 = (ULONG_PTR)pe.ImageBuff_To_FileBuff((char*)TempBuff, &strip_length);

	//1.6 更新peinfo信息
	pe.GetPEInformation_1((char*)TempBuff_1, &peinfo, strip_length);

	//1.7 加密代码段
	DWORD dwVirtualAddr = XorCode(byXor, peinfo);

	//----去掉随机加载基址----
	peinfo.OptionalHeader->DllCharacteristics = 0;

	/*----------------------------------------------------------------------------------*/
	/* 2、获取Stub文件的PE信息，将必要的信息设置到Stub中								*/
	/*----------------------------------------------------------------------------------*/
		//2.1 加载Stub.dll
#ifdef _WIN64	
#ifdef UNICODE
	//HMODULE hModule = LoadLibrary(L"../Stub/x64/Release/Stub.dll");
#else
	//HMODULE hModule = LoadLibrary("../Stub/x64/Release/Stub.dll");
#endif // UNICODE


#else

#ifdef UNICODE
	//HMODULE hModule = LoadLibrary(L"../Stub/Release/Stub.dll");
#else
	HMODULE hModule = LoadLibrary("../Stub/Release/Stub.dll");
#endif // UNICODE

#endif
	//HMODULE hModule = LoadLibrary(L"Stub.dll");
	if (NULL == hModule) hModule = LoadLibrary("Stub.dll");

	if (NULL == hModule)
	{
		MessageBoxA(hwndDlg, "Stub.dll加载失败，请检查路径", "提示", MB_OK);
		return;
	}

	//2.2 把相关信息保存Stub.dll的g_stcParam全局变量里
	//HMODULE 
	PGLOBAL_PARAM pstcParam = (PGLOBAL_PARAM)GetProcAddress(hModule, "g_stcParam");

	pstcParam->dwImageBase = peinfo.ImageBase;
	pstcParam->dwCodeSize = m_encryptCodeSize;
	pstcParam->ulBaseOfCode = peinfo.BaseOfCode;
	pstcParam->dwOEP = peinfo.AddressOfEntryPoint;
	pstcParam->byXor = byXor;
	pstcParam->lpStartVA = (PBYTE)dwVirtualAddr;
	//保存TLS回调函数指针
	pstcParam->stlAddressOfCallback = m_TlsAddressOfCallback;
	//保存IAT的信息到Stub.dll的g_stcParam全局变量里
	pstcParam->stcPEImportDir = m_PEImportDir;
	pstcParam->stcPERelocDir = m_PERelocDir;
	pstcParam->dwIATSectionBase = m_IATSectionBase;
	pstcParam->dwIATSectionSize = m_IATSectionSize;
	pstcParam->dwNumOfIATFuns = m_dwNumOfIATFuns;
	pstcParam->dwSizeOfModBuf = m_dwSizeOfModBuf;

	pstcParam->dwSizeOfFunBuf = m_dwSizeOfFunBuf;
	pstcParam->pProctect.EncryptionIAT = 1;
	pstcParam->pProctect.AddReDebug = 0;

	//传递vm虚拟机信息
	pstcParam->m_vminformation = vminformation;

	//2.3 添加Stub代码段到被加壳程序中
	//2.3.1 读取Stub代码段
	AllocMemory m_alloc;
	MODULEINFO modinfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	pstcParam->dwIATBaseRVA = peinfo.SizeofImage + modinfo.SizeOfImage;

	PBYTE lpMod = m_alloc.auto_malloc<PBYTE>(modinfo.SizeOfImage);
	//拷贝Stub.dll的镜像内存到lpMod
	memcpy_s(lpMod, modinfo.SizeOfImage, hModule, modinfo.SizeOfImage);
	PEInfo stubpeinfo;

	pe.GetPEInformation_1((char*)lpMod, &stubpeinfo);


	//2.3.2 修复重定位表
	ULONG_PTR TarSizeofImage = pe.AlignSize(peinfo.SizeofImage, peinfo.OptionalHeader->SectionAlignment);
	ULONG_PTR value = stubpeinfo.ImageBase - (peinfo.OptionalHeader->ImageBase + TarSizeofImage);

	if (value != 0)
	{
		//给Stub.dll添加重定位
		pe.PerformBaseRelocation((ULONG_PTR)lpMod, value);
	}

	//2.3.3 修改被加壳程序的OEP，指向stub
	peinfo.OptionalHeader->AddressOfEntryPoint = TarSizeofImage + (pstcParam->dwStart - (ULONG_PTR)hModule);

	//2.3.4 添加新节存放Stub.dll
	pe.addSeciton(peinfo.FileBuffer, stubpeinfo.SizeofImage, (char*)".vmp0");

	//2.3.5 去掉Stub.dll的PE特征
	/*for (int i = 0; i < 0x380; i++)
	{
		*(char*)(stubpeinfo.FileBuffer + i) ^=  (0x15+rand()%1000);
	}*/


	/*----------------------------------------------------------------------------------*/
	/*	3、合并目标PE和Stub.dll以及把处理好的文件存盘									*/
	/*----------------------------------------------------------------------------------*/
		//3.0 合并目标PE和Stub.dll
	PBYTE NewBuffer = MergeSection(peinfo, stubpeinfo, lpMod, byXor);

	//3.1 清除不需要的目录信息
	ClearDirTable((char*)NewBuffer);

	//3.2 保存文件
	SaveFile_pack(strPath, (char*)NewBuffer, m_uTotalSize);

	//3.3 释放Stub.dll
	FreeLibrary(hModule);
	return;
}


//机器码绑定(将CPU序列号同主程序入口进行亦或)
void Pack::XorMachineCode(ULONGLONG cpuId, PEInfo& peinfo)
{
	//获取被加壳程序的OEP
	ULONG_PTR uCodeBase = peinfo.FileBuffer + peinfo.AddressOfEntryPoint;

	//与代码段进行亦或
	*(ULONGLONG*)uCodeBase ^= cpuId;
}

//获取CPU序列号(64位与32位通用)
ULONGLONG Pack::GetCPUID()
{
	ULONGLONG cpuId = 0;
	__asm {
		mov eax, 01h   //eax=1:取CPU序列号
		xor edx, edx
		cpuid
		mov dword ptr cpuId[0], eax
		mov dword ptr cpuId[4], edx
	}
	return cpuId;
}

void Pack::DealTLS(ULONG_PTR imgBuff) {
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imgBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)imgBuff + pDosHeader->e_lfanew);

	IMAGE_DATA_DIRECTORY tlsDataDirectory = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

	//没有TLS返回
	if (tlsDataDirectory.VirtualAddress == 0) {
		m_isUseTLS = false;
		return;
	}
	//如果有TLS
	PIMAGE_TLS_DIRECTORY tlsDir = (PIMAGE_TLS_DIRECTORY)(tlsDataDirectory.VirtualAddress + (DWORD)imgBuff);
	//保存TLS回调函数
	m_TlsAddressOfCallback = tlsDir->AddressOfCallBacks;
	
	//防止壳代码运行之前执行TLS回调
	tlsDir->AddressOfCallBacks = 0;
	

	
}

//加密代码段
DWORD Pack::XorCode(BYTE byXOR, PEInfo peinfo)
{
	//判断基址在哪一节
	ULONG Entry = peinfo.BaseOfCode;
	PIMAGE_SECTION_HEADER Temp = peinfo.pSectionHeader;

	for (int i = 0; i < peinfo.NumberOfSections; i++)
	{
		if (Entry >= Temp->PointerToRawData && Entry < Temp->PointerToRawData + Temp->SizeOfRawData)
		{
			break;
		}
		++Temp;
	}

	//在内存中找到代码段的相对偏移位置
	PBYTE pBase = (PBYTE)(Temp->PointerToRawData + (ULONGLONG)peinfo.FileBuffer);

	//做异或运算加密
	/*for (int i = 0; i < peinfo.SizeOfCode; i++)
	{
		pBase[i] ^= byXOR;
	}*/

	m_encryptCodeSize = Temp->SizeOfRawData;
	//这里不取peinfo.SizeOfCode 取SizeOfRawData 
	for (int i = 0; i < m_encryptCodeSize; i++)
	{
		pBase[i] ^= byXOR;
	}
	return Temp->VirtualAddress;
}

//获取IAT表信息和重定位信息
void Pack::getinfo(char* m_pFileBuf)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)m_pFileBuf;
	PIMAGE_NT_HEADERS m_pNtHeader = (PIMAGE_NT_HEADERS)(m_pFileBuf + pDosHeader->e_lfanew);


	//保存重定位目录信息
	m_PERelocDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);

	//保存IAT信息目录信息
	m_PEImportDir =
		IMAGE_DATA_DIRECTORY(m_pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]);

	//获取IAT所在的区段的起始位置和大小
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(m_pNtHeader);
	for (DWORD i = 0; i < m_pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		//获取IAT所在区段
		if (m_PEImportDir.VirtualAddress >= pSectionHeader->VirtualAddress &&
			m_PEImportDir.VirtualAddress < pSectionHeader[1].VirtualAddress)
		{
			//保存该区段的起始地址和大小
			m_IATSectionBase = pSectionHeader->VirtualAddress;
			m_IATSectionSize = pSectionHeader[1].VirtualAddress - pSectionHeader->VirtualAddress;
			break;
		}
	}
}

//保存IAT表
void Pack::SaveImportTab(char* m_pFileBuf)
{
	//0.获取导入表结构体指针
	PE pe;
	DWORD Virtual = pe.GET_HEADER_DICTIONARY((ULONG_PTR)m_pFileBuf, 1);
	if (Virtual == 0)
	{
		return;
	}
	PIMAGE_IMPORT_DESCRIPTOR pPEImport = (PIMAGE_IMPORT_DESCRIPTOR)(Virtual + m_pFileBuf);
	//1.第一遍循环确定 m_pModNameBuf 和 m_pFunNameBuf 的大小
	//所有导入表dll名字长度和
	DWORD dwSizeOfModBuf = 0;
	//所有导入表函数名字长度和
	DWORD dwSizeOfFunBuf = 0;
	m_dwNumOfIATFuns = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		dwSizeOfModBuf += (strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			// 判断Ordinal最高位 为1时，表示函数以序号方式输入
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				m_dwNumOfIATFuns++;
			}
			//判断Ordinal最高位 为1时，表示函数以字符串类型方式输入，这时双字的值是指向字符串的RVA
			else
			{
				m_dwNumOfIATFuns++;
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				dwSizeOfFunBuf += (strlen(pstcFunName->Name) + 1);
			}
			pIAT++;
		}
		pPEImport++;
	}

	//2.第二遍循环保存信息

	//申请内存保存IAT表信息
	m_pModNameBuf = m_allocMemory.auto_malloc<PVOID>(dwSizeOfModBuf);
	m_pFunNameBuf = m_allocMemory.auto_malloc<PVOID>(dwSizeOfFunBuf);
	m_pMyImport = m_allocMemory.auto_malloc<PMYIMPORT>(m_dwNumOfIATFuns * sizeof(MYIMPORT));


	pPEImport = (PIMAGE_IMPORT_DESCRIPTOR)(Virtual + m_pFileBuf);
	ULONG_PTR TempNumOfFuns = 0;
	ULONG_PTR TempModRVA = 0;
	ULONG_PTR TempFunRVA = 0;
	while (pPEImport->Name)
	{
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		//拷贝导入表dll名字到m_pModNameBuf
		memcpy_s((PCHAR)m_pModNameBuf + TempModRVA, strlen(pModName) + 1,
			pModName, strlen(pModName) + 1);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);

		while (pIAT->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//保存以序号出方式的函数信息
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (ULONG_PTR)pIAT - (ULONG_PTR)m_pFileBuf;
				m_pMyImport[TempNumOfFuns].m_bIsOrdinal = TRUE;
#ifdef _WIN64
				m_pMyImport[TempNumOfFuns].m_Ordinal = pIAT->u1.Ordinal & 0x7FFFFFFFFFFFFFFF;
#else
				m_pMyImport[TempNumOfFuns].m_Ordinal = pIAT->u1.Ordinal & 0x7FFFFFFF;
#endif // DEBUG

				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
			}
			else
			{
				//保存名称号出方式的函数信息
				m_pMyImport[TempNumOfFuns].m_dwIATAddr = (ULONG_PTR)pIAT - (ULONG_PTR)m_pFileBuf;

				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				memcpy_s((PCHAR)m_pFunNameBuf + TempFunRVA, strlen(pstcFunName->Name) + 1,
					pstcFunName->Name, strlen(pstcFunName->Name) + 1);

				m_pMyImport[TempNumOfFuns].m_dwFunNameRVA = TempFunRVA;
				m_pMyImport[TempNumOfFuns].m_dwModNameRVA = TempModRVA;
				TempFunRVA += (strlen(pstcFunName->Name) + 1);
			}
			TempNumOfFuns++;
			pIAT++;
		}
		TempModRVA += (strlen(pModName) + 1);
		pPEImport++;
	}

	//逆序排列m_pMyImport
	MYIMPORT stcTemp = { 0 };
	DWORD dwTempNum = m_dwNumOfIATFuns / 2;
	for (DWORD i = 0; i < dwTempNum; i++)
	{
		m_pMyImport[i];
		m_pMyImport[m_dwNumOfIATFuns - i - 1];
		memcpy_s(&stcTemp, sizeof(MYIMPORT), &m_pMyImport[i], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[i], sizeof(MYIMPORT), &m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT));
		memcpy_s(&m_pMyImport[m_dwNumOfIATFuns - i - 1], sizeof(MYIMPORT), &stcTemp, sizeof(MYIMPORT));
	}

	//保存信息
	m_dwSizeOfModBuf = dwSizeOfModBuf;
	m_dwSizeOfFunBuf = dwSizeOfFunBuf;
}

//清除IAT表
void Pack::ClearImportTab(char* m_pFileBuf)
{
	PE pe;
	DWORD DirData = pe.GET_HEADER_DICTIONARY((ULONG_PTR)m_pFileBuf, 1);
	//1、获取导入表结构体指针
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(m_pFileBuf + DirData);

	//2.开始循环抹去IAT(导入表)数据
	//每循环一次抹去一个Dll的所有导入信息
	while (pPEImport->Name)
	{
		//2.1.抹去模块名
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(m_pFileBuf + dwModNameRVA);
		memset(pModName, 0, strlen(pModName));

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->FirstThunk);
		PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(m_pFileBuf + pPEImport->OriginalFirstThunk);

		//2.2. 抹去IAT、INT和函数名函数序号
		while (pIAT->u1.AddressOfData)
		{
			//判断是输出函数名还是序号
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//抹去序号就是将pIAT清空
			}
			else
			{
				//输出函数名
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(m_pFileBuf + dwFunNameRVA);
				//清除函数名和函数序号
				memset(pstcFunName, 0, strlen(pstcFunName->Name) + sizeof(WORD));
			}
			memset(pINT, 0, sizeof(IMAGE_THUNK_DATA));
			memset(pIAT, 0, sizeof(IMAGE_THUNK_DATA));
			pINT++;
			pIAT++;
		}

		//2.3.抹去导入表目录信息
		memset(pPEImport, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));

		//遍历下一个模块
		pPEImport++;
	}

}

//清除不需要的目录表信息
void Pack::ClearDirTable(char* filebuff)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)filebuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(filebuff + pDosHeader->e_lfanew);

	//清除不需要的目录表信息
	//只留输出表，重定位表，资源表
	DWORD dwCount = 15;
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (i != IMAGE_DIRECTORY_ENTRY_EXPORT &&
			i != IMAGE_DIRECTORY_ENTRY_RESOURCE &&
			i != IMAGE_DIRECTORY_ENTRY_BASERELOC && 
			i!= IMAGE_DIRECTORY_ENTRY_TLS)
		{
			pNtHeader->OptionalHeader.DataDirectory[i].VirtualAddress = 0;
			pNtHeader->OptionalHeader.DataDirectory[i].Size = 0;
		}
	}
}


//合并目标PE和Stub.dll
PBYTE Pack::MergeSection(PEInfo peinfo, PEInfo stubpeinfo, PBYTE lpMod, BYTE byXor)
{
	PE pe;
	//1、 计算保存IAT所用的空间大小
	DWORD dwIATSize = 0;
	dwIATSize = m_dwSizeOfModBuf + m_dwSizeOfFunBuf + m_dwNumOfIATFuns * sizeof(MYIMPORT);

	if (dwIATSize % peinfo.SectionAlignment)
	{
		dwIATSize = (dwIATSize / peinfo.SectionAlignment + 1) * peinfo.SectionAlignment;
	}
	if (dwIATSize != 0)
	{
		//添加新节存放IAT表
		pe.addSeciton(peinfo.FileBuffer, dwIATSize, (char*)".vmp1");
	}

	//获取最后一个区段的偏移和大小


	//2、 申请新内存合并目标PE和Stub.dll
	//因为文件可能有附加数据 文件真正大小可能没有文件对齐
	ULONG_PTR TarFileSize = pe.AlignSize(peinfo.FileSize, peinfo.OptionalHeader->FileAlignment);//被加壳程序对齐后的文件大小
	ULONG_PTR TotalSize = stubpeinfo.SizeofImage + TarFileSize + dwIATSize;
	m_uTotalSize = TotalSize;//记录合并后的总大小,保存文件时要用到
	PBYTE NewBuffer = m_allocMemory.auto_malloc<PBYTE>(TotalSize + 10);
	memset(NewBuffer, 0, TotalSize + 10);

	// 第一步拷贝peinfo
	memcpy_s(NewBuffer, peinfo.FileSize, (char*)peinfo.FileBuffer, peinfo.FileSize);
	//第二部拷贝Stub dll 内容
	memcpy_s(NewBuffer + TarFileSize, stubpeinfo.SizeofImage, lpMod, stubpeinfo.SizeofImage);

	//3、如果选择了加密IAT，则把IAT数据到拷贝 '.vmp1' 节
	//拷贝IAT信息
	if (dwIATSize == 0)
	{
		return NewBuffer;
	}
	ULONG_PTR dwIATBaseRVA = TarFileSize + stubpeinfo.SizeofImage;

	// 拷贝MYIMPORT结构信息
	memcpy_s(NewBuffer + dwIATBaseRVA,
		dwIATSize, m_pMyImport, m_dwNumOfIATFuns * sizeof(MYIMPORT));

	//加密模块名
	for (DWORD i = 0; i < m_dwSizeOfModBuf; i++)
	{
		if (((char*)m_pModNameBuf)[i] != 0)
		{
			((char*)m_pModNameBuf)[i] ^= byXor;
		}
	}

	//拷贝模块名
	memcpy_s(NewBuffer + dwIATBaseRVA + m_dwNumOfIATFuns * sizeof(MYIMPORT),
		dwIATSize, m_pModNameBuf, m_dwSizeOfModBuf);

	//IAT函数名加密
	for (DWORD i = 0; i < m_dwSizeOfFunBuf; i++)
	{
		if (((char*)m_pFunNameBuf)[i] != 0)
		{
			((char*)m_pFunNameBuf)[i] ^= byXor;
		}
	}

	//拷贝函数名
	memcpy_s(NewBuffer + dwIATBaseRVA + m_dwNumOfIATFuns * sizeof(MYIMPORT) + m_dwSizeOfModBuf,
		dwIATSize, m_pFunNameBuf, m_dwSizeOfFunBuf);

	return NewBuffer;
}

//保存文件
void Pack::SaveFile_pack(TCHAR* strPath, char* NewBuffer, ULONG_PTR m_uTotalSize)
{
	FileOperation fileopt;

	CString str = strPath;
	int cutsize = str.RightFindChar(strPath, 0x5c);//0x5c是‘\’这个字符的二进制
	CString s = str.MidCut(cutsize + 1, _tcslen(strPath) - cutsize - 5);


	TCHAR Path_Out[MAX_PATH];
	_tgetcwd(Path_Out, MAX_PATH);//获取当前程序的路径

	str = Path_Out;
#ifdef UNICODE
	str = str + L"\\" + s.GetString() + L"_vmp.exe";
#else
	str = str + "\\" + s.GetString() + "_vmp.exe";
#endif // !UNICODE


	bool res = fileopt.SaveFile_((char*)NewBuffer, m_uTotalSize, str.GetString());
	if (res == 0)
	{
		MessageBoxW(NULL, L"文件保存失败!!", L"温馨提示", MB_ICONHAND);
		return;
	}
	TCHAR temp[] = _T("加壳成功！\r\n文件所在路径：\r\n");
	CString ss = temp;
	ss += str;
#ifdef UNICODE
	MessageBoxW(NULL, ss.GetString(), L"温馨提示", MB_ICONINFORMATION);
#else
	MessageBoxA(NULL, ss.GetString(), "温馨提示", MB_ICONINFORMATION);
#endif // !UNICODE

}
