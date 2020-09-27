//DESIGNED BY CAILUO @2020-02-10 
//MINI-SUPPORT @ C++14

#pragma once

#ifndef __CL_MEMPOOL_HPP__
#define __CL_MEMPOOL_HPP__

#include <cassert>
#include <map>
#include <string>
#include <stdexcept>

#ifndef CLMP_COMMON_SUPPORT
#define CLMP_COMMON_SUPPORT

#ifndef CLMP_USE_LOCK_TYPE
#define CLMP_USE_LOCK_TYPE 1 // 0=无锁（快），1=用RWLock（读写锁）， 2=用C++锁，3=用windows临界区锁
#endif

#if CLMP_USE_LOCK_TYPE == 3
#include "windows.h" //windows平台,临界区
class CLMemPoolLock : CRITICAL_SECTION {
public:
	CLMemPoolLock() { InitializeCriticalSection(this); }
	~CLMemPoolLock() { DeleteCriticalSection(this); }
	void lock() { EnterCriticalSection(this); }
	void unlock() { LeaveCriticalSection(this); }
};
#elif CLMP_USE_LOCK_TYPE == 2
#include <mutex> // C++ STL mutex
class CLMemPoolLock:public std::mutex {};
#elif CLMP_USE_LOCK_TYPE == 1
#include "../_cl_common/CLCommon.h" //高速自定义读写锁
class CLMemPoolLock :public RWLock {};
#else
class CLMemPoolLock {  //no use lock, it do nothing
public:
	void lock() {} 
	void unlock() {} 
};
#endif

#ifndef _CL_DIFVARS_SUPPORT_
#define _CL_DIFVARS_SUPPORT_
#ifdef UNICODE
typedef wchar_t Char;
#define tstring wstring
#ifndef _T
#define _T(x)  L ## x
#endif
#ifndef _tprintf_s
#define _tprintf_s  wprintf_s
#define _stprintf_s swprintf_s
#define _tcscpy_s   wcscpy_s
#define _tcscat_s   wcscat_s
#define _itot_s     _itow_s
#endif
#else
typedef char Char;
#define tstring string
#ifndef _T
#define _T(x)  x
#endif
#ifndef _tprintf_s
#define _tprintf_s  printf_s
#define _stprintf_s sprintf_s
#define _tcscpy_s   strcpy_s
#define _tcscat_s   strcat_s
#define _itot_s     _itoa_s
#endif
#endif
typedef const Char* PCStr;
typedef Char* PStr;
#ifndef BUFSIZE
#define BUFSIZE 4096
#endif
#ifndef max
#define max(a,b) ((a) < (b) ? (b) : (a))
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#endif

//标准化接口
struct IMenPool {
	virtual void arrangement(bool) = 0;
	virtual void deallocate(void*) = 0;
	virtual void giveUp(void*) = 0;
	struct MenPoolInfo {
		PCStr structType;//内存池结构类型（单个或连续）
		PCStr memPoolTypeName;//指定的类型名		
	};
	virtual MenPoolInfo getInfo() const = 0;
};

#endif

template <typename value_type>
class CLMemPoolBlock;

template <typename value_type>
class CLMemPool;

////内存池表类型
class MemPoolsTable
	:public std::map<void*, std::tstring>,
	public CLMemPoolLock
{};
//取得内存池表
inline MemPoolsTable* getMemPoolsTable() {
	static MemPoolsTable _mplst;
	return &_mplst;
}
////增加内存池到内存池表
inline void addMemPoolToTable(PCStr name,void* pMemPool)
{
	getMemPoolsTable()->lock();
	(*getMemPoolsTable())[pMemPool] = (name == nullptr ? _T("") : name);
	getMemPoolsTable()->unlock();
}

//内存池对象单元模板类
template <typename value_type>
class CLMemPoolUnit {
	typedef CLMemPoolUnit<value_type> unit_type;
	typedef CLMemPoolBlock<value_type> block_type;
	typedef CLMemPool<value_type> container_type;
	friend class block_type;
	friend class container_type;
public:
	struct DataHead {
		unit_type* pPre;
		unit_type* pNext;
		block_type* pThisBlock;
		size_t utUsedCounts;
	};
private:
	DataHead hdr;
	unit_type* pThis;
	value_type data;//包裹数据对象

	//在构造时候显示调用
	unit_type* init(
		block_type* _pThisBlock = 0,
		unit_type* _pPre = 0,
		unit_type* _pNext = 0,
		size_t _utUsedCounts = 0
	) { 
		hdr.pThisBlock = _pThisBlock;
		hdr.pPre = _pPre;
		hdr.pNext = _pNext;
		hdr.utUsedCounts = _utUsedCounts;
		pThis = this;
		return this; 
	}
	//注意：构造和析构函数用于显示的调用来构造和析构内部包裹对象
	//他们不应该被系统调用，且内部不能有任何的数据处理过程；
	CLMemPoolUnit()
		:pThis(this)
	{}
	template <typename... Args>
	CLMemPoolUnit(Args&&... args):data(std::forward<Args>(args)...){}
	~CLMemPoolUnit() {}
public:
	inline block_type* getOwnerBlock() { return hdr.pThisBlock; }
	static size_t getMemSize() { return sizeof(unit_type); }
};

