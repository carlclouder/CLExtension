
#pragma once

#ifndef __CL_JSONTASK__
#define __CL_JSONTASK__

//input outside library
#include "../_cl_common/CLCommon.h"
#include "../_jsoncpp/json.h"
#include "../_cl_string/CLString.h"


//取得节点工作类型名,lpkey为一个变量名
#define GET_JOB_TYPE( lpkey ) LPCSTR lpkey = var->trdParm_json[JTASK_Key_JobType].isString() ? \
	(var->trdParm_json[JTASK_Key_JobType].asCString()): "*未知类型*" ;
//取得网页原始编码,code为一个变量名
#define GET_ORG_CODE( code ) byte code = 0;setOrgCode(var,code);
//返回0，并退出
#define RET_FALSE \
	goto Ret0;
//返回1，并退出
#define RET_TRUE \
	goto Ret1;
//函数体末尾退出宏
#define PROC_RETURN \
	Ret1:\
	return 1;\
	Ret0:\
	return 0;
//让单元判断是否需要构造字符对象
#define logoutu( x ) (isLogOpen() ? logout((x)):0)
//定义常量字符串指针变量宏,失败后退出
#define MAKEVAR_LPTR_( varName , varSec )  \
	LPCSTR varName = var->trdParm_json[#varSec].isString() ? \
	var->trdParm_json[#varSec].asCString() : \
	(logoutu( (var->strTmp % _T("ERROR：第") << (var->index) <<  _T("工作单元的\"")	<< _T( #varSec ) <<_T("\"字段不存在或类型错误！")).string()),0);\
	if(varName == 0)\
		RET_FALSE;	
//定义常量字符串指针变量宏,失败后退出
#define MAKEVAR_ARRAY_( varName , varSec , numb ) \
	Json::Value varName;if(var->trdParm_json[#varSec].isArray())\
	varName = var->trdParm_json[#varSec];\
	if(varName.size() != numb ){\
		logoutu( (var->strTmp % _T("ERROR：第") << (var->index) <<  _T("工作单元的\"")	<< _T( #varSec ) <<_T("\"字段的列表不存在或内容个数错误！")).string()); \
		RET_FALSE;\
	}	

//将步长输出
#define STEPS_PUTOUT( ci, cn ) \
	stepsOut((ci),(cn));

//循环开始宏
#define CYCLE_BEGIN_() \
	double _s_ticks = CLTime::getTickCountsMicro(),_s_ticks2 = _s_ticks; \
	for (;;) { \
		WHEN_EVENT_QUIT_ARRIVE_BREAK_LOOP(var->quitEvent); \
		WHEN_WM_QUIT_ARRIVE_BREAK_LOOP(); \
		STEPS_PUTOUT(1, 100);

//循环结尾宏
#define CYCLE_END_() \
cycle: \
	_s_ticks = CLTime::getTickCountsMicro();\
	logoutu( var->strTmp.format(200,_T("完成: [%s]-><%.6fs> ..."),var->fatherRootKey.string(),_s_ticks - _s_ticks2).string());\
	_s_ticks2 = _s_ticks;\
	}


using namespace std;
using namespace Json;

//定义的固化key内容
#define JTASK_Key_State ("状态")
#define JTASK_Key_OpemLog ("外显")
#define JTASK_Key_JobType ("采集类型")
#define JTASK_Key_Notes ("注释")
#define JTASK_Key_ExeTrdNumber ("执行线程数")
#define JTASK_Key_ExeTimePoint ("执行时间点")
#define JTASK_Key_ExeTimePoint_Date ("日期区间")
#define JTASK_Key_ExeTimePoint_Time ("时间区间")
#define JTASK_Key_ExeTimePoint_Week ("星期")
#define JTASK_Key_ExeJobsVar ("工作组参数")
#define JTASK_Key_DataPageUrl ("数据页面链接")
#define JTASK_Key_DataOrgCode ("原始数据编码")


typedef int(__cdecl *JTASK_INFLOGOUT_CALLBACK)(LPCTSTR format); //数据输出回调函数指针
typedef int(__cdecl *JTASK_STEPSPUTOUT_CALLBACK)(DWORD currentIndex, DWORD totalCounts); //数据输出回调函数指针

//节点共享的父类数据,内部单元共享参数传递结构体
typedef class JTASK_SHARE {
public:
	JTASK_INFLOGOUT_CALLBACK* ppFuncInfputout;//输出信息接口指针变量的指针
	JTASK_STEPSPUTOUT_CALLBACK* ppFuncStepsPutout;//输出线程内部当前所处进度的接口变量指针
	UINT* pTrdQuitMsgId;
	HWND* pHWnd;
}*PJTASK_SHARE;

//JTASKU内部使用的线程数据结构包，可由子类继承,重载reset()实现初始化
typedef class JTASK_TRDINF {
public:
	//传入变量------------------------------------------
	size_t index;//线程编号，1开始计数
	DWORD nId;//本线程id	
	HANDLE handle;//本线程句柄	
	HANDLE quitEvent;//退出信号事件，该变量可监控外部退出事件
	DWORD ownerTrdId;//调用线程id
	UINT quitMsgId;//线程退出后，向主线程或窗口过程发送的消息 
	HWND hWnd;//发送消息给窗口过程

	CLString fatherRootKey;//父单元根键名
	size_t fatherRootKeyIndex;//父单元所处索引号

	//预定义功能变量--------------------------------
	CLString strTmp;//供线程使用的临时变量
	//传出变量--------------------------------------
	int iRet; // -1表示没有执行主过程，0表示执行出错，1表示成功
	//工作组Json参数变量--------------------------------------
	Json::Value trdParm_json;//传递给线程的json对象参数
	
	JTASK_TRDINF(); 
	virtual~JTASK_TRDINF();
	virtual void reset(); 
} *PJTASK_TRDINF;//JTASKU内部使用的线程数据结构包

//工作节点单元(单位工作组)类,请在子类中重载mainProc()函数实现自定义功能
//1、可根据需要定义类A，继承自JTASK_TRDINF数据结构，增加自定义变量；
//2、定义工作组模版类B，继承自模板类CLJsonTaskUnit<A>，并重写主过程方法mainProc()，如果对结构A的数据填充做操作请重载fillThreadData()；
template<typename JTASK_TRDINF_>
class CLJsonTaskUnit :public CLTaskSvc{
private:
	CLString m_rootKey;
	BOOL m_isSetJson;
	Value m_json;
	const JTASK_SHARE* m_pShare;
	size_t m_unitIndex;
	std::vector<JTASK_TRDINF_*> JTASK_TRDINFLst;
	size_t m_trdNumber;
	BOOL m_isLogOpen;
public:
	//内部工作组是否在运行中
	BOOL isJobsRunning()
	{
		size_t si = JTASK_TRDINFLst.size();

		for (size_t i = 0; i < si; i++) {
			PJTASK_TRDINF p = JTASK_TRDINFLst.at(i);
			lock();
			if (isTrdRunning(p->handle)) {
				unlock();
				return TRUE;
			}
			else unlock();
		}
		return FALSE;
	}
	CLStringRC rootKey() { return m_rootKey; }//取得该节点的主键值
	BOOL isSetJson() const { return m_isSetJson; }//检查是否设置过数据
	CLJsonTaskUnit& quitJobs() { _quitAndClearTrdLst(0); return *this; }//退出工作组的外部唯一手段，退出并清理工作组所有资源（如果有的话）
	CLJsonTaskUnit(const JTASK_SHARE* pShare) :m_pShare(pShare)
	{
		_init();
	}
	virtual ~CLJsonTaskUnit()
	{
		_reset();
		_clear();
	}
	//启动节点 -1节点禁用， 0时间范围外，1正常组装并启动，2未组装工作组已启动
	int execute(BOOL timeCheck)
	{
		//流程组是否运行
		if (isJobsRunning()) {
			logoutu(CLString().format(200, _T("节点\"%s\"工作组已运行。"), rootKey().string()).string());
			return 2;
		}

		//是否节点可用状态
		if (isUnitAvailable() == FALSE) {
			logoutu(CLString().format(200, _T("节点\"%s\"已禁用。"), rootKey().string()).string());
			return -1;
		}

		//是否在时间范围	
		if (timeCheck == TRUE && _isInTimeRang() == FALSE) {
			logoutu(CLString().format(200, _T("节点\"%s\"处在执行时间范围外。"), rootKey().string()).string());
			return 0;
		}

		logoutu(CLString().format(200, _T("节点\"%s\"运行。"), rootKey().string()).string());

		//获取线程条数
		if (m_json[JTASK_Key_ExeTrdNumber].isInt() == true) {
			m_trdNumber = m_json[JTASK_Key_ExeTrdNumber].asInt() < 1 ? CLTaskSvc::getCpuCoreCounts() : m_json[JTASK_Key_ExeTrdNumber].asInt();
		}
		else m_trdNumber = CLTaskSvc::getCpuCoreCounts();

		//组装线程队列
		_quitAndClearTrdLst(m_trdNumber);//退出,整理列表

		size_t curr = 0, cshould = m_trdNumber; size_t _index = 0;
		auto ite = JTASK_TRDINFLst.begin();
		for (; ite != JTASK_TRDINFLst.end(); )
		{
			(*ite)->reset();
			(*ite)->quitMsgId = m_pShare ? *(m_pShare->pTrdQuitMsgId) : WM_NULL;
			(*ite)->hWnd = (m_pShare && (m_pShare->pHWnd) && *(m_pShare->pHWnd) && ::IsWindow(*(m_pShare->pHWnd))) ? *(m_pShare->pHWnd) : 0;
			(*ite)->ownerTrdId = GetCurrentThreadId();
			(*ite)->trdParm_json = m_json[JTASK_Key_ExeJobsVar];
			(*ite)->fatherRootKey = rootKey();
			(*ite)->fatherRootKeyIndex = m_unitIndex;
			(*ite)->index = ++_index;
			if (FALSE == fillThreadData((*ite), _index, JTASK_TRDINFLst.size()))
				return -1;
			++ite;
		}
		//启动所有线程
		curr = CLTaskSvc::start(m_trdNumber,TRUE);
		for (CLTaskSvc::ThreadsTable::const_iterator i = getThreadsTable().cbegin(); i != getThreadsTable().cend(); )
		{
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->handle = i->second.hThread;
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->nId = i->second.tId;
			JTASK_TRDINFLst.at(i->second.nIndex - 1)->quitEvent = i->second.quitEvent;
			++i;
		}
		resume();//显示启动

		logoutu(CLString().format(200, curr == cshould ? _T("工作组启动 <%d/%d>,time: %s .") : _T("工作组启动 <%d/%d>,time: %s,WARNING:启动并不完全！")
			, curr, cshould, CLTime().getLocalTime().dateTimeStr(CLString()).string()).string());

		return curr;
	}
	//节点是否可启动，不管有无设置必要数据
	BOOL isUnitAvailable()
	{
		if (!isSetJson())
			return FALSE;
		if (m_json[JTASK_Key_State].isNull() == true)
			return TRUE;
		else if (m_json[JTASK_Key_State].isBool() == false)
			return TRUE;
		else if (m_json[JTASK_Key_State].asBool() == true)
			return TRUE;
		else
			return FALSE;
	}
	CLJsonTaskUnit& setUnitJson(const Value& json, int index, LPCSTR rootKey = 0)
	{
		m_json = json;
		m_rootKey = rootKey;
		m_unitIndex = index < 0 ? (size_t)0 : (size_t)index;
		if (m_json.type() == Json::objectValue)
			m_isSetJson = TRUE;
		if (isSetJson())
			m_isLogOpen = m_json[JTASK_Key_OpemLog].isBool() ? m_json[JTASK_Key_OpemLog].asBool() : TRUE;
		else m_isLogOpen = TRUE;
		if (rootKey) {
			logoutu(CLString().format(200, _T("设置执行节点\"%s\"。"), this->rootKey().string()).string());
		}
		
		return *this;
	}
	inline BOOL isLogOpen() const {return m_isLogOpen;}
	static BOOL isTrdRunning(HANDLE hd) { return hd && ::WaitForSingleObject(hd, 0) == WAIT_TIMEOUT; }
	//由json文件设置取得网络数据的编码形式
	static void setOrgCode(PJTASK_TRDINF var, byte& code)
	{
		assert(var != NULL);
		code = EnCode_UTF8;
		if (var->trdParm_json[JTASK_Key_DataOrgCode].isString()) {
			if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UTF-8") == 0)
				code = EnCode_UTF8;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UTF8") == 0)
				code = EnCode_UTF8;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "ASCII") == 0)
				code = EnCode_ASCII;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UNICODE") == 0)
				code = EnCode_ULE;
			else if (_stricmp(var->trdParm_json[JTASK_Key_DataOrgCode].asCString(), "UNICODE_BIG_ENDIAN") == 0)
				code = EnCode_UBE;
		}
	}
protected:			
	void _init(){
		m_isSetJson = FALSE;
		m_trdNumber = 1;
		m_unitIndex = -1;
		m_isLogOpen = TRUE;
	}
	void _reset(){}
	void _clear(){
		for (size_t i = 0; i < JTASK_TRDINFLst.size(); i++) {
			JTASK_TRDINF_* p = JTASK_TRDINFLst.at(i);
			if (p)
				delete p;
		}
		JTASK_TRDINFLst.clear();
	}
	BOOL _isInTimeRang()
	{
		if (!isSetJson())return FALSE;
		if (m_json[JTASK_Key_ExeTimePoint].isObject() == false)return FALSE;
		BOOL bd = TRUE, bt = TRUE;
		TIMESTAMP ct = CLTime::getLocalTime_ll();
		Value tjson = m_json[JTASK_Key_ExeTimePoint];
		if (tjson[JTASK_Key_ExeTimePoint_Date].isNull() == false && tjson[JTASK_Key_ExeTimePoint_Date].isArray() == true) {//set time
			long date = (long)(ct / 1000000000);
			Value ary = tjson[JTASK_Key_ExeTimePoint_Date];
			Value::UInt si = ary.size();
			bd = FALSE;
			for (Value::UInt i = 0; i < si; i += 2)
			{
				if (ary[i].isInt() == true) {
					if (date < ary[i].asInt())
						continue;
				}
				if (ary[i + 1].isInt() == true) {
					if (date <= ary[i + 1].asInt()) {
						bd = TRUE; break;;
					}
				}
				else bd = TRUE;
			}
		}
		if (tjson[JTASK_Key_ExeTimePoint_Time].isNull() == false && tjson[JTASK_Key_ExeTimePoint_Time].isObject() == true) {
			long time = (long)(ct % 1000000000);
			int wk = CLTime::getDayOfWeekByStamp_ll(ct);
			Value tp = tjson[JTASK_Key_ExeTimePoint_Time];
			Value::Members member = tp.getMemberNames();
			bt = FALSE;
			CLString szt;
			for (size_t i = 0; bt == FALSE && i < member.size(); i++)
			{
				szt = member.at(i).c_str();
				if (szt.size() >= 2 && szt.find(_T(',')) > -1) {// check ,
					szt.trim().split(_T(','));
					BOOL isyes = FALSE;
					for (size_t k = 0; isyes == FALSE && szt.getVT().size() > k; k++)
					{
						if (wk == _ttoi(szt.getVT().at(k))) { isyes = TRUE; }
					}
					if (isyes == FALSE)continue;
				}
				else if (szt.size() >= 3 && szt.find(_T('-')) > -1) { // check -
					szt.trim().split(_T('-'));
					int k1 = _ttoi(szt.getVT().at(0));
					int k2 = _ttoi(szt.getVT().at(1));
					if (BETWEEN(wk, k1, k2) == FALSE)continue;
				}
				else { if (szt.stoi() != wk)continue; }
				Value ary = tp[szt.getASCII()];
				Value::UInt si = ary.size();
				for (Value::UInt i = 0; i < si; i += 2)
				{
					if (ary[i].isInt() == true) {
						if (time < ary[i].asInt())
							continue;
					}
					if (ary[i + 1].isInt() == true) {
						if (time <= ary[i + 1].asInt()) {
							bt = TRUE; break;;
						}
					}
					else bt = TRUE;
				}
			}
		}
		return (bd && bt) ? TRUE : FALSE;
	}
	//只退出线程，不清理列表
	void _quitJobs()
	{
		size_t si = JTASK_TRDINFLst.size();
		for (size_t i = 0; i < si; i++)
		{
			PJTASK_TRDINF p = JTASK_TRDINFLst.at(i);
			HANDLE hd = p->handle;
			lock();
			if (isTrdRunning(p->handle)) {
				::PostThreadMessage(p->nId, WM_QUIT, 0, 0);
				unlock();
				::WaitForSingleObject(hd, INFINITE);
			}
			else unlock();
			p->reset();
		}		

		CLTaskSvc::close();
	}
	//退出线程，清理列表,留下指定的个数的列表变量,0表示退出并清理全部对象
	void _quitAndClearTrdLst(size_t leave  = 0 )
	{
		_quitJobs();
		size_t si = JTASK_TRDINFLst.size();
		if (leave == 0) {
			for (size_t i = 0; i < si; i++)
				delete JTASK_TRDINFLst.at(i);
			JTASK_TRDINFLst.clear();
		}
		else if (leave > si) {
			for (; si < leave; si++) { JTASK_TRDINFLst.push_back(new JTASK_TRDINF_); }
		}
		else if (leave < si) {
			for (size_t i = si - 1; i >= leave; i--) {
				delete JTASK_TRDINFLst.at(i);
				JTASK_TRDINFLst.erase(JTASK_TRDINFLst.begin() + i);
			}
		}
	}
	//（无特殊情况请勿重载）
	virtual DWORD run(PCLTaskSvcTrdParam _var);
		
	//用于输出外现信息的函数，可重载以改变实现（无特殊情况请勿重载）
	virtual int logout(LPCTSTR str) {
		return (m_isLogOpen ? (m_pShare ? ((m_pShare->ppFuncInfputout) ? ((*(m_pShare->ppFuncInfputout)) ? (*(m_pShare->ppFuncInfputout))(str) : FALSE) : FALSE) : FALSE) : FALSE);
	}
	//用于输出步长信息的函数，可重载以改变实现（无特殊情况请勿重载）（自带线程组锁）
	virtual int stepsOut(DWORD currentIndex, DWORD totalCounts) {
		if (m_pShare && (m_pShare->ppFuncStepsPutout) && (*(m_pShare->ppFuncStepsPutout))) { 
			lock(0); int rt = (*(m_pShare->ppFuncStepsPutout))(currentIndex, totalCounts); unlock(0); return rt; 
		}else return 0;
	}
	//（主要过程函数，用于子类重载）工作线程的主要历程函数
	virtual int mainProc(PJTASK_TRDINF var);	
	//（主要过程函数，用于子类重载）处理线程数据结构体的填充操作，若返回FALSE将不启动线程组
	virtual BOOL fillThreadData(PJTASK_TRDINF p, size_t ci, size_t cn) {
		return TRUE;
	}
};

template<typename JTASK_TRDINF_>
DWORD CLJsonTaskUnit<JTASK_TRDINF_>::run(PCLTaskSvcTrdParam _var)
{
	PJTASK_TRDINF var = NULL;
	for (size_t i = 0; i < JTASK_TRDINFLst.size(); i++)
	{
		PJTASK_TRDINF p = (PJTASK_TRDINF)JTASK_TRDINFLst.at(i);
		if (p->index == _var->info.nIndex) {
			var = p;
			break;
		}
	}
	assert(var != NULL);
	if (var == NULL) 
		return 0;
	//------------------------------------------------------------------------------------------	
	LPCSTR _szType = ("\0");
	if (var->trdParm_json[JTASK_Key_JobType].isString() == false) {
		logoutu(var->strTmp.format(200, _T("ERROR : \"采集类型\"字段数据不存在或为空，第%d工作单元停止并退出。"), var->index).string());
		goto Exit1;
	}
	else
		_szType = var->trdParm_json[JTASK_Key_JobType].asCString();
	logoutu(var->strTmp.format(200, _T("节点\"%s\"，类型\"%s\"，第%d工作单元启动。"), var->fatherRootKey.string(), _szType, var->index).string());

	//------------------------------------------------------------------------------------------
	int rt = mainProc(var);
	var->iRet = rt;
	//------------------------------------------------------------------------------------------	
Exit1:

	//处理自清理	
	logoutu(var->strTmp.format(200, _T("节点\"%s\",第%d工作单元退出。"), var->fatherRootKey.string(), var->index).string());
	//退出前发出一条信息给调用线程或窗口过程
	BOOL bp = FALSE;
	if (var->quitMsgId != WM_NULL) {
		if (var->hWnd)
			bp = ::PostMessage(var->hWnd, var->quitMsgId, var->fatherRootKeyIndex, var->index);
		else if (var->ownerTrdId)
			bp = ::PostThreadMessage(var->ownerTrdId, var->quitMsgId, var->fatherRootKeyIndex, var->index);
	}
	return 1;
}

template<typename JTASK_TRDINF_>
int CLJsonTaskUnit<JTASK_TRDINF_>::mainProc(PJTASK_TRDINF var)
{
	assert(var != NULL);
	//------------------------------------------------------------------------------------
	GET_JOB_TYPE(lpJobType);
	CYCLE_BEGIN_();
	logoutu(var->strTmp.format(_T(">>> 节点 (%s, %zd, %s) : 请子类化并重写mainProc()方法，不要直接实例化基类！ ..."),
		var->fatherRootKey.string(), var->index, lpJobType).string());
	throw MessageBox(NULL, _T("请子类化并重写mainProc()方法，不要直接实例化基类！"), _T("exception"), MB_ICONERROR);
	Sleep(1000);
	CYCLE_END_();
	PROC_RETURN;
}

//实现和json数据格式绑定的多任务处理类工具，使用方法如下
//1、可根据需要定义类A，继承自JTASK_TRDINF数据结构，增加自定义变量；
//2、定义工作组模版类B，继承自模板类CLJsonTaskUnit<A>，并重写主过程方法mainProc()，如果对结构A的数据填充做操作请重载fillThreadData()；
//3、定义泛型对象C，类型为CLJsonTask<A,B>,用C实例化；
template<
	typename JTASK_TRDINF_, 
	template<typename JTASK_TRDINF__> typename JTUNIT = CLJsonTaskUnit
>
class CLJsonTask
{
#define JTUNIT_TYPE JTUNIT<JTASK_TRDINF_>
private:
	//friend class JTUNIT_TYPE;
	CLString m_jsonPath;
	CLString m_jsonStr;
	BOOL m_bJsonEnable;
	Value m_jsonMain;
	Reader m_jsonReader;
	Value::Members m_jsonMember;
	UINT m_trdQuitMsg;
	HWND m_trdQuitMsgTagWnd;
	BOOL m_timeCheckFlag;

	JTASK_INFLOGOUT_CALLBACK m_pFuncInfputout;
	JTASK_STEPSPUTOUT_CALLBACK m_pFuncStepsPutout;

	std::vector<JTUNIT_TYPE*> m_vtStoreUnitLst;
	std::vector<JTUNIT_TYPE*> m_vtUnitLst;
	JTASK_SHARE m_share;

protected:
	inline JTUNIT_TYPE* _getAqUnit(int iIndex = 0)const {
		return (iIndex >= 0 && (size_t)iIndex < m_vtUnitLst.size()) ? m_vtUnitLst.at(iIndex) : 0;
	}
	inline JTUNIT_TYPE* _adJTASKUnit() {
		m_vtStoreUnitLst.push_back(new JTUNIT_TYPE(&m_share));
		return m_vtStoreUnitLst.at(m_vtStoreUnitLst.size() - 1);
	}
	void _init()
	{
		m_pFuncInfputout = 0;
		m_pFuncStepsPutout = 0;
		m_trdQuitMsg = WM_NULL;
		m_bJsonEnable = FALSE;
		m_share.ppFuncInfputout = &m_pFuncInfputout;
		m_share.ppFuncStepsPutout = &m_pFuncStepsPutout;
		m_share.pTrdQuitMsgId = &m_trdQuitMsg;
		m_share.pHWnd = &m_trdQuitMsgTagWnd;
		setTimeCheckFlag(TRUE);
	}
	void _reset(){
	}
	void _clear()	{
		quitJobs();
		for (size_t i = 0; i < m_vtStoreUnitLst.size(); i++)
			delete m_vtStoreUnitLst.at(i);
		m_vtStoreUnitLst.clear();
		m_vtUnitLst.clear();
	}
	JTUNIT_TYPE* _newOneUnit(size_t curSize)	{
		size_t si = m_vtStoreUnitLst.size();
		if (si > curSize)return m_vtStoreUnitLst.at(si - 1);
		else return _adJTASKUnit();
	}
public:
	CLJsonTask(){
		_init();
	}
	virtual ~CLJsonTask(){
		_reset();
		_clear();
	}	
	//返回保存json字串对象
	inline CLStringRC getJsomString() { return m_jsonStr; }
	//返回内部json结构对象
	inline const Value& getValue() { return m_jsonMain; }
	//取得已设置的节点个数
	inline size_t getUnitCounts() { return m_vtUnitLst.size(); }
	//检查是否设置可用的json数据
	inline BOOL isAlready() { return isSetJson(); }
	//设置信息字符串输出接口设置
	inline CLJsonTask& setLogoutProc(JTASK_INFLOGOUT_CALLBACK pFuncIn = 0) { m_pFuncInfputout = pFuncIn; return *this; }
	//设置步长外显接口函数
	inline CLJsonTask& setStepsPutoutProc(JTASK_STEPSPUTOUT_CALLBACK pFuncIn = 0) { m_pFuncStepsPutout = pFuncIn; return *this; }
	//该函数为JTASK设置一个自定义消息，该消息将在JTASK内部某节点工作组的每一条线程退出时候向启动JTASK线程（或某窗口过程发送该消息，如果窗口句柄tagHwnd设置的话）
	//若该消息为WM_NULL(0)则工作组线程退出时不发送消息。若tagHwnd不为NULL则忽略调用线程直接向窗口过程发送消息。
	//在消息的接收端WPARAM参数将接受到发送该消息的线程所在的节点在JTASK内部的索引号（从0开始），LPARAM参数将接收线程所处在的工作组中的索引号（从1开始）
	inline CLJsonTask& setTrdQuitMsgId(UINT msg = WM_NULL, HWND tagHwnd = 0) { m_trdQuitMsg = msg; m_trdQuitMsgTagWnd = tagHwnd; return *this; }
	//检查是否设置输出外显函数指针
	inline BOOL isSetLogoutFunc() { return m_pFuncInfputout ? TRUE : FALSE; }
	//检查是否设置运行步数输出函数指针
	inline BOOL isSetStepsFunc() { return m_pFuncStepsPutout ? TRUE : FALSE; }
	//是否设置了时间检查标记，若设置则要检查运行时间
	inline BOOL isTimeCheckOpen() const { return m_timeCheckFlag; }
	//设置时间检查标记，TRUE为打开
	inline CLJsonTask& setTimeCheckFlag(BOOL timeCheckFlag) { m_timeCheckFlag = timeCheckFlag ? TRUE : FALSE; return *this; }
	//通过文件设置json数据
	CLJsonTask& setJsonByLoadFile(LPCTSTR lpFilePath)
	{
		logout(_T("读取文件，设置Json数据。"));
		if (isAnyoneExecuting())
		{
			logout(_T("WARNING : 存在运行中的工作组，忽略本次Json数据设置！")); return *this;
		}
		m_bJsonEnable = FALSE;
		m_jsonPath = lpFilePath;
		m_jsonStr.empty();
		m_jsonMain.clear();
		m_vtUnitLst.clear();
		if (m_jsonPath.fileExists()) {
			CLString m_tmp;
			m_tmp.readFile(m_jsonPath);
			if (!m_jsonReader.parse(m_tmp.getASCII(), m_tmp.getASCII() + strlen(m_tmp.getASCII()), m_jsonMain)) {
				logout(_T("ERROR : Json数据解析失败,请检查数据格式正确性！")); return *this;
			}
			m_jsonStr = m_tmp;
			if (m_jsonMain.type() == Json::nullValue) {
				logout(_T("ERROR : 读取的Json数据为空数据！")); return *this;
			}
			logout(_T("Json数据已读取并解析完毕。"));
			m_jsonMember = m_jsonMain.getMemberNames();
			for (auto it = m_jsonMember.begin(); it != m_jsonMember.end(); it++) {
				if (!m_jsonMain[*it].isObject())
					continue;
				JTUNIT_TYPE* p = _newOneUnit(m_vtUnitLst.size());
				m_vtUnitLst.push_back(p);
				p->setUnitJson(m_jsonMain[*it], m_vtUnitLst.size() - 1, (*it).c_str());
			}
			m_bJsonEnable = TRUE;
		}
		else logout(_T("ERROR : 指定的Json数据文件不存在！"));
		return *this;
	}
	//通过字符串设置json数据
	CLJsonTask& setJson(LPCTSTR lpJsonStr)
	{
		logout(_T("设置Json数据。"));
		if (isAnyoneExecuting())
		{
			logout(_T("WARNING : 存在运行中的工作组，忽略本次Json数据设置！")); return *this;
		}
		m_bJsonEnable = FALSE;
		m_jsonStr = lpJsonStr;
		m_jsonPath.empty();
		m_jsonMain.clear();
		m_vtUnitLst.clear();
		if (m_jsonStr.strlen() > 0) {
			if (!m_jsonReader.parse(m_jsonStr.getASCII(), m_jsonStr.getASCII() + strlen(m_jsonStr.getASCII()), m_jsonMain))
			{
				logout(_T("ERROR : Json字符串数据解析失败,请检查数据格式正确性！"));
				m_jsonStr.empty();
				return *this;
			}
			if (m_jsonMain.type() == Json::nullValue) {
				logout(_T("ERROR : 读取的Json数据为空数据！")); return *this;
			}
			logout(_T("Json数据解析设置完毕。"));
			m_jsonMember = m_jsonMain.getMemberNames();
			for (auto it = m_jsonMember.begin(); it != m_jsonMember.end(); it++) {
				if (!m_jsonMain[*it].isObject())
					continue;
				JTUNIT_TYPE* p = _newOneUnit(m_vtUnitLst.size());
				m_vtUnitLst.push_back(p);
				p->setUnitJson(m_jsonMain[*it], m_vtUnitLst.size() - 1, (*it).c_str());

			}
			m_bJsonEnable = TRUE;
		}
		else logout(_T("ERROR : 指定的Json数据字串长度为0 ！"));
		return *this;
	}
	//判断是否已设置json数据，可返回已设置json的字符串
	BOOL isSetJson(LPTSTR lpFilePathBuf = 0, size_t nBufSizeInChar = MAX_PATH)
	{
		if (m_bJsonEnable) {
			if (lpFilePathBuf) _tcsncpy_s(lpFilePathBuf, nBufSizeInChar, m_jsonPath.string(), m_jsonPath.strlen());
			return TRUE;
		}
		else return FALSE;
	}
	//执行所有节点
	CLJsonTask& executeAll()
	{
		if (!isAlready())return *this;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; p = _getAqUnit(i); i++)
			p->execute();
		return *this;
	}
	//-1节点禁用， 0时间范围外，1正常组装并启动，2未组装工作组已启动
	int execute(int nIndex = 0)
	{
		if (!isAlready())return -1;
		JTUNIT_TYPE* p = 0;
		if (p = _getAqUnit(nIndex))
			return p->execute(m_timeCheckFlag);
		else return -1;
	}
	//-1节点禁用， 0时间范围外，1正常组装并启动，2未组装工作组已启动
	int execute(LPCTSTR lpKey = 0)
	{
		if (!isAlready())
			return -1;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; ((p = _getAqUnit(i)) != NULL) && (size_t)i < m_vtUnitLst.size(); i++)
			if (p->rootKey() == lpKey)
				return p->execute(m_timeCheckFlag);
		return -1;
	}
	//-1节点禁用， 0时间范围外，1正常组装并启动，2未组装工作组已启动
	int executeNextUnit(LPCTSTR lpCurrUnitKey = 0, int offi = 1)
	{
		int k = -1;
		if ((k = getUnitIndex(lpCurrUnitKey)) == -1)
			return -1;
		int si = m_vtUnitLst.size();
		k = (offi + k) % si;
		if (k < 0) k = si + k;
		return _getAqUnit(k)->execute(m_timeCheckFlag);
	}
	//-1节点禁用， 0时间范围外，1正常组装并启动，2未组装工作组已启动
	int executeNextUnit(int ci = -1, int offi = 1)
	{
		if (_getAqUnit(ci) == NULL)
			return -1;
		int si = m_vtUnitLst.size();
		ci = (offi + ci) % si;
		if (ci < 0) ci = si + ci;
		return _getAqUnit(ci)->execute(m_timeCheckFlag);
	}
	//退出工作组的唯一方法，阻塞模式的
	CLJsonTask& quitJobs()
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			m_vtUnitLst.at(i)->quitJobs();
		}
		return *this;
	}
	//是否有节点正在运行中
	BOOL isAnyoneExecuting(LPTSTR lpMainFlagBuf = 0, size_t nBufSizeInChar = MAX_PATH)
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			if (m_vtUnitLst.at(i)->isJobsRunning()) {
				if (lpMainFlagBuf != NULL && nBufSizeInChar > 0) {
					_tcscpy_s(lpMainFlagBuf, nBufSizeInChar, m_vtUnitLst.at(i)->rootKey().string());
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	//检查某个指定节点是否在运行
	BOOL isUnitExecuting(LPCTSTR lpMainFlagBuf = 0)
	{
		size_t si = m_vtUnitLst.size();
		for (size_t i = 0; i < si; i++)
		{
			if (m_vtUnitLst.at(i)->rootKey().compareNoCase(lpMainFlagBuf) == 0
				&& m_vtUnitLst.at(i)->isJobsRunning()) {
				return TRUE;
			}
		}
		return FALSE;
	}
	//检查某个指定节点是否在运行
	BOOL isUnitExecuting(int index = 0)
	{
		auto p = _getAqUnit(index);
		return p ? p->isJobsRunning() : FALSE;
	}
	//检查某个指定节点是否已经在内部待命或加载了
	BOOL isUnitAvailable(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return FALSE;
		if (isSetJson() == FALSE)return FALSE;
		JTUNIT_TYPE* p = 0;
		for (int i = 0; p = _getAqUnit(i); i++) { if (p->rootKey().compareNoCase(lpKey) == 0) { return p->isUnitAvailable(); } }
		return FALSE;
	}
	//检查某个指定节点是否已经在内部待命或加载了
	BOOL isUnitAvailable(int index = 0)
	{
		JTUNIT_TYPE* p = _getAqUnit(index);
		return  p ? p->isUnitAvailable() : FALSE;
	}
	//检查某个指定节点是否已经在内部存在了
	BOOL haveSomeUnit(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return FALSE;
		if (isSetJson() == FALSE)return FALSE;
#ifndef UNICODE
		for (auto i = m_jsonMember.begin(); i != m_jsonMember.end(); i++) { if (_stricmp((*i).c_str(), lpKey) == 0)return TRUE; }
#else
		CLString str;
		for (auto i = m_jsonMember.begin(); i != m_jsonMember.end(); i++) { str = lpKey; if (_stricmp((*i).c_str(), str.getASCII()) == 0)return TRUE; }
#endif // !UNICODE

		return FALSE;
	}
	//返回 没有找到返回 NULL
	LPCTSTR getUnitKey(int index = 0)
	{
		auto p = _getAqUnit(index);
		return p ? p->rootKey().string() : 0;
	}
	//返回指定名称的节点在内部的索引值，0开始，没有找到返回-1
	int getUnitIndex(LPCTSTR lpKey = 0)
	{
		if (!lpKey)return -1;
		for (size_t i = 0; i < m_vtUnitLst.size(); i++) { if (m_vtUnitLst.at(i)->rootKey() == lpKey)return i; }
		return -1;
	}
	//取得某节点的线程数
	size_t getUnitRunningTrdCounts(int index = 0)
	{
		JTUNIT_TYPE* p = _getAqUnit(index);
		if (!p)return 0;
		size_t ret = 0;
		p->lock();
		for (size_t i = 0; i < p->JTASK_TRDINFLst.size(); i++) {
			if (JTUNIT_TYPE::isTrdRunning(p->JTASK_TRDINFLst.at(i)->handle))
				ret++;
		}
		p->unlock();
		return ret;
	}
	//用于输出外现信息的函数
	virtual int logout(LPCTSTR str) {
		return (m_pFuncInfputout ? (*m_pFuncInfputout)(str) : FALSE);
	}
};

//由基类组成的工具体，未实现mainProc过程，请勿使用该类实例化；
//1、可根据需要定义类A，继承自JTASK_TRDINF数据结构，增加自定义变量；
//2、定义工作组模版类B，继承自模板类CLJsonTaskUnit<A>，并重写主过程方法mainProc()，如果对结构A的数据填充做操作请重载fillThreadData()；
//3、定义泛型对象C，类型为CLJsonTask<A,B>,用C实例化；
typedef CLJsonTask<JTASK_TRDINF, CLJsonTaskUnit> CLJsonTaskBase;

#endif // !__CL_JSONTASK__