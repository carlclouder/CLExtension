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
#define CLMP_USE_LOCK_TYPE 1 // 0=�������죩��1=��RWLock����д������ 2=��C++����3=��windows�ٽ�����
#endif

#if CLMP_USE_LOCK_TYPE == 3
#include "windows.h" //windowsƽ̨,�ٽ���
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
#include "../_cl_common/CLCommon.h" //�����Զ����д��
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

//��׼���ӿ�
struct IMenPool {
	virtual void arrangement(bool) = 0;
	virtual void deallocate(void*) = 0;
	virtual void giveUp(void*) = 0;
	struct MenPoolInfo {
		PCStr structType;//�ڴ�ؽṹ���ͣ�������������
		PCStr memPoolTypeName;//ָ����������		
	};
	virtual MenPoolInfo getInfo() const = 0;
};

#endif

template <typename value_type>
class CLMemPoolBlock;

template <typename value_type>
class CLMemPool;

////�ڴ�ر�����
class MemPoolsTable
	:public std::map<void*, std::tstring>,
	public CLMemPoolLock
{};
//ȡ���ڴ�ر�
inline MemPoolsTable* getMemPoolsTable() {
	static MemPoolsTable _mplst;
	return &_mplst;
}
////�����ڴ�ص��ڴ�ر�
inline void addMemPoolToTable(PCStr name,void* pMemPool)
{
	getMemPoolsTable()->lock();
	(*getMemPoolsTable())[pMemPool] = (name == nullptr ? _T("") : name);
	getMemPoolsTable()->unlock();
}

//�ڴ�ض���Ԫģ����
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
	value_type data;//�������ݶ���

	//�ڹ���ʱ����ʾ����
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
	//ע�⣺�������������������ʾ�ĵ���������������ڲ���������
	//���ǲ�Ӧ�ñ�ϵͳ���ã����ڲ��������κε����ݴ�����̣�
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

