
#include "CLCommon.h"
#include <dbghelp.h>
#include "../_cl_time/CLTime.h"
#include "../_cl_string/CLString.h"
#include <iostream>
#include "comutil.h"
#pragma comment(lib,"comsuppw.lib")
#pragma comment(lib,"comsupp.lib")
#pragma comment(lib,"kernel32.lib")

using namespace std;

#include <wtypes.h>
#include "..\_cl_neuralNetwork\CreativeLus.h"
class CLCpuUsageCs
{
public:
	CLCpuUsageCs(CRITICAL_SECTION& ref_cs)
	{
		m_cs = ref_cs;
		EnterCriticalSection(&m_cs);
	}

	~CLCpuUsageCs()
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};
class CLCpuDelay
{
public:
	inline void Mark() { m_mark = ::GetTickCount(); }
	inline int  MSec() { return (::GetTickCount() - m_mark) & 0x7FFFFFFF; }

private:
	DWORD m_mark;
};
CLCpuDelay	CLCpuUsage::s_delay;
int		CLCpuUsage::s_count = 0;
int		CLCpuUsage::s_index = 0;
int		CLCpuUsage::s_lastCpu = 0;
int		CLCpuUsage::s_cpu[CLCDelay_DATA_COUNT];
__int64 CLCpuUsage::s_time = 0;
__int64 CLCpuUsage::s_idleTime = 0;
__int64 CLCpuUsage::s_kernelTime = 0;
__int64 CLCpuUsage::s_userTime = 0;
CLCpuUsage::CLCpuUsage()
{
	::InitializeCriticalSection(&m_lock);
	s_delay.Mark();
}
CLCpuUsage::~CLCpuUsage()
{
	::DeleteCriticalSection(&m_lock);
}
int CLCpuUsage::GetUsage()
{
	__int64 sTime;
	int sLastCpu;

#define LOCK_START	{ CLCpuUsageCs lock(m_lock);
#define LOCK_END	}

	LOCK_START
		sTime = s_time;
	sLastCpu = s_lastCpu;
	LOCK_END

		if (s_delay.MSec() <= CLCDelay_DELAY_DIFF)
			return sLastCpu;

	__int64 time;
	__int64 idleTime;
	__int64 kernelTime;
	__int64 userTime;

	GetSystemTimeAsFileTime((LPFILETIME)& time);

	GetSystemTimes(
		(LPFILETIME)& idleTime,
		(LPFILETIME)& kernelTime,
		(LPFILETIME)& userTime
	);

	if (0 == sTime)
	{
		LOCK_START
			s_time = time;
		s_idleTime = idleTime;
		s_kernelTime = kernelTime;
		s_userTime = userTime;
		s_lastCpu = 0;

		sLastCpu = s_lastCpu;
		LOCK_END

			s_delay.Mark();
		return sLastCpu;
	}

	int iCpu;

	LOCK_START
		__int64 usr = userTime - s_userTime;
	__int64 ker = kernelTime - s_kernelTime;
	__int64 idl = idleTime - s_idleTime;

	__int64 sys = (usr + ker);
	if (0 == sys)
		iCpu = 0;
	else
		iCpu = (int)((sys - idl) * 100 / sys);

	s_time = time;
	s_idleTime = idleTime;
	s_kernelTime = kernelTime;
	s_userTime = userTime;

	s_cpu[(s_index++) % CLCDelay_DATA_COUNT] = iCpu;
	s_count++;
	if (s_count > CLCDelay_DATA_COUNT)
		s_count = CLCDelay_DATA_COUNT;

	int i;
	iCpu = 0;
	for (i = 0; i < s_count; i++)
		iCpu += s_cpu[i];

	iCpu /= s_count;
	s_lastCpu = iCpu;

	sLastCpu = s_lastCpu;
	LOCK_END

		s_delay.Mark();
	return sLastCpu;
}

//求和
VT SUM(VD_RC vec) {
	VT ret = 0;
	for (VDIC i = vec.cbegin(); i != vec.cend();)
	{
		ret += *i; ++i;
	}CLCheckNan(ret);
	return ret;
}
VT SUM(VTArray vec, VTArraySize si) {
	VT ret = 0;
	for (size_t i = 0; i < si;) {
		ret += vec[i]; ++i;
	}CLCheckNan(ret);
	return ret;
}

//均值
VT AVG(VD_RC vec) {
	assert(vec.size() > 0);
	return SUM(vec) / vec.size();
}
VT AVG(VTArray vec, VTArraySize si) {
	assert(si > 0);
	return SUM(vec, si) / si;
}

//期望
VT E(VD_RC v_X) {
	return AVG(v_X);
}
VT E(VD_RC v_X, VD_RC v_pi) {
	if (v_pi.size() == 0)
		return AVG(v_X);
	size_t si = v_X.size();
	assert(si > 0 && si <= v_pi.size());
	VT ret = 0;
	for (size_t i = 0; i < si; i++) {
		ret += v_X.at(i) * v_pi.at(i);
	}
	CLCheckNan(ret);
	return ret;
}
VT E(VTArray v_X, VTArray v_pi, VTArraySize si)
{
	if (v_pi  == 0)
		return AVG(v_X,si);
	VT ret = 0;
	for (size_t i = 0; i < si; i++) {
		ret += v_X[i] * v_pi[i];
	}
	CLCheckNan(ret);
	return ret;
}
VT E(VTArray v_X, VTArraySize si)
{
	return AVG(v_X, si);
}

//方差
VT VAR(VD_RC v_X, VD_RC v_pi) {
	VT m_E = E(v_X, v_pi);
	size_t si = v_X.size();
	VD v_E;
	v_E.reserve(si);
	for (size_t i = 0; i < si; i++) {
		v_E.push_back((v_X.at(i) - m_E)*(v_X.at(i) - m_E));
	}
	return E(v_E, v_pi);
}
VT VAR(VD_RC v_X) {
	VT m_E = E(v_X);
	VT ret = 0;
	size_t si = v_X.size();
	for (size_t i = 0; i < si; i++) {
		VT t = v_X.at(i) - m_E;
		ret += t * t;
	}CLCheckNan(ret);
	return ret / si;
}
VT VAR(VTArray v_X, VTArray v_pi, VTArraySize si)
{
	VT m_E = E(v_X, v_pi,si);
	double* v_E =  new double[si];
	for (size_t i = 0; i < si; i++) {
		v_E[i]=((v_X[i] -m_E)*(v_X[i] - m_E));
	}
	VT rt = E(v_E, v_pi,si);
	delete[] v_E;
	return rt;
}
VT VAR(VTArray v_X, VTArraySize si)
{
	VT m_E = E(v_X, si);
	VT ret = 0;
	for (size_t i = 0; i < si; i++) {
		VT t = v_X[i] - m_E; ret += t * t;
	}CLCheckNan(ret);
	return ret / si;
}

