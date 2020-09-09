#pragma once
#ifndef __CL_ARRAYTEMPLATE_H__
#define __CL_ARRAYTEMPLATE_H__

#include "windows.h"
#include <vector>
#include <stack>

using namespace std;

#ifndef __CL_VALUEPACK__
#define __CL_VALUEPACK__
//基础数值类型的对象化封装类模板
template<class T>
struct CLValuePacking {	
	using value_type = T;
	using ref = value_type&;
	using refc = const value_type&;
	value_type v;
	CLValuePacking() {}
	CLValuePacking(value_type _v): v (_v){}
	template<class T2>
	CLValuePacking(const CLValuePacking<T2>& _v) : v(_v) {}
	operator value_type& () { return v; }
	template<class T2>
	operator T2 () const { return T2(v); }
	value_type operator%(int v2) const { return int(v) % v2; }
	ref operator%=(int v2) { return v = int(v) % v2; }
	template<class T2>
	ref operator=(T2 v2) { return v = value_type(v2); }
	//运算
	ref operator--() {
		return --v;
	}
	ref operator++() {
		return ++v;
	}
	value_type operator--(int) {
		return v--;
	}
	value_type operator++(int) {
		return v++;
	}
};
template<class T1, class T2>
T1 operator%(T1 v, const CLValuePacking<T2>& t) {
	return int(v) % int(t);
}


//实例化 基础数值型包装类 模版类
#define CLVPack(valueClass,definedClassName)\
template class CLValuePacking<valueClass>;\
typedef CLValuePacking<valueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

#endif

