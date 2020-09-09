#include "CLTime.h"
#include <iostream>
CLTime::CLTime(const SYSTEMTIME& _in_sysTime)
{
	init();
	m_at = _in_sysTime;
}

CLTime::CLTime(int dYears,int dMonths, int dDays,int dHours,
	int dMinutes,int dSeconds,int dMMSeconds)
{
	init();
	set(dYears,dMonths,dDays,dHours,dMinutes,dSeconds,dMMSeconds);	
}

CLTime::CLTime(UINT uType,unsigned long long timeStamp)
{
	init();
	if(uType == CTT_CREATEBYTIMESTAMP){	set(timeStamp);}
	else if (uType == CTT_CREATEBYLOCALTIME)
	{getLocalTime();}
	else if (uType == CTT_CREATEBYUTCTIME)
	{getSystemTime();}
	else {ZeroMemory(&m_at,sizeof(SYSTEMTIME));}
}

CLTime::CLTime(const CLTime& tagCLTime)
{
	init();
	set(tagCLTime);
}

CLTime::CLTime()
{
	init();
	ZeroMemory(&m_at,sizeof(SYSTEMTIME));
}

CLTime::CLTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds)
{
	init();
	set(0,0,0,dHours,dMinutes,dSeconds,dMMSeconds);
}

CLTime::CLTime(int dYears,int dMonths,int dDays)
{
	init();
	set(dYears,dMonths,dDays,0,0,0,0);
}

CLTimeR CLTime::init()
{	
	m_isLocalTime = -1;
	m_startTimeExFlag = m_startTimeFlag = 0;
	return *this;
}

CLTimeR CLTime::getLocalTime()
{
	GetLocalTime(&m_at);
	m_isLocalTime = 1;
	return *this;
}

CLTimeR CLTime::getSystemTime()
{
	GetSystemTime(&m_at);
	m_isLocalTime = 0;
	return *this;
}

CLString& CLTime::dateStr( CLString& outputTagObj,LPCTSTR szType/*="-"*/)
{
	outputTagObj.format(10,_T("%04d-%02d-%02d"),m_at.wYear,m_at.wMonth,m_at.wDay);
	if(szType)
		outputTagObj.replace(_T("-"),szType);
	return outputTagObj;
}

CLString& CLTime::dateStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType /*= NULL*/)
{
	outputTagObj.format(23,_T("%04d-%02d-%02d")	,(int)EXTR_YEAR(timeStamp),(int)EXTR_MON(timeStamp),(int)EXTR_DAY(timeStamp));
	if(szType && (*szType != _T('-')))
		outputTagObj.replace(_T("-"),szType);
	return outputTagObj;
}

CLString& CLTime::timeStr( CLString& outputTagObj,LPCTSTR szType/*=":"*/)
{
	outputTagObj.format(8,_T("%02d:%02d:%02d"),m_at.wHour,m_at.wMinute,m_at.wSecond);
	if(szType  && (*szType != ':'))
		outputTagObj.replace(_T(":"),szType);
	return outputTagObj;
}

CLString& CLTime::timeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szType /*= NULL*/)
{
	outputTagObj.format(8,_T("%02d:%02d:%02d"),(int)EXTR_HOUR(timeStamp),
		(int)EXTR_MINU(timeStamp),(int)EXTR_SEC(timeStamp));
	if(szType && (*szType != _T(':')))
		outputTagObj.replace(_T(":"),szType);
	return outputTagObj;
}

CLString& CLTime::dateTimeStr( CLString& outputTagObj,LPCTSTR szDType/*="-"*/,LPCTSTR szTType/*=":"*/)
{
	outputTagObj.format(23,_T("%04d-%02d-%02d (%d) %02d:%02d:%02d")
		,m_at.wYear,m_at.wMonth,m_at.wDay,m_at.wDayOfWeek
		,m_at.wHour,m_at.wMinute,m_at.wSecond);
	if(szDType && (*szDType != _T('-')))
		outputTagObj.replace(_T("-"),szDType);
	if(szTType && (*szTType != _T(':')))
		outputTagObj.replace(_T(":"),szTType);
	return outputTagObj;
}

CLString& CLTime::dateTimeStr_ll(TIMESTAMP timeStamp, CLString& outputTagObj,LPCTSTR szDType/*=_T("-")*/,LPCTSTR szTType/*=_T(":")*/)
{
	outputTagObj.format(23,_T("%04d-%02d-%02d (%d) %02d:%02d:%02d")	,(int)EXTR_YEAR(timeStamp),(int)EXTR_MON(timeStamp),
		(int)EXTR_DAY(timeStamp),CLTime::getDayOfWeekByStamp_ll(timeStamp),(int)EXTR_HOUR(timeStamp),
		(int)EXTR_MINU(timeStamp),(int)EXTR_SEC(timeStamp));
	if(szDType && (*szDType != _T('-')))
		outputTagObj.replace(_T("-"),szDType);
	if(szTType && (*szTType != _T(':')))
		outputTagObj.replace(_T(":"),szTType);
	return outputTagObj;
}