//协方差
VT COV(VD_RC v_X, VD_RC v_Y, VD_RC v_pix, VD_RC v_piy) {
	assert(v_X.size() == v_Y.size());
	VD v_E;
	VT m_Ex = E(v_X, v_pix);
	VT m_Ey = E(v_Y, v_piy);
	for (size_t i = 0; i < v_X.size(); i++) {
		v_E.push_back((v_X.at(i) - m_Ex)*(v_Y.at(i) - m_Ey));
	}
	return E(v_E);
}
VT COV(VD_RC v_X, VD_RC v_Y) {
	assert(v_X.size() == v_Y.size());
	VT m_Ex = E(v_X);
	VT m_Ey = E(v_Y);
	VT ret = 0;
	size_t si = v_X.size();
	for (size_t i = 0; i < si; i++) {
		ret += (v_X.at(i) - m_Ex)*(v_Y.at(i) - m_Ey);
	}CLCheckNan(ret);
	return ret / si;
}
VT COV(VTArray v_X, VTArray v_Y, VTArray v_pix, VTArray v_piy, VTArraySize si)
{
	VD v_E;
	VT m_Ex = E(v_X, v_pix,si);
	VT m_Ey = E(v_Y, v_piy,si);
	v_E.reserve(si);
	for (size_t i = 0; i < si; i++) {
		v_E.push_back((v_X[i] - m_Ex)*(v_Y[i] - m_Ey));
	}
	return E(v_E);
}
VT COV(VTArray v_X, VTArray v_Y, VTArraySize si)
{
	VT m_Ex = E(v_X, si);
	VT m_Ey = E(v_Y, si);
	VT ret = 0;
	for (size_t i = 0; i < si; i++) {
		ret += (v_X[i] - m_Ex)*(v_Y[i] - m_Ey);
	}CLCheckNan(ret);
	return ret / si;
}

//标准差
VT DVA(VD_RC v_X, VD_RC v_pix) {
	return sqrt(VAR(v_X, v_pix));
}
VT DVA(VD_RC v_X) {
	return sqrt(VAR(v_X));
}
VT DVA(VTArray v_X, VTArray v_pix, VTArraySize si)
{
	return sqrt(VAR(v_X,  v_pix, si));
}
VT DVA(VTArray v_X, VTArraySize si) {
	return sqrt(VAR(v_X, si));
}

//求相关系数
VT COR(VD_RC v_X, VD_RC v_Y, VD_RC v_pix, VD_RC v_piy) {
	VT v1 = COV(v_X, v_Y, v_pix, v_piy); if (v1 == 0)return 0;
	VT v2 = DVA(v_X, v_pix);
	VT v3 = DVA(v_Y, v_piy);
	//assert(v2*v3 != 0);
	return v2 * v3 == 0 ? 0 : v1 / (v2*v3);
}
VT COR(VD_RC v_X, VD_RC v_Y) {
	VT v1 = COV(v_X, v_Y); if (v1 == 0)return 0;
	VT v2 = DVA(v_X);
	VT v3 = DVA(v_Y);
	//assert(v2*v3 != 0);
	return v2 * v3 == 0 ? 0 : v1 / (v2*v3);
}
VT COR(VTArray v_X, VTArray v_Y, VTArray v_pix, VTArray v_piy, VTArraySize si)
{
	VT v1 = COV(v_X, v_Y, v_pix, v_piy, si);
	if (v1 == 0)return 0;
	VT v2 = DVA(v_X, v_pix, si);
	VT v3 = DVA(v_Y, v_piy, si);
	//assert(v2*v3 != 0);
	return v2 * v3 == 0 ? 0 : v1 / (v2*v3);
}
VT COR(VTArray v_X, VTArray v_Y, VTArraySize si) {
	VT v1 = COV(v_X, v_Y, si); if (v1 == 0)return 0;
	VT v2 = DVA(v_X, si); 
	VT v3 = DVA(v_Y, si);
	//assert(v2*v3 != 0);
	return v2 * v3 == 0 ? 0 : v1 / (v2*v3);
}

void setRandomSeed()
{
	UINT num = (unsigned)time(NULL);
	srand((unsigned)time(NULL));
	CHAR str[100];
	_itoa_s(num, str, 16); double v = 0, rt = num;
	for (int i = 0; str[i] && i < 100; i++)
	{
		v = pow(double(str[i]), RAND_F_A_B(0.01, 10.0))*(i + 1) + (i + 1);
		if (v < 0)v *= (-1.0);
		rt += (v);
	}
	UINT num2 = (UINT)(rt < 10000 ? (rt * 10000000000) : rt);
	srand(num2);
}

static random_device rd;
static mt19937_64 gen(rd());
//static uniform_int_distribution<unsigned long long> _dst(0, INT_MAX);
static uniform_int_distribution<unsigned long long> _dst(0, RANDDIV_MAX);
unsigned long long randi()
{
	return _dst(gen);
	//unsigned long long hight = _dst(gen);
	//unsigned long long low = _dst(gen);
}

void unlock(void)
{
}


#ifdef UNICODE
#define _tcssprintf swprintf_s
#define tcsplitpath _wsplitpath_s
#else
#define _tcssprintf sprintf_s
#define tcsplitpath _splitpath_s
#endif


//-----------------------------------------------------------------------------
// GLOBALs
//-----------------------------------------------------------------------------
CLMiniDumper       *gs_pMiniDumper = NULL;
LPCRITICAL_SECTION gs_pCriticalSection = NULL;

