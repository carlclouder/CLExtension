#pragma once
#ifndef __CL_PROCESSINFO_H__
#define __CL_PROCESSINFO_H__



#include <windows.h>  
#include <imagehlp.h> 
#pragma comment(lib, "imagehlp.lib ")
#include <tlhelp32.h> 
#include <vector>  
#include <tchar.h>
#include <assert.h>
#include "../_cl_common/CLCommon.h"


using namespace std;

//保存函数ThunkData相关信息结构体
typedef struct _FUNCTION
{
	PIMAGE_IMPORT_BY_NAME functionImportByName;
	PIMAGE_THUNK_DATA functionOrThunkData;
	PIMAGE_THUNK_DATA functionThunkData;
	FARPROC functionAddr;
	CHAR functionName[MAX_PATH];
}FUNCTION, *PFUNCTION;

//保存Dll导入表相关信息结构体
typedef struct _IIDINFO
{
	PIMAGE_IMPORT_DESCRIPTOR pThisModImportDescStructAddr;//模块导入表信息描述表的结构体指针
	DWORD_PTR pMainModBaseAddr;//该导入表所在的主模块基地址
	DWORD_PTR subModNameRelateAddr;
	LPCSTR subModNameAddrInChar;
	CHAR subModNameCH[MAX_PATH];
	HMODULE subHModule;
	DWORD_PTR subModRelateOFT;
	PIMAGE_THUNK_DATA subModOFT;
	DWORD_PTR subModIATRelateAddr;
	PIMAGE_THUNK_DATA subModIATAddr;
	std::vector<FUNCTION> modFuncArry;//导入函数的ThunkData相关信息结构体
}IIDINFO, *PIIDINFO;//Dll导入表中的一个导入模块的相关信息结构体

typedef struct _MODIATINFO {
	UINT modIndex;//模块在进程中的查找顺序从0开始
	MODULEENTRY32 modInfo;//模块信息结构体
	std::vector<IIDINFO> iidLst;//该模块的所有导入表信息结构体队列
}MODIATINFO, *PMODIATINFO;//模块iat信息列表

typedef struct _PROIATINFO {
	PROCESSENTRY32 proInfo;//进程信息结构	  
	std::vector<MODIATINFO> modIatLst;//进程的所有的加载模块信息列表
}PROIATINFO, *PPROIATINFO;//IAT输出结构体

typedef struct _FUNCTIONEXP {
	DWORD_PTR addressOrdinal;
	DWORD_PTR exportOrdinal;
	DWORD_PTR baseOrdinal;
	DWORD_PTR rvaOfFunction;
	FARPROC addressOfFunction;
	CHAR functionName[MAX_PATH];
}FUNCTIONEXP, *PFUNCTIONEXP;

typedef struct _RMTDATA
{
	int type;
	HMODULE tagMod;
	DWORD size;

	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_IMPORT_BY_NAME pImgImpByName;

	TCHAR dllNameTH[MAX_PATH];

	CHAR funcNameCH[MAX_PATH];
	CHAR func2NameCH[MAX_PATH];

	TCHAR msg1TH[MAX_PATH];
	TCHAR msg2TH[MAX_PATH];
	TCHAR msg3TH[MAX_PATH];
	TCHAR msg4TH[MAX_PATH];
	TCHAR msg5TH[MAX_PATH];
	TCHAR msg6TH[MAX_PATH];
	TCHAR msg7TH[MAX_PATH];

	FARPROC m_funcAddr1_MessageBoxA;
	FARPROC m_funcAddr2_MessageBox;
	FARPROC m_funcAddr3_GetModuleHandle;
	FARPROC m_funcAddr4_LoadLibrary;
	FARPROC m_funcAddr5_GetProcAddress;
	FARPROC m_funcAddr6_GetModuleHandleA;
	FARPROC m_funcAddr7_IsBadReadPtr;
}RMTDATA, *PRMTDATA;//远程注入时候写入远程进程的数据结构体

typedef struct _RMTMODULE
{
	HMODULE tagMod;
	CHAR dllNameCH[MAX_PATH];
}RMTMODULE, *PRMTMODULE;

typedef DWORD_PTR LASTERRORID; //定义的GetlastError的返回值类型；

//API劫持所采用额结构体
typedef struct _APIHOOK32_ENTRY
{
	LPCTSTR     pszAPINameTH; //api劫持的目标函数名。
	LPCTSTR     pszCalleeModuleNameTH; //导出api所在的dll名称。
	FARPROC     pfnOriginApiAddress; //api在导出dll中的真实入口点,由GetProcAddress()取得真实入口点。
	FARPROC     pfnDummyFuncAddress; //替换的的新入口点值。
	HMODULE     hModCallerModule; //要进行api替换操作的目标模块句柄，当为NULL时候替换除了挂载体本身dll外的其他所有的进程内模块（包括所有系统模块）。
}APIHOOK32_ENTRY, *PAPIHOOK32_ENTRY;