CLTime& CLTime::getNewTime(int dYears,int dMonths, int dDays,int dHours,int dMinutes,int dSeconds,int dMinisecond)
{	
	int s = dMinisecond/1000,d=dMinisecond%1000;
	int s2 = (m_at.wMilliseconds + 1000 + d)/1000,
		d2 = (m_at.wMilliseconds + 1000 + d)%1000;
	time_t t = dYears*365 + dMonths*30 + dDays;
	t = t*24+dHours;
	t = t*60+dMinutes;
	t = t*60+dSeconds+s+s2-1;
	tm tm1;
	setST2Tm(tm1,m_at);
	time_t t2 = mktime(&tm1);
	t2 = t2 + t;
	errno_t err = localtime_s(&tm1,&t2);
	if(err != 0){
		
	}
	else {
		setTm2ST(m_at,tm1);
		m_at.wMilliseconds = d2;
	}
	return (*this);
}

TIMESTAMP CLTime::getNewTime_ll(TIMESTAMP orgStamp,int dYears,int dMonths, int dDays,int dHours,int dMinutes,int dSeconds,int dMinisecond)
{
	int y,m,dd,h,mi,ss,ms;
	stampToTime_ll(orgStamp,(int*)&y,(int*)&m,(int*)&dd,(int*)&h,(int*)&mi,(int*)&ss,(int*)&ms);
	int s = dMinisecond/1000,d=dMinisecond%1000;
	int s2 = (ms + 1000 + d)/1000,
		d2 = (ms + 1000 + d)%1000;
	time_t t = dYears*365 + dMonths*30 + dDays;
	t = t*24+dHours;
	t = t*60+dMinutes;
	t = t*60+dSeconds+s+s2-1;
	tm tm1;	
	(tm1).tm_sec = ss;
		(tm1).tm_min = mi;
		(tm1).tm_hour = h;
		(tm1).tm_mday = dd;
		(tm1).tm_mon = m-1;
		(tm1).tm_year = y-1900;
	time_t t2 = mktime(&tm1);
	t2 = t2 + t;
	errno_t err = localtime_s(&tm1,&t2);
	if(err != 0){
		return 0;
	}
	else {
		dd = (tm1).tm_mday;
		h = (tm1).tm_hour;
		mi = (tm1).tm_min;
		m = (tm1).tm_mon+1;
		ss = (tm1).tm_sec;
		y = (tm1).tm_year+1900;
		return MAKE_STAMP(y,m,dd,h,mi,ss,d2);
	}
}

CLTime& CLTime::getNewTimeByDays(int dDays)
{
	return getNewTime(0,0,dDays);
}

CLTime::~CLTime()
{

}

CLTime& CLTime::set(int dYears,int dMonths,int dDays,int dHours,int dMinutes,int dSeconds,int dMMSeconds)
{
	assert( dYears >= 0 && dMonths >= 0 && dDays >= 0 );
	m_at.wYear = dYears;
	m_at.wMonth = dMonths;
	m_at.wDay = dDays;
	m_at.wDayOfWeek = (WORD)getDayOfWeekByZeller(dYears,dMonths,dDays);
	m_at.wHour = dHours;		
	m_at.wMinute = dMinutes;
	m_at.wSecond = dSeconds;
	m_at.wMilliseconds = dMMSeconds;		
	return *this;
}

CLTime& CLTime::set(const CLTime& _in_cltime)
{
	m_at = _in_cltime.dataConst();
	m_isLocalTime = _in_cltime.isLocalTime();
	return *this;
}

CLTime& CLTime::set(const SYSTEMTIME& _in_cltime)
{
	m_at = _in_cltime;
	return *this;
}

CLTime& CLTime::set(unsigned long long timeStamp)
{
	tm t;
	localtime_s(&t,(time_t*)&timeStamp);
	setTm2ST(m_at,t);
	m_at.wMilliseconds = 0;
	m_isLocalTime = 1;
	return *this;
}

CLTime& CLTime::setByStamp(TIMESTAMP cTimeStamp)
{
	stampToTime_ll(cTimeStamp,(int*)&m_at.wYear,(int*)&m_at.wMonth,(int*)&m_at.wDay,(int*)&m_at.wHour,(int*)&m_at.wMinute,(int*)&m_at.wSecond,(int*)&m_at.wMilliseconds);
	m_at.wDayOfWeek = getDayOfWeekByKimLarsson(m_at.wYear,m_at.wMonth,m_at.wDay);
	return *this;
}