//-----------------------------------------------------------------------------
// APIs
//-----------------------------------------------------------------------------
// Based on dbghelp.h
typedef BOOL(WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
{
	if (pModuleName == 0)
	{
		return FALSE;
	}

	WCHAR szFileName[_MAX_FNAME] = { 0 };
	_wsplitpath_s(pModuleName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, NULL, 0);

	if (_wcsicmp(szFileName, L"ntdll") == 0)
		return TRUE;

	return FALSE;
}

BOOL WINAPI MiniDumpCallback(PVOID							 pParam,
	const PMINIDUMP_CALLBACK_INPUT   pInput,
	PMINIDUMP_CALLBACK_OUTPUT        pOutput)
{
	if (pInput == 0 || pOutput == 0)
		return FALSE;

	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
		{
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
		}
		return TRUE;
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	default:;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// Name: unhandledExceptionHandler()
// Desc: Call-back filter function for unhandled exceptions
//-----------------------------------------------------------------------------
LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS *pExceptionInfo)
{
	if (NULL == gs_pMiniDumper)
		return EXCEPTION_CONTINUE_SEARCH;

	return gs_pMiniDumper->writeMiniDump(pExceptionInfo);
}

// 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效  
void DisableSetUnhandledExceptionFilter()
{
	HMODULE hModule = LoadLibrary(_T("kernel32.dll"));
	if (hModule)
	{
		void* pAddr = (void*)GetProcAddress(hModule, "SetUnhandledExceptionFilter");
		if (pAddr)
		{
			unsigned char code[16] = { 0 };
			int			  size = 0;

			code[size++] = 0x33;
			code[size++] = 0xC0;
			code[size++] = 0xC2;
			code[size++] = 0x04;
			code[size++] = 0x00;

			DWORD dwOldFlag = 0;
			DWORD dwTempFlag = 0;

			VirtualProtect(pAddr, size, PAGE_READWRITE, &dwOldFlag);
			WriteProcessMemory(GetCurrentProcess(), pAddr, code, size, NULL);
			VirtualProtect(pAddr, size, dwOldFlag, &dwTempFlag);
		}
		FreeLibrary(hModule);
	}
}

//-----------------------------------------------------------------------------
// Name: create()
// Desc: Instanse gs_pMiniDumper
//-----------------------------------------------------------------------------
HRESULT CLMiniDumper::create(int dumpType)
{
	if (NULL == gs_pMiniDumper)
	{
		gs_pMiniDumper = new CLMiniDumper(dumpType);
	}
	if (NULL == gs_pCriticalSection)
	{
		gs_pCriticalSection = new CRITICAL_SECTION;
		InitializeCriticalSection(gs_pCriticalSection);
	}

	return(S_OK);
}

//-----------------------------------------------------------------------------
// Name: release()
// Desc: Release gs_pMiniDumper
//-----------------------------------------------------------------------------
HRESULT  CLMiniDumper::release()
{
	if (NULL != gs_pMiniDumper)
	{
		delete gs_pMiniDumper;
		gs_pMiniDumper = NULL;
	}
	if (NULL != gs_pCriticalSection)
	{
		DeleteCriticalSection(gs_pCriticalSection);
		gs_pCriticalSection = NULL;
	}

	return(S_OK);
}

//-----------------------------------------------------------------------------
// Name: CLMiniDumper()
// Desc: Constructor
//-----------------------------------------------------------------------------
CLMiniDumper::CLMiniDumper(int dumpType) :m_dumpType(dumpType)
{
	// 使应用程序能够取代每个进程和线程的顶级异常处理程序	
	::SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	DisableSetUnhandledExceptionFilter();
}

//-----------------------------------------------------------------------------
// Name: ~CLMiniDumper()
// Desc: Destructor
//-----------------------------------------------------------------------------
CLMiniDumper::~CLMiniDumper(void)
{

}

//-----------------------------------------------------------------------------
// Name: setMiniDumpFileName()
// Desc: 
//-----------------------------------------------------------------------------
void CLMiniDumper::setMiniDumpFileName(void)
{
	//time_t currentTime;
	//time(&currentTime);

	//_tcssprintf(m_szMiniDumpPath, _T("%s.%lld.dmp"), m_szAppPath, (__int64)currentTime);
	_tcssprintf(m_szMiniDumpPath, _T("%s.%lld.dmp"), m_szAppPath, CLTime::getLocalTime_ll() / 1000);
}

//-----------------------------------------------------------------------------
// Name: getImpersonationToken()
// Desc: The method acts as a potential workaround for the fact that the 
//       current thread may not have a token assigned to it, and if not, the 
//       process token is received.
//-----------------------------------------------------------------------------
BOOL CLMiniDumper::getImpersonationToken(HANDLE* phToken)
{
	*phToken = NULL;
	if (!OpenThreadToken(GetCurrentThread(),
		TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
		TRUE,
		phToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			// No impersonation token for the current thread is available. 
			// Let's go for the process token instead.
			if (!OpenProcessToken(GetCurrentProcess(),
				TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
				phToken))
				return FALSE;
		}
		else
			return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: enablePrivilege()
// Desc: Since a MiniDump contains a lot of meta-data about the OS and 
//       application state at the time of the dump, it is a rather privileged 
//       operation. This means we need to set the SeDebugPrivilege to be able 
//       to call MiniDumpWriteDump.
//-----------------------------------------------------------------------------
BOOL CLMiniDumper::enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld)
{
	BOOL				bOk = FALSE;
	TOKEN_PRIVILEGES	tp;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bOk = LookupPrivilegeValue(0, pszPriv, &tp.Privileges[0].Luid);
	if (bOk)
	{
		DWORD cbOld = sizeof(*ptpOld);
		bOk = AdjustTokenPrivileges(hToken, FALSE, &tp, cbOld, ptpOld, &cbOld);
	}

	return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

//-----------------------------------------------------------------------------
// Name: restorePrivilege()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CLMiniDumper::restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld)
{
	BOOL bOk = AdjustTokenPrivileges(hToken, FALSE, ptpOld, 0, NULL, NULL);
	return (bOk && (ERROR_NOT_ALL_ASSIGNED != GetLastError()));
}

//-----------------------------------------------------------------------------
// Name: writeMiniDump()
// Desc: 
//-----------------------------------------------------------------------------
LONG CLMiniDumper::writeMiniDump(EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG	retval = EXCEPTION_CONTINUE_SEARCH;
	HANDLE	hImpersonationToken = NULL;

	if (!getImpersonationToken(&hImpersonationToken))
		return FALSE;

	// You have to find the right dbghelp.dll. 
	// Look next to the EXE first since the one in System32 might be old (Win2k)
	HMODULE hDll = NULL;
	if (GetModuleFileName(NULL, m_szAppPath, _MAX_PATH))
	{
		TCHAR szDir[_MAX_DIR] = { 0 };
		TCHAR	szDbgHelpPath[MAX_PATH] = { 0 };

		tcsplitpath(m_szAppPath, szDir, MAX_PATH-1, NULL, 0, NULL, 0, NULL, 0);
		_tcscat_s(szDbgHelpPath, MAX_PATH, szDir);
		_tcscat_s(szDbgHelpPath, _T("DBGHELP.DLL"));

		hDll = ::LoadLibrary(szDbgHelpPath);
	}

	if (hDll == NULL)
	{
		// If we haven't found it yet - try one more time.
		hDll = ::LoadLibrary(_T("DBGHELP.DLL"));
	}

	if (hDll)
	{
		// Get the address of the MiniDumpWriteDump function, which writes 
		// user-mode mini-dump information to a specified file.
		MINIDUMPWRITEDUMP MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
		if (MiniDumpWriteDump != NULL)
		{
			setMiniDumpFileName();

			// Create the mini-dump file...
			HANDLE hFile = ::CreateFile(m_szMiniDumpPath,
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = NULL;

				MINIDUMP_CALLBACK_INFORMATION mci;
				mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
				mci.CallbackParam = 0;

				// We need the SeDebugPrivilege to be able to run MiniDumpWriteDump
				TOKEN_PRIVILEGES tp;
				BOOL bPrivilegeEnabled = enablePrivilege(SE_DEBUG_NAME, hImpersonationToken, &tp);
				BOOL bOk;

				// DBGHELP.dll is not thread-safe, so we need to restrict access...
				EnterCriticalSection(gs_pCriticalSection);
				{
					// Write out the mini-dump data to the file...
					bOk = MiniDumpWriteDump(
						GetCurrentProcess(),
						GetCurrentProcessId(),
						hFile,
						MINIDUMP_TYPE(m_dumpType),
						(NULL == pExceptionInfo) ? (NULL) : (&ExInfo),
						NULL,
						&mci);
				}
				LeaveCriticalSection(gs_pCriticalSection);

				// Restore the privileges when done
				if (bPrivilegeEnabled)
					restorePrivilege(hImpersonationToken, &tp);

				if (bOk)
				{
					retval = EXCEPTION_EXECUTE_HANDLER;
				}

				::CloseHandle(hFile);
			}
		}
		FreeLibrary(hDll);
	}
	
	if (NULL != pExceptionInfo)
	{
		//MessageBox(NULL,_T("reach the TerminateProcess(GetCurrentProcess(), 0)"),NULL,MB_OK);
		TerminateProcess(GetCurrentProcess(), 0);
	}

	return retval;
}

CLTaskSvc::CLTaskSvc(void)
{
	initLock(0);
	setPriority();
}

BOOL CLTaskSvc::init(PCLTaskSvcTrdParam var){
	return TRUE;
}
DWORD CLTaskSvc::run(PCLTaskSvcTrdParam var){
	return 1;
}
DWORD CLTaskSvc::exist(DWORD runReturnValue, PCLTaskSvcTrdParam var){
	return runReturnValue;
}

CLTaskSvc::~CLTaskSvc()
{
	close();
}

size_t CLTaskSvc::start(int _num,BOOL isSuspend)
{
	lock();
	CLTaskSvc::close();//调用基类关闭函数,清空队列
	assert(CLTaskSvc::isRunning() == FALSE);	
	CLTaskSvcTrdSupport inf;
	size_t rt = 0;
	std::vector<LONG_PTR>* var = NULL;
	std::vector<std::vector<LONG_PTR>*> varTable;
	int num = _num == 0 ? getCpuCoreCounts() : _num;
	for (int i = 0; i < num; i++)
	{
		inf.reset();
		if (var == NULL) { //不存在就创建
			var = new std::vector<LONG_PTR>;
			var->push_back((LONG_PTR)this); //对象指正
			var->push_back((LONG_PTR)i+1); //标号
			var->push_back((LONG_PTR)CreateEvent(0, 0, 0, 0));//退出事件
			var->push_back(0);//成功执行的线程总数
		}
		inf.hThread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE> (CLTaskSvc::workThread), var, CREATE_SUSPENDED, &inf.tId);
		if (inf.hThread && inf.hThread != INVALID_HANDLE_VALUE)
		{
			SetThreadPriority(inf.hThread,nPriority);
			rt++;
			inf.nIndex = (int)rt;			
			inf.quitEvent = (HANDLE)(*var)[2];
			vec_threads[inf.tId] = inf;
			var->at(1) = (LONG_PTR)rt;
			var->push_back((LONG_PTR)inf.hThread);//压入真是句柄
			varTable.push_back(var);
			var = NULL; //成功就设0
		}
	}
	
	for (auto i = varTable.begin(); i != varTable.end(); )
	{
		if ((*i) != NULL) {
			(*i)->at(3) = (LONG_PTR)rt;
		}
		++i;
	}
	unlock();
	if (var)
	{
		CloseHandle((HANDLE)var->at(2));
		delete var;//删除多余
	}
	if (isSuspend == FALSE)		//	显示启动
		resume();	
	return rt;
}
void CLTaskSvc::resume()
{
	lock();	
	for (auto i = vec_threads.cbegin(); i != vec_threads.cend();) {
		if(i->second.hThread)
			ResumeThread(i->second.hThread); 
		++i;
	}
	unlock();
	Sleep(0);
}
void CLTaskSvc::suspend()
{
	lock();
	for (auto i = vec_threads.cbegin(); i != vec_threads.cend();) {
		if (i->second.hThread)
			SuspendThread(i->second.hThread);
		++i;
	}
	unlock();
}
size_t CLTaskSvc::getThreadsNum() const
{
	return vec_threads.size();
}

size_t CLTaskSvc::getActivateThreadsNum() const
{
	size_t rt = 0;
	for (auto i = vec_threads.cbegin(); i != vec_threads.cend();)
	{
		if (WaitForSingleObject(i->second.hThread, 0) == WAIT_TIMEOUT)
			rt++;
		++i;
	}
	return rt;
}
DWORD CLTaskSvc::getCpuCoreCounts()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}



BOOL CHECK_WM_QUIT() {
	MSG _msgCur;
	return ::PeekMessage(&_msgCur, (HWND)0, WM_QUIT, WM_QUIT, PM_NOREMOVE);	//如果WM_QUIT消息返回真	
}

HRESULT checkComErrorManual()
{
	HRESULT hr = 0;
	BSTR bstrDescription = 0;
	BSTR bstrSource = 0;
	BSTR bstrHelpFile = 0;
	IErrorInfo* pErrInfo = NULL; // naked COM interface pointer
	hr = ::GetErrorInfo(0, &pErrInfo);
		if (hr != S_OK)goto finl;
		hr = pErrInfo->GetDescription(&bstrDescription);
		if (FAILED(hr))goto finl;
		hr = pErrInfo->GetSource(&bstrSource);
		if (FAILED(hr))goto finl;
		hr = pErrInfo->GetHelpFile(&bstrHelpFile);
finl:
	if (pErrInfo) {
		pErrInfo->Release();
		LPCSTR lpc = NULL;
		if (bstrDescription) {
			lpc = _com_util::ConvertBSTRToString(bstrDescription);
			::SysFreeString(bstrDescription);
			if(IDYES == MessageBoxA(NULL, lpc, "Com Error", MB_ICONERROR | MB_YESNO))
				throw "Com Error Exception!";
			delete[] lpc;
		}
		if (bstrSource) {
			lpc = _com_util::ConvertBSTRToString(bstrSource);
			::SysFreeString(bstrSource);
			if (IDYES == MessageBoxA(NULL, lpc, "Com Error", MB_ICONERROR | MB_YESNO))
				throw "Com Error Exception!";
			delete[] lpc;
		}
		if (bstrHelpFile) {
			lpc = _com_util::ConvertBSTRToString(bstrHelpFile);
			::SysFreeString(bstrHelpFile);
			if (IDYES == MessageBoxA(NULL, lpc, "Com Error", MB_ICONERROR | MB_YESNO))
				throw "Com Error Exception!";
			delete[] lpc;
		}
	}
	return hr;
}

HRESULT converXlsToCsv(LPCTSTR pSrcFileName, LPCTSTR pDesFileName)
{
	HRESULT hr = S_OK;
	//单线程方式创建COM对象
	::CoInitialize(0);
	if (1)
	{
		try
		{
			//初始化EXCEL对象
			CComDispatchDriver execlAppDsp;
			hr = execlAppDsp.CoCreateInstance(L"Excel.Application", 0, CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER);
			if (hr != S_OK) throw hr;
			//获取工作页，打开XLS文件
			CComVariant vWorkbooks;
			hr = execlAppDsp.GetPropertyByName(L"Workbooks", &vWorkbooks);
			if (hr != S_OK) throw hr;
			CComDispatchDriver booksDsp(vWorkbooks.pdispVal);
			CComVariant vSrcFileName(pSrcFileName);
			hr = booksDsp.Invoke1(L"Open", &vSrcFileName);
			if (hr != S_OK) throw hr;
			//关闭警告对话框
			CComVariant vDisplay(L"FALSE");
			hr = execlAppDsp.PutPropertyByName(L"DisplayAlerts", &vDisplay);
			if (hr != S_OK) throw hr;
			//打开活动页，另存为CSV文件
			CComVariant vActBook;
			hr = execlAppDsp.GetPropertyByName(L"ActiveWorkbook", &vActBook);
			if (hr != S_OK) throw hr;
			CComDispatchDriver activeBookDsp(vActBook.pdispVal);
			CComVariant vDesFileName(pDesFileName);
			CComVariant vFileType(6);
			hr = activeBookDsp.Invoke2(L"SaveAs", &vDesFileName, &vFileType);
			if (hr != S_OK) throw hr;
		}
		catch (...)
		{
			HRESULT hre = checkComErrorManual();
		}
	}
	//释放
	::CoUninitialize();
	return hr;
}

BOOL writeBufToFile(LPCTSTR saveFile,const char* buf, size_t nDataSize,LPDWORD writeNumber){
	HANDLE hFile = CreateFile(saveFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	{
		BOOL rt = WriteFile(hFile, buf, nDataSize, writeNumber, NULL);
		CloseHandle(hFile);
		return rt;
	}
	return FALSE;
}
int printfWithBackCover(int bakCoverChars, const TCHAR* szFormat, ...){
	for (int i = 0; i < bakCoverChars; i++)
		_tprintf_s(_T("\b \b"));
		//_tprintf_s(_T("\b"));
	va_list pArgs;
	va_start(pArgs, szFormat);
	int maxStrlen =  _vtprintf_s(szFormat, pArgs);
	va_end(pArgs);
	return maxStrlen;
}
int printfWithBackCoverEnd(int bakCoverChars)
{
	printfWithBackCover(bakCoverChars, _T("%s"), _T("  \b\b"));
	return 0;
}
//按标号拆分cindex是从1开始的
void getTrdSection(size_t ssi, size_t sectionTotals, size_t cindex, OUT size_t &is, OUT size_t &ie) {
	size_t n = ssi / sectionTotals;
	size_t yu = ssi % sectionTotals;
	if (cindex < yu)
	{
		is = cindex * n + cindex;
		ie = is + n + 1;
	}
	else {
		is = cindex * n + yu;
		ie = is + n;
	}
}

#define MAXBLOCKSIZE 65535
#ifndef WIN32
BOOL downloadUrlFile(LPCTSTR Url, LPCSTR save_as)/*将Url指向的地址的文件下载到save_as指向的本地文件*/
{
	char Temp[MAXBLOCKSIZE];
	DWORD Number = 1;
	BOOL rt = FALSE;
	FILE* stream;
	HINTERNET hSession = InternetOpen("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET handle2 = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (handle2 != NULL)
		{
			errno_t et;
			if ((et = fopen_s(&stream, save_as, "wb")) == NULL)
			{
				while (Number > 0)
				{
					InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);

					fwrite(Temp, sizeof(char), Number, stream);
				}
				fclose(stream);
				rt = TRUE;
			}
			InternetCloseHandle(handle2);
			handle2 = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}
	return rt;
}
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT char*& buf,IN OUT size_t & nBufSize,OUT size_t & nDataSize )/*将Url指向的地址的文件下载到save_as指向的本地文件*/
{
	char Temp[MAXBLOCKSIZE];
	DWORD Number =1;
	BOOL rt = FALSE;
	HINTERNET hSession = InternetOpenA("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	
	if (hSession != NULL)
	{
		HINTERNET handle2 = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0);	
		
		if (handle2 != NULL)
		{
			nDataSize = 0;
			while (InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number) && Number > 0)
			{
				size_t nNewDataSize = nDataSize + Number;
				if (nBufSize < nNewDataSize) {
					size_t nNewBufSize = nNewDataSize * 1.1;
					char* _lpNew = new char[nNewBufSize];
					if (nBufSize > 0 && buf != NULL) {
						memcpy_s(_lpNew, nNewBufSize, buf, nDataSize);
						memcpy_s(_lpNew+ nDataSize, nNewBufSize- nDataSize, Temp, Number);
						delete[] buf; buf = _lpNew;						
					}
					else {
						memcpy_s((buf = _lpNew), nNewBufSize, Temp, Number);
					}
					nBufSize = nNewBufSize;
				}
				else {
					memcpy_s(buf + nDataSize, nBufSize - nDataSize, Temp, Number);
				}		
				nDataSize = nNewDataSize;
			}
			rt = TRUE;
			if (save_as) 
				writeBufToFile(save_as, buf, nDataSize, &Number);			
			InternetCloseHandle(handle2);
			handle2 = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}
	return rt;
}
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT DataBuffer& buf)/*将Url指向的地址的文件下载到save_as指向的本地文件*/
{
	BOOL rt = FALSE;
	HINTERNET handleFile = 0;
	HINTERNET hSession = InternetOpenA("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hSession)goto end;
	//DWORD option = 0; 
		//auto bIsq = InternetSetOptionA(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, (option = 2000, &option), sizeof(option));			//2秒的连接超时	
		//bIsq = InternetSetOptionA(hSession, INTERNET_OPTION_SEND_TIMEOUT, (option = 2000, &option), sizeof(option));					//2秒的发送超时	
		//bIsq = InternetSetOptionA(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, (option = 2000, &option), sizeof(option));				//2秒的接收超时	
		//bIsq = InternetSetOptionA(hSession, INTERNET_OPTION_DATA_SEND_TIMEOUT, (option = 2000, &option), sizeof(option));			//2秒的发送超时	
		//bIsq = InternetSetOptionA(hSession, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, (option = 2000, &option), sizeof(option));	    //2秒的接收超时

	//handleFile = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 0);
	handleFile = InternetOpenUrl(hSession, Url, NULL, 0,  INTERNET_FLAG_DONT_CACHE, 0);
	if (!handleFile)goto end;
	DWORD dwRead = 0;
	rt = InternetQueryDataAvailable(handleFile, &dwRead, 0, 0);
	if (!rt) { buf.clear(); goto end; }
	buf.resize(dwRead);
	rt = InternetReadFile(handleFile, buf.data(), buf.size(), &dwRead);
	if (!rt) { buf.clear(); goto end; }
	buf.resize(dwRead);
	if (save_as) {
		DWORD dwWrite = 0;
		rt = writeBufToFile(save_as, buf.data(), buf.size(), &dwWrite);
	}
end:
	if (handleFile)InternetCloseHandle(handleFile);
	if (hSession)InternetCloseHandle(hSession);
	return rt;
}
#else
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
BOOL downloadUrlFile(LPCTSTR Url, LPCTSTR save_as)/*将Url指向的地址的文件下载到save_as指向的本地文件*/
{
	if (URLDownloadToFile(NULL, Url, save_as, 0, 0) == S_OK)
		if (GetFileAttributes(save_as) != 0XFFFFFFFF) //0XFFFFFFFF表示文件不存在
			return TRUE;
	return FALSE;
}
BOOL downloadUrl(LPCTSTR url, LPCTSTR save_as, DataBuffer& buf)
{
	LPSTREAM lpStream = 0;
	buf.clear();
	if (URLOpenBlockingStream(NULL, url, &lpStream, 0, 0) == S_OK) {
		char tmp[MAXBLOCKSIZE];
		ULONG writed = 0;
		auto rt = lpStream->Read(tmp, MAXBLOCKSIZE, &writed);
		while (rt == S_OK || rt == E_PENDING) {
			if (rt == E_PENDING) { writed = 0; }
			else if (writed > 0) {
				buf.reserve(buf.size() + writed);
				for (ULONG i = 0; i < writed; ++i)buf.push_back(tmp[i]);
			}
			rt = lpStream->Read(tmp, MAXBLOCKSIZE, &writed);
		}
		lpStream->Release();		
		if (save_as) {
			DWORD dwWrite = 0;
			rt = writeBufToFile(save_as, buf.data(), buf.size(), &dwWrite);
		}
		return TRUE;
	}
	return FALSE;
}
#endif
DWORD CLTaskSvc::workThread(LPVOID param)
{	
	assert(param != NULL);
	std::vector<LONG_PTR>* var = (std::vector<LONG_PTR>*)param;
	CLTaskSvc* pts = (CLTaskSvc*)var->at(0);
	pts->lock();//自锁，等待调用者全部恢复完成
	pts->unlock();
	DWORD rt = 0;
	CLTaskSvcTrdSupport inf;	
	inf.nIndex = var->at(1);
	inf.quitEvent = (HANDLE)var->at(2);
	inf.tId = GetCurrentThreadId();
	inf.nTotals = (int)var->at(3);
	inf.hThread = (HANDLE)var->at(4);
	delete var;
	assert(inf.hThread != INVALID_HANDLE_VALUE);
	CLTaskSvcTrdParam data(inf);	
	if (pts->init(&data))
		rt = pts->run(&data);
	rt = pts->exist(rt, &data);
	if (data.info.quitEvent != 0 && data.info.quitEvent != INVALID_HANDLE_VALUE)
		CloseHandle(data.info.quitEvent);	
	return rt;
}

void CLTaskSvc::close()
{	
	auto ic = vec_threads.find(GetCurrentThreadId());
	if (ic == vec_threads.end()) {
		lock();
		if (getThreadsNum() > 0)
		{
			for (auto i = vec_threads.begin(); i != vec_threads.end();)
			{
				DWORD rt = WaitForSingleObject(i->second.hThread, 0);
				if (rt == WAIT_TIMEOUT) {
					::PostThreadMessage(i->second.tId, WM_QUIT, 0, 0);//第一种方式：发送WM_QUIT消息
					if (i->second.quitEvent) {
						SetEvent(i->second.quitEvent);//第二种方式：设置退出事件对象为有信号状态；
					}
					else
						if (i->second.hThread)
							TerminateThread(i->second.hThread, 0);//第三种方式，强制结束（如果这样可能造成内存泄漏）
				}
				++i;
			}
			for (auto i = vec_threads.begin(); i != vec_threads.end();)
			{
				DWORD rt = WaitForSingleObject(i->second.hThread, m_waitTimeToQuit);
				if (rt == WAIT_TIMEOUT)
				{
					if(i->second.hThread)TerminateThread(i->second.hThread, 0);
				}
				if(i->second.hThread)
					CloseHandle(i->second.hThread);
				i->second.reset();
				++i;
			}			
			vec_threads.clear();
		}		
		unlock();
	}	
}

//CLTaskSvcTrdSupport CLTaskSvc::getThreadInfo()
//{
//	DWORD nid = GetCurrentThreadId();
//	auto i = vec_threads.find(nid);
//	if (i != vec_threads.end())
//		return i->second;
//	else
//		return CLTaskSvcTrdSupport();
//}

//_clnew::~_clnew()
//	{
//		size_t si = size();
//		if (si > 0) {
//			CLString file;
//			file.getSpecialFolderPath() << _T("/CL_MEM_LOG_") << CLTime::getLocalTime_ll() << _T(".txt");
//			CLString tst;
//			size_t n = 1;
//			for (auto i = cbegin(); i != cend(); n++)
//			{
//				tst % n << _T(" : file=\"") << i->second.lpszFileName << _T("\", Line=") << i->second.nLine;
//				if (tst.writeLineToFile(n == 1 ? file.string() : NULL) == FALSE)
//					break;
//				++i;
//			}
//			tst.closeFile();
//		}
//	}
//
//_clnew& _clnew::getNewMen() {	
//	static _clnew _mem;
//	return _mem;
//}
//void* __cdecl operator new(size_t nSize, int nLine, LPCSTR lpszFileName) {
//	void* p = malloc(nSize);
//	_menUnit* i = &(getNewMen()[p]);
//	i->nLine = nLine;
//	i->lpszFileName = lpszFileName;
//	return p;
//}
//void __cdecl operator delete(void* p, int nLine, LPCSTR lpszFileName) {
//	getNewMen().erase(getNewMen().find(p));
//	return free(p);
//}
//void* __cdecl operator new[](size_t nSize, int nLine, LPCSTR lpszFileName) {
//	void* p = malloc(nSize);
//	_menUnit* i = &(getNewMen()[p]);
//	i->nLine = nLine;
//	i->lpszFileName = lpszFileName;
//	return p;
//}
//void __cdecl operator delete[](void* p, int nLine, LPCSTR lpszFileName) {
//	getNewMen().erase(getNewMen().find(p));
//	return free(p);
//}

//以普通权限启动进程
BOOL createProcessLow(TCHAR * lpApplicationName,
	TCHAR * lpCommandLine,
	TCHAR * lpDirectory,
	UINT nShow)
{
	if (!isVistaOrLater()
		|| !isAdminPrivilege())
	{
		HINSTANCE hRet = ShellExecute(NULL, _T("open"), lpApplicationName, lpCommandLine, lpDirectory, nShow);
		return (hRet > HINSTANCE(32));
	}
	HANDLE hToken = dupExplorerToken();

	if (hToken == NULL)
		return FALSE;

	static HMODULE hDll = LoadLibrary(_T("ADVAPI32.dll"));
	if (!hDll)
	{
		CloseHandle(hToken);
		return FALSE;
	}
#ifdef UNICODE
	PCreateProcessWithToken pfn = (PCreateProcessWithToken)GetProcAddress(hDll, "CreateProcessWithTokenW");
#else
	PCreateProcessWithToken pfn = (PCreateProcessWithToken)GetProcAddress(hDll, "CreateProcessWithTokenA");
#endif
	if (!pfn)
	{
		CloseHandle(hToken);
		return FALSE;
	}
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	BOOL ret = pfn(hToken,
		LOGON_WITH_PROFILE,
		lpApplicationName,
		lpCommandLine,
		NORMAL_PRIORITY_CLASS,
		NULL,
		lpDirectory,
		&si,
		&pi);
	if (ret)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	CloseHandle(hToken);
	return ret;
}

//以管理员权限启动进程
BOOL createProcessHigh(TCHAR * lpApplicationName,
	TCHAR * lpCommandLine,
	TCHAR * lpDirectory,
	UINT nShow)
{
#ifdef _UNICODE
	wstring command;
#else
	string command;
#endif

	if (lpCommandLine)
	{
		command = lpCommandLine;
	}
	if (isVistaOrLater()
		&& !isAdminPrivilege())
	{
		command += _T(" -Admin");
	}

	HINSTANCE hRet = ShellExecute(NULL, _T("runas"), lpApplicationName, command.c_str(), lpDirectory, nShow);
	return (hRet > (HINSTANCE)32 );
}


HANDLE dupExplorerToken()
{
	DWORD dwPid = 0;
	HWND hwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (NULL == hwnd)
		return NULL;

	GetWindowThreadProcessId(hwnd, &dwPid);
	if (dwPid == 0)
		return NULL;

	HANDLE hExplorer = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hExplorer == NULL)
		return NULL;

	HANDLE hToken = NULL;
	OpenProcessToken(hExplorer, TOKEN_DUPLICATE, &hToken);
	CloseHandle(hExplorer);

	HANDLE hNewToken = NULL;
	DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hNewToken);
	CloseHandle(hToken);

	return hNewToken;
}