//�ڴ���ڴ�����ģ����
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
	//�жϸ��ڴ���Ƿ���ڿ��õ�Ԫ
	inline bool isUsable() {
		return pUsableLst ? true : false;
	}
	//������ָ����ս����ڴ�鵥Ԫ������������Ѿ�������ľ͵��ñ�׼������
	unit_type* freeOneData(unit_type* pUnit) {
		assert(pUnit !=nullptr);
		//ִ�ж�������
		if (pUnit->hdr.utUsedCounts) {
			pUnit->unit_type::~CLMemPoolUnit();
			pUnit->hdr.utUsedCounts = 0;
			nHasUsedUnitCounts--;
		}
		return putToUsable(pUnit);
	}
	//������ָ����ս����ڴ�鵥Ԫ,������������
	unit_type* giveUpOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//��ִ�ж���������ֱ�ӷ���δ������
		return putToUsable(pUnit);
	}
	//ȡ��һ���������õĶ���Ԫ�������乹���Ķ���ָ�롣ע�⣺������ʱ�ĵ�ǰ��������ѹ��죬����Թ��촫�Ρ�
	template <typename... Args>
	value_type* getOneData(Args&&... args) {
		assert(pUsableLst != nullptr);
		if(!pUsableLst->hdr.utUsedCounts) {
			//ִ�ж���Ĭ�Ϲ��죬��ṹ���ڲ�������������
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
	//�����ڴ�
	void alloc(size_t unitCounts = 1) {
		assert(pMainDataLst == 0 && pUsableLst == 0 && pUnusableLst == 0);
		if (pMainDataLst == 0) {
			//��һ�������ڴ�ռ�
			pMainDataLst = (unit_type *)malloc(sizeof(unit_type)*(nMaxDataCounts = nMaxUsable = (unitCounts == 0 ? 1 : unitCounts)));
			//��һ��ִ�п��ö��еĳ�ʼ�����ӹ���
			pUsableLst = &pMainDataLst[0];
			pMainDataLst[0].init(this, 0, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[1]);
			for (size_t i = 1; i < nMaxDataCounts-1; ++i) {
				pMainDataLst[i].init(this, &pMainDataLst[i-1], &pMainDataLst[i + 1]);
			}
			pMainDataLst[nMaxDataCounts - 1].init(this, nMaxDataCounts <= 1 ? 0 : &pMainDataLst[nMaxDataCounts - 2], 0);
		}
	};
	//���������Ѿ�������Ķ��󣬵�û��ʹ�õĶ���
	void distructCreatedUsable() {
		if (nHasUsedUnitCounts == 0)
			return;
		//���������ڿ������е��ѹ���
		for (unit_type* pc = pUsableLst; pc != nullptr; ) {
			if (pc->hdr.utUsedCounts) {
				pc->unit_type::~CLMemPoolUnit();
				pc->hdr.utUsedCounts = 0;
				nHasUsedUnitCounts--;
			}
			pc = pc->hdr.pNext;
		}
	}
	//�ͷ��ڴ�
	void releaseObjMenBlock() {
		if (pMainDataLst) {
			distructCreatedUsable();
			//����������ʹ�õĶ���
			for (unit_type* pc = pUnusableLst; pc != nullptr; ) {			
				pUnusableLst = freeOneData(pc);
				pc->init();
				pc = pUnusableLst;
			}
			//�ͷŶ�̬�ڴ�
			free(pMainDataLst);
			pMainDataLst = 0;
		}		
	}
	//����ǰ������,�������,����ԭ����ָ��
#define _extruct_pUnitBlock(pUnit) \
		((((pUnit)->pPre) ? ((pUnit)->pPre->pNext = (pUnit)->pNext) : 0),(((pUnit)->pNext)?((pUnit)->pNext->pPre = (pUnit)->pPre):0),(pUnit))
#define _extruct_pUnit(pUnit) \
		((((pUnit)->hdr.pPre) ? ((pUnit)->hdr.pPre->hdr.pNext = (pUnit)->hdr.pNext) : 0),(((pUnit)->hdr.pNext)?((pUnit)->hdr.pNext->hdr.pPre = (pUnit)->hdr.pPre):0),(pUnit))
	
	//���������ö���ͷ�����ز����ö���ͷָ��
	unit_type*  putToUsable(unit_type* pUnit) {
		assert(this == pUnit->hdr.pThisBlock);
		//���������б�ͷ
		if (pUnusableLst && pUnusableLst == pUnit)
			pUnusableLst = pUnit->hdr.pNext;

		//����ǰ������,�������
		_extruct_pUnit(pUnit);

		//��������б�
		if (pUsableLst) {pUsableLst->hdr.pPre = pUnit;}
		pUnit->hdr.pNext = pUsableLst;
		pUnit->hdr.pPre = 0;
		pUsableLst = pUnit;
		nMaxUnusable -= 1;
		nMaxUsable += 1;
		return pUnusableLst;
	}

	//���ؿ��ö���ָ��
	value_type*  putToUnusable(unit_type* pUnit) {
		assert(this == pUnit->hdr.pThisBlock);
		//��������б�ͷ
		if (pUsableLst && pUsableLst == pUnit)
			pUsableLst = pUnit->hdr.pNext;

		//����ǰ������,�������
		_extruct_pUnit(pUnit);
		
		//���벻�����б�
		if (pUnusableLst) {	pUnusableLst->hdr.pPre = pUnit;}
		pUnit->hdr.pNext = pUnusableLst;
		pUnit->hdr.pPre = 0;
		pUnusableLst = pUnit;
		nMaxUnusable += 1;
		nMaxUsable -= 1;
		return &pUnit->data;
	}
	container_type* const pOwnerMemPool; //�����ڴ��
	unit_type* pMainDataLst;//�ڴ����
	size_t nMaxDataCounts;
	unit_type* pUsableLst;//�����б�
	size_t nMaxUsable;
	unit_type* pUnusableLst;//�������б�
	size_t nMaxUnusable;
	size_t nHasUsedUnitCounts;
	CLMemPoolBlock<value_type>* pPre;
	CLMemPoolBlock<value_type>* pNext;
public:
	inline container_type* getOwnerMemPool() { return pOwnerMemPool;}
	inline size_t getMemSize() const { return sizeof(block_type) + nMaxDataCounts * unit_type::getMemSize(); }
};


//�ڴ�ض���ģ���ࡣ
 //��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���
template <typename value_type> 
class CLMemPool :public IMenPool,public CLMemPoolLock {
	typedef CLMemPoolUnit<value_type> unit_type;
	typedef CLMemPoolBlock<value_type> block_type;
	typedef CLMemPool<value_type> container_type;
	friend class unit_type;
	friend class block_type;
private:
	//��������Ʊ�ʶ��Ŀ�������ڴ�أ������ڴ�ر�������ʾ���øù��췽����
	//��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���
	CLMemPool(PCStr _lpTypeName, size_t PerBlockMemCapacity_MB = 5) :
		pEntry(nullptr),
		pCurrentUsingBlock(nullptr)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(_lpTypeName);
	}
	//����Ŀ�������ڴ�أ������ڴ�ر�������ʾ���øù��췽����
	//��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���	
	CLMemPool(size_t PerBlockMemCapacity_MB = 1) :
		pEntry(nullptr),
		pCurrentUsingBlock(nullptr)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(nullptr);
	}