CLTime& CLTime::operator=(const SYSTEMTIME& _in_cltime)
{
	return set(_in_cltime);
}

CLTime& CLTime::operator=(int time)
{
	return setByStamp((LONGLONG)time);
}

CLTime& CLTime::operator=(TIMESTAMP time)
{
	return setByStamp((LONGLONG)time);
}

CLTime& CLTime::operator=(long time)
{
	return setByStamp((LONGLONG)time);
}

CLTime& CLTime::setTime(int dHours,int dMinutes,int dSeconds,int dMMSeconds)
{
	m_at.wHour = dHours;	
	m_at.wMinute = dMinutes;
	m_at.wSecond = dSeconds;
	m_at.wMilliseconds = dMMSeconds;
	return *this;
}
CLTime& CLTime::setTime(const SYSTEMTIME& sysTime)
{
	m_at.wHour = sysTime.wHour;	
	m_at.wMinute = sysTime.wMinute;
	m_at.wSecond = sysTime.wSecond;
	m_at.wMilliseconds = sysTime.wMilliseconds;
	return *this;
}
CLTime& CLTime::setTime(const CLTime& cTime)
{
	return setTime(cTime.dataConst());
}

CLTime& CLTime::setTimeByStamp(TIMESTAMP cTimeStamp)
{
	stampToTime_ll(cTimeStamp,0,0,0,(int*)&m_at.wHour,
		(int*)&m_at.wMinute,(int*)&m_at.wSecond,(int*)&m_at.wMilliseconds);
	return *this;
}

CLTime& CLTime::setDateByStamp(TIMESTAMP cTimeStamp)
{
	stampToTime_ll(cTimeStamp,(int*)&m_at.wYear,(int*)&m_at.wMonth,(int*)&m_at.wDay,0,0,0,0);
	m_at.wDayOfWeek = getDayOfWeekByKimLarsson(m_at.wYear,m_at.wMonth,m_at.wDay);
	return *this;
}

CLTimeR CLTime::setTimeByString(LPCTSTR timeString,int hPos,int hN,int miPos,int miN,int sPos,int sN,int misPos /*= -1*/,int misN /*= -1*/)
{
	assert(timeString != NULL);
	TCHAR tmp[9]={0};
	int i;
	if(hPos >= 0 && hN > 0){
		memcpy_s(tmp,hN*sizeof(TCHAR),(timeString + hPos),hN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wHour = ( BETWEEN(i,0,23) ? i : abs(i)%24);
	}
	else if(hN == 0)m_at.wHour = 0;
	if(miPos >= 0 && miN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,miN*sizeof(TCHAR),(timeString + miPos),miN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wMinute = ( BETWEEN(i,0,59) ? i : abs(i)%60);
	}
	else if(miN == 0)m_at.wMinute = 0;
	if(sPos >= 0 && sN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,sN*sizeof(TCHAR),(timeString + sPos),sN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wSecond = ( BETWEEN(i,0,59) ? i : abs(i)%60);
	}
	else if(sN == 0)m_at.wSecond = 0;
	if(misPos >= 0 && misN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,misN*sizeof(TCHAR),(timeString + misPos),misN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wMilliseconds = ( BETWEEN(i,0,999) ? i : abs(i)%1000);
	}
	else if(misN == 0)m_at.wMilliseconds = 0;
	return *this;
}

CLTimeR CLTime::setDateByString(LPCTSTR dateString,int yearPos /*= 0*/,int yearN /*= 4*/,int monPos /*= 5*/,int monN /*= 2*/,int dayPos /*= 8*/,int dayN /*= 2*/)
{
	assert(dateString != NULL);
	TCHAR tmp[9]={0};
	int i;
	if(yearPos >= 0 && yearN > 0){
		memcpy_s(tmp,yearN*sizeof(TCHAR),(dateString + yearPos),yearN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wYear = ( i>=0 ? i : 0);
	}
	else if(yearN == 0)m_at.wYear = 0;
	if(monPos >= 0 && monN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,monN*sizeof(TCHAR),(dateString + monPos),monN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wMonth = ( BETWEEN(i,1,12) ? i : abs(i)%12+1);
	}
	else if(monN == 0)m_at.wMonth = 0;
	if(dayPos >= 0 && dayN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,dayN*sizeof(TCHAR),(dateString + dayPos),dayN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m_at.wDay = ( BETWEEN(i,1,31) ? i : abs(i)%31+1);
	}
	else if(dayN == 0)m_at.wDay = 0;
	m_at.wDayOfWeek = getDayOfWeekByKimLarsson(m_at.wYear,m_at.wMonth,m_at.wDay);
	return *this;
}

TIMESTAMP CLTime::getTimeByString_ll(LPCTSTR timeString,int hPos,int hN,int miPos,int miN,int sPos,int sN,int misPos /*= -1*/,int misN /*= -1*/)
{
	if(!timeString || std::_tcslen(timeString)<5)
		return 0;
	TCHAR tmp[9]={0};
	int i;
	TIMESTAMP h=0,m=0,s=0,ms=0;
	if(hPos >= 0 && hN > 0){
		memcpy_s(tmp,hN*sizeof(TCHAR),(timeString + hPos),hN*sizeof(TCHAR));
		i = _ttoi(tmp);
		h = ( BETWEEN(i,0,23) ? i : abs(i)%24);
	}
	else if(hN == 0)h = 0;
	if(miPos >= 0 && miN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,miN*sizeof(TCHAR),(timeString + miPos),miN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m = ( BETWEEN(i,0,59) ? i : abs(i)%60);
	}
	else if(miN == 0)m = 0;
	if(sPos >= 0 && sN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,sN*sizeof(TCHAR),(timeString + sPos),sN*sizeof(TCHAR));
		i = _ttoi(tmp);
		s = ( BETWEEN(i,0,59) ? i : abs(i)%60);
	}
	else if(sN == 0)s = 0;
	if(misPos >= 0 && misN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,misN*sizeof(TCHAR),(timeString + misPos),misN*sizeof(TCHAR));
		i = _ttoi(tmp);
		ms = ( BETWEEN(i,0,999) ? i : abs(i)%1000);
	}
	else if(misN == 0)ms = 0;
	return MAKE_STAMP_TIME(h,m,s,ms);
}