#pragma warning(disable: 4996)      
BOOL isVistaOrLater()
{
	OSVERSIONINFOEX version = { sizeof(OSVERSIONINFOEX) };
	if (!GetVersionEx((LPOSVERSIONINFO)&version))
	{
		version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((LPOSVERSIONINFO)&version))
		{
			return FALSE;
		}
	}

	return (version.dwMajorVersion >= 6);
}

BOOL isAdminPrivilege()
{
	BOOL bIsAdmin = FALSE;
	BOOL bRet = FALSE;
	SID_IDENTIFIER_AUTHORITY idetifier = SECURITY_NT_AUTHORITY;
	PSID pAdministratorGroup;
	if (AllocateAndInitializeSid(
		&idetifier,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorGroup))
	{
		if (!CheckTokenMembership(NULL, pAdministratorGroup, &bRet))
		{
			bIsAdmin = FALSE;
		}
		if (bRet)
		{
			bIsAdmin = TRUE;
		}
		FreeSid(pAdministratorGroup);
	}

	return bIsAdmin;
}

#define CALL_VAR(apiName,varName) \
if ((varName)== 0) { \
	auto le = GetLastError();\
	if (le != ERROR_SUCCESS) {\
		(CLString() << _T(#apiName) << _T(", ")<< CLString().getLastErrorStringR(le) << CLRet).logout();\
	}\
}
#define CALL_HANDLE(apiProcess,varName) \
if (((varName) = (apiProcess)) == 0) { \
	auto le = GetLastError();\
	if (le != ERROR_SUCCESS) {\
		(CLString() << _T(#apiProcess) << _T("; ")<< CLString().getLastErrorStringR(le) << CLRet).logout();\
	}\
}
#define CALL_BOOL(apiProcess) \
 (((apiProcess) == FALSE) ? ((GetLastError() != ERROR_SUCCESS) ? ((CLString() << _T(#apiProcess) << _T("; ") << CLString().getLastErrorStringR( GetLastError()) << CLRet).logout(),FALSE):FALSE):TRUE)

BOOL isInstallService(LPCTSTR servName) {
	BOOL rt = FALSE;
	SC_HANDLE scmHandle;
	CALL_HANDLE(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS), scmHandle);
	if (scmHandle) {
		SC_HANDLE serviceHandle;;
		serviceHandle = OpenService(scmHandle, servName, SERVICE_QUERY_STATUS);
		if (serviceHandle) { 
			rt = TRUE;
			CALL_BOOL(CloseServiceHandle(serviceHandle));
		}
		CALL_BOOL(CloseServiceHandle(scmHandle));
	}
	return rt;
}
BOOL installService(LPCTSTR servName, LPCTSTR lpszBinaryPathName, LPCTSTR lpServiceDisplayName, LPCTSTR lpDescription)
{
	BOOL rt = FALSE;
	SC_HANDLE schSCManager, schService;
	CALL_HANDLE(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS), schSCManager);
	if (schSCManager) {
		schService = CreateService((struct SC_HANDLE__ *)schSCManager,
			servName,
			lpServiceDisplayName ? lpServiceDisplayName : servName,         // service name to display  
			SERVICE_ALL_ACCESS,          // desired access  
			SERVICE_WIN32_OWN_PROCESS, // service type  
			SERVICE_AUTO_START,        // start type  
			SERVICE_ERROR_NORMAL,        // error control type  
			lpszBinaryPathName,          // service's binary  
			NULL,                        // no load ordering group  
			NULL,                        // no tag identifier  
			NULL,                        // no dependencies  
			NULL,                        // LocalSystem account  
			NULL);                       // no password  
		CALL_VAR(CreateService, schService);
		if (schService) {
			rt = TRUE;
			if (lpDescription) {
				SERVICE_DESCRIPTION   sdBuf = { 0 };
				TCHAR Des[1024] = { 0 };
				_tcscpy_s(Des, 1024, lpDescription);
				sdBuf.lpDescription = Des;
				CALL_BOOL(ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sdBuf));
			}
			Sleep(1000);
			CALL_BOOL(CloseServiceHandle((struct SC_HANDLE__ *)schService));
		}		
		CALL_BOOL(CloseServiceHandle((struct SC_HANDLE__ *)schSCManager));
	}
	return rt;
}

BOOL startService(LPCTSTR servName)
{
	BOOL rt = FALSE;
	SC_HANDLE scmHandle;
	CALL_HANDLE(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS), scmHandle);
	if (scmHandle) {
		SC_HANDLE serviceHandle;
		CALL_HANDLE(OpenService(scmHandle, servName, SERVICE_ALL_ACCESS), serviceHandle);
		if (serviceHandle) {
			SERVICE_STATUS serviceStatus = { 0 };
			CALL_BOOL(QueryServiceStatus(serviceHandle, &serviceStatus));
			if (serviceStatus.dwCurrentState == SERVICE_START &&
				serviceStatus.dwCurrentState != SERVICE_START_PENDING)
			{
				rt = TRUE;
			}
			else rt = CALL_BOOL(StartService(serviceHandle, 0, NULL));
			Sleep(1000);
			CALL_BOOL(CloseServiceHandle(serviceHandle));
		}
		CALL_BOOL(CloseServiceHandle(scmHandle));
	}
	return rt;
}
BOOL stopService(LPCTSTR servName)
{
	BOOL rt = FALSE;
	SC_HANDLE scmHandle;
	CALL_HANDLE(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS), scmHandle);
	if (scmHandle) {
		SC_HANDLE serviceHandle;
		CALL_HANDLE(OpenService(scmHandle, servName, SERVICE_ALL_ACCESS), serviceHandle);
		if (serviceHandle) {
			SERVICE_STATUS serviceStatus;
			CALL_BOOL(QueryServiceStatus(serviceHandle, &serviceStatus));
			if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
				CALL_BOOL(ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus));
				do {
					CALL_BOOL(QueryServiceStatus(serviceHandle, &serviceStatus));
					Sleep(1000);
				} while (serviceStatus.dwCurrentState != SERVICE_STOPPED);		
				rt = TRUE;
				Sleep(1000);
			}
			CALL_BOOL(CloseServiceHandle(serviceHandle));
		}
		CALL_BOOL(CloseServiceHandle(scmHandle));		
	}
	return rt;
}
BOOL uninstallService(LPCTSTR servName)
{
	BOOL rt = FALSE;
	SC_HANDLE scmHandle;
	CALL_HANDLE(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS), scmHandle);
	if (scmHandle) {
		SC_HANDLE serviceHandle;
		CALL_HANDLE(OpenService(scmHandle, servName, SERVICE_ALL_ACCESS), serviceHandle);
		if (serviceHandle) {
			SERVICE_STATUS serviceStatus;
			CALL_BOOL(QueryServiceStatus(serviceHandle, &serviceStatus));
			if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
				CALL_BOOL(ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus));
				do {
					CALL_BOOL(QueryServiceStatus(serviceHandle, &serviceStatus));
					Sleep(1000);
				} while (serviceStatus.dwCurrentState != SERVICE_STOPPED);
				Sleep(1000);
			}
			rt = CALL_BOOL(DeleteService(serviceHandle));
			CALL_BOOL(CloseServiceHandle(serviceHandle));
		}
		CALL_BOOL(CloseServiceHandle(scmHandle));
	}
	return rt;
}


int messageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, DWORD dwMilliseconds) {
	typedef int(WINAPI * PMessageBoxTimeoutA)(HWND, LPCSTR, LPCSTR, UINT, WORD, DWORD);
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	if (hUser32 != NULL && hUser32 != INVALID_HANDLE_VALUE) {
		PMessageBoxTimeoutA pfun = (PMessageBoxTimeoutA)GetProcAddress(hUser32, "MessageBoxTimeoutA");
		int ret = pfun(hWnd, lpText, lpCaption, uType, 0, dwMilliseconds);
		FreeLibrary(hUser32);
		return ret;
	}
	else return 0;
}
int messageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, DWORD dwMilliseconds) {
	typedef int(WINAPI * PMessageBoxTimeoutW)(HWND, LPCWSTR, LPCWSTR, UINT, WORD, DWORD);
	HMODULE hUser32 = LoadLibraryW(L"user32.dll");
	if (hUser32 != NULL && hUser32 != INVALID_HANDLE_VALUE) {
		PMessageBoxTimeoutW pfun = (PMessageBoxTimeoutW)GetProcAddress(hUser32, "MessageBoxTimeoutW");
		int ret = pfun(hWnd, lpText, lpCaption, uType, 0, dwMilliseconds);
		FreeLibrary(hUser32);
		return ret;
	}
	else return 0;	
}