public:	
	virtual ~CLMemPool() {
		//��Ҫ�ͷ��κ��ڴ棬������һ������
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
		::addMemPoolToTable(lpTypeName, (CLMemPool<void*>*)this);//����֮��������
	}
	PCStr getMemPoolTypeName()const {
		return lpTypeName;
	}
	//�����ڴ棬�ͷŵ�ǰû��ʹ�õĿռ�黹ϵͳ���ú��������ڴ�ط���ʹ�ú�ռ�ýϴ���Դ����һ��ͳһ�ͷš�
	//distructCreatedButNoUsed = true ��ʾ����δ��ʹ�õ��ѹ���Ķ���
	virtual void arrangement(bool distructCreatedButNoUsed = true) {
		lock();
		pCurrentUsingBlock = nullptr;	//	��0
		for (block_type* pi = pEntry; pi != nullptr; )
		{
			block_type* pib = pi->pNext;
			if (distructCreatedButNoUsed)//�Ƿ�����δ�ö���
				pi->distructCreatedUsable();
			if (pi->nMaxUnusable == 0 && pi->nHasUsedUnitCounts == 0) { //�ͷ�������û�б�ʹ��,û���ѹ���
				if (pEntry == pi)
					pEntry = pi->pNext;
				delete _extruct_pUnitBlock(pi);
			}
			pi = pib;
		}
		unlock();
	}
	//ǿ���ͷ������ڴ档
	virtual void releaseObjMenPool() {
		lock();
		for (block_type* pi = pEntry; pi != nullptr; )
		{
			pCurrentUsingBlock = pi->pNext;
			delete pi; // �ͷſ�
			pi = pCurrentUsingBlock;
		}
		m_PerBlockMemCapacity = 0;
		pEntry = 0;
		pCurrentUsingBlock = 0;
		unlock();
	}
	//���ڴ�ش����ڴ涯̬����һ���������������Ѿ���������ͷ���ָ�룬���δ���죬�ȹ��첢���������ָ�롣
	//�÷������ܿɱ�������췽ʽ���Ρ�
	//ע�⣺������ʱ�ĵ�ǰ��������ѹ���������磺����giveUpObj()����ʹ��Ȩ�����յĶ��󣩣�����Ա��ι��촫�Ρ�
	template <typename... Args>
	value_type* allocate(Args&&... args) {
		lock();		
		value_type* rt = getAvailableBlock()->getOneData(std::forward<Args>(args)...);
		unlock();
		return rt;
	}

#define MUHEARDER( _ty, pData )  (*((CLMemPoolUnit<_ty>**)(((char*)(pData))-(sizeof(CLMemPoolUnit<_ty>*)))))

	//���ڴ���ͷ�һ����̬����Ķ��󣬽����������������
	virtual void deallocate(void* pDelete) {
		unit_type* pUnit = MUHEARDER(value_type,pDelete);
		if(!checkPtrValidity(pUnit)
			)return;//ָ��Ƿ������ػ����쳣
		lock();
		pUnit->getOwnerBlock()->freeOneData(pUnit);
		unlock();
	}
	//���ڴ���ͷ�һ����̬����Ķ��󣬲�������
	virtual void giveUp(void* pDelete) {
		unit_type* pUnit = MUHEARDER(value_type, pDelete);
		if (!checkPtrValidity(pUnit))
			return;//ָ��Ƿ������ػ����쳣
		lock();
		pUnit->getOwnerBlock()->giveUpOneData(pUnit);
		unlock();
	}
	//�ж�ָ���Ƿ����ڱ��ڴ�ع���
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
	//���õ����ڴ��Ĵ�С����λMB��Ĭ��5MB
	virtual void setPerBlockMemCapacity(size_t PerBlockMemCapacity_MB = 1) {
		lock();
		m_PerBlockMemCapacity = PerBlockMemCapacity_MB == 0 ? 1 : PerBlockMemCapacity_MB;
		unlock();
	}
	//�ڴ��Сת�����ַ���
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
	//ͳ���ڴ�ʹ���������������̨�����ǰ�ڴ�ص��ڴ�ʹ�������
	//bLog = true��ʾ�����Ϣ������̨����ֻͳ���ڴ�ʹ������bDeTail = false��ʾ���ü����,outInfoString��̬��Ϣ�ַ���������Ҫ�ֶ��ͷ�
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
	//�����ڴ�ʹ����bytes
	size_t getMemSize() const { return dumpInfo(false, false, nullptr); }
	//�����ڴ�ظ�����Ϣ��
	virtual MenPoolInfo getInfo() const {
		return std::move(MenPoolInfo{ "single" , getMemPoolTypeName() });
	};

