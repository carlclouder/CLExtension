#pragma once
#ifndef __CL_TIME_H__
#define __CL_TIME_H__

#include "windows.h"
#include <time.h>
#include "../_cl_common/CLCommon.h"
#include "../_cl_string/CLString.h"


//时间数据操作工具类
#define CLTIMESTRINGLENGTH 35

#define CTT_CREATEBYNULL         0//空方式创建
#define CTT_CREATEBYLOCALTIME    1//local方式创建
#define CTT_CREATEBYUTCTIME      2//utc方式创建
#define CTT_CREATEBYTIMESTAMP    3//时间戳方式创建

class CLTime;
typedef CLTime CLTT;
typedef CLTime* PCLTime;
typedef const PCLTime PCLCTime;
typedef CLTime& CLTimeR;
typedef const CLTimeR CLTimeRC;

typedef long long TIMESTAMP,*PTIMESTAMP;//定义的时间戳类型，是一个long long类型的显示意义的值，eg: 20150521183526142,表示2015年5月21日18点35分26秒142毫秒


//日期掩盖值
#define DATE_COVER (TIMESTAMP(1000000000)) 
//TIMESTAMP值时间，留下date部分 20170101093030000 -> 20170101000000000
#define LEAVE_DATE( stamp ) (TIMESTAMP(stamp)/DATE_COVER*DATE_COVER)
//TIMESTAMP值时间，提取date部分 20170101093030000 -> 20170101
#define EXTR_DATE( stamp ) (TIMESTAMP(stamp)/DATE_COVER)
//TIMESTAMP值时间，提取time部分 20170101093030123 -> 93030123
#define EXTR_TIME( stamp ) (TIMESTAMP(stamp)%DATE_COVER)
//TIMESTAMP值时间，提取time部分（不包括毫秒,包含毫秒位并且为0） 20170101093030123 -> 93030000
#define EXTR_TIMENO( stamp ) (TIMESTAMP(stamp)%DATE_COVER/1000i64*1000i64)
//TIMESTAMP值时间，提取time部分（不包括毫秒,不包含毫秒位） 20170101 093030 123 -> 93030
#define EXTR_TIMES( stamp ) (TIMESTAMP(stamp)%DATE_COVER/1000i64)
//TIMESTAMP值时间，提取year部分 20170101093030000 -> 2017
#define EXTR_YEAR( stamp ) (long(TIMESTAMP(stamp)/10000000000000i64))
//TIMESTAMP值时间，提取month部分 20171021093030000 -> 10
#define EXTR_MON( stamp )  (long(TIMESTAMP(stamp)%10000000000000i64/100000000000i64))
//TIMESTAMP值时间，提取day部分 20170121093030000 -> 21
#define EXTR_DAY( stamp )  (long(TIMESTAMP(stamp)%100000000000i64/1000000000i64))
//TIMESTAMP值时间，提取hour部分 20170101093030000 -> 09
#define EXTR_HOUR( stamp ) (long(TIMESTAMP(stamp)%1000000000i64/10000000i64))
//TIMESTAMP值时间，提取minute部分 20170101092530000 -> 25
#define EXTR_MINU( stamp ) (long(TIMESTAMP(stamp)%10000000i64/100000i64))
//TIMESTAMP值时间，提取second部分 20170101092530127 -> 30
#define EXTR_SEC( stamp )  (long(TIMESTAMP(stamp)%100000i64/1000i64))
//TIMESTAMP值时间，提取millisecond部分 20170101093030315 -> 315
#define EXTR_MIS( stamp )  (long(TIMESTAMP(stamp)%1000i64))
//组合两个TIMESTAMP值时间，分别提取date和time部分组合成新的时间值。
//20170101093030000 + 20140202101350102 = 20170101101350102
#define COMB_DATE_TIME(date,time) (((TIMESTAMP)(date)/DATE_COVER*DATE_COVER) +((TIMESTAMP)(time)%DATE_COVER))
//组合各时分秒值时间，到一个TIMESTAMP值时间。
// 11：15：21 （876） ->  111521876
#define MAKE_STAMP_TIME(h,mi,s,ms) ((TIMESTAMP)(h)*10000000i64 +(TIMESTAMP)(mi)*100000i64 +(TIMESTAMP)(s)*1000i64 +(ms))
//组合各年月日值时间，到一个TIMESTAMP值时间。
// 2017-11-21         ->  20171121000000000
#define MAKE_STAMP_DATE(y,m,d) ((TIMESTAMP)(y)*10000000000000i64 +(TIMESTAMP)(m)*100000000000i64 +(TIMESTAMP)(d)*1000000000i64)
//组合各 年月日 时分秒 值时间，到一个TIMESTAMP值时间。
// 2017-11-21 10：15：21 （876）  ->  20171121101521876
#define MAKE_STAMP(y,m,d,h,mi,s,ms) ((TIMESTAMP)ms + ((TIMESTAMP)s*1000i64) + ((TIMESTAMP)mi*100000i64) + ((TIMESTAMP)h*10000000i64) + ((TIMESTAMP)d*1000000000i64) + ((TIMESTAMP)m*100000000000i64) + ((TIMESTAMP)y*10000000000000i64))