//int messageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, DWORD dwMilliseconds)
//{
//	return MessageBoxTimeout(hWnd, lpText, lpCaption, uType, 0,dwMilliseconds);
//}

void getCpuInfo(CLString& chProcessorName, CLString& chProcessorType, DWORD& dwCoreNum, DWORD& dwMaxClockSpeed)
{
	CLString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//注册表子键路径  
	CLString regkey;//定义注册表类对象  
	//获取ProcessorNameString字段值  
	if (regkey.getReg(HKEY_LOCAL_MACHINE, strPath, _T("ProcessorNameString")))
	{
		chProcessorName = regkey;
	}

	//查询CPU主频  
	DWORD dwValue;
	if (strPath.asPathToGetRegValue(HKEY_LOCAL_MACHINE, _T("~MHz"), sizeof(DWORD), &dwValue, REG_DWORD)) {
		dwMaxClockSpeed = dwValue;
	}
	//UpdateData(FALSE);  

	//获取CPU核心数目  
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);
	dwCoreNum = si.dwNumberOfProcessors;

	switch (si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
	{
		chProcessorType.format(_T("Intel 386 processor"));
	}
	break;
	case PROCESSOR_INTEL_486:
	{
		chProcessorType.format(_T("Intel 486 Processor"));
	}
	break;
	case PROCESSOR_INTEL_PENTIUM:
	{
		chProcessorType.format(_T("Intel Pentium Processor"));
	}
	break;
	case PROCESSOR_INTEL_IA64:
	{
		chProcessorType.format(_T("Intel IA64 Processor"));
	}
	break;
	case PROCESSOR_AMD_X8664:
	{
		chProcessorType.format(_T("AMD X8664 Processor"));
	}
	break;
	default:
		chProcessorType.format(_T("未知"));
		break;
	}
}