//内存池内存块对象模板类
template <typename value_type>
class CLMemPoolBlock {
	typedef CLMemPoolUnit<value_type> unit_type;
	typedef CLMemPoolBlock<value_type> block_type;
	typedef CLMemPool<value_type> container_type;
	friend class unit_type;
	friend class container_type;

private:
	explicit CLMemPoolBlock(size_t PerBlockMemCapacity_MB , container_type* _pOwnerMemPool)
		:pOwnerMemPool(_pOwnerMemPool)
	{
		init();
		alloc(PerBlockMemCapacity_MB * 1024 * 1024 / sizeof(unit_type));
	}
	~CLMemPoolBlock() {
		releaseObjMenBlock();
	}
	//判断该内存块是否存在可用单元
	inline bool isUsable() {
		return pUsableLst ? true : false;
	}
	//将对象指针回收进入内存块单元，如果对象是已经构造过的就调用标准析构。
	unit_type* freeOneData(unit_type* pUnit) {
		assert(pUnit !=nullptr);
		//执行对象析构
		if (pUnit->hdr.utUsedCounts) {
			pUnit->unit_type::~CLMemPoolUnit();
			pUnit->hdr.utUsedCounts = 0;
			nHasUsedUnitCounts--;
		}
		return putToUsable(pUnit);
	}
	//将对象指针回收进入内存块单元,而不调用析构
	unit_type* giveUpOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//不执行对象析构，直接放入未用连条
		return putToUsable(pUnit);
	}
	//取出一个待命可用的对象单元，返回其构造后的对象指针。注意：若申请时的当前分配对象已构造，则忽略构造传参。
	template <typename... Args>
	value_type* getOneData(Args&&... args) {
		assert(pUsableLst != nullptr);
		if(!pUsableLst->hdr.utUsedCounts) {
			//执行对象默认构造，这会构造内部包裹的数据类
			pUsableLst->unit_type::CLMemPoolUnit(std::forward<Args>(args)...);
			pUsableLst->hdr.utUsedCounts = 1;
			nHasUsedUnitCounts++;
		}
		return putToUnusable(pUsableLst);
	}
	inline void init() {
		pUsableLst = 0;
		nMaxUsable = 0;
		pUnusableLst = 0;
		nMaxUnusable = 0;
		nHasUsedUnitCounts = 0;
		pPre = 0;
		pNext = 0;
		pMainDataLst = 0;
		nMaxDataCounts = 0;
	}
	//分配内存
	void alloc(size_t unitCounts = 1) {
		assert(pMainDataLst == 0 && pUsableLst == 0 && pUnusableLst == 0);
		if (pMainDataLst == 0) {
			//第一次申请内存空间
			pMainDataLst = (unit_type *)malloc(sizeof(unit_type)*(nMaxDataCounts = nMaxUsable = (unitCounts == 0 ? 1 : unitCounts)));
			//第一次执行可用队列的初始化连接工作
			pUsableLst = &pMainDataLst[0];
			pMainDataLst[0].init(this, 0, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[1]);
			for (size_t i = 1; i < nMaxDataCounts-1; ++i) {
				pMainDataLst[i].init(this, &pMainDataLst[i-1], &pMainDataLst[i + 1]);
			}
			pMainDataLst[nMaxDataCounts - 1].init(this, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[nMaxDataCounts - 2], 0);
		}
	};
	//析构所有已经构造过的对象，但没有使用的对象。
	void distructCreatedUsable() {
		if (nHasUsedUnitCounts == 0)
			return;
		//析构所有在可用连中的已构造
		for (unit_type* pc = pUsableLst; pc != nullptr; ) {
			if (pc->hdr.utUsedCounts) {
				pc->unit_type::~CLMemPoolUnit();
				pc->hdr.utUsedCounts = 0;
				nHasUsedUnitCounts--;
			}
			pc = pc->hdr.pNext;
		}
	}
	//释放内存
	void releaseObjMenBlock() {
		if (pMainDataLst) {
			distructCreatedUsable();
			//析构所有已使用的对象
			for (unit_type* pc = pUnusableLst; pc != nullptr; ) {			
				pUnusableLst = freeOneData(pc);
				pc->init();
				pc = pUnusableLst;
			}
			//释放动态内存
			free(pMainDataLst);
			pMainDataLst = 0;
		}		
	}
	//处理前后连接,隔离对象,返回原对象指针
#define _extruct_pUnitBlock(pUnit) \
		((((pUnit)->pPre) ? ((pUnit)->pPre->pNext = (pUnit)->pNext) : 0),(((pUnit)->pNext)?((pUnit)->pNext->pPre = (pUnit)->pPre):0),(pUnit))
#define _extruct_pUnit(pUnit) \
		((((pUnit)->hdr.pPre) ? ((pUnit)->hdr.pPre->hdr.pNext = (pUnit)->hdr.pNext) : 0),(((pUnit)->hdr.pNext)?((pUnit)->hdr.pNext->hdr.pPre = (pUnit)->hdr.pPre):0),(pUnit))
	
	//对象放入可用队列头，返回不可用队列头指针
	unit_type*  putToUsable(unit_type* pUnit) {
		assert(this == pUnit->hdr.pThisBlock);
		//处理不可用列表头
		if (pUnusableLst && pUnusableLst == pUnit)
			pUnusableLst = pUnit->hdr.pNext;

		//处理前后连接,隔离对象
		_extruct_pUnit(pUnit);

		//接入可用列表
		if (pUsableLst) {pUsableLst->hdr.pPre = pUnit;}
		pUnit->hdr.pNext = pUsableLst;
		pUnit->hdr.pPre = 0;
		pUsableLst = pUnit;
		nMaxUnusable -= 1;
		nMaxUsable += 1;
		return pUnusableLst;
	}

	//返回可用对象指针
	value_type*  putToUnusable(unit_type* pUnit) {
		assert(this == pUnit->hdr.pThisBlock);
		//处理可用列表头
		if (pUsableLst && pUsableLst == pUnit)
			pUsableLst = pUnit->hdr.pNext;

		//处理前后连接,隔离对象
		_extruct_pUnit(pUnit);
		
		//接入不可用列表
		if (pUnusableLst) {	pUnusableLst->hdr.pPre = pUnit;}
		pUnit->hdr.pNext = pUnusableLst;
		pUnit->hdr.pPre = 0;
		pUnusableLst = pUnit;
		nMaxUnusable += 1;
		nMaxUsable -= 1;
		return &pUnit->data;
	}
	container_type* const pOwnerMemPool; //所属内存池
	unit_type* pMainDataLst;//内存队列
	size_t nMaxDataCounts;
	unit_type* pUsableLst;//可用列表
	size_t nMaxUsable;
	unit_type* pUnusableLst;//不可用列表
	size_t nMaxUnusable;
	size_t nHasUsedUnitCounts;
	CLMemPoolBlock<value_type>* pPre;
	CLMemPoolBlock<value_type>* pNext;
public:
	inline container_type* getOwnerMemPool() { return pOwnerMemPool;}
	inline size_t getMemSize() const { return sizeof(block_type) + nMaxDataCounts * unit_type::getMemSize(); }
};


//内存池对象模板类。
 //请使用静态的getMemPool()方法获取唯一的内存池对象。
template <typename value_type> 
class CLMemPool :public IMenPool,public CLMemPoolLock {
	typedef CLMemPoolUnit<value_type> unit_type;
	typedef CLMemPoolBlock<value_type> block_type;
	typedef CLMemPool<value_type> container_type;
	friend class unit_type;
	friend class block_type;
private:
	//构造带名称标识的目标类型内存池，加入内存池表。请勿显示调用该构造方法。
	//请使用静态的getMemPool()方法获取唯一的内存池对象。
	CLMemPool(PCStr _lpTypeName, size_t PerBlockMemCapacity_MB = 5) :
		pEntry(nullptr),
		pCurrentUsingBlock(nullptr)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(_lpTypeName);
	}
	//构造目标类型内存池，加入内存池表。请勿显示调用该构造方法。
	//请使用静态的getMemPool()方法获取唯一的内存池对象。	
	CLMemPool(size_t PerBlockMemCapacity_MB = 1) :
		pEntry(nullptr),
		pCurrentUsingBlock(nullptr)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(nullptr);
	}