//时间数据操作工具类
class CLTime{
protected:

	SYSTEMTIME m_at;
	int m_isLocalTime;

	SYSTEMTIME m_sysTimeStart,m_sysTimeEnd;

	LARGE_INTEGER m_largeIntegerStart,m_largeIntegerEnd;

	byte m_startTimeFlag,m_startTimeExFlag;

	CLTimeR init();


public:
	//默认构造函数，内部将保存本地时间
	CLTime();
	//参数可以是CTT_CREATEBYTIMESTAMP；CTT_CREATEBYLOCALTIME；CTT_CREATEBYUTCTIME
	CLTime(UINT uType,unsigned long long timeStamp = 0);
	CLTime(int dYears,int dMonths,int dDays,int dHours,int dMinutes,int dSeconds,int dMMSeconds=0);
	CLTime(int dYears,int dMonths,int dDays);
	CLTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds);
	CLTime(const SYSTEMTIME& _in_sysTime);
	CLTime(const CLTime& tagCLTime);
	virtual ~CLTime();

	//SYSTEMTIME结构体复制Tm结构体
#define setST2Tm( Tm , systemTime ) \
	{\
		(Tm).tm_sec = (systemTime).wSecond;\
		(Tm).tm_min = (systemTime).wMinute;\
		(Tm).tm_hour = (systemTime).wHour;\
		(Tm).tm_mday = (systemTime).wDay;\
		(Tm).tm_mon = (systemTime).wMonth-1;\
		(Tm).tm_year = (systemTime).wYear-1900;\
		(Tm).tm_wday = (systemTime).wDayOfWeek;\
	}
	//Tm结构体复制到SYSTEMTIME
#define setTm2ST( systemTime , Tm )\
	{\
		(systemTime).wDay = (Tm).tm_mday;\
		(systemTime).wDayOfWeek = (Tm).tm_wday;\
		(systemTime).wHour = (Tm).tm_hour;\
		(systemTime).wMinute = (Tm).tm_min;\
		(systemTime).wMonth = (Tm).tm_mon+1;\
		(systemTime).wSecond = (Tm).tm_sec;\
		(systemTime).wYear = (Tm).tm_year+1900;\
	}
	//TIMESTAMP结构体复制Tm结构体,按时间自动生成星期数