//对象类型 多维安全数组类 模版类（注意：对象类应该妥善处理对象赋值或拷贝构造对内部动态指针变量的影响，避免引起多重释放）
template <class T>
class CLArrayTemplate {
public:
	using obj = CLArrayTemplate<T>;
	using pObj = CLArrayTemplate<T>*;
	using pObjc = const CLArrayTemplate<T>*;
	using ref = CLArrayTemplate<T> &;
	using refc = const CLArrayTemplate<T> &;
	using vlist = initializer_list<T>;
	using data = vector<CLArrayTemplate<T>*>;
	using pdata = vector<CLArrayTemplate<T>*>*;
protected:
	class VPackage {//包装类
	public:
		T* m_value;	//节点值对象
		inline VPackage()
			:m_value(new T)
		{
		}
		inline ~VPackage() {
			if (m_value != 0) {
				delete m_value;
				m_value = 0;
			}
		}
	};
	VPackage m_vPack;
	pdata m_sublst;//节点的子对象表
	byte m_createByLst;
	stack<pObj> objLibrary;//mem pool
	void _release() {
		//释放内存池
		_releaseLib();
		//释放表本身，及子连
		if (m_sublst != NULL) {
			for (size_t i = 0, si = m_sublst->size(); i < si; i++) {
				auto pc = m_sublst->at(i);
				if (pc != 0) {
					_deleteOne(pc);
				}
			}
			delete m_sublst;
			m_sublst = NULL;
		}
	};
	void _releaseLib() {
		while (!objLibrary.empty()) //释放库
		{
			pObj pc = objLibrary.top();
			objLibrary.pop();
			_deleteOne(pc);
		}
	};
	virtual pObj _newObj() {
		pObj pnew;
		if (!objLibrary.empty()) {
			pnew = objLibrary.top();
			objLibrary.pop();
			pnew->m_vPack.VPackage::VPackage();
			pnew->m_createByLst = false;
			return pnew;
		}
		else {
			pnew = (pObj)malloc(sizeof(obj));
			pnew->CLArrayTemplate::CLArrayTemplate();
			return pnew;
		}
	}
	virtual void _storeOne(void* p) {
		((pObj)p)->clear();//清理子连
		((pObj)p)->m_vPack.VPackage::~VPackage();//析构对象
		objLibrary.push((pObj)p);
	}
	virtual void _deleteOne(void* p) {
		((pObj)p)->CLArrayTemplate::~CLArrayTemplate();
		free(p);
	}
	inline size_t _getObjSizeof() const {
		return  _getClassSizeof() + (m_vPack.m_value ? sizeof(T) : 0);
	}
	inline virtual size_t _getClassSizeof() const {
		return sizeof(obj);
	}
	inline void* _newData() { return new data; }
	void _copyList(const pdata& tag_sublst) {
		size_t lsi = (tag_sublst != 0 ? tag_sublst->size() : 0);
		if (lsi > 0) {
			if (!m_sublst)
				m_sublst = (pdata)_newData();
			size_t si = m_sublst->size();
			if (si < lsi) {
				m_sublst->resize(lsi, (pObj)0);
			}
			for (size_t i = 0; i < lsi; i++)
			{
				auto& p = m_sublst->at(i);
				auto& pt = tag_sublst->at(i);
				if (pt != 0) {
					if (p == 0)
						p = _newObj();
					*(p->m_vPack.m_value) = *(pt->m_vPack.m_value);
				}
			}
		}
	}
	void _clear() {
		if (m_sublst != NULL) {
			for (size_t i = 0, si = m_sublst->size(); i < si; i++) {
				auto& pc = m_sublst->at(i);
				if (pc != 0) {
					_storeOne(pc);
				}
			}
			m_sublst->clear();
		}
	};
public:	
	//取得对象内存总占用大小,包括已使用的内存和已缓存在内存池中备用的内存
	size_t getMemerySize() const {
		size_t sii = getUsedMemerySize();
		sii += getSpareMemerySize();
		return sii;
	}
	//释放结构所有占用内存,包括已使用的内存和已缓存在内存池中备用的内存
	void releaseMemery() {
		_release();
	}
	//取得已在内存池中保留的备用内存的占用大小（不包含正在使用的内存）
	size_t getSpareMemerySize() const {
		size_t sii = 0;
		stack<pObj> stk2 = objLibrary;
		while (!stk2.empty())
		{
			sii += (stk2.top()->getMemerySize());
			stk2.pop();
		}
		return sii;
	}
	//释放已在内存池中保留的备用内存的占用大小（不包含正在使用的内存）
	void releaseSpareMemery() {
		_releaseLib();
	}
	//取得对象已使用的内存占用大小（不包缓存在内存池中备用的内存）
	size_t getUsedMemerySize()const {
		size_t sii = _getObjSizeof();
		if (m_sublst != NULL) {
			sii += (m_sublst->capacity() * sizeof(pObj));
			for (size_t i = 0, si = m_sublst->size(); i < si; ++i)
			{
				pObj p = (m_sublst->at(i));
				if (p)
					sii += p->getMemerySize();
			}
		}
		return sii;
	}
	inline CLArrayTemplate()
		:m_sublst(0), m_createByLst(false) {
	}
	template<class otherT>
	inline CLArrayTemplate(otherT v)
		: m_sublst(0), m_createByLst(false) {
		*m_vPack.m_value = v;
	}
	inline CLArrayTemplate(refc tag)
		: m_sublst(0), m_createByLst(false) {
		*this = tag;
	}
	inline CLArrayTemplate(const vlist& _Ilist)
		: m_sublst(0), m_createByLst(true) {
		*this = _Ilist;
	}
	//析构函数 释放所有对象及子节点的内存
	inline virtual ~CLArrayTemplate() {
		_release();
	}
	inline ref operator =(T v) {
		return (m_createByLst = false, *m_vPack.m_value = v), * this;
	}
	ref operator =(refc tag) {
		if (!tag.m_createByLst)
			m_createByLst = false, * m_vPack.m_value = (*tag.m_vPack.m_value);
		_copyList(tag.m_sublst);
		return *this;
	}
	ref operator =(const vlist& _Ilist) {
		size_t lsi = _Ilist.size();
		if (lsi > 0) {
			if (!m_sublst)
				m_sublst = (pdata)_newData();
			auto si = m_sublst->size();
			if (si < lsi) {
				m_sublst->resize(lsi, (pObj)0);
			}
			for (size_t i = 0; i < lsi; i++)
			{
				auto& p = m_sublst->at(i);
				if (p == 0)
					p = _newObj();
				*(p->m_vPack.m_value) = *(_Ilist.begin() + i);
			}
		}
		return *this;
	}
	//向队列中直接产生i个实例对象，以便后续at或[]操作使用，在未具体调用单元之前，
	//函数就会将对象准备就绪（动态申请内存），函数不会改变已有的对象；
	void generate(int i) {
		size_t _i = (i < 0 ? 0 : i);
		if (!m_sublst)m_sublst = new data;
		if (m_sublst->size() >= _i) {
			for (size_t i = 0; i < _i; i++)
			{
				auto& pc = m_sublst->at(i);
				if (pc == 0)
					pc = _newObj();
			}
			return;
		}
		m_sublst->resize(_i, (pObj)0);
		for (size_t i = 0; i < _i; i++)
		{
			auto& pc = m_sublst->at(i);
			if (pc == 0)
				pc = _newObj();
		}
		return;
	}
	//就会删除多余的链表对象，保留指定的个数,不会初始化已有的对象
	void resize(int i) {
		size_t _i = (i < 0 ? 0 : i);
		if (!m_sublst)
			m_sublst = new data;
		size_t si = m_sublst->size();
		if (si > _i) {
			for (size_t i = si; i > _i; --i)
			{
				auto& pc = m_sublst->at(i - 1);
				if (pc != 0) {
					_storeOne(pc);
					pc = 0;
				}
			}
			m_sublst->resize(_i);
			return;
		}
		else if (si < _i)
			m_sublst->resize(_i, 0);
		return;
	}