typedef FARPROC(WINAPI *PGetProcAddress)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI *PLoadLibraryA)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI *PLoadLibraryW)(HMODULE, LPCTSTR);
typedef HANDLE(WINAPI *PCreateRemoteThread)(HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD_PTR, LPDWORD);

class CLProcessInfo
{
private:


	//指针位标
	size_t _currentIndex;
	//系统所有进程信息队列
	std::vector<PROCESSENTRY32*> _ProEnt32Array;
	CLMemeryManagerTemplate<PROCESSENTRY32> Pe32M;
	//进程中所有线程信息队列
	std::vector<THREADENTRY32*> _ThrEnt32Array;
	CLMemeryManagerTemplate<THREADENTRY32> Te32M;
	//进程所有模块信息队列
	std::vector<MODULEENTRY32*> _ModEnt32Array;
	CLMemeryManagerTemplate<MODULEENTRY32> Me32M;
	//进程所有模块导入表信息队列
	std::vector<IIDINFO*> _IIDArray;
	CLMemeryManagerTemplate<IIDINFO> IIDM;
	//CLMemeryManagerTemplate<FUNCTION> FIM;

	int _currentProID;
	HMODULE _currentModHandle;

	//初始化进程信息队列
	LASTERRORID initProEnt32Array(void);
	//清除进程信息队列
	void clearProEnt32Array(void);
	//清除线程模块信息队列
	void clearThrEnt32Array(void);
	//清除进程模块信息队列
	void clearModEnt32Array(void);
	//清除进程输入表信息队列
	void clearIIDArray(void);


	static HMODULE getRemoteModuleHandle(HANDLE hProcess, LPCSTR pModuleName, RMTDATA* prdata1, PTHREAD_START_ROUTINE pfnThreadRtn);

	static BOOL getRemoteFuncName(HANDLE hProcess, LPCSTR pFuncName, RMTDATA * prdata1, PTHREAD_START_ROUTINE pfnThreadRtn, PIMAGE_IMPORT_BY_NAME pIIBN);

	static LASTERRORID remoteReleaseDllByRemoHModule(DWORD_PTR dwProcessId, HMODULE hDllRemoteModule, size_t cntUsage);

	// 查看进程中某目标模块的导入表，找到对应的dll和函数并替换。(核心函数)
	static LASTERRORID _setApiToNewAdr(APIHOOK32_ENTRY* phk, HANDLE hOpenedProcess);

	//动态取得函数地址
	//static PCreateRemoteThread pCreateRemoteThread();
	static BOOL CALLBACK _enumFuncCallBack(HWND hwnd, LPARAM lParam);
public:
	//构造函数。构造时候自动产生当前时点的进程列表。
	CLProcessInfo();
	virtual ~CLProcessInfo();
	//清除所有队列数据释放内存
	void clearAllInfo(void);
	//取得内部进程队列
	const std::vector<PROCESSENTRY32*>& info(void) const;
	//取得内部模块队列
	const std::vector<MODULEENTRY32*>& modInfo(void) const;
	//取得内部线程队列
	const std::vector<THREADENTRY32*>& thrInfo(void) const;
	//取得内部导入表队列
	const std::vector<IIDINFO*>& iidInfo(void) const;
	//初始化线程信息队列，返回GetLastError()代码。
	LASTERRORID initThrEnt32Array(int nProID = 0);
	//初始化模块信息队列，返回GetLastError()代码。
	LASTERRORID initModEnt32Array(int nProID = 0);
	//初始化输入表信息队列，远程进程方式，内部必须保存得有远程进程的id。，返回GetLastError()代码。
	LASTERRORID initIIDArray(HMODULE tagMod);
	//初始化输入表信息队列，远程进程方式。，返回GetLastError()代码。
	LASTERRORID initRemoteIIDArray(LONG dwProcessId, HMODULE tagMod);
	//初始化输入表信息队列，当前进程方式，返回GetLastError()代码。
	LASTERRORID initIIDArray1(HMODULE tagMod);
	//以当前内部保存的远程进程id来卸载远程进程中对应名称的dll，若该内部id已不可用则仅仅更新内部队列信息。返回GetLastError()代码。
	LASTERRORID remoteReleaseDllByName(LPCTSTR pDllName);