#define setTS2Tm( Tm , timeStamp ) \
	{\
	(Tm).tm_sec = EXTR_SEC(timeStamp);\
	(Tm).tm_min = EXTR_MINU(timeStamp);\
	(Tm).tm_hour = EXTR_HOUR(timeStamp);\
	(Tm).tm_mday = EXTR_DAY(timeStamp);\
	(Tm).tm_mon = EXTR_MON(timeStamp)-1;\
	(Tm).tm_year = EXTR_YEAR(timeStamp)-1900;\
	(Tm).tm_wday = CLTime::getDayOfWeekByKimLarsson((Tm).tm_year+1900,(Tm).tm_mon+1,(Tm).tm_mday);\
	}
	//Tm结构体复制TIMESTAMP结构体，需要额外增加毫秒数据，忽略星期数据
#define setTm2TS( timeStamp , Tm , miniSec ) \
	{(timeStamp) = (TIMESTAMP)MAKE_STAMP((Tm).tm_year+1900,(Tm).tm_mon+1,(Tm).tm_mday,(Tm).tm_hour,(Tm).tm_min,(Tm).tm_sec) , miniSec);}

	//取得保存的时间数据状态是否是本地时间，如果是返回1，不是返回0，没有记录返回-1
	int isLocalTime() const;

	//赋值函数设置时间记录值，参数为： 时间戳(该函数使用的时间戳是标准的秒时间戳 而不是 自定义的longlong时间值)
	CLTime& set(unsigned long long timeStamp);

	//赋值函数设置时间记录值，参数为： 年 月 日 时 分 秒 毫秒 的int值
	CLTime& set(int dYears,int dMonths,int dDays,int dHours=0,int dMinutes=0,int dSeconds=0,int dMMSeconds=0);

	//赋值函数设置时间记录值，参数： 同类型引用
	CLTime& set(const CLTime& _in_cltime);
	CLTime& set(const SYSTEMTIME& _in_cltime);
	
	//重载=运算，参数：同类型引用
	CLTime& operator=(const CLTime& _in_cltime);
	CLTime& operator=(const SYSTEMTIME& _in_cltime);
	CLTime& operator=(int time);
	CLTime& operator=(long time);
	CLTime& operator=(TIMESTAMP time);

	//设置时间，日期不改变
	CLTime& setTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds=0);
	CLTime& setTime(const SYSTEMTIME& sysTime);
	CLTime& setTime(const CLTime& cTime);

	//通过一个类似时间输出格式的TIMESTAMP值作为时间戳，设置时间。
	CLTime& setByStamp(TIMESTAMP cTimeStamp);
	CLTime& setTimeByStamp(TIMESTAMP cTimeStamp);
	CLTime& setDateByStamp(TIMESTAMP cTimeStamp);

	//用字符串中描述时间的文字去设置时间；（该函数不会改变日期位的值）
	//参数说明：hPos,hN 分别表示描述小时的文字的首字符出现在字符串中的位置以0为起始的索引和该描述所用的字符个数。
	//hPos < 0或hN < 0时，则小时的描述将被忽略不修改原值;
    //hN = 0时，小时的描述将被忽略并以0赋值。
	//hPos >= 0且hN > 0时，小时的描述才解析并相应赋值。
	//miPos代表分，sPos代表秒，misPos代表毫秒，方法类同hPos说明。
	//若解析的小时值超出范围(0~23),则进行取绝对值之后取余操作。
	//注意：1、每一个描述所用的字符数，最多不能超过8个，超过会出错。
	//      2、每个描述对应的位置计算时候请注意ascii编码方式时候汉字占2位的情况。
	//eg： “18：30：54” 传参方式为 ("18：30：54",0,2,3,2,6,2,-1,-1) 
	CLTimeR setTimeByString(LPCTSTR timeString,int hPos = 0,int hN = 2,int miPos = 3,int miN = 2,int sPos = 6,int sN = 2,int misPos = 0,int misN = 0);
	CLTimeR setDateByString(LPCTSTR dateString,int yearPos = 0,int yearN = 4,int monPos = 5,int monN = 2,int dayPos = 8,int dayN = 2);

	//用字符串中描述时间的文字去取得一个TIMESTAMP时间；（该函数不会改变日期位的值）
	//参数说明：hPos,hN 分别表示描述小时的文字的首字符出现在字符串中的位置以0为起始的索引和该描述所用的字符个数。
	//hPos < 0或hN < 0时，则小时的描述将被忽略不修改原值;
	//hN = 0时，小时的描述将被忽略并以0赋值。
	//hPos >= 0且hN > 0时，小时的描述才解析并相应赋值。
	//miPos代表分，sPos代表秒，misPos代表毫秒，方法类同hPos说明。
	//若解析的小时值超出范围(0~23),则进行取绝对值之后取余操作。
	//注意：1、每一个描述所用的字符数，最多不能超过8个，超过会出错。
	//      2、每个描述对应的位置计算时候请注意ascii编码方式时候汉字占2位的情况。
	//eg： “18：30：54” 传参方式为 ("18：30：54",0,2,3,2,6,2,-1,-1) 
	static TIMESTAMP getTimeByString_ll(LPCTSTR timeString,int hPos = 0,int hN = 2,int miPos = 3,int miN = 2,int sPos = 6,int sN = 2,int misPos = 0,int misN = 0);
	static TIMESTAMP getDateByString_ll(LPCTSTR dateString,int yearPos = 0,int yearN = 4,int monPos = 5,int monN = 2,int dayPos = 8,int dayN = 2);

	//设置日期，时间不改变（三参数版本会自动计算新的星期数）
	CLTime& setDate(int dYears,int dMonths,int dDays);
	CLTime& setDate(const SYSTEMTIME& sysTime);
	CLTime& setDate(const CLTime& cTime);	

	const SYSTEMTIME& dataConst() const;//取得数据结构体（只读形式获取数据变量）
	CLTimeR getLocalTime();//取得本地时间
	CLTimeR getSystemTime();//取得UTC时间

	//得到日期字串，szType表示分隔符所用符号，默认格式xxxx-xx-xx,返回值为outputTagObj对象，而非调用者本身
	CLString& dateStr( CLString& outputTagObj,LPCTSTR szType = NULL);
	static CLString& dateStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType = NULL);

	//得到时间字串，szType表示分隔符所用符号 ，默认格式xx:xx:xx,返回值为outputTagObj对象，而非调用者本身
	CLString& timeStr( CLString& outputTagObj,LPCTSTR szType = NULL);
	static CLString& timeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType = NULL);

	//得到日期和时间字串，szType表示分隔符所用符号，默认格式xxxx-xx-xx (x) xx:xx:xx,返回值为outputTagObj对象，而非调用者本身
	CLString& dateTimeStr( CLString& outputTagObj,LPCTSTR szDType=_T("-"),LPCTSTR szTType=_T(":"));
	static CLString& dateTimeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szDType=_T("-"),LPCTSTR szTType=_T(":"));

	//取得时间差计算后的日期时间，只能计算到1970年以后。
	CLTime& getNewTime(int dYears=0,int dMonths=0,int dDays=0,int dHours=0,int dMinutes=0,int dSeconds=0,int dMinisecond=0);
	static TIMESTAMP getNewTime_ll(TIMESTAMP orgStamp,int dYears=0,int dMonths=0, int dDays=0,int dHours=0,int dMinutes=0,int dSeconds=0,int dMinisecond=0);

	//取得时间差计算后的日期时间，只能计算1970年以后的日期时间。
	CLTime& getNewTimeByDays(int dDays=0);

	//比较函数，比较两个时间的日期和时间部分,时间大于返回1，相等返回0，小于返回-1
	int compare(const CLTime& tagTime) const;
	int compare(const SYSTEMTIME& tagSysTime) const;
	int compare(int y,int m,int d,int h,int mi,int s,int mis) const;
	static int compare(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1,
		int y2,int m2,int d2,int h2,int mi2,int s2,int mis2);

	//比较函数，比较两个时间的日期部分,时间大于返回1，相等返回0，小于返回-1
	int compareDate(const CLTime& tagTime) const;
	int compareDate(const SYSTEMTIME& tagSysTime) const;
	int compareDate(int y,int m,int d) const;
	static int compareDate(int y,int m,int d,int y2,int m2,int d2);

	//比较函数，比较两个时间的时间部分,时间大于返回1，相等返回0，小于返回-1
	int compareTime(const CLTime& tagTime) const;
	int compareTime(const SYSTEMTIME& tagSysTime) const;
	int compareTime(int h,int mi,int s,int mis) const;
	static int compareTime(int h,int mi,int s,int mis,int h2,int mi2,int s2,int mis2);

	//计算windows下的时间差，需要两个存储时间的指针变量；
	static double calcTimeDelta(const SYSTEMTIME* beforetime,const SYSTEMTIME* behandtime);
	static double calcTimeDelta(const SYSTEMTIME& beforetime,const SYSTEMTIME& behandtime);
	static double calcTimeDelta(CLTimeRC beforetime,CLTimeRC behandtime);

	//计算时间差时分秒计算，返回秒
	static double calcTimeDelta(int hf,int mf,int sf,int msf,int hb,int mb,int sb,int msb);
	static double calcTimeDelta_ll(TIMESTAMP beforetime,TIMESTAMP behandtime);

	//开始计时（计时启动），不首先调用此函数Timing系列函数将会出错；Ex函数采用微妙级别计数。
	CLTime& timingStart(void);
	CLTime& timingStartEx(void);

	//取得自计时开始起到当前调用时刻的系统消耗时间，以秒为单位；Ex函数采用微妙级别计数。
	//参数isNeedResetStartTime指定是否将重置startTime到新的当前值（默认为False）。
	double timingGetSpentTime(BOOL isNeedResetStartTime = FALSE);
	double timingGetSpentTimeEx(BOOL isNeedResetStartTimeEx = FALSE);
	double timingDumpSpentTimeC(BOOL isNeedResetStartTime = FALSE,LPCTSTR lpHdr = _T("\nTimeSpent="),LPCTSTR lpEnd = _T("s\n"));
	double timingDumpSpentTimeExC(BOOL isNeedResetStartTimeEx = FALSE,LPCTSTR lpHdr = _T("\nTimeSpent="),LPCTSTR lpEnd = _T("s\n"));

	//用Zeller算法计算的星期数，0表周日，1~6对应周一~周六。
	static int getDayOfWeekByZeller(int year,int month,int day);
	//用KimLason计算法计算的星期数，0表周日，1~6对应周一~周六。
	static int getDayOfWeekByKimLarsson(int year,int month,int day);

	//获得星期数
	inline static int getDayOfWeekByStamp_ll(TIMESTAMP time){ return getDayOfWeekByZeller((int)EXTR_YEAR(time), (int)EXTR_MON(time), (int)EXTR_DAY(time));}

	//对象的日期时间值在两个日期时间之间
	BOOL between(CLTimeRC timeA,CLTimeRC timeB);
	BOOL between(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL between(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1,
		int y2,int m2,int d2,int h2,int mi2,int s2,int mis2);

	//对象的时间值在两个时间之间
	BOOL betweenTime(CLTimeRC timeA,CLTimeRC timeB);
	BOOL betweenTime(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL betweenTime(int h1,int mi1,int s1,int mis1,int h2,int mi2,int s2,int mis2);

	//对象的日期值在两个日期之间
	BOOL betweenDate(CLTimeRC timeA,CLTimeRC timeB);
	BOOL betweenDate(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB);
	BOOL betweenDate(int y1,int m1,int d1,int y2,int m2,int d2);

	//由一个longlong类型的时间，大小和日期值一致，拆分成所需的时间。
	static void stampToTime_ll(TIMESTAMP stamp,int *y,int *m,int *d,int *h,int *mi,int *s,int *ms);
	//由内部保存的数据返回一个longlong类型的时间，大小和日期值一致。
	TIMESTAMP getStamp() const;
	//取得本地时间，并返回一个longlong类型的时间数据。
	static TIMESTAMP getLocalTime_ll();
	#define _getLocalTimell() CLTime::getLocalTime_ll()
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHMSM(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t),EXTR_MIS(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHMS(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMDHM(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t), EXTR_HOUR(t), EXTR_MINU(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_HMSM(t) EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t),EXTR_MIS(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_YMD(t) EXTR_YEAR(t), EXTR_MON(t), EXTR_DAY(t)
#define EXTR_TIMESTAMP_TO_SPRINTF_HMS(t) EXTR_HOUR(t), EXTR_MINU(t), EXTR_SEC(t)
	//取得系统时间，并返回一个longlong类型的时间数据。
	static TIMESTAMP getSystemTime_ll();
	#define _getSystemTimell() CLTime::getSystemTime_ll()
	//计算两个时间的时间差的各单位总和，eg：共xx年，共xx月，共xx日，共xx时，共xx分，共xx秒，共xx毫秒
	static int calcDiffTimeSum( TIMESTAMP startTime, TIMESTAMP endTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0);
	inline CLTimeR calcDiffTimeSum( TIMESTAMP difTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0){
			calcDiffTimeSum( this->getStamp(),difTime ,pNweek,pNyear,pNmon,pNday,pNhour,pNmin,pNsec,pNminiSec);
			return *this;
	}
	inline CLTimeR calcDiffTimeSum( CLTimeRC difTime ,OUT double* pNweek = 0,OUT double* pNyear = 0,OUT double* pNmon = 0,OUT double* pNday = 0,
		OUT double* pNhour = 0,OUT double* pNmin = 0,OUT double* pNsec = 0, OUT double* pNminiSec = 0){
		calcDiffTimeSum( this->getStamp(),difTime.getStamp() ,pNweek,pNyear,pNmon,pNday,pNhour,pNmin,pNsec,pNminiSec);
		return *this;
	}
	//计算两个时间的叠合时间差，eg：xx年xx月xx日xx时xx分xx秒xx毫秒
	static int calcDiffTime( TIMESTAMP startTime, TIMESTAMP endTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0);
	inline CLTimeR calcDiffTime( TIMESTAMP difTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0){
			calcDiffTime( this->getStamp(),difTime ,pyear,pmon,pday,phour,pmin,psec,pminiSec);
			return *this;
	}
	inline CLTimeR calcDiffTime( CLTimeRC difTime ,OUT long* pyear = 0,OUT long* pmon = 0,OUT long* pday = 0,
		OUT long* phour = 0,OUT long* pmin = 0,OUT long* psec = 0, OUT long* pminiSec = 0){
			calcDiffTime( this->getStamp(),difTime.getStamp() ,pyear,pmon,pday,phour,pmin,psec,pminiSec);
			return *this;
	}
	//取得微秒级的时间计数，单位为秒，windows xp以上支持。
	static double getTickCountsMicro() {
		static LARGE_INTEGER freq = { 0 };
		if (freq.QuadPart == 0)
			if (QueryPerformanceFrequency(&freq) == FALSE)
				return 0.0;
		LARGE_INTEGER counter;
		return QueryPerformanceCounter(&counter) ? (((double)counter.QuadPart) / ((double)freq.QuadPart)) : 0.0;
	}
	//由计算时间推算后续过程还需要的剩余时间,返回值：剩余时间（秒）。函数调用前请打开timingStartEx()函数
	// curIndex 当前下标, startIndex 起始下标, endIndex 终了下标（执行条件取 curIndex < endIndex）,  stepLen 步长,
	// 输出文字结果字符串缓冲（字） lpOutStringBuf = 0,字符串缓冲大小（字） size_t bufSize = 0
	double timingGetRestOfTimeEx(long endIndex, long startIndex = 0, long curIndex = 0, long stepLen = 1, LPTSTR lpOutStringBuf = 0, size_t bufSize = 0);
};
#endif