	//返回内部值的引用 
	inline T& operator()() { return *m_vPack.m_value; }

	//清除子连，保留子连对象，区别于reset();
	inline void clear() {
		_clear();
	}
	//可以使得对象本身就像一个计算数一样被使用于数学运算,常值类型转换
	inline operator T() const {
		return *m_vPack.m_value;
	}
	//可以使得对象本身就像一个计算数一样被使用于数学运算,可变类型转换
	inline operator T& () {
		return *m_vPack.m_value;
	}
	//取得链的第i个对象，i<0自动取0，i>链最大数时自动扩容，因此i永远不会产生越界错误
	ref at(size_t _i) {
		if (!m_sublst)
			m_sublst = new data;
		if (m_sublst->size() < _i + 1) {
			m_sublst->resize(_i + 1, (pObj)0);
			auto& pc = m_sublst->at(_i);
			pc = _newObj();
			return *pc;
		}
		auto& pc = m_sublst->at(_i);
		if (pc == 0)
			pc = _newObj();
		return *pc;
	}
	template<class Ti>
	inline ref at(const Ti& i) {
		return at(i < 0 ? 0 : size_t(i));
	}
	//取得链的第i个对象，i<0自动取0，i>链最大数时自动扩容，因此i永远不会产生越界错误
	inline ref operator[](size_t i) { return at(i); }
	template<class Ti>
	inline ref operator[](const Ti& i) { return at(i); }
	//取得链的单元数
	inline size_t size() const {
		return m_sublst != NULL ? m_sublst->size() : 0;
	}
	//向连的末尾增加元素
	inline ref push_back(T v) {
		return at(size()) = v, *this;
	}
};

//实例化 对象类型的 多维安全数组 模版类（注意：对象类应该妥善处理对象赋值或拷贝构造对内部动态指针变量的影响，避免引起多重释放）
#define CLArrayObj(objClass,definedClassName)\
template class CLArrayTemplate<objClass>;\
typedef CLArrayTemplate<objClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