public:	
	virtual ~CLMemPool() {
		//不要释放任何内存，而是做一次整理
		arrangement();
		getMemPoolsTable()->lock();
		for (auto  i = ::getMemPoolsTable()->find((CLMemPool<void*>*)this); 
			i != ::getMemPoolsTable()->end(); ){
			::getMemPoolsTable()->erase(i);
			break;
		} 
		getMemPoolsTable()->unlock();
	}
	void setMemPoolTypeName(PCStr _lpTypeName) {
		lpTypeName = _lpTypeName ;
		::addMemPoolToTable(lpTypeName, (CLMemPool<void*>*)this);//构造之后加入队列
	}
	PCStr getMemPoolTypeName()const {
		return lpTypeName;
	}
	//整理内存，释放当前没有使用的空间归还系统。该函数用于内存池反复使用后占用较大资源后做一次统一释放。
	//distructCreatedButNoUsed = true 表示析构未被使用但已构造的对象
	virtual void arrangement(bool distructCreatedButNoUsed = true) {
		lock();
		pCurrentUsingBlock = nullptr;	//	置0
		for (block_type* pi = pEntry; pi != nullptr; )
		{
			block_type* pib = pi->pNext;
			if (distructCreatedButNoUsed)//是否析构未用对象
				pi->distructCreatedUsable();
			if (pi->nMaxUnusable == 0 && pi->nHasUsedUnitCounts == 0) { //释放条件，没有被使用,没有已构造
				if (pEntry == pi)
					pEntry = pi->pNext;
				delete _extruct_pUnitBlock(pi);
			}
			pi = pib;
		}
		unlock();
	}
	//强行释放所有内存。
	virtual void releaseObjMenPool() {
		lock();
		for (block_type* pi = pEntry; pi != nullptr; )
		{
			pCurrentUsingBlock = pi->pNext;
			delete pi; // 释放块
			pi = pCurrentUsingBlock;
		}
		m_PerBlockMemCapacity = 0;
		pEntry = 0;
		pCurrentUsingBlock = 0;
		unlock();
	}
	//向内存池储备内存动态申请一个对象，若对象是已经构造过，就返回指针，如果未构造，先构造并返回其对象指针。
	//该方法接受可变参数构造方式传参。
	//注意：若申请时的当前分配对象已构造过（比如：是由giveUpObj()放弃使用权而回收的对象），则忽略本次构造传参。
	template <typename... Args>
	value_type* allocate(Args&&... args) {
		lock();		
		value_type* rt = getAvailableBlock()->getOneData(std::forward<Args>(args)...);
		unlock();
		return rt;
	}

#define MUHEARDER( _ty, pData )  (*((CLMemPoolUnit<_ty>**)(((char*)(pData))-(sizeof(CLMemPoolUnit<_ty>*)))))

	//向内存池释放一个动态申请的对象，进行相关析构操作。
	virtual void deallocate(void* pDelete) {
		unit_type* pUnit = MUHEARDER(value_type,pDelete);
		if(!checkPtrValidity(pUnit)
			)return;//指针非法即返回或抛异常
		lock();
		pUnit->getOwnerBlock()->freeOneData(pUnit);
		unlock();
	}
	//向内存池释放一个动态申请的对象，不析构。
	virtual void giveUp(void* pDelete) {
		unit_type* pUnit = MUHEARDER(value_type, pDelete);
		if (!checkPtrValidity(pUnit))
			return;//指针非法即返回或抛异常
		lock();
		pUnit->getOwnerBlock()->giveUpOneData(pUnit);
		unlock();
	}
	//判断指针是否属于本内存池构造
	bool checkPtrValidity(unit_type* pUnit){
		if(pUnit->getOwnerBlock()->getOwnerMemPool() != this)
			throw std::runtime_error("Tag obj is not created by this mem pool!");
		if (!pUnit->hdr.utUsedCounts) {
#ifdef _DEBUG
			if(pUnit->hdr.utUsedCounts)
				throw std::runtime_error("Tag obj has been give up!");
			else
				throw std::runtime_error("Tag obj has been distructed!");
#else
			return false;
#endif
		}
		return true;
	}
	//设置单个内存块的大小，单位MB，默认5MB
	virtual void setPerBlockMemCapacity(size_t PerBlockMemCapacity_MB = 1) {
		lock();
		m_PerBlockMemCapacity = PerBlockMemCapacity_MB == 0 ? 1 : PerBlockMemCapacity_MB;
		unlock();
	}
	//内存大小转文字字符串
	static tstring memorySizeString(size_t mem) {
		size_t Tb = 0, Gb = 0, Mb = 0, Kb = 0, Byte = 0;
		Kb = mem / 1024; Byte = mem % 1024;
		Mb = Kb / 1024; Kb = Kb % 1024;
		Gb = Mb / 1024; Mb = Mb % 1024;
		Tb = Gb / 1024; Gb = Gb % 1024;
		Char ret[150] = { 0 },stb[15] = { 0 },sgb[15] = { 0 },smb[15] = { 0 },skb[15] = { 0 },sbb[15] = { 0 };
		_tcscat_s(ret, _T(" "));	
		if (Tb > 0) {
			_itot_s(Tb, stb, 10);_tcscat_s(ret, stb); _tcscat_s(ret, _T("T "));
		}
		if (Gb > 0)
		{
			_itot_s(Gb, sgb, 10); _tcscat_s(ret, sgb); _tcscat_s(ret, _T("G "));
		}
		if (Mb > 0)
		{
			_itot_s(Mb, smb, 10); _tcscat_s(ret, smb); _tcscat_s(ret, _T("M "));
		}
		if (Kb > 0)
		{
			_itot_s(Kb, skb, 10); _tcscat_s(ret, skb); _tcscat_s(ret, _T("K "));
		}
		if (Byte > 0)
		{
			_itot_s(Byte, sbb, 10); _tcscat_s(ret, sbb); _tcscat_s(ret, _T("B "));
		}
		if(mem == 0)
			_stprintf_s(ret, _T(" %dB "), 0);		
		return ret;
	}
	//统计内存使用量，并控制向控台输出当前内存池的内存使用情况。
	//bLog = true表示输出信息到控制台否则只统计内存使用量，bDeTail = false表示采用简化输出,outInfoString动态信息字符串内容需要手动释放
	virtual size_t dumpInfo(bool bLog = true, bool bDeTail = false,PStr* outInfoString = nullptr) {
		std::tstring strAll;
#define dumpInfoMax 1000
		Char tem[dumpInfoMax];
		if (bLog || outInfoString) {
			if (bDeTail)
				_stprintf_s(tem, dumpInfoMax,_T("\r\n>>The MemmeryPool(%s, Type= %s) Dumper Detail----------------------------------- \r\n    All blocks information:\r\n"), getInfo().structType, 
					(lpTypeName ? lpTypeName:_T("#UnkownTypeName")));
			else
				_stprintf_s(tem, dumpInfoMax,_T("\r\n>>The MemmeryPool(%s, Type= %s) Dumper Simple----------------------------------- \r\n"),getInfo().structType, 
					(lpTypeName ? lpTypeName : _T("#UnkownTypeName")));
			strAll += tem;
		}
		size_t mem = sizeof(container_type);
		size_t si = 0;
		size_t siu = 0;
		size_t sit = 0;
		lockShared();
		for (const block_type* pc = pEntry; pc; )
		{
			mem += pc->getMemSize();
			si++;
			siu += pc->nMaxUnusable;
			sit += pc->nMaxDataCounts;
			size_t n = pc->nMaxDataCounts == 0 ? 0 : (pc->nMaxUnusable * 50) / pc->nMaxDataCounts;
			if ((bLog || outInfoString) && bDeTail) {
				_stprintf_s(tem, dumpInfoMax,_T("    %zd. block : ["), si); strAll += tem;
				for (size_t i = 0; i < 50; ++i)
				{
					if (i < n)_stprintf_s(tem, dumpInfoMax,_T("*"));
					else _stprintf_s(tem, dumpInfoMax,_T("-"));
					strAll += tem;
				}
				_stprintf_s(tem, dumpInfoMax,_T("] <used=%zd/%zd, %zd%%>\r\n"), pc->nMaxUnusable, pc->nMaxDataCounts, n * 2);
				strAll += tem;
			}
			pc = pc->pNext;
		}
		unlockShared();
		if (bLog || outInfoString) {
			_stprintf_s(tem, dumpInfoMax, _T(">>Summary: mem=%s, blocks=%zd, total=%zd, used=%zd.\r\n\r\n"), container_type::memorySizeString(mem).c_str(), si, sit, siu);
			strAll += tem;
		}
		if (bLog)
			_tprintf_s(_T("%s"),strAll.c_str());
		if (outInfoString) {
			size_t nn = strAll.length() + 1;
			*outInfoString = new Char[nn];
			_tcscpy_s(*outInfoString, nn, strAll.c_str());
		}
		return mem;
	}
	//返回内存使用量bytes
	size_t getMemSize() const { return dumpInfo(false, false, nullptr); }
	//返回内存池各种信息，
	virtual MenPoolInfo getInfo() const {
		return std::move(MenPoolInfo{ "single" , getMemPoolTypeName() });
	};