	//远程置入时候的置入体函数。
	static DWORD_PTR WINAPI ThreadFunc1_GetImportDesc(PRMTDATA pData);
	//函数用来计算植入体函数的代码段的字节大小。
	static DWORD_PTR WINAPI ThreadFunc1_GetImportDescEnd(void);
	//更新所有信息队列。如果当前内部保存的进程id对应的进程或模块句柄对应的句柄已不存在，则会清理掉内部维护的队列数据信息。
	CLProcessInfo& updateInfo(void);
	//提高进程Debug权限
	static LASTERRORID enablePrivilege(LPCTSTR seName = SE_DEBUG_NAME);

	//由进程id在内部的进程列表中找出对应的进程信息结构体，调用前可调用updateInfo()更新维护的进程列表。
	BOOL findProcessStructFromInnerProListByProID(DWORD_PTR dProcessID, PROCESSENTRY32& outStruct);
	//由进程名在内部的进程列表中的第一个找出的进程信息结构体，调用前可调用updateInfo()更新维护的进程列表。
	BOOL findProcessStructFromInnerProListByNameFirst(LPCSTR lpProName, PROCESSENTRY32& outStruct);
	//由进程名在内部的进程列表中的下一个找出的进程信息结构体，调用前可调用updateInfo()更新维护的进程列表。
	BOOL findProcessStructFromInnerProListByNameNext(LPCSTR lpProName, PROCESSENTRY32& outStruct);

	//由模块句柄在内部的Mod列表中找出对应的模块信息结构体，调用前提：必须初始化过模块表队列initModEnt32Array()。
	BOOL findModuleStructFromInnerModListByHandle(HMODULE hModule, MODULEENTRY32& outStruct);
	//由模块名称在内部的Mod列表中找出对应的模块信息结构体，调用前提：必须初始化过模块表队列initModEnt32Array()。
	BOOL findModuleStructFromInnerModListByModName(LPCSTR lpModuleName, MODULEENTRY32& outStruct);
	//由模块名称在内部的iid列表中找出对应的导入表模块的信息结构体，,dll名称字符串必须猜用字节码，不能采用UNICODE，调用前提：必须初始化过模块表队列initModEnt32Array()。
	BOOL findIIDStructFromInnerIIDListByDllName(LPCSTR lpDllName, IIDINFO& outStruct);
	//由模块句柄在内部的iid列表中找出对应的导入表模块的信息结构体，调用前提：必须初始化过导入表队列initIIDArray()。
	BOOL findIIDStructFromInnerIIDListByDllHandle(HMODULE hModule, IIDINFO& outStruct);
	//由API函数名在内部的iid列表中找出对应的api导入表信息结构体，调用前提：必须初始化过导入表队列initIIDArray()。
	BOOL findApiStructFromInnerIIDListByApiName(LPCSTR lpApiName, FUNCTION& outStruct);
	//返回上一次初始化模块信息队列后对应的内部进程ID，也就是调用initThrEnt32Array或initThrEnt32Array传入的那个进程id，，若没有初始化模块列表则该值为-1.
	int currentInnerProcessID(void) const;
	//返回上一次初始化导入表信息队列后对应的内部模块句柄，也就是调用initIIDArray传入的那个模块句柄，若没有初始化导入表则该值为NULL。
	HMODULE currentInnerModuleHandle(void) const;
	
	//任意的通过进程id或进程名，遍历进程表，取得进程信息结构体数据，并返回进程主模块句柄。
	static HMODULE getProcessInfo(DWORD_PTR dwProcessId, LPCTSTR szProcessName, PROCESSENTRY32& out_ProcessEntry32Str);
	typedef struct _str_proinfo_pack {
		PROCESSENTRY32 pro = { sizeof(PROCESSENTRY32) };
		MODULEENTRY32 mainMod = { sizeof(MODULEENTRY32) };
		THREADENTRY32 mainThread = { sizeof(THREADENTRY32) };
		HEAPLIST32 mainHeapList = { sizeof(HEAPLIST32) };
		std::vector<MODULEENTRY32> subMods;
		std::vector<THREADENTRY32> subThreads;
		std::vector<HEAPLIST32> subHeapList;
	}PROINFO,*PPROINFO;
	typedef std::vector<PROINFO> PROINFOTABLE;
	//任意的通过进程进程名，遍历进程表，取得进程信息结构体数据，并返回进程信息结构体枚举列表。
	static PROINFOTABLE& enumProcessInfo(LPCTSTR szProcessName, PROINFOTABLE& out_ProcessInfoTable);
	//返回进程指定的模块信息结构体数据，遍历进程模块表来查询，模块存在返回模块句柄，否则返回NULL。
	static HMODULE getModuleInfoOfProcess(DWORD_PTR dwProcessId, LPCTSTR szDllName, MODULEENTRY32& out_ModuleEntry32Str);
	//由文件名取得进程ID，需要遍历进程表。
	static DWORD_PTR getProcessIDByName(LPCTSTR lpszProcessName);
	//由进程id取得进程全路径名，失败返回false
	static BOOL getProcessFileNameByPid(DWORD_PTR dwPid, LPTSTR out_ProcessFullPathName, DWORD_PTR nSize);