//基础数值类型 多维安全数组类 模版类
template<class T>
class CLArrayTemplateV :public CLArrayTemplate<CLValuePacking<T>> {
public:
	using obj = CLArrayTemplateV<T>;
	using obj_base = CLArrayTemplate<CLValuePacking<T>>;
	using pObj = CLArrayTemplateV<T>*;
	using pObj_base = CLArrayTemplate<CLValuePacking<T>>*;
	using ref_base = CLArrayTemplate<CLValuePacking<T>> &;
	using refc_base = const CLArrayTemplate<CLValuePacking<T>> &;
	using ref = CLArrayTemplateV<T> &;
	using refc = const CLArrayTemplateV<T> &;
	using vlist = initializer_list<CLValuePacking<T>>;
protected:
	virtual pObj _newObj() {
		pObj pnew;
		if (!obj_base::objLibrary.empty()) {
			pnew = (pObj)obj_base::objLibrary.top();
			obj_base::objLibrary.pop();
			pnew->clear();//清理子连
			pnew->reset();//重置对象
			return pnew;
		}
		else {
			pnew = (pObj)malloc(sizeof(obj));
			pnew->CLArrayTemplateV::CLArrayTemplateV();
			return pnew;
		}
	}
	virtual void _storeOne(void* p) {
		obj_base::objLibrary.push((pObj)p);
	}
	virtual void _deleteOne(void* p) {
		((pObj)p)->CLArrayTemplateV::~CLArrayTemplateV();
		free(p);
	}
	inline virtual size_t _getClassSizeof() const {
		return sizeof(obj);
	}
public:
	//搜索值是否存在，返回索引从1开始，没找到返回0，startIndex表示搜索的起始索引从0开始计
	size_t find(const T& tag, size_t startIndex = 0) {
		for (size_t i = startIndex, si = obj_base::size(); i < si; i++)
		{
			auto& pc = obj_base::m_sublst->at(i);
			if (pc)
				if ((*(pc))() == tag)
					return i + 1;
		}
		return 0;
	}
	inline void reset() {
		obj_base::m_createByLst = false;
		(*obj_base::m_vPack.m_value) = 0;
	}
	//构造函数 并初始化值为
	inline CLArrayTemplateV()
	{
		(*obj_base::m_vPack.m_value).v = 0;
	}
	template<class otherT>
	inline CLArrayTemplateV(otherT v)
	{
		(*obj_base::m_vPack.m_value).v = v;
	}
	CLArrayTemplateV(refc tag)
	{
		if (tag.m_createByLst)
			obj_base::m_createByLst = true, (*obj_base::m_vPack.m_value).v = 0;
		else
			(*obj_base::m_vPack.m_value).v = (*tag.m_vPack.m_value).v;
		obj_base::_copyList(tag.m_sublst);
	}
	CLArrayTemplateV(const vlist& _Ilist)
	{
		obj_base::operator = (_Ilist);
		obj_base::m_createByLst = true;
		(*obj_base::m_vPack.m_value).v = 0;
	}
	inline virtual ~CLArrayTemplateV() {
		obj_base::_release();
	}
	template<class otherT>
	inline ref operator =(otherT value) {
		return (obj_base::m_createByLst = false, (*obj_base::m_vPack.m_value).v = T(value)), * this;
	}
	//inline ref operator =(T value) {return (obj_base::m_createByLst = false, (*obj_base::m_vPack.m_value).v = value), * this;}	
	ref operator =(refc tag) {
		if (tag.m_createByLst)
			obj_base::m_createByLst = true, (*obj_base::m_vPack.m_value).v = 0;
		else
			(*obj_base::m_vPack.m_value).v = (*tag.m_vPack.m_value).v;
		obj_base::_copyList(tag.m_sublst);
		return *this;
	}
	inline ref operator =(const vlist& _Ilist) {
		return obj_base::operator = (_Ilist), * this;
	}
	//可以使得对象本身就像一个计算数一样被使用于数学运算,常值类型转换
	inline operator T() const {
		return (*obj_base::m_vPack.m_value).v;
	}
	//可以使得对象本身就像一个计算数一样被使用于数学运算,可变类型转换
	inline operator T& () {
		return (*obj_base::m_vPack.m_value).v;
	}
	inline ref at(size_t i) {
		return (ref) * &(obj_base::at(i));
	}
	template<class Ti>
	inline ref at(const Ti& i) {
		return (ref) * &(obj_base::at(i < 0 ? 0 : size_t(i)));
	}
	//取得链的第i个对象，i<0自动取0，i>链最大数时自动扩容，因此i永远不会产生越界错误
	inline ref operator[](size_t i) { return (ref) * &(obj_base::at(i)); }
	template<class Ti>
	inline ref operator[](const Ti& i) { return (ref) * &(obj_base::at(i < 0 ? 0 : size_t(i))); }