private:
	PCStr lpTypeName;
	size_t m_PerBlockMemCapacity;
	block_type* pEntry;
	block_type* pCurrentUsingBlock;	

	//检索列表取得可用的块，没有就创建
	block_type* getAvailableBlock() {
		if (pCurrentUsingBlock == nullptr) {
			if (pEntry == nullptr) {
				pCurrentUsingBlock = pEntry = new block_type(m_PerBlockMemCapacity, this);
			}
			else pCurrentUsingBlock = pEntry;
		}
	
#define Insert_Block_Micro(pEnd,pInsert) \
		{\
			if ((pEnd)->pNext)\
				(pEnd)->pNext->pPre = (pInsert);\
			(pInsert)->pNext = (pEnd)->pNext;\
			(pInsert)->pPre = (pEnd);\
			(pEnd)->pNext = (pInsert);\
		}
		//检索可用的块
		block_type* pStartBlock = pCurrentUsingBlock;
		for (; pCurrentUsingBlock->isUsable() == false;)
		{
			if (pCurrentUsingBlock->pNext == nullptr) { //到了末尾
				if (pCurrentUsingBlock != pEntry) { //不是头节点，就跳到头节点
					if (pStartBlock == pEntry) {
						pStartBlock = new block_type(m_PerBlockMemCapacity, this);
						Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
						pCurrentUsingBlock = pStartBlock;
					}
					else pCurrentUsingBlock = pEntry;
				}
				else { //是头节点，则只有介个块，增加块					
					pStartBlock = new block_type(m_PerBlockMemCapacity, this);
					Insert_Block_Micro(pEntry, pStartBlock);
					pCurrentUsingBlock = pStartBlock;
				}
			}
			else if (pCurrentUsingBlock->pNext == pStartBlock) {//绕了一圈了
				pStartBlock = new block_type(m_PerBlockMemCapacity, this);
				Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
				pCurrentUsingBlock = pStartBlock;
			}
			else pCurrentUsingBlock = pCurrentUsingBlock->pNext;
		}
		return pCurrentUsingBlock;
	}
public:
	//取得类型对应的内存池对象
	static CLMemPool* getMenPool() {
		static CLMemPool _MenPool;
		return &_MenPool;
	}
	static CLMemPool* getMenPool(PCStr typeName) {
		CLMemPool* pc = getMenPool();
		if (pc->lpTypeName == nullptr && typeName != nullptr) {
			pc->setMemPoolTypeName(typeName);
		}
		return pc;
	}
	//向内存池动态申请一个对象，返回对象指针。对应的，该指针必须用deleteObj释放。也可用giveUpObj放弃使用权（该方式不执行对象析构）。
	//注意：若申请时的当前分配对象已构造过（比如：是由giveUpObj()放弃使用权而回收的对象），则忽略本次构造传参。
	template <typename... Args>
	static value_type* newObjFromMemPool(PCStr name = nullptr, Args&&... args) {
		return (value_type*)getMenPool(name)->allocate(std::forward<Args>(args)...);
	}	
	//设置内存池中内存块当个最大内存占用大小，单位MB
	static void setMemPoolBlockCapacity(size_t mb = 1) {
		assert(mb != 0);
		getMenPool()->setPerBlockMemCapacity(mb);
	}
};

//整理所有内存池
inline void arrangeAllMemPools()
{
	getMemPoolsTable()->lock();
	for (auto& i : *getMemPoolsTable())
		((CLMemPool<void*>*)i.first)->arrangement();
	getMemPoolsTable()->unlock();
}
//输出内存池表所有对象
inline size_t dumpMemPoolsTable(bool bLog = true, bool bDeTail = false,  PStr* outInfoString = nullptr)
{
	size_t mem = 0;
	PStr lpT = 0;
	std::tstring str, strh;
	if (bLog || outInfoString) {
		if (bDeTail)strh = _T("\r\n>>>>The MemmeryPool Table Dumper Detail----------------------------------- \r\n");
		else strh = _T("\r\n>>>>The MemmeryPool Table Dumper Simple----------------------------------- \r\n");
	}
	size_t ic = 1;
	Char cts[BUFSIZE];
	getMemPoolsTable()->lock();
	for (auto i = getMemPoolsTable()->cbegin(); i != getMemPoolsTable()->cend(); ic++)
	{
		auto* pobj = ((CLMemPool<void*>*)i->first);
		size_t memc = (pobj->dumpInfo(false, bDeTail, ((bLog || outInfoString) ? &lpT : nullptr)));
		mem += memc;
		if (bLog || outInfoString) {
			if (pobj->getMemPoolTypeName())
				_stprintf_s(cts, _T("  >>MemPool(%zd), %s, type= %s, mem=%s \r\n"), ic
					, pobj->getInfo().structType, i->second.c_str(), CLMemPool<void*>::memorySizeString(memc).c_str());
			else
				_stprintf_s(cts, _T("  >>MemPool(%zd), %s, %s, mem=%s \r\n"), ic, pobj->getInfo().structType, _T("#UnkownTypeName"),
					CLMemPool<void*>::memorySizeString(memc).c_str());
			strh += cts;
		}
		if (bLog || outInfoString) {
			if (lpT) {
				str += lpT;
				delete[] lpT;
				lpT = 0;
			}
		}
		++i;
	}
	if (bLog || outInfoString) {
		_stprintf_s(cts, _T(">>>>Total Mem =%s \r\n\r\n>>>>Per MemmeryPool Information: \r\n"), CLMemPool<void*>::memorySizeString(mem).c_str());
		strh += cts;
		strh += str;
	}
	if (bLog)
		_tprintf_s(_T("%s"), strh.c_str());
	getMemPoolsTable()->unlock();
	if (outInfoString) {
		*outInfoString = new Char[strh.length() + 1];
		_tcscpy_s(*outInfoString, strh.length() + 1, strh.c_str());
	}
	return mem;
}