TIMESTAMP CLTime::getDateByString_ll(LPCTSTR dateString,int yearPos /*= 0*/,int yearN /*= 4*/,int monPos /*= 5*/,int monN /*= 2*/,int dayPos /*= 8*/,int dayN /*= 2*/)
{
	//assert(dateString != NULL);
	if(!dateString || std::_tcslen(dateString)<6)
		return 0;
	TCHAR tmp[9]={0};
	int i;
	TIMESTAMP y=0,m=0,d=0;
	if(yearPos >= 0 && yearN > 0){
		memcpy_s(tmp,yearN*sizeof(TCHAR),(dateString + yearPos),yearN*sizeof(TCHAR));
		i = _ttoi(tmp);
		y = ( i>=0 ? i : 0);
	}
	else if(yearN == 0)y = 0;
	if(monPos >= 0 && monN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,monN*sizeof(TCHAR),(dateString + monPos),monN*sizeof(TCHAR));
		i = _ttoi(tmp);
		m = ( BETWEEN(i,1,12) ? i : abs(i)%12+1);
	}
	else if(monN == 0)m = 0;
	if(dayPos >= 0 && dayN > 0){
		ZeroMemory(tmp,9);
		memcpy_s(tmp,dayN*sizeof(TCHAR),(dateString + dayPos),dayN*sizeof(TCHAR));
		i = _ttoi(tmp);
		d = ( BETWEEN(i,1,31) ? i : abs(i)%31+1);
	}
	else if(dayN == 0)d = 0;
	return MAKE_STAMP_DATE(y,m,d);
}


CLTime& CLTime::setDate(int dYears,int dMonths,int dDays)
{
	assert( dYears >= 0 && dMonths >= 0 && dDays >= 0 );
	m_at.wYear = dYears;
	m_at.wMonth = dMonths;
	m_at.wDay = dDays;
	m_at.wDayOfWeek = (WORD)getDayOfWeekByKimLarsson(dYears,dMonths,dDays);
	return *this;
}
CLTime& CLTime::setDate(const SYSTEMTIME& sysTime)
{
	m_at.wYear = sysTime.wYear;	
	m_at.wMonth = sysTime.wMonth;
	m_at.wDay = sysTime.wDay;
	m_at.wDayOfWeek = sysTime.wDayOfWeek;
	return *this;
}
CLTime& CLTime::setDate(const CLTime& cTime)
{
	return setDate(cTime.dataConst());
}

int CLTime::isLocalTime() const
{
	return m_isLocalTime;
}

CLTime& CLTime::operator=(const CLTime& _in_cltime)
{
	return set(_in_cltime);
};

const SYSTEMTIME& CLTime::dataConst() const
{
	return  m_at;
}

int CLTime::compare(const CLTime& tagTime)  const
{
	return compare(tagTime.dataConst());
}