	//运算

	obj operator--(int) {
		obj a = *this;
		++((*obj_base::m_vPack.m_value).v);
		return a;
	}
	obj operator++(int) {
		obj a = *this;
		++((*obj_base::m_vPack.m_value).v);
		return a;
	}
	inline ref operator--() {
		return --(*obj_base::m_vPack.m_value).v, * this;
	}
	inline ref operator++() {
		return ++(*obj_base::m_vPack.m_value).v, * this;
	}
	inline int operator%(int v) {
		return (((int)((*obj_base::m_vPack.m_value).v)) % v);
	}
};

//实例化 基础数值类型的 多维安全数组 模版类
#define CLArrayV(baseValueClass,definedClassName)\
template class CLArrayTemplateV<baseValueClass>;\
typedef CLArrayTemplateV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

template<class T>
class CLSingleArrayV :public vector<T> {
public:
	using obj = CLSingleArrayV<T>;
	using pObj = CLSingleArrayV<T>*;
	using ref = CLSingleArrayV<T> &;
	using refc = const CLSingleArrayV<T> &;
	using vlist = initializer_list<T>;
protected:
	T m_value;
	void _init(const T& v) { m_value = v; }
public:
	inline ref reset() { return m_value = 0, *this; }
	size_t getMemerySize() const {
		return sizeof(CLSingleArrayV) + sizeof(T) * vector<T>::capacity();
	}
	inline CLSingleArrayV() { _init(0); }
	inline CLSingleArrayV(const T& v) { _init(v); }
	inline CLSingleArrayV(const vlist& list) {
		_init(0);
		vector<T>::operator=(list);
	}
	inline ~CLSingleArrayV() {}
	inline operator T() const {
		return m_value;
	}
	inline operator T& () {
		return m_value;
	}
	inline T& operator ()() {
		return m_value;
	}
	inline T& operator =(const T& v) {
		return m_value = v, m_value;
	}
	inline T& operator =(const vlist& list) {
		vector<T>::operator=(list);
		return m_value;
	}	
	inline T& operator--() {
		return --m_value, m_value;
	}
	inline T& operator++() {
		return ++m_value, m_value;
	}
	inline obj operator--(int) {
		obj a(m_value);
		--m_value;
		return a;
	}
	inline obj operator++(int) {
		obj a(m_value);
		++m_value;
		return a;
	}
	inline int operator%(int v) {
		return (((int)m_value) % v);
	}	
};

template<class T>
class CLSingleSafeArrayV :public CLSingleArrayV<T> {
public:
	inline CLSingleSafeArrayV() {}
	inline CLSingleSafeArrayV(const T& v)
		:CLSingleArrayV<T>(v){ }
	inline CLSingleSafeArrayV(const CLSingleArrayV<T>::vlist& list)
		: CLSingleArrayV<T>(list) {	}
	inline T& at(size_t i) {
		//size_t csi = vector<T>::size();
		if (i >= vector<T>::size()) {
			vector<T>::resize(i + 1, 0);
		}
		return vector<T>::operator[](i);
	}
	template<class Ti>
	inline T& at(const Ti& i) {
		return at(i < 0 ? size_t(0) : size_t(i));
	}
	inline T& operator[](size_t i) {
		return at(i);
	}
	template<class Ti>
	inline T& operator[](const Ti& i) {
		return at(i < 0 ? size_t(0) : size_t(i));
	}
};

//实例化 基础数值类型的 一维非安全数组 模版类
#define CLArrayVN(baseValueClass,definedClassName)\
template class CLSingleArrayV<baseValueClass>;\
typedef CLSingleArrayV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

//实例化 基础数值类型的 一维安全数组 模版类
#define CLArrayVS(baseValueClass,definedClassName)\
template class CLSingleSafeArrayV<baseValueClass>;\
typedef CLSingleSafeArrayV<baseValueClass> definedClassName;\
typedef definedClassName& definedClassName##R;\
typedef const definedClassName& definedClassName##RC;\
typedef definedClassName* P##definedClassName;\
typedef const definedClassName* PC##definedClassName;

#endif