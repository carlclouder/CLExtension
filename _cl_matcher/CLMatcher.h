#pragma once
#ifndef __CL_MATCHER_H__
#define __CL_MATCHER_H__

#include "windows.h"
#include "../_cl_string/CLString.h"

class CLMatcher;
typedef CLMatcher* PCLMatcher;
typedef const PCLMatcher PCLCMatcher;
typedef CLMatcher& CLMatcherR;
typedef const CLMatcherR CLMatcherRC;

#define MAX_LEN 256
typedef struct _str_kwUnit{
	TCHAR kwd[MAX_LEN];//关键字
	byte nCharCounts;//关键字字符个数
	byte nCp;//内部使用的临时变量
	LPCTSTR lpFirst;//第一次匹配的位置
	size_t nMatch;//匹配次数
	_str_kwUnit(){reset();}
	_str_kwUnit& reset(){ZeroMemory(this,sizeof(_str_kwUnit));return *this;}
}KwUnit,*PKwUnit;
typedef struct _str_KeyWordsLst{ 			
	_str_kwUnit unit[MAX_LEN];//关键字数组
	byte nUnitCounts;//关键字组数
	_str_KeyWordsLst(){}
	_str_KeyWordsLst& reset(){ZeroMemory(this,sizeof(_str_KeyWordsLst));return *this; }
}KeyWordsLst,*PKeyWordsLst;

//列表匹配工具类
class CLMatcher{
protected:
	KeyWordsLst m_kwLst;  
	LPCTSTR pOrgHead;
	TCHAR m_currentChar;
	CLString m_splitLst;

public:
	//默认构造函数，内部将保存本地时间
	CLMatcher();
	virtual~CLMatcher() {};
	//清除所有内部数据和设置
	CLMatcherR reset();
	//设置字符串源
	CLMatcherR setOrg(LPCTSTR text);
	//设置关键字列表，关键字间以,隔开（自动执行关键字的trim操作）
	CLMatcherR setKeyLst(LPCTSTR keyString);
	//执行全文匹配，执行前请调用setOrg和setKeyLst完成设置，执行后调用getResult取得结果
	CLMatcherR match();
	//在match操作后调用，取得关键字信息结构体，可从unit中取得结果
	const KeyWordsLst& getResult() const;
	//匹配度
	float matchRat() const;
    //关键字个数
	UINT getKeyWordCounts() const;
};

#endif