int CLTime::compare(const SYSTEMTIME& tagSysTime) const
{
	return compare(tagSysTime.wYear,tagSysTime.wMonth,tagSysTime.wDay,tagSysTime.wHour,tagSysTime.wMinute,tagSysTime.wSecond,tagSysTime.wMilliseconds);
}

int CLTime::compare(int y,int m,int d,int h,int mi,int s,int mis) const
{
	return compare(m_at.wYear,m_at.wMonth,m_at.wDay,m_at.wHour,m_at.wMinute,m_at.wSecond,m_at.wMilliseconds,y,m,d,h,mi,s,mis);
}

int CLTime::compare(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1, int y2,int m2,int d2,int h2,int mi2,int s2,int mis2)
{
	int rt = 0;
	if( (rt = compareDate(y1,m1,d1,y2,m2,d2)) > 0 )
		return 1;
	else if( rt < 0 )
		return -1;
	else if( (rt = compareTime(h1,mi1,s1,mis1,h2,mi2,s2,mis2)) > 0 )
		return 1;
	else if( rt < 0 )
		return -1;
	else return 0;
}

int CLTime::compareDate(const CLTime& tagTime) const
{
	return compareDate(tagTime.dataConst());
}

int CLTime::compareDate(const SYSTEMTIME& tagSysTime) const
{
	return compareDate(tagSysTime.wYear,tagSysTime.wMonth,tagSysTime.wDay);
}

int CLTime::compareDate(int y,int m,int d) const
{
	return compareDate(m_at.wYear,m_at.wMonth,m_at.wDay,y,m,d);
}

int CLTime::compareDate(int y2,int m2,int d2,int y,int m,int d)
{
	if(y2 > y)
		return 1;
	else if(y2 < y)
		return -1;
	else if(m2 > m)
		return 1;
	else if(m2 < m)
		return -1;
	else if(d2 > d)
		return 1;
	else if(d2 < d)
		return -1;
	else return 0;
}

int CLTime::compareTime(const CLTime& tagTime) const
{
	return compareTime(tagTime.dataConst());
}

int CLTime::compareTime(const SYSTEMTIME& tagSysTime) const
{
	return compareTime(tagSysTime.wHour,tagSysTime.wMinute,tagSysTime.wSecond,tagSysTime.wMilliseconds);
}

int CLTime::compareTime(int h,int mi,int s,int mis) const
{
	return compareTime(m_at.wHour,m_at.wMinute,m_at.wSecond,m_at.wMilliseconds,h,mi,s,mis);
}

int CLTime::compareTime(int h2,int mi2,int s2,int mis2,int h,int mi,int s,int mis)
{
	if(h2 > h)
		return 1;
	else if(h2 < h)
		return -1;
	else if(mi2 > mi)
		return 1;
	else if(mi2 <mi)
		return -1;
	else if(s2 > s)
		return 1;
	else if(s2 < s)
		return -1;
	else if(mis2 > mis)
		return 1;
	else if(mis2 < mis)
		return -1;
	else return 0;
}

double CLTime::calcTimeDelta(const SYSTEMTIME* beforetime,const SYSTEMTIME* behandtime)
{
	int dyear = (int)behandtime->wYear - (int)beforetime->wYear;
	int dmonth = (int)behandtime->wMonth - (int)beforetime->wMonth + dyear * 12;
	int dday = (int)behandtime->wDay - (int)beforetime->wDay + dmonth * 30;
	int dhour = (int)behandtime->wHour - (int)beforetime->wHour + dday * 24;
	int dminute = (int)behandtime->wMinute - (int)beforetime->wMinute + dhour * 60;
	int dsecond = (int)behandtime->wSecond - (int)beforetime->wSecond + dminute * 60;
	double dmillisecond = (double)behandtime->wMilliseconds - (double)beforetime->wMilliseconds + dsecond * 1000.0;
	return dmillisecond / 1000.0;
}
double CLTime::calcTimeDelta_ll(TIMESTAMP beforetime,TIMESTAMP behandtime)
{
	int dyear = (int)EXTR_YEAR(behandtime)-(int)EXTR_YEAR(beforetime);
	int dmonth = (int)EXTR_MON(behandtime)-(int)EXTR_MON(beforetime) + dyear * 12;
	int dday = (int)EXTR_DAY(behandtime)-(int)EXTR_DAY(beforetime) + dmonth * 30;
	int dhour = (int)EXTR_HOUR(behandtime)-(int)EXTR_HOUR(beforetime) + dday * 24;
	int dminute = (int)EXTR_MINU(behandtime)-(int)EXTR_MINU(beforetime) + dhour * 60;
	int dsecond = (int)EXTR_SEC(behandtime)-(int)EXTR_SEC(beforetime) + dminute * 60;
	double dmillisecond = (double)EXTR_MIS(behandtime)-(double)EXTR_MIS(beforetime) + dsecond * 1000.0;
	return dmillisecond / 1000.0;
}