private:
	PCStr lpTypeName;
	size_t m_PerBlockMemCapacity;
	block_type* pEntry;
	block_type* pCurrentUsingBlock;	

	//�����б�ȡ�ÿ��õĿ飬û�оʹ���
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
		//�������õĿ�
		block_type* pStartBlock = pCurrentUsingBlock;
		for (; pCurrentUsingBlock->isUsable() == false;)
		{
			if (pCurrentUsingBlock->pNext == nullptr) { //����ĩβ
				if (pCurrentUsingBlock != pEntry) { //����ͷ�ڵ㣬������ͷ�ڵ�
					if (pStartBlock == pEntry) {
						pStartBlock = new block_type(m_PerBlockMemCapacity, this);
						Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
						pCurrentUsingBlock = pStartBlock;
					}
					else pCurrentUsingBlock = pEntry;
				}
				else { //��ͷ�ڵ㣬��ֻ�н���飬���ӿ�					
					pStartBlock = new block_type(m_PerBlockMemCapacity, this);
					Insert_Block_Micro(pEntry, pStartBlock);
					pCurrentUsingBlock = pStartBlock;
				}
			}
			else if (pCurrentUsingBlock->pNext == pStartBlock) {//����һȦ��
				pStartBlock = new block_type(m_PerBlockMemCapacity, this);
				Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
				pCurrentUsingBlock = pStartBlock;
			}
			else pCurrentUsingBlock = pCurrentUsingBlock->pNext;
		}
		return pCurrentUsingBlock;
	}
public:
	//ȡ�����Ͷ�Ӧ���ڴ�ض���
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
	//���ڴ�ض�̬����һ�����󣬷��ض���ָ�롣��Ӧ�ģ���ָ�������deleteObj�ͷš�Ҳ����giveUpObj����ʹ��Ȩ���÷�ʽ��ִ�ж�����������
	//ע�⣺������ʱ�ĵ�ǰ��������ѹ���������磺����giveUpObj()����ʹ��Ȩ�����յĶ��󣩣�����Ա��ι��촫�Ρ�
	template <typename... Args>
	static value_type* newObjFromMemPool(PCStr name = nullptr, Args&&... args) {
		return (value_type*)getMenPool(name)->allocate(std::forward<Args>(args)...);
	}	
	//�����ڴ�����ڴ�鵱������ڴ�ռ�ô�С����λMB
	static void setMemPoolBlockCapacity(size_t mb = 1) {
		assert(mb != 0);
		getMenPool()->setPerBlockMemCapacity(mb);
	}
};

//���������ڴ��
inline void arrangeAllMemPools()
{
	getMemPoolsTable()->lock();
	for (auto& i : *getMemPoolsTable())
		((CLMemPool<void*>*)i.first)->arrangement();
	getMemPoolsTable()->unlock();
}
//����ڴ�ر����ж���
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

//�����ڴ��ʵ��
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

//�ڴ�ض���Ԫģ����
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
	UnitPack* data;;//�������ݶ���,�ṹ�磺{this,UnitPack ...}
	char* const buffer;;//�������ݶ���,�ṹ�磺{this,UnitPack ...}	

	//�ڹ���ʱ����ʾ����
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
	//ע�⣺�������������������ʾ�ĵ���������������ڲ���������
	//���ǲ�Ӧ�ñ�ϵͳ���ã����ڲ��������κε����ݴ�����̣�
	CLMemPoolUnitEx(size_t utStroeSize = 1):
		buffer((char*)::malloc(sizeof(unit_type*) + sizeof(UnitPack)* utStroeSize))//�����������ָ��Ļ�����
	{
		*((unit_type**)buffer) = this; //�ڻ�����ͷ����������ĵ�Ԫ��Ϣָ��
		data = ((UnitPack*)(buffer + sizeof(unit_type*)));
		init();
	};
	~CLMemPoolUnitEx() {
		::free(buffer);
	}
	//������������ֱ�����_createdCounts������,�����ѹ���Ķ������ظ�����
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

