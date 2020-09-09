#pragma once
#ifndef __CL_SMARTLINE_H__
#define __CL_SMARTLINE_H__

#include "windows.h"
#include "shlobj.h"
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <queue>
#include <assert.h>
#include <conio.h>
#include <iOS>
#include <TlHelp32.h>    
#include <atlstr.h>    
#include <locale.h> 
#include <map>
#include "../_cl_common/CLCommon.h"

#define SL_TYPE_NO       (0x0)  //模式类型：尽有基本数据曲线，不计算扩展曲线 
#define SL_TYPE_DIFF     (0x1)  //模式类型：计算 移动差值   曲线  
#define SL_TYPE_AVER     (0x2)  //模式类型：计算 移动平均值 曲线 
//#define SL_TYPE_ALL      (SL_TYPE_AVER|SL_TYPE_DIFF)  //模式类型：计算 移动平均值 和 移动差值 曲线   
#define SL_TYPE_VARI     (0x4|SL_TYPE_AVER)  //模式类型：计算 方差 曲线  
#define SL_TYPE_STDD     (0x8|SL_TYPE_VARI)  //模式类型：计算 标准差 曲线  
#define SL_TYPE_NOCYCLE  (0x10)  //模式扩展类型：计算所有曲线数据 采用不循环模式（线性模式），该模式要平凡移动曲线数据，非常消耗cpu，尽量少用   
class CLSmartLine{
public:
	using valueType = double;
	using sizeType = long;
	using flagType = int;
protected:
	/*valueType* lst;
	valueType* avr;
	valueType* dif;
	valueType* var;
	valueType* stdd;*/
	vector<valueType> lst;
	vector<valueType> avr;
	vector<valueType> dif;
	vector<valueType> var;
	vector<valueType> stdd;
	flagType flag;
	sizeType nMax;
	sizeType ci;
	sizeType divN;
	valueType total;
	valueType defV;
	void _init();
	void _release();
	void _construct(sizeType nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
#ifdef _DEBUG
	//std::vector<VT> lstCheck;
#endif
public:
	CLSmartLine();
	CLSmartLine(sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
	//CLSmartLine(const CLSmartLine& tag);
	CLSmartLine(const valueType* _pv,sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	CLSmartLine(const std::vector<valueType>& _pv,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	void construct(sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO,valueType _defV = 0);
	void construct(const valueType* _pv,sizeType _nmax,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	void construct(const std::vector<valueType>& _pv,sizeType _divn = 0, flagType _flag = SL_TYPE_NO);
	virtual ~CLSmartLine();	
	void reset();//重置
	void release();//释放内存并重置
	valueType& operator [](sizeType i);//返回的是基本曲线数据，而非扩展曲线
	valueType& at(sizeType i){ return (*this)[i];}
	//取得有效安全的索引号
	sizeType index(sizeType offset = 0){  
		if(nMax <= 0 )goto err;
		if(flag & SL_TYPE_NOCYCLE){
			sizeType i = (ci + offset);
			return ( (i > (nMax - 1)) ? (nMax - 1) : ( (i < 0) ? (nMax + i%nMax) : i ) );
		}
		else {sizeType i = (ci+offset)%nMax;return (i >= 0) ? i:(nMax + i);}
err:
		::MessageBox(NULL,_T("MOVEAVERAGE类“index操作”非法，对象没有构造 ！"),_T("MOVEAVERAGE Alert"),MB_ICONERROR);
		throw std::logic_error("MOVEAVERAGE类“index操作”非法，对象没有构造 ！");
		return -1;
	};
	//加入一个数据到数据中，并根据类型计算扩展曲线
	sizeType push(valueType v);
	//返回最新移动总和值
	inline valueType sum() const { return total; }

	////返回最新平均值
	//inline valueType aver() const {return (avr) ? avr[ci] : 0;}		
	////返回基础数据曲线数据列表，没有构造初始化情况下可能为NULL
	//inline const valueType* vlst() const {return lst;}
	////返回移动平均值曲线数据列表，可能为NULL
	//inline const valueType* avrLst() const {return avr;}
	////返回移动差值曲线数据列表，可能为NULL
	//inline const valueType* difLst() const {return dif;}
	////返回移动方差曲线数据列表，可能为NULL
	//inline const valueType* varLst() const {return var;}
	////返回移动标准差曲线数据列表，可能为NULL
	//inline const valueType* stddLst() const {return stdd;}
	////返回最新值
	//inline valueType cv() const {return (lst)? lst[ci] : 0; } 
	////返回最新差值
	//inline valueType diff() const {return (dif) ? dif[ci] : 0;}
	////返回最新方差值
	//inline valueType vari() const {return (var) ? var[ci] : 0;}
	////返回最新标准差值
	//inline valueType stddev() const {return (stdd) ? stdd[ci] : 0;}

	
	//返回基础数据曲线数据列表，没有构造初始化情况下可能为NULL
	inline const valueType* vlst() const { return lst.data();}
	//返回移动平均值曲线数据列表，可能为NULL
	inline const valueType* avrLst() const { return avr.data(); }
	//返回移动差值曲线数据列表，可能为NULL
	inline const valueType* difLst() const { return dif.data(); }
	//返回移动方差曲线数据列表，可能为NULL
	inline const valueType* varLst() const { return var.data(); }
	//返回移动标准差曲线数据列表，可能为NULL
	inline const valueType* stddLst() const { return stdd.data(); }
	//返回最新值
	inline valueType cv() const { return lst[ci]; }
	//返回最新平均值
	inline valueType aver() const { return avr[ci]; }
	//返回最新差值
	inline valueType diff() const { return dif[ci]; }
	//返回最新方差值
	inline valueType vari() const { return var[ci]; }
	//返回最新标准差值
	inline valueType stddev() const { return stdd[ci]; }

	//返回曲线最大数据个数
	inline sizeType maxCounts() const {return nMax;}
	//返回基本曲线的默认填充值
	inline valueType defaultValue() const {return defV;}
	//返回对象类型标签（SL_TYPE_NO SL_TYPE_AVER SL_TYPE_DIFF SL_TYPE_ALL SL_TYPE_NOCYCLE 一种或多种组合）
	inline sizeType typeFlag() const {return flag;}
	//返回移动平均模式下的跨度
	inline sizeType moveStep() const {return divN;}
	//返回移动平均模式下在当前index之后的数据全部置0后的数据列表
	const valueType* zeroAvrLstEnd();
	//返回移动差值模式下在当前index之后的数据全部置0后的数据列表
	const valueType* zeroDifLstEnd();
	const valueType* zeroVarLstEnd();
	const valueType* zeroStddLstEnd();
	
	void dumpVlst()const;
	void dumpAvrLst()const;
	void dumpDifLst()const;
	void dumpVariLst()const;
	void dumpStddLst()const;
	void dumpAllLst()const;

	//将内部曲线数据全部按nMax尺寸反向。函数使用后push操作将混乱，一般只有在操作结束后才执行该方法。
	CLSmartLine& reverse();
};
typedef CLSmartLine &CLSmartLineR,*PCLMoveAverage;
typedef const CLSmartLineR CLSmartLineRC;
typedef const PCLMoveAverage PCCLMoveAverage;

#endif