//连续内存池实现
#ifndef __CL_MEMPOOLEX_HPP__
#define __CL_MEMPOOLEX_HPP__

#include <cassert>
#include <map>
#include <unordered_map>
#include <string>
#include <stdexcept>

template <typename value_type>
class CLMemPoolBlockEx;

template <typename value_type>
class CLMemPoolEx;

//内存池对象单元模板类
template <typename value_type>
class CLMemPoolUnitEx {
	typedef CLMemPoolUnitEx<value_type> unit_type;
	typedef CLMemPoolBlockEx<value_type> block_type;
	typedef CLMemPoolEx<value_type> container_type;
	friend class block_type;
	friend class container_type;
public:
	struct DataHead {
		unit_type* pPre;
		unit_type* pNext;
		block_type* pThisBlock;
		size_t utUsedCounts;
	};
	struct UnitPack {
		value_type v;
		UnitPack() {};
		template <typename... Args>
		UnitPack(Args&&... args) 
			:v(std::forward<Args>(args)...) {}
		~UnitPack() {};
	};
private:
	DataHead hdr;
	UnitPack* data;;//包裹数据对象,结构如：{this,UnitPack ...}
	char* const buffer;;//包裹数据对象,结构如：{this,UnitPack ...}	

	//在构造时候显示调用
	unit_type* init(
		block_type* _pThisBlock = 0,
		unit_type* _pPre = 0,
		unit_type* _pNext = 0,
		size_t _isUsed = 0
	) {
		hdr.pThisBlock = _pThisBlock;
		hdr.pPre = _pPre;
		hdr.pNext = _pNext;
		hdr.utUsedCounts = _isUsed;
		return this;
	}
	//注意：构造和析构函数用于显示的调用来构造和析构内部包裹对象
	//他们不应该被系统调用，且内部不能有任何的数据处理过程；
	CLMemPoolUnitEx(size_t utStroeSize = 1):
		buffer((char*)::malloc(sizeof(unit_type*) + sizeof(UnitPack)* utStroeSize))//构造带有自身指针的缓冲区
	{
		*((unit_type**)buffer) = this; //在缓冲区头部保存自身的单元信息指针
		data = ((UnitPack*)(buffer + sizeof(unit_type*)));
		init();
	};
	~CLMemPoolUnitEx() {
		::free(buffer);
	}
	//函数增补构造直到完成_createdCounts个构造,对于已构造的对象不在重复构造
	template <typename... Args>
	value_type* construct(size_t _createdCounts, Args&&... args) {
		for (size_t i = hdr.utUsedCounts; i < _createdCounts; i++)
		{
			data[i].UnitPack::UnitPack(std::forward<Args>(args)...);
		}
		if (_createdCounts > hdr.utUsedCounts)
			hdr.utUsedCounts = _createdCounts;
		return &data->v;
	}
	void destroy() {
		for (size_t i = 0; i < hdr.utUsedCounts; i++)
		{
			data[i].UnitPack::~UnitPack();
		}
		hdr.utUsedCounts = 0;
	}
public:
	inline block_type* getOwnerBlock() { return hdr.pThisBlock; }
	inline bool isCreated()const { return hdr.utUsedCounts ? true : false; }
	inline static size_t getMemSize(size_t storeCounts) { return sizeof(unit_type) + storeCounts * sizeof(UnitPack); }
};

//内存池内存块对象模板类
template <typename value_type>
class CLMemPoolBlockEx {
	typedef CLMemPoolUnitEx<value_type> unit_type;
	typedef CLMemPoolBlockEx<value_type> block_type;
	typedef CLMemPoolEx<value_type> container_type;
	friend class unit_type;
	friend class container_type;

private:
	explicit CLMemPoolBlockEx(size_t utMaxStoreCounts, size_t PerBlockMemCapacity_MB, container_type* _pOwnerMemPool)
		:pOwnerMemPool(_pOwnerMemPool), utStroeCounts(utMaxStoreCounts)
	{
		if (utStroeCounts == 0)
			throw std::runtime_error("CLMemPoolBlockEx utStroeCounts is 0!");
		init();
		alloc(PerBlockMemCapacity_MB * 1024 * 1024 / unit_type::getMemSize(getMaxUnitStoreCounts()));
	}
	~CLMemPoolBlockEx() {
		releaseObjMenBlock();
	}
	//判断该内存块是否存在可用单元
	inline bool isUsable() {
		return pUsableLst ? true : false;
	}
	//将对象指针回收进入内存块单元，如果对象是已经构造过的就调用标准析构。
	unit_type* freeOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//执行对象析构
		if (pUnit->isCreated()) {
			pUnit->destroy();
			nHasUsedUnitCounts--;
		}
		return putToUsable(pUnit);
	}
	//将对象指针回收进入内存块单元,而不调用析构
	unit_type* giveUpOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//不执行对象析构，直接放入未用连条
		return putToUsable(pUnit);
	}
	//取出一个待命可用的对象单元，返回其构造后的对象指针。注意：若申请时的当前分配对象已构造，则忽略构造传参。
	template <typename... Args>
	value_type* getOneData(size_t _createdCounts, Args&&... args) {
		assert(pUsableLst != nullptr);
		if (pUsableLst->hdr.utUsedCounts < _createdCounts) {
			//执行对象默认构造，这会构造内部包裹的数据类
			pUsableLst->construct(_createdCounts, std::forward<Args>(args)...);
			nHasUsedUnitCounts++;
		}
		return putToUnusable(pUsableLst);
	}
	void init() {
		pUsableLst = 0;
		nMaxUsable = 0;
		pUnusableLst = 0;
		nMaxUnusable = 0;
		nHasUsedUnitCounts = 0;
		pPre = 0;
		pNext = 0;
		pMainDataLst = 0;
		nMaxDataCounts = 0;
	}
	//分配内存,但不会构造目标对象
	void alloc(size_t unitCounts = 1) {
		assert(pMainDataLst == 0 && pUsableLst == 0 && pUnusableLst == 0);
		if (pMainDataLst == 0) {
			//第一次申请内存空间
			pUsableLst = pMainDataLst = (unit_type*)malloc(sizeof(unit_type) * (nMaxDataCounts = nMaxUsable = max(1, unitCounts)));
			//第一次执行可用队列的初始化连接工作
			pMainDataLst[0].unit_type::CLMemPoolUnitEx(getMaxUnitStoreCounts());
			pMainDataLst[0].init(this, 0, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[1]);
			for (size_t i = 1; i < nMaxDataCounts - 1; ++i) {
				pMainDataLst[i].unit_type::CLMemPoolUnitEx(getMaxUnitStoreCounts());
				pMainDataLst[i].init(this, &pMainDataLst[i - 1], &pMainDataLst[i + 1]);
			}
			pMainDataLst[nMaxDataCounts - 1].unit_type::CLMemPoolUnitEx(getMaxUnitStoreCounts());
			pMainDataLst[nMaxDataCounts - 1].init(this, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[nMaxDataCounts - 2], 0);
		}
	};
	//析构所有已经构造过但没有使用的对象,即在可用联内的被放弃使用但是已经构造过的对象。
	void distructCreatedUsable() {
		if (nHasUsedUnitCounts == 0)
			return;
		//析构所有在可用连中的已构造
		for (unit_type* pc = pUsableLst; pc != nullptr; ) {
			if (pc->isCreated()) {
				pc->destroy();
				nHasUsedUnitCounts--;
			}
			pc = pc->hdr.pNext;
		}
	}
	//释放内存
	void releaseObjMenBlock() {
		if (pMainDataLst) {
			distructCreatedUsable();
			//析构所有已使用的对象
			for (unit_type* pc = pUnusableLst; pc != nullptr; ) {
				pUnusableLst = freeOneData(pc);
				pc = pUnusableLst;
			}
			for (unit_type* pc = pUsableLst; pc != nullptr; ) {
				pc->~unit_type();
				pc = pc->hdr.pNext;
			}
			//释放动态内存
			free(pMainDataLst);
			pMainDataLst = 0;
		}
	}
	//处理前后连接,隔离对象,返回原对象指针