double CLTime::calcTimeDelta(int hf,int mf,int sf,int msf,int hb,int mb,int sb,int msb)
{
	int dhour = hb - hf;
	int dminute = mb - mf + dhour * 60;
	int dsecond = sb - sf + dminute * 60;
	double dmillisecond = (double)msb - (double)msf + dsecond * 1000.0;
	return dmillisecond / 1000.0;
}

double CLTime::calcTimeDelta(CLTimeRC beforetime,CLTimeRC behandtime)
{
	return calcTimeDelta(&beforetime.dataConst(),&behandtime.dataConst());
}

double CLTime::calcTimeDelta(const SYSTEMTIME& beforetime,const SYSTEMTIME& behandtime)
{
	return calcTimeDelta(&beforetime,&behandtime);
}

CLTime& CLTime::timingStart(void)
{
	GetSystemTime(&m_sysTimeStart);
	m_startTimeFlag = 1;
	return *this;
}

CLTime& CLTime::timingStartEx(void)
{
	QueryPerformanceCounter(&m_largeIntegerStart);
	m_startTimeExFlag = 1;
	return *this;
}

double CLTime::timingGetSpentTime(BOOL isNeedResetStartTime)
{
	if(m_startTimeFlag == 0)
	{
		::MessageBox(0,_T("CLTime.timingGetSpentTime() Error,no start!"),_T("CLTime Runtime Alert"),MB_ICONERROR);
		return 0;
	}
	GetSystemTime(&m_sysTimeEnd);
	
	double rt = CLTime::calcTimeDelta(&m_sysTimeStart,&m_sysTimeEnd);
	if(isNeedResetStartTime)
		m_sysTimeStart = m_sysTimeEnd;
	return rt;
}

double CLTime::timingGetSpentTimeEx(BOOL isNeedResetStartTime)
{
	if(m_startTimeExFlag == 0)
	{
		::MessageBox(0,_T("CLTime.timingGetSpentTimeEx() Error,no start!"),_T("CLTime Runtime Alert"),MB_ICONERROR);
		return 0;
	}
	QueryPerformanceCounter(&m_largeIntegerEnd);
	LARGE_INTEGER Freg;
	QueryPerformanceFrequency(&Freg);
	double rt = (double)(m_largeIntegerEnd.QuadPart - m_largeIntegerStart.QuadPart) / (double)Freg.QuadPart; 
	if(isNeedResetStartTime)
		m_largeIntegerStart = m_largeIntegerEnd;
	return rt;
}

int CLTime::getDayOfWeekByZeller(int year,int month,int day)
{
	assert( year >= 0 && month >= 0 && day >= 0 );
	if(month == 1 || month == 2){
		month += 12;
		year --;
	}
	return (((year/100)/4-2*(year/100)+(year-100*(year/100))+(year-100*(year/100))/4+(26*(month+1)/10)+day-1)%7+7)%7;
}

int CLTime::getDayOfWeekByKimLarsson(int year,int month,int day)
{
	assert( year >= 0 && month >= 0 && day >= 0 );
	if(month == 1 || month == 2){
		month += 12;
		year --;
	}
	return (day+2*month+3*(month+1)/5+year+year/4-year/100+year/400+1)%7;
}

BOOL CLTime::between(CLTimeRC timeA,CLTimeRC timeB)
{
	if(timeA.compare(timeB) <= 0){
		return compare(timeA) >= 0 &&  compare(timeB) <= 0;
	}
	else{
		return compare(timeB) >= 0 &&  compare(timeA) <= 0;
	}
}

BOOL CLTime::between(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB)
{
	CLTime a(timeA),b(timeB);
	return between(a,b);
}

BOOL CLTime::between(int y1,int m1,int d1,int h1,int mi1,int s1,int mis1, 
	int y2,int m2,int d2,int h2,int mi2,int s2,int mis2)
{

	CLTime a(y1,m1,d1,h1,mi1,s1,mis1),b(y2,m2,d2,h2,mi2,s2,mis2);
	return between(a,b);
}

BOOL CLTime::betweenTime(CLTimeRC timeA,CLTimeRC timeB)
{
	if(timeA.compareTime(timeB) <= 0){
		return compareTime(timeA) >= 0 &&  compareTime(timeB) <= 0;
	}
	else{
		return compareTime(timeB) >= 0 &&  compareTime(timeA) <= 0;
	}
}