inline CLCSLock::~CLCSLock()
{
	if (m_csLst) {
		std::map<int, cstype*>::iterator i = m_csLst->begin();
		for (; i != m_csLst->end();)
		{
			if (i->second) {
				delete i->second; i->second = 0;
			}
			else {
				//throw std::logic_error("CriticalSection itern didn't alloc!");
			}
			++i;
		}
		m_csLst->clear();
		delete m_csLst;
		m_csLst = 0;
	}
	/*if (m_cs) {
	delete m_cs;
	m_cs = 0;
	}*/
}

//自由锁，可用于子类自由调用，csId在lock和unlock过程中前后必须一致

inline void CLCSLock::lock(int csId)
{
	if (m_csLst == 0) {
		m_csLst = new std::map<int, cstype*>;
		m_csLst == 0 ? throw std::logic_error("CriticalSection List didn't alloc!") : 0;
	}
	std::map<int, cstype*>::iterator i = m_csLst->find(csId);
	cstype* pcs = 0;
	if (i == m_csLst->end()) {
		pcs = new cstype;
		(*m_csLst)[csId] = pcs;
	}
	else { pcs = i->second; }
	pcs ? pcs->lock() : throw std::logic_error("CriticalSection itern didn't alloc!");
}

//自由锁，可用于子类自由调用，csId在lock和unlock过程中前后必须一致

