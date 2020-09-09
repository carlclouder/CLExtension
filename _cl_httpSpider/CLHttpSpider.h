#pragma once
#ifndef __CL_HTTPSPIDER_H__
#define __CL_HTTPSPIDER_H__

#include "../../_cl_extension/_cl_extension.h"
#include "windows.h"

class CLHttpSpider;
typedef CLHttpSpider *PCLHttpSpider;
typedef const CLHttpSpider *PCCLHttpSpider;
typedef CLHttpSpider& CLHttpSpiderR;
typedef const CLHttpSpider& CLHttpSpiderRC;



class CLHttpSpider{
protected:
	CLString m_rem;
	CLString m_org;
	CLString m_value;
	CLString m_tmp;
	int m_ceng;
	
	virtual LPCTSTR _labelRecrsion(LPCTSTR ptcTag,LPCTSTR ptcRem,std::vector<CLString>& resuleSet,int nlayer,LPCTSTR lpHeader,size_t nAllSize);//抽取函数
	virtual BOOL _checkSelfClose(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize);//检验该位置是否为自闭合标签,从<后面开始搜索
	virtual BOOL _checkNotes(LPCTSTR pc,LPCTSTR &p1,LPCTSTR lpHeader,size_t nAllSize);//从<后面开始
	//从<开始搜索,如果成功p1返回标签词根尾部之后的指针，p2返回标签结束之后的指针
	virtual BOOL _isFlag(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize);
	//从<开始搜索,如果成功p1返回<的指针，p2返回>之后的指针
	virtual BOOL _tatchEnd(LPCTSTR pc,LPCTSTR lpflag,LPCTSTR &p1,LPCTSTR &p2,LPCTSTR lpHeader,size_t nAllSize);
	//从<开始搜索
	virtual LPCTSTR _deleteFlagPair(LPCTSTR pc,LPCTSTR lpHeader,size_t nAllSize);
public:
	CLHttpSpider();	
	virtual ~CLHttpSpider() {}
/* 
lpRem样式举例：
<html><body><div>{2}<div>{4}<div>{3}<div>{1,2}<div><table><tr>{1-50,1}<td>{1-5,1}[_value_]
*/
	CLHttpSpiderR set(LPCTSTR lpOrg,LPCTSTR lpRem);
	CLHttpSpiderR setRem(LPCTSTR lpRem);
	CLHttpSpiderR setOrg(LPCTSTR lpOrg);
	//抽取内容到容器，bClearSetsBeforeExtract指明在抽取前是否清空容器
	virtual std::vector<CLString>& extract(std::vector<CLString>& resultSet,BOOL bClearSetsBeforeExtract = TRUE);
};

#endif