//�ڴ���ڴ�����ģ����
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
	//�жϸ��ڴ���Ƿ���ڿ��õ�Ԫ
	inline bool isUsable() {
		return pUsableLst ? true : false;
	}
	//������ָ����ս����ڴ�鵥Ԫ������������Ѿ�������ľ͵��ñ�׼������
	unit_type* freeOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//ִ�ж�������
		if (pUnit->isCreated()) {
			pUnit->destroy();
			nHasUsedUnitCounts--;
		}
		return putToUsable(pUnit);
	}
	//������ָ����ս����ڴ�鵥Ԫ,������������
	unit_type* giveUpOneData(unit_type* pUnit) {
		assert(pUnit != nullptr);
		//��ִ�ж���������ֱ�ӷ���δ������
		return putToUsable(pUnit);
	}
	//ȡ��һ���������õĶ���Ԫ�������乹���Ķ���ָ�롣ע�⣺������ʱ�ĵ�ǰ��������ѹ��죬����Թ��촫�Ρ�
	template <typename... Args>
	value_type* getOneData(size_t _createdCounts, Args&&... args) {
		assert(pUsableLst != nullptr);
		if (pUsableLst->hdr.utUsedCounts < _createdCounts) {
			//ִ�ж���Ĭ�Ϲ��죬��ṹ���ڲ�������������
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
	//�����ڴ�,�����ṹ��Ŀ�����
	void alloc(size_t unitCounts = 1) {
		assert(pMainDataLst == 0 && pUsableLst == 0 && pUnusableLst == 0);
		if (pMainDataLst == 0) {
			//��һ�������ڴ�ռ�
			pUsableLst = pMainDataLst = (unit_type*)malloc(sizeof(unit_type) * (nMaxDataCounts = nMaxUsable = max(1, unitCounts)));
			//��һ��ִ�п��ö��еĳ�ʼ�����ӹ���
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
	//���������Ѿ��������û��ʹ�õĶ���,���ڿ������ڵı�����ʹ�õ����Ѿ�������Ķ���
	void distructCreatedUsable() {
		if (nHasUsedUnitCounts == 0)
			return;
		//���������ڿ������е��ѹ���
		for (unit_type* pc = pUsableLst; pc != nullptr; ) {
			if (pc->isCreated()) {
				pc->destroy();
				nHasUsedUnitCounts--;
			}
			pc = pc->hdr.pNext;
		}
	}
	//�ͷ��ڴ�
	void releaseObjMenBlock() {
		if (pMainDataLst) {
			distructCreatedUsable();
			//����������ʹ�õĶ���
			for (unit_type* pc = pUnusableLst; pc != nullptr; ) {
				pUnusableLst = freeOneData(pc);
				pc = pUnusableLst;
			}
			for (unit_type* pc = pUsableLst; pc != nullptr; ) {
				pc->~unit_type();
				pc = pc->hdr.pNext;
			}
			//�ͷŶ�̬�ڴ�
			free(pMainDataLst);
			pMainDataLst = 0;
		}
	}
	//����ǰ������,�������,����ԭ����ָ��
#define _extruct_pUnitBlock(pUnit) \
		((((pUnit)->pPre) ? ((pUnit)->pPre->pNext = (pUnit)->pNext) : 0),(((pUnit)->pNext)?((pUnit)->pNext->pPre = (pUnit)->pPre):0),(pUnit))
#define _extruct_pUnit(pUnit) \
		((((pUnit)->hdr.pPre) ? ((pUnit)->hdr.pPre->hdr.pNext = (pUnit)->hdr.pNext) : 0),(((pUnit)->hdr.pNext)?((pUnit)->hdr.pNext->hdr.pPre = (pUnit)->hdr.pPre):0),(pUnit))

	//���������ö���ͷ�����ز����ö���ͷָ��
	unit_type* putToUsable(unit_type* pUnit) {
		assert(this == pUnit->getOwnerBlock());
		//���������б�ͷ
		if (pUnusableLst && pUnusableLst == pUnit)
			pUnusableLst = pUnit->hdr.pNext;

		//����ǰ������,�������
		_extruct_pUnit(pUnit);

		//��������б�
		if (pUsableLst) { pUsableLst->hdr.pPre = pUnit; }
		pUnit->hdr.pNext = pUsableLst;
		pUnit->hdr.pPre = 0;
		pUsableLst = pUnit;
		nMaxUnusable -= 1;
		nMaxUsable += 1;
		return pUnusableLst;
	}

	//���ؿ��ö���ָ��
	value_type* putToUnusable(unit_type* pUnit) {
		assert(this == pUnit->getOwnerBlock());
		//��������б�ͷ
		if (pUsableLst && pUsableLst == pUnit)
			pUsableLst = pUnit->hdr.pNext;

		//����ǰ������,�������
		_extruct_pUnit(pUnit);

		//���벻�����б�
		if (pUnusableLst) { pUnusableLst->hdr.pPre = pUnit; }
		pUnit->hdr.pNext = pUnusableLst;
		pUnit->hdr.pPre = 0;
		pUnusableLst = pUnit;
		nMaxUnusable += 1;
		nMaxUsable -= 1;
		return &pUnit->data->v;
	}
	container_type* const pOwnerMemPool; //�����ڴ��
	unit_type* pMainDataLst;//�ڴ����
	size_t nMaxDataCounts;
	unit_type* pUsableLst;//�����б�
	size_t nMaxUsable;
	unit_type* pUnusableLst;//�������б�
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

//�����ڴ�ض���ģ���ࡣ
 //��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���
template <typename value_type>
class CLMemPoolEx :public IMenPool, public CLMemPoolLock {
	typedef CLMemPoolUnitEx<value_type> unit_type;
	typedef CLMemPoolBlockEx<value_type> block_type;
	typedef CLMemPoolEx<value_type> container_type;
	friend class unit_type;
	friend class block_type;
private:
	//��������Ʊ�ʶ��Ŀ�������ڴ�أ������ڴ�ر�������ʾ���øù��췽����
	//��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���
	CLMemPoolEx(PCStr _lpTypeName, size_t PerBlockMemCapacity_MB = 1)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(_lpTypeName);
	}
	//����Ŀ�������ڴ�أ������ڴ�ر�������ʾ���øù��췽����
	//��ʹ�þ�̬��getMemPool()������ȡΨһ���ڴ�ض���	
	CLMemPoolEx(size_t PerBlockMemCapacity_MB = 1)
	{
		setPerBlockMemCapacity(PerBlockMemCapacity_MB);
		setMemPoolTypeName(nullptr);
	}
public:	
	virtual ~CLMemPoolEx() {
		//��Ҫ�ͷ��κ��ڴ棬������һ������
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
		::addMemPoolToTable(lpTypeName, (void*)this);//����֮��������
	}
	PCStr getMemPoolTypeName()const {
		return lpTypeName;
	}
	//�����ڴ棬�ͷŵ�ǰû��ʹ�õĿռ�黹ϵͳ���ú��������ڴ�ط���ʹ�ú�ռ�ýϴ���Դ����һ��ͳһ�ͷš�
	//distructCreatedButNoUsed = true ��ʾ����δ��ʹ�õ��ѹ���Ķ���
	virtual void arrangement(bool distructCreatedButNoUsed = true) {
		lock();
		for (auto i = dataSets.begin(); i != dataSets.end();) {
			auto& pEntry = i->second.pEntry;
			auto& pCurrentUsingBlock = i->second.pCurrentUsingBlock;
			pCurrentUsingBlock = nullptr;	//	��0
			for (block_type* pi = pEntry; pi != nullptr; )
			{
				block_type* pib = pi->pNext;
				if (distructCreatedButNoUsed)//�Ƿ�����δ�ö���
					pi->distructCreatedUsable();
				if (pi->nMaxUnusable == 0 && pi->nHasUsedUnitCounts == 0) { //�ͷ�������û�б�ʹ��,û���ѹ���
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
	//ǿ���ͷ������ڴ档
	virtual void releaseObjMenPool() {
		lock();
		for (auto& i : dataSets)
		{
			i.second.pCurrentUsingBlock = nullptr;
			for (block_type* pi = i.second.pEntry; pi != nullptr; )
			{
				auto pbk = pi->pNext;
				delete pi; // �ͷſ�
				pi = pbk;
			}
			i.second.pEntry = nullptr;
		}
		m_PerBlockMemCapacity = 0;
		unlock();
	}
	//���ڴ�ش����ڴ涯̬����һ���������������Ѿ���������ͷ���ָ�룬���δ���죬�ȹ��첢���������ָ�롣
	//�÷������ܿɱ�������췽ʽ���Ρ�
	//ע�⣺������ʱ�ĵ�ǰ��������ѹ���������磺����giveUpObj()����ʹ��Ȩ�����յĶ��󣩣�����Ա��ι��촫�Ρ�
	template <typename... Args>
	value_type* allocate(size_t _counts, Args&&... args) {
		lock();
		value_type* rt = getAvailableBlock(_counts)->getOneData(_counts, std::forward<Args>(args)...);
		unlock();
		return rt;
	}

#define MUHEARDEREX( _ty, pData )  (*((CLMemPoolUnitEx<_ty>**)(((char*)(pData))-sizeof(CLMemPoolUnitEx<_ty>*))))

	//���ڴ���ͷ�һ����̬����Ķ��󣬽����������������
	virtual void deallocate(void* pDelete) {
		unit_type* pUnit = MUHEARDEREX(value_type, pDelete);
		if (!checkPtrValidity(pUnit)
			)return;//ָ��Ƿ������ػ����쳣
		lock();
		pUnit->getOwnerBlock()->freeOneData(pUnit);
		unlock();
	}
	//���ڴ���ͷ�һ����̬����Ķ��󣬲�������
	virtual void giveUp(void* pDelete) {
		unit_type* pUnit = MUHEARDEREX(value_type, pDelete);
		if (!checkPtrValidity(pUnit))
			return;//ָ��Ƿ������ػ����쳣
		lock();
		pUnit->getOwnerBlock()->giveUpOneData(pUnit);
		unlock();
	}
	//�ж�ָ���Ƿ����ڱ��ڴ�ع���
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
	//���õ����ڴ��Ĵ�С����λMB��Ĭ��5MB
	virtual void setPerBlockMemCapacity(size_t PerBlockMemCapacity_MB = 1) {
		lock();
		m_PerBlockMemCapacity = PerBlockMemCapacity_MB == 0 ? 1 : PerBlockMemCapacity_MB;
		unlock();
	}
	//ͳ���ڴ�ʹ���������������̨�����ǰ�ڴ�ص��ڴ�ʹ�������
	//bLog = true��ʾ�����Ϣ������̨����ֻͳ���ڴ�ʹ������bDeTail = false��ʾ���ü����,outInfoString��̬��Ϣ�ַ���������Ҫ�ֶ��ͷ�
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
	//�����ڴ�ʹ����bytes
	size_t getMemSize() const { return dumpInfo(false, false, nullptr); }
	//����������Ϣ��
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
	

	//�����б�ȡ�ÿ��õĿ飬û�оʹ���
	block_type* getAvailableBlock(size_t _counts) {
		//ģʽһ��ֱ����������÷�ʽ�ڵ�����ʱ���Ƚ���
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
		//�������õĿ�
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
			else if (pCurrentUsingBlock->pNext == pStartBlock) {//�����¿飬�ڶ�����
				pStartBlock = new block_type(id, m_PerBlockMemCapacity, this);
				Insert_Block_Micro(pCurrentUsingBlock, pStartBlock);
				pCurrentUsingBlock = pStartBlock;
			}
			else pCurrentUsingBlock = pCurrentUsingBlock->pNext;
		}
		return pCurrentUsingBlock;
	}
public:
	//ȡ�����Ͷ�Ӧ���ڴ�ض���
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
	//���ڴ�ض�̬����һ�����󣬷��ض���ָ�롣��Ӧ�ģ���ָ�������deleteObj�ͷš�Ҳ����giveUpObj����ʹ��Ȩ���÷�ʽ��ִ�ж�����������
	//ע�⣺������ʱ�ĵ�ǰ��������ѹ���������磺����giveUpObj()����ʹ��Ȩ�����յĶ��󣩣�����Ա��ι��촫�Ρ�
	template <typename... Args>
	static value_type* newObjFromMemPool(PCStr name, size_t _counts, Args&&... args) {
		return  _counts <= 1 ? (value_type*)CLMemPool<value_type>::getMenPool(name)->allocate(std::forward<Args>(args)...):
			(value_type*)getMenPool(name)->allocate(_counts, std::forward<Args>(args)...);
	}
	//�����ڴ�����ڴ�鵱������ڴ�ռ�ô�С����λMB
	static void setMemPoolBlockCapacity(size_t mb = 1) {
		assert(mb != 0);
		getMenPool()->setPerBlockMemCapacity(mb);
	}
};

#endif


//�ͷŰ���CLMemPool��CLMemPoolEx�����ĵ����������Ķ��󣬹黹�ڴ�أ������������������
inline void deleteObjBackToMemPool(void* pDelete) {
	assert(pDelete != nullptr);
	MUHEARDER(char, pDelete)->getOwnerBlock()->getOwnerMemPool()->deallocate(pDelete);
}
//��������CLMemPool��CLMemPoolEx�����ĵ����������Ķ���Ĳ���Ȩ���Ż��ڴ�أ����ǲ�������������
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
//Ŀ�ģ����챣��typeName���͵��ڴ�أ����δ������Ļ�����������������counts��typeName���͹����Ķ�������ָ��ͷ��
//���ã������ڴ�ض�̬����һ��������������(��һ����������������ͣ��ڶ������������������ĸ��������������������ǿɱ������Ĺ������)�����ض���ָ�룻
//��������Ӧ�ģ��÷������ص�ָ�������deleteObj/deleteObjs�ͷţ�Ҳ����giveUpObj/giveUpObjs����ʹ��Ȩ��giveUpObj/giveUpObjs��ʽ��ִ�ж�������,ֻ�ǰѶ���ԭ���Ż��ڴ�أ���
//ע�⣺���ڴ�ز���Ԥ������ƣ����Ը��ٵ���ɶ�д����ָ�� counts > 1 �ɷ������������Ķ��󣨶�����У�����ָ�� counts = 1 ֻ�ܷ��䵥�����󣬴�ʱ�����˻�ΪnewObj()��Ϊ��
//ע�⣺������ʱ�ĵ�ǰ��������ѹ���������磺����giveUpObj()����ʹ��Ȩ�����յĶ��󣩣�����Ա��ι��촫�Σ�
//ע�⣺���䵥��������ٶȻ��Ը��ڷ�����������
#define newObjs( typeName ,counts, ... ) (CLMemPoolEx<typeName>::newObjFromMemPool(_T(#typeName) ,(counts), __VA_ARGS__ ))
//���챣��typeName���͵��ڴ�أ����δ������Ļ����������ص���typeName���;��������Ķ���ָ�룬����Ķ����ǵ����������Ķ���
//��Ҫ���������Ķ���������ʹ��newObjs()������ָ�� counts > 1��
#define newObj( typeName , ... ) (CLMemPool<typeName>::newObjFromMemPool(_T(#typeName) , __VA_ARGS__ ))

//���ɴ���ŵ��ڴ�غͶ���ָ��ĺꣻ����ָ���ڴ�����ƣ�������δ��ָ�����Ļ���
#define newObjAndNamed( typeName, NameString , ... ) (CLMemPool<typeName>::newObjFromMemPool(_T(#NameString), __VA_ARGS__ ))
//���ɴ���ŵ��ڴ�غͶ���ָ��ĺꣻ����ָ���ڴ�����ƣ�������δ��ָ�����Ļ���
#define newObjsAndNamed( typeName, NameString ,counts, ... ) (CLMemPoolEx<typeName>::newObjFromMemPool(_T(#NameString),(counts), __VA_ARGS__ ))
//�ͷŰ���CLMemPool��CLMemPoolEx�����ĵ����������Ķ���ָ�룬��Դ�黹�ڴ�أ�pObjToDelete���ͷŵĶ���ָ�룬�������ڲ�ָ���̬ʵ�ֲ�ͬ���ͷ���Ϊ��
#define deleteObj( pObjToDelete ) (deleteObjBackToMemPool((pObjToDelete)))
//�ͷŰ���CLMemPool��CLMemPoolEx�����ĵ����������Ķ���ָ�룬��Դ�黹�ڴ�أ���deleteObj()������Ч���������ڲ�ָ���̬ʵ�ֲ�ͬ���ͷ���Ϊ��
#define deleteObjs( pObjToDelete ) deleteObj( pObjToDelete )
//��������CLMemPool��CLMemPoolEx�����ĵ����������Ķ����ʹ��Ȩ�������������󣩣����Ż��ڴ�ش��ã�pObjToDelete���ͷŵĶ���ָ�룬�������ڲ�ָ���̬ʵ�ֲ�ͬ�ķ�����Ϊ��
#define giveUpObj( pObjToDelete ) (giveUpObjBackToCLMemPool((pObjToDelete)))
//��������CLMemPool��CLMemPoolEx�����ĵ����������Ķ����ʹ��Ȩ�������������󣩣���giveUpObj()������Ч���������ڲ�ָ���̬ʵ�ֲ�ͬ�ķ�����Ϊ��
#define giveUpObjs( pObjToDelete ) giveUpObj( pObjToDelete )

#endif