inline void CLCSLock::unlock(int csId)
{
	if (m_csLst == 0)
		throw std::logic_error("CriticalSection List didn't alloc!");
	std::map<int, cstype*>::iterator i = m_csLst->find(csId);
	if (i == m_csLst->end()) {
		throw std::logic_error("CriticalSection itern has not entered!");
	}
	else {
		i->second ? i->second->unlock() : throw std::logic_error("CriticalSection itern didn't alloc!");
	}
}

inline _rw_cs::_rw_cs() noexcept {
	InitializeSRWLock(this);
	ownerThreadId = 0;
	lockCounts = 0;
}

#define OPEN_DEADLOCK_CHECK  FALSE //设置值为TRUE，用于检查死锁或长等待异常
inline void _rw_cs::lock() {
#if OPEN_DEADLOCK_CHECK == TRUE
	auto cid = GetCurrentThreadId();
	size_t counts = 0;
	while (!trylock()) {
		Sleep(1);
		++counts;
		if (counts >= 2000) {
			char str[100];
			sprintf_s(str, "产生异常：线程(%ld)可能发生死锁，一直在等待所有者线程(%ld)释放锁！\r\n", cid, ownerThreadId);
			messageBoxT(0, str, "cs lock alert", MB_OK | MB_ICONWARNING,3000);
			cout << str;
			counts = 0;
		}
	}
	return;
#else
	auto cid = GetCurrentThreadId();
	if (ownerThreadId != cid) {
		AcquireSRWLockExclusive(this);
		ownerThreadId = cid;
	}
	++lockCounts;
#endif
}

inline void _rw_cs::unlock() {
	auto cid = GetCurrentThreadId();
	if (cid != ownerThreadId)
		return;
	if (--lockCounts == 0) {
		ownerThreadId = 0;
		ReleaseSRWLockExclusive(this);
	}
}

inline bool _rw_cs::trylock() {
	auto cid = GetCurrentThreadId();
	if (ownerThreadId != cid) {
		if (TryAcquireSRWLockExclusive(this) == FALSE)
			return false;
		ownerThreadId = cid;
	}
	++lockCounts;
	return true;
}

inline _at_cs::_at_cs() noexcept {
	bLock = FALSE;
	ownerThreadId = 0;
	lockCounts = 0;
}

inline void _at_cs::lock() {
	auto cid = GetCurrentThreadId();
	if (ownerThreadId != cid) {
		while (InterlockedExchange(&bLock, TRUE) == TRUE) {
			Sleep(0);
		}
		ownerThreadId = cid;
	}
	++lockCounts;
}

inline void _at_cs::unlock() {
	auto cid = GetCurrentThreadId();
	if (cid != ownerThreadId)
		return;
	if (--lockCounts == 0) {
		ownerThreadId = 0;
		bLock = FALSE;
	}
}

inline bool _at_cs::trylock() {
	auto cid = GetCurrentThreadId();
	if (ownerThreadId != cid) {
		while (InterlockedExchange(&bLock, TRUE) == TRUE)
			return false;
		ownerThreadId = cid;
	}
	++lockCounts;
	return true;
}

inline CLParallelPass::CLParallelPass(UINT passCounts) {
	init(passCounts);
}

inline CLParallelPass::~CLParallelPass() {
	if (flag != NULL) {
		delete[] flag;
		m_counts = 0;
		m_bufCounts = 0;
	}
}

inline CLParallelPass& CLParallelPass::init(UINT passCounts) {
	if (flag) {
		if (passCounts > m_bufCounts) {
			delete[] flag;
			flag = new UCHAR[m_bufCounts = passCounts];
		}
	}
	else {
		if (passCounts > 0)
			flag = new UCHAR[m_bufCounts = passCounts];
	}
	m_counts = passCounts;
	reclaimAllPass();
	return *this;
}

inline UINT CLParallelPass::waitPass(UINT passIndex) {
	if (passIndex >= m_counts)
		throw std::overflow_error("CLParallelPass::waitPass(),passIndex out of range!");
	UINT all = 0;
	for (;;) {
		if (flag[passIndex] == 1)
			return m_key;
		if (++all >= m_core)
			SwitchToThread(), all = 0;
	}
	return m_key;
}

inline void CLParallelPass::giveBackPass(UINT passIndex) {
	if (passIndex >= m_counts)
		throw std::overflow_error("CLParallelPass::giveBackPass(),passIndex out of range!");
	flag[passIndex] = 0;
}

inline void CLParallelPass::handOutPass(UINT orderKey) {
	m_key = orderKey;
	for (UINT i = 0; i < m_counts; i++)
		flag[i] = 1;
	for (UINT i = 0; i < m_counts; i++)
	{
		UINT all = 0;
		while (flag[i] != 0) {
			if (++all >= m_core)
				SwitchToThread(), all = 0;
		}
	}
}

inline void CLParallelPass::reclaimAllPass() {
	for (UINT i = 0; i < m_counts; i++)
		flag[i] = 0;
}

inline UINT CLParallelPass::getPassCounts() const {
	return m_counts;
}