#define _extruct_pUnitBlock(pUnit) \
		((((pUnit)->pPre) ? ((pUnit)->pPre->pNext = (pUnit)->pNext) : 0),(((pUnit)->pNext)?((pUnit)->pNext->pPre = (pUnit)->pPre):0),(pUnit))
#define _extruct_pUnit(pUnit) \
		((((pUnit)->hdr.pPre) ? ((pUnit)->hdr.pPre->hdr.pNext = (pUnit)->hdr.pNext) : 0),(((pUnit)->hdr.pNext)?((pUnit)->hdr.pNext->hdr.pPre = (pUnit)->hdr.pPre):0),(pUnit))

	//对象放入可用队列头，返回不可用队列头指针
	unit_type* putToUsable(unit_type* pUnit) {
		assert(this == pUnit->getOwnerBlock());
		//处理不可用列表头
		if (pUnusableLst && pUnusableLst == pUnit)
			pUnusableLst = pUnit->hdr.pNext;

		//处理前后连接,隔离对象
		_extruct_pUnit(pUnit);

		//接入可用列表
		if (pUsableLst) { pUsableLst->hdr.pPre = pUnit; }
		pUnit->hdr.pNext = pUsableLst;
		pUnit->hdr.pPre = 0;
		pUsableLst = pUnit;
		nMaxUnusable -= 1;
		nMaxUsable += 1;
		return pUnusableLst;
	}

	//返回可用对象指针
	value_type* putToUnusable(unit_type* pUnit) {
		assert(this == pUnit->getOwnerBlock());
		//处理可用列表头
		if (pUsableLst && pUsableLst == pUnit)
			pUsableLst = pUnit->hdr.pNext;

		//处理前后连接,隔离对象
		_extruct_pUnit(pUnit);

		//接入不可用列表
		if (pUnusableLst) { pUnusableLst->hdr.pPre = pUnit; }
		pUnit->hdr.pNext = pUnusableLst;
		pUnit->hdr.pPre = 0;
		pUnusableLst = pUnit;
		nMaxUnusable += 1;
		nMaxUsable -= 1;
		return &pUnit->data->v;
	}
	container_type* const pOwnerMemPool; //所属内存池
	unit_type* pMainDataLst;//内存队列
	size_t nMaxDataCounts;
	unit_type* pUsableLst;//可用列表
	size_t nMaxUsable;
	unit_type* pUnusableLst;//不可用列表
	size_t nMaxUnusable;
	size_t nHasUsedUnitCounts;
	block_type* pPre;
	block_type* pNext;
	const size_t utStroeCounts;
public:
	inline container_type* getOwnerMemPool() {
		return pOwnerMemPool;
	}
	inline size_t getMaxUnitStoreCounts()const {
		return utStroeCounts;
	}
	size_t getMemSize() const {
		return sizeof(block_type) + nMaxDataCounts * unit_type::getMemSize(getMaxUnitStoreCounts());
	}
};

//连续内存池对象模板类。
 //请使用静态的getMemPool()方法获取唯一的内存池对象。
template <typename value_type>
class CLMemPoolEx :public IMenPool, public CLMemPoolLock {
	typedef CLMemPoolUnitEx<value_type> unit_type;
	typedef CLMemPoolBlockEx<value_type> block_type;
	typedef CLMemPoolEx<value_type> container_type;
	friend class unit_type;
	friend class block_type;
private:
	//构造带名称标识的目标类型内存池，加入内存池表。请勿显示调用该构造方法。
	//请使用静态的getMemPool()方法获取唯一的内存池对象。
	CLMemPoolEx(PCStr _lpTypeName, size_t PerBlockMemCapacity_MB = 1)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(_lpTypeName);
	}
	//构造目标类型内存池，加入内存池表。请勿显示调用该构造方法。
	//请使用静态的getMemPool()方法获取唯一的内存池对象。	
	CLMemPoolEx(size_t PerBlockMemCapacity_MB = 1)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(nullptr);
	}
public:	
	virtual ~CLMemPoolEx() {
		//不要释放任何内存，而是做一次整理
		arrangement();
		getMemPoolsTable()->lock();
		for (auto  i = ::getMemPoolsTable()->find((void*)this);
			i != ::getMemPoolsTable()->end(); ) {
			::getMemPoolsTable()->erase(i);
			break;
		}
		getMemPoolsTable()->unlock();
	}
	void setMemPoolTypeName(PCStr _lpTypeName) {
		lpTypeName = _lpTypeName;
		::addMemPoolToTable(lpTypeName, (void*)this);//构造之后加入队列
	}
	PCStr getMemPoolTypeName()const {
		return lpTypeName;
	}
	//整理内存，释放当前没有使用的空间归还系统。该函数用于内存池反复使用后占用较大资源后做一次统一释放。
	//distructCreatedButNoUsed = true 表示析构未被使用但已构造的对象
	virtual void arrangement(bool distructCreatedButNoUsed = true) {
		lock();
		for (auto i = dataSets.begin(); i != dataSets.end();) {
			auto& pEntry = i->second.pEntry;
			auto& pCurrentUsingBlock = i->second.pCurrentUsingBlock;
			pCurrentUsingBlock = nullptr;	//	置0
			for (block_type* pi = pEntry; pi != nullptr; )
			{
				block_type* pib = pi->pNext;
				if (distructCreatedButNoUsed)//是否析构未用对象
					pi->distructCreatedUsable();
				if (pi->nMaxUnusable == 0 && pi->nHasUsedUnitCounts == 0) { //释放条件，没有被使用,没有已构造
					if (pEntry == pi)
						pEntry = pi->pNext;
					delete _extruct_pUnitBlock(pi);
				}
				pi = pib;
			}
			if (pEntry == nullptr)
				i = dataSets.erase(i);
			else
				++i;
		}
		unlock();
	}
	//强行释放所有内存。
	virtual void releaseObjMenPool() {
		lock();
		for (auto& i : dataSets)
		{
			i.second.pCurrentUsingBlock = nullptr;
			for (block_type* pi = i.second.pEntry; pi != nullptr; )
			{
				auto pbk = pi->pNext;
				delete pi; // 释放块
				pi = pbk;
			}
			i.second.pEntry = nullptr;
		}
		m_PerBlockMemCapacity = 0;
		unlock();
	}
	//向内存池储备内存动态申请一个对象，若对象是已经构造过，就返回指针，如果未构造，先构造并返回其对象指针。
	//该方法接受可变参数构造方式传参。
	//注意：若申请时的当前分配对象已构造过（比如：是由giveUpObj()放弃使用权而回收的对象），则忽略本次构造传参。
	template <typename... Args>
	value_type* allocate(size_t _counts, Args&&... args) {
		lock();
		value_type* rt = getAvailableBlock(_counts)->getOneData(_counts, std::forward<Args>(args)...);
		unlock();
		return rt;
	}