BOOL CLTime::betweenTime(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB)
{
	CLTime a(timeA),b(timeB);
	return betweenTime(a,b);
}

BOOL CLTime::betweenTime(int h1,int mi1,int s1,int mis1, int h2,int mi2,int s2,int mis2)
{
	CLTime a(0,0,0,h1,mi1,s1,mis1),b(0,0,0,h2,mi2,s2,mis2);
	return betweenTime(a,b);
}

BOOL CLTime::betweenDate(CLTimeRC timeA,CLTimeRC timeB)
{
	if(timeA.compareDate(timeB) <= 0){
		return compareDate(timeA) >= 0 &&  compareDate(timeB) <= 0;
	}
	else{
		return compareDate(timeB) >= 0 &&  compareDate(timeA) <= 0;
	}
}

BOOL CLTime::betweenDate(const SYSTEMTIME& timeA,const SYSTEMTIME& timeB)
{
	CLTime a(timeA),b(timeB);
	return betweenDate(a,b);
}

BOOL CLTime::betweenDate(int y1,int m1,int d1, int y2,int m2,int d2)
{
	CLTime a(y1,m1,d1,0,0,0,0),b(y2,m2,d2,0,0,0,0);
	return betweenDate(a,b);
}


void CLTime::stampToTime_ll(TIMESTAMP stamp,int *y,int *m,int *d,int *h,int *mi,int *s,int *ms)
{
	if(y)*y= EXTR_YEAR(stamp);
	if(m)*m=EXTR_MON(stamp);
	if(d)*d=EXTR_DAY(stamp);
	if(h)*h=EXTR_HOUR(stamp);
	if(mi)*mi=EXTR_MINU(stamp);
	if(s)*s=EXTR_SEC(stamp);
	if(ms)*ms=EXTR_MIS(stamp);
}

TIMESTAMP CLTime::getStamp() const{
	return  ((TIMESTAMP)m_at.wMilliseconds + ((TIMESTAMP)m_at.wSecond*1000) + ((TIMESTAMP)m_at.wMinute*100000) + ((TIMESTAMP)m_at.wHour*10000000) + ((TIMESTAMP)m_at.wDay*1000000000) + ((TIMESTAMP)m_at.wMonth*100000000000) + ((TIMESTAMP)m_at.wYear*10000000000000));
}

TIMESTAMP CLTime::getLocalTime_ll()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	return MAKE_STAMP(t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);
}

TIMESTAMP CLTime::getSystemTime_ll()
{
	SYSTEMTIME t;
	GetSystemTime(&t);
	return MAKE_STAMP(t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);
}

int CLTime::calcDiffTimeSum( TIMESTAMP _startTime, TIMESTAMP _endTime ,OUT double* pweek /*= 0*/,OUT double* pyear /*= 0*/,
	OUT double* pmon /*= 0*/,OUT double* pday /*= 0*/, OUT double* phour /*= 0*/,OUT double* pmin /*= 0*/,OUT double* psec /*= 0*/, OUT double* pminiSec /*= 0*/ )
{
	TIMESTAMP startTime,endTime;BOOL bIsRig = TRUE;
	if(_startTime > _endTime){startTime = _endTime;endTime = _startTime;bIsRig = FALSE;}
	else{startTime = _startTime;endTime = _endTime;}
	tm st,et;
	double minsecS = EXTR_MIS(startTime);
	double minsecE = EXTR_MIS(endTime);
	setTS2Tm( st , startTime );
	setTS2Tm( et , endTime );
	time_t tts = mktime(&st);
	time_t tte = mktime(&et);
	if( minsecS <  minsecE){ minsecE = minsecE - minsecS;}
	else{ minsecE = minsecE + 1000 - minsecS; tte-=1;}	
	double dif = /* difftime(tts,tte)*/ tte - tts;
	if(pminiSec)*pminiSec = bIsRig ? (dif*1000 + minsecE) :(dif*1000 + minsecE)*(-1.0);
	double sec = dif + minsecE/1000;
	double minu = sec / 60;
	double hour = minu / 60;
	double day = hour / 24;
	double mon = day / 30;
	double year = mon / 12;
	if(psec)*psec = bIsRig ? (sec) :(sec)*(-1.0);
	if(pmin)*pmin = bIsRig ? (minu) :(minu)*(-1.0);
	if(phour)*phour = bIsRig ? (hour) :(hour)*(-1.0);
	if(pday)*pday = bIsRig ? (day) :(day)*(-1.0);
	if(pmon)*pmon = bIsRig ? (mon) :(mon)*(-1.0);
	if(pyear)*pyear = bIsRig ? (year) :(year)*(-1.0);
	if(pweek)*pyear = bIsRig ? (sec/3600/24/7) :(sec/3600/24/7)*(-1.0);
	return 1;
}