	//功能 通过创建远程线程给其他进程加载指定的Dll
	static DWORD_PTR remoteloadDll(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath);
	//功能 通过创建远程线程卸载其他进程加载的Dll，cntUsage表示dll减少的引用次数，若为0表示完全卸载dll
	static DWORD_PTR remoteReleaseDll(DWORD_PTR dwProcessId, LPCTSTR szDllName, DWORD_PTR cntUsage = 0);

	//功能 通过钩子技术加载Dll，lpszFucName表示要调用的dll中的导出函数名，如果设置NULL，默认调用"InsertDll"；
	//返回钩子过程句柄，而非dll句柄
	//函数历程为:setHookByDll() -> Loadliberary(Dll) -> GetProcAddress(InsertDLL) -> InsertDLL() ->
	//SetWindowHookEx() -> Dllmain -> SetWindowsAPIHook()
	static HHOOK setHookByDll(HMODULE &out_hHookDll, LPCTSTR lpszDllPath, LPCSTR lpszFucName = NULL, DWORD_PTR dwThrId = 0, WPARAM wParam = 0, LPARAM lParam = 0);
	//static DWORD_PTR releaseHook(DWORD_PTR dwProcessId, LPCTSTR lpszDllPath, WPARAM hTargetModle, LPARAM lParam);

	//API地址替换函数，phk->hModCallerModule == NULL就挂载所有子模块
	static LASTERRORID hookWindowsAPI(APIHOOK32_ENTRY* phk);
	//还原Api原始地址,从所有子模块卸载
	static LASTERRORID unHookWindowsAPI(APIHOOK32_ENTRY* phk);

	//判断系统是否是64位的
	static BOOL is64BitSystem();
	//判断进程是否是64位的,返回TRUE结果_out_bIs64BitPro的值才有效，_out_bIs64BitPro为TRUE表示进程位64位进程
	static BOOL is64BitProcess(DWORD_PTR nProID, BOOL& _out_bIs64BitPro);
	//由进程id获得主窗口句柄，没有窗口返回NULL（通过枚举所有窗口对比后返回匹配值）
	static HWND getHwndByProID(DWORD_PTR processID);

	//取得进程(或远程进程)的信息，进程中所有的模块信息，和进程中所有模块的导入表信息。失败返回失败时点的GetLastError()值。
	//_out_priIatInfo会在函数内使用前自动清理，也不需要手动释放内存。
	//在过程中如果出现任何错误都会返回，此时的输出结果集合_out_priIatInfo中可能保存有一部分已经获取到的信息，但是并不完全。
	//注意：该函数可能会消耗大量的时间，且远程进程是通过代码注入获取进程IAT的所以请在调用前检查调用者权限和被查看者进程的进程保护状态，可能因为权限不够而无法打开进程获取远程信息。
	static LASTERRORID getProIATInfo(DWORD_PTR processID, PROIATINFO& _out_priIatInfo);

	//列举进程中已加载的模块导出表中的所有函数，_baseAddr是目标模块句柄（也可以是模块基地址，内存镜像基地址），_out_Sets是一个保存查询结果的结果集（无需初始化），
	//_familyFlag表示函数族标头（型如：Nt, Zw , Rtl, _, __, A, Abc 等等均可）。
	//设置_familyFlag值后，结果集只返回带有该族标头的函数（族标头区分大小写），当_familyFlag = NULL，""，"\0"时忽略族标头过滤条件，获取所有导出函数，也包括导出的无名函数。
	//当_familyFlag为指定的某一特定函数名时候，函数功能等价于查询某个单一函数，此时应该设置_isFullMatch = TRUE 表示族标头必须完全匹配，查找到函数立即返回，不再枚举后续值。
	//如果没有找到任何结果（也就是结果集_out_Sets对象数量为0）将返回ERROR_INVALID_FUNCTION。若函数枚举成功将返回ERROR_SUCCESS。
	static LASTERRORID emunExportFunction(LPCVOID _baseAddr, std::vector<FUNCTIONEXP>& _out_Sets, LPCSTR  _familyFlag = NULL, BOOL _isFullMatch = FALSE);


	static HWND getConsoleHwnd(void); //取得当前控台程序的窗口句柄

};

#endif