#define MUHEARDEREX( _ty, pData )  (*((CLMemPoolUnitEx<_ty>**)(((char*)(pData))-sizeof(CLMemPoolUnitEx<_ty>*))))

	//向内存池释放一个动态申请的对象，进行相关析构操作。
	virtual void deallocate(void* pDelete) {
		unit_type* pUnit = MUHEARDEREX(value_type, pDelete);
		if (!checkPtrValidity(pUnit)
			)return;//指针非法即返回或抛异常
		lock();
		pUnit->getOwnerBlock()->freeOneData(pUnit);
		unlock();
	}
	//向内存池释放一个动态申请的对象，不析构。
	virtual void giveUp(void* pDelete) {
		unit_type* pUnit = MUHEARDEREX(value_type, pDelete);
		if (!checkPtrValidity(pUnit))
			return;//指针非法即返回或抛异常
		lock();
		pUnit->getOwnerBlock()->giveUpOneData(pUnit);
		unlock();
	}
	//判断指针是否属于本内存池构造
	bool checkPtrValidity(void* _pUnit) {
		unit_type*& pUnit = (unit_type*&)_pUnit;
		if (pUnit->getOwnerBlock()->getOwnerMemPool() != this)
			throw std::runtime_error("Tag obj is not created by this mem pool!");
		if (!pUnit->isCreated()) {
#ifdef _DEBUG
			if (pUnit->isCreated())
				throw std::runtime_error("Tag obj has been give up!");
			else
				throw std::runtime_error("Tag obj has been distructed!");
#else
			return false;
#endif
		}
		return true;
	}
	//设置单个内存块的大小，单位MB，默认5MB
	virtual void setPerBlockMemCapacity(size_t PerBlockMemCapacity_MB = 1) {
		lock();
		m_PerBlockMemCapacity = PerBlockMemCapacity_MB == 0 ? 1 : PerBlockMemCapacity_MB;
		unlock();
	}
	//统计内存使用量，并控制向控台输出当前内存池的内存使用情况。
	//bLog = true表示输出信息到控制台否则只统计内存使用量，bDeTail = false表示采用简化输出,outInfoString动态信息字符串内容需要手动释放
	virtual size_t dumpInfo(bool bLog = true, bool bDeTail = false, PStr* outInfoString = nullptr) {
		std::tstring strAll;
#define dumpInfoMaxEx 10000
		Char tem[dumpInfoMaxEx];
		if (bLog || outInfoString) {
			if (bDeTail)
				_stprintf_s(tem, dumpInfoMaxEx, _T("\r\n>>The MemmeryPool(%s, Type= %s) Dumper Detail----------------------------------- \r\n    All blocks information:\r\n"),
					getInfo().structType, (lpTypeName ? lpTypeName : _T("#UnkownTypeName")));
			else
				_stprintf_s(tem, dumpInfoMaxEx, _T("\r\n>>The MemmeryPool(%s, Type= %s) Dumper Simple----------------------------------- \r\n"),
					getInfo().structType, (lpTypeName ? lpTypeName : _T("#UnkownTypeName")));
			strAll += tem;
		}
		size_t si = 0;
		size_t siu = 0;
		size_t sit = 0;
		size_t mem = sizeof(container_type);
		size_t si2 = 0;
		lockShared();
		for (auto& ite : dataSets)
		{
			auto& pEntry = ite.second.pEntry;
			auto id = ite.first;
			
			for (const block_type* pc = pEntry; pc; )
			{
				mem += pc->getMemSize();
				si++;
				si2++;
				siu += pc->nMaxUnusable;
				sit += pc->nMaxDataCounts;
				size_t n = pc->nMaxDataCounts == 0 ? 0 : (pc->nMaxUnusable * 50) / pc->nMaxDataCounts;
				if ((bLog || outInfoString) && bDeTail) {
					_stprintf_s(tem, dumpInfoMaxEx, _T("    %zd. block_%zd : ["), si2,id); strAll += tem;
					for (size_t i = 0; i < 50; ++i)
					{
						if (i < n)_stprintf_s(tem, dumpInfoMaxEx, _T("*"));
						else _stprintf_s(tem, dumpInfoMaxEx, _T("-"));
						strAll += tem;
					}
					_stprintf_s(tem, dumpInfoMaxEx, _T("] <used=%zd/%zd, %zd%%>\r\n"), pc->nMaxUnusable, pc->nMaxDataCounts, n * 2);
					strAll += tem;
				}
				pc = pc->pNext;
			}
		}
		unlockShared();
		if (bLog || outInfoString) {			
			_stprintf_s(tem, dumpInfoMaxEx, _T(">>Summary: mem=%s, blocks=%zd, total=%zd, used=%zd.\r\n\r\n"), CLMemPool<void*>::memorySizeString(mem).c_str(), si, sit, siu);
			strAll += tem;
		}
		if (bLog)
			_tprintf_s(_T("%s"), strAll.c_str());
		if (outInfoString) {
			size_t nn = strAll.length() + 1;
			*outInfoString = new Char[nn];
			_tcscpy_s(*outInfoString, nn, strAll.c_str());
		}
		return mem;
	}
	//返回内存使用量bytes
	size_t getMemSize() const { return dumpInfo(false, false, nullptr); }
	//返回类型信息，
	virtual MenPoolInfo getInfo() const {
		return std::move(MenPoolInfo{ "continuity",getMemPoolTypeName()});
	};