int CLTime::calcDiffTime( TIMESTAMP _startTime, TIMESTAMP _endTime ,OUT long* pyear /*= 0*/,OUT long* pmon /*= 0*/,OUT long* pday /*= 0*/, OUT long* phour /*= 0*/,
	OUT long* pmin /*= 0*/,OUT long* psec /*= 0*/, OUT long* pminiSec /*= 0*/ )
{
	TIMESTAMP startTime,endTime;BOOL bIsRig = TRUE;
	if(_startTime > _endTime){startTime = _endTime;endTime = _startTime;bIsRig = FALSE;}
	else{startTime = _startTime;endTime = _endTime;}
	tm st,et;
	TIMESTAMP minsecS = EXTR_MIS(startTime);
	TIMESTAMP minsecE = EXTR_MIS(endTime);
	setTS2Tm( st , startTime );
	setTS2Tm( et , endTime );
	time_t tts = mktime(&st);
	time_t tte = mktime(&et);
	if( minsecS <  minsecE){ minsecE = minsecE - minsecS;}
	else{ minsecE = minsecE + 1000 - minsecS; tte-=1;}	
	double dif =/* difftime(tts,tte)*/ tte - tts;
	if(pminiSec)*pminiSec = bIsRig? minsecE : (-1.0)*minsecE;
	long sec = (LONGLONG)dif % 60;
	dif = (LONGLONG)dif / 60;
	long minu = (LONGLONG)dif % 60;
	dif = (LONGLONG)dif / 60;
	long hour = (LONGLONG)dif % 24;
	dif = (LONGLONG)dif / 24;
	long day = (LONGLONG)dif % 30;
	dif = (LONGLONG)dif / 30;
	long mon = (LONGLONG)dif % 12;
	dif = (LONGLONG)dif / 12;
	long year = (LONGLONG)dif;
	if(psec)*psec = bIsRig? sec : (-1.0)*sec;
	if(pmin)*pmin = bIsRig? minu : (-1.0)*minu;
	if(phour)*phour = bIsRig? hour : (-1.0)*hour;
	if(pday)*pday = bIsRig? day : (-1.0)*day;
	if(pmon)*pmon = bIsRig? mon : (-1.0)*mon;
	if(pyear)*pyear = bIsRig? year : (-1.0)*year;
	return 1;
}

double CLTime::timingGetRestOfTimeEx(long endIndex, long startIndex , long curIndex , long stepLen , LPTSTR lpOutStringBuf , size_t bufSize ) {
	if (m_startTimeExFlag == 0)
	{
		::MessageBox(0, _T("CLTime.timingGetSpentTimeEx() Error,no start!"), _T("CLTime Runtime Alert"), MB_ICONERROR);
		return 0;
	}
	double ss = timingGetSpentTimeEx() / double(curIndex - startIndex + 1) * double(endIndex - curIndex - 1);
	if (lpOutStringBuf && bufSize) {
		long s = ss;
		long mi = long(s / 60); s = s % 60;
		long ho = long(mi / 60); mi = mi % 60;
		long day = long(ho / 24); ho = ho % 24;
		long mon = long(day / 30); day = day % 30;
		long yar = long(mon / 12); mon = mon % 12;
		CLString str;
		if (yar != 0)str << yar << _T("年");
		if (mon != 0)str << mon << _T("月");
		if (day != 0)str << day << _T("日");
		if (ho != 0)str << ho << _T("时");
		if (mi != 0)str << mi << _T("分");
		if (s != 0)str << s << _T("秒");
		_tcscpy_s(lpOutStringBuf, bufSize, str());
	}
	return ss;
}

double CLTime::timingDumpSpentTimeC( BOOL isNeedResetStartTime /*= FALSE*/,LPCTSTR lpHdr /*= 0*/,LPCTSTR lpEnd /*= 0*/ )
{
	double r = timingGetSpentTime(isNeedResetStartTime);
	_tprintf(_T("%s%f%s"),lpHdr?lpHdr:_T(""),r,lpEnd?lpEnd:_T(""));
	return r;
}

double CLTime::timingDumpSpentTimeExC( BOOL isNeedResetStartTimeEx /*= FALSE*/,LPCTSTR lpHdr /*= 0*/,LPCTSTR lpEnd /*= 0*/ )
{
	double r = timingGetSpentTimeEx(isNeedResetStartTimeEx);
	_tprintf(_T("%s%f%s"),lpHdr?lpHdr:_T(""),r,lpEnd?lpEnd:_T(""));
	return r;
}