private:
	PCStr lpTypeName;
	size_t m_PerBlockMemCapacity;
	struct BlockPack {
		block_type* pEntry = nullptr;
		block_type* pCurrentUsingBlock = nullptr;
	};
	std::unordered_map<size_t, BlockPack> dataSets;
	

	//检索列表取得可用的块，没有就创建
	block_type* getAvailableBlock(size_t _counts) {
		//模式一：直接求对数，该方式在低量级时候会比较慢
		//size_t id = _counts <= 4 ? _counts : pow(2, size_t(log2(_counts)) + 1);
		size_t id = 1;while ((id *= 2) < _counts);

		auto& it = dataSets[id];
		auto& pCurrentUsingBlock = it.pCurrentUsingBlock;
		auto& pEntry = it.pEntry;

		if (pCurrentUsingBlock == nullptr) {
			if (pEntry == nullptr) {
				pCurrentUsingBlock = pEntry = new block_type(id, m_PerBlockMemCapacity, this);
			}
			else pCurrentUsingBlock = pEntry;
		}
		block_type* pStartBlock = pCurrentUsingBlock;
		//检索可用的块
		for (; pCurrentUsingBlock->isUsable() == false;)
		{
			if (pCurrentUsingBlock->pNext == nullptr) {
				if (pCurrentUsingBlock != pEntry) {
					if (pStartBlock == pEntry) {
						pStartBlock = new block_type(id,m_PerBlockMemCapacity, this);
						Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
						pCurrentUsingBlock = pStartBlock;
					}
					else pCurrentUsingBlock = pEntry;
				}
				else {
					pStartBlock = new block_type(id, m_PerBlockMemCapacity, this);
					Insert_Block_Micro(pEntry, pStartBlock);
					pCurrentUsingBlock = pStartBlock;
				}
			}
			else if (pCurrentUsingBlock->pNext == pStartBlock) {//插入新块，在队列中
				pStartBlock = new block_type(id, m_PerBlockMemCapacity, this);
				Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
				pCurrentUsingBlock = pStartBlock;
			}
			else pCurrentUsingBlock = pCurrentUsingBlock->pNext;
		}
		return pCurrentUsingBlock;
	}
public:
	//取得类型对应的内存池对象
	static CLMemPoolEx* getMenPool() {
		static CLMemPoolEx _MenPool;
		return &_MenPool;
	}
	static CLMemPoolEx* getMenPool(PCStr typeName) {
		CLMemPoolEx* pc = getMenPool();
		if (pc->lpTypeName == nullptr && typeName != nullptr) {
			pc->setMemPoolTypeName(typeName);
		}
		return pc;
	}
	//向内存池动态申请一个对象，返回对象指针。对应的，该指针必须用deleteObj释放。也可用giveUpObj放弃使用权（该方式不执行对象析构）。
	//注意：若申请时的当前分配对象已构造过（比如：是由giveUpObj()放弃使用权而回收的对象），则忽略本次构造传参。
	template <typename... Args>
	static value_type* newObjFromMemPool(PCStr name, size_t _counts, Args&&... args) {
		return  _counts <= 1 ? (value_type*)CLMemPool<value_type>::getMenPool(name)->allocate(std::forward<Args>(args)...):
			(value_type*)getMenPool(name)->allocate(_counts, std::forward<Args>(args)...);
	}
	//设置内存池中内存块当个最大内存占用大小，单位MB
	static void setMemPoolBlockCapacity(size_t mb = 1) {
		assert(mb != 0);
		getMenPool()->setPerBlockMemCapacity(mb);
	}
};

#endif


//释放包括CLMemPool和CLMemPoolEx创建的单个或连续的对象，归还内存池，会做相关析构操作。
inline void deleteObjBackToMemPool(void* pDelete) {
	assert(pDelete != nullptr);
	MUHEARDER(char, pDelete)->getOwnerBlock()->getOwnerMemPool()->deallocate(pDelete);
}
//放弃包括CLMemPool和CLMemPoolEx创建的单个或连续的对象的操作权，放回内存池，但是不做析构操作。
inline void giveUpObjBackToCLMemPool(void* pDelete) {
	assert(pDelete != nullptr);
	MUHEARDER(char, pDelete)->getOwnerBlock()->getOwnerMemPool()->giveUp(pDelete);
}

#undef newObj
#undef newObjs
#undef newObjAndNamed
#undef newObjsAndNamed
#undef deleteObj
#undef deleteObjs
#undef giveUpObj
#undef giveUpObjs
//目的：构造保存typeName类型的内存池（如果未构造过的话），并返回连续的counts个typeName类型构造后的对象数组指针头；
//作用：他向内存池动态申请一个对象或对象数组(第一个参数是申请的类型，第二个参数连续分配对象的个数，第三个参数可以是可变数量的构造参数)，返回对象指针；
//方法：对应的，该方法返回的指针必须用deleteObj/deleteObjs释放，也可用giveUpObj/giveUpObjs放弃使用权（giveUpObj/giveUpObjs方式不执行对象析构,只是把对象原样放回内存池）；
//注意：该内存池采用预分配机制，可以高速的完成读写，当指定 counts > 1 可分配连续数量的对象（对象队列），当指定 counts = 1 只能分配单个对象，此时则函数退化为newObj()行为；
//注意：若申请时的当前分配对象已构造过（比如：是由giveUpObj()放弃使用权而回收的对象），则忽略本次构造传参；
//注意：分配单个对象的速度会略高于分配连续对象；
#define newObjs( typeName ,counts, ... ) (CLMemPoolEx<typeName>::newObjFromMemPool(_T(#typeName) ,(counts), __VA_ARGS__ ))
//构造保存typeName类型的内存池（如果未构造过的话），并返回单个typeName类型经过构造后的对象指针，分配的对象是单个不连续的对象；
//若要分配连续的对象数组需使用newObjs()，并且指定 counts > 1；
#define newObj( typeName , ... ) (CLMemPool<typeName>::newObjFromMemPool(_T(#typeName) , __VA_ARGS__ ))

//生成代标号的内存池和对象指针的宏；并且指定内存池名称（若名称未被指定过的话）
#define newObjAndNamed( typeName, NameString , ... ) (CLMemPool<typeName>::newObjFromMemPool(_T(#NameString), __VA_ARGS__ ))
//生成代标号的内存池和对象指针的宏；并且指定内存池名称（若名称未被指定过的话）
#define newObjsAndNamed( typeName, NameString ,counts, ... ) (CLMemPoolEx<typeName>::newObjFromMemPool(_T(#NameString),(counts), __VA_ARGS__ ))
//释放包括CLMemPool和CLMemPoolEx创建的单个或连续的对象指针，资源归还内存池，pObjToDelete是释放的对象指针，函数由内部指针多态实现不同的释放行为；
#define deleteObj( pObjToDelete ) (deleteObjBackToMemPool((pObjToDelete)))
//释放包括CLMemPool和CLMemPoolEx创建的单个或连续的对象指针，资源归还内存池，与deleteObj()方法等效，函数由内部指针多态实现不同的释放行为；
#define deleteObjs( pObjToDelete ) deleteObj( pObjToDelete )
//放弃包括CLMemPool和CLMemPoolEx创建的单个或连续的对象的使用权（不会析构对象），并放回内存池待用，pObjToDelete是释放的对象指针，函数由内部指针多态实现不同的放弃行为；
#define giveUpObj( pObjToDelete ) (giveUpObjBackToCLMemPool((pObjToDelete)))
//放弃包括CLMemPool和CLMemPoolEx创建的单个或连续的对象的使用权（不会析构对象），与giveUpObj()方法等效，函数由内部指针多态实现不同的放弃行为；
#define giveUpObjs( pObjToDelete ) giveUpObj( pObjToDelete )

#endif