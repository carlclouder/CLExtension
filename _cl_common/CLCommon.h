 #pragma once
#ifndef __CL_COMMON_H__
#define __CL_COMMON_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include "windows.h"
#include <map>
#include <vector>
#include <tchar.h>
#include <assert.h>
#include <ctime>
#include <iostream>
#include <unordered_map>
#include "unknwn.h"
#include "Mshtml.h"
#include "comutil.h"
#include <atomic>
#include <random>

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

using namespace std;

//区间比较宏
#define BETWEEN(x,a,b)  (( (a) <= (x) && (x) <= (b) ) || ( (b) <= (x) && (x) <= (a) ))
#define BETWEENO(x,a,b)  (( (a) < (x) && (x) < (b) ) || ( (b) < (x) && (x) < (a) ))

//返回类对象自身的引用的宏
#define _RETSELF_ \
	return *this;

//返回数组中的最大值的宏
#define listMax( var , lst , lstSize ) \
	(var) = (lst)[0];\
	for (size_t _ise231=1,_si = (lstSize);_ise231 < _si;_ise231++)if( (var) < (lst)[_ise231]) (var) = (lst)[_ise231];
#define listMaxI( var , lst , lstSize ,index ) \
	(var) = (lst)[(index) = 0];\
	for (size_t _ise231=1,_si = (lstSize);_ise231 < _si;_ise231++)if( (var) < (lst)[_ise231]) (var) = (lst)[(index) = _ise231];
//返回vector中的最大值的宏
#define vectorMax( var , lst ) listMax(var,lst,(lst).size())
#define vectorMaxI( var , lst ,index ) listMaxI(var,lst,(lst).size(),index)

//升序排列宏( 临时变量 ， 目标结果容器vec ， 原数据数组 ， 原数组成员数 )
#define vectorAesOdr( t , m_vtOff , offset , nSi ) \
{\
	(m_vtOff).clear();\
	for(size_t ci=0;ci<(nSi);ci++)m_vtOff.push_back(offset[ci]);\
	for (size_t ci = 0;ci<(nSi);ci++){for (size_t cj = ci+1;cj<nSi;cj++){	if( (m_vtOff[ci]) > (m_vtOff[cj]) ){t = m_vtOff[ci];m_vtOff[ci] = m_vtOff[cj];m_vtOff[cj] = t;}}}\
}
//降序排列宏( 临时变量 ， 目标结果容器vec ， 原数据数组 ， 原数组成员数 )
#define vectorDesOdr( t , m_vtOff , offset , nSi ) \
{\
	(m_vtOff).clear();\
	for(size_t ci=0;ci<(nSi);ci++)m_vtOff.push_back(offset[ci]);\
	for (size_t ci = 0;ci<(nSi);ci++){for (size_t cj = ci+1;cj<nSi;cj++){	if( (m_vtOff[ci]) < (m_vtOff[cj]) ){t = m_vtOff[ci];m_vtOff[ci] = m_vtOff[cj];m_vtOff[cj] = t;}}}\
}

#ifndef _CL_COMMON_MATH_DEF_
#define _CL_COMMON_MATH_DEF_

typedef double VT; 
//VT数列
typedef std::vector<VT> VD;
typedef VD& VD_R;
typedef const VD& VD_RC;
//VT数列选择器
typedef std::vector<VT>::iterator VDI;
typedef VDI& VDIR;
typedef std::vector<VT>::const_iterator VDIC;
typedef VDIC& VDICR;
//VT数组
typedef const VT* VTArray;
typedef size_t VTArraySize;

//求和
VT SUM(VD_RC vec);
//均值
VT AVG(VD_RC vec);
//期望(pi权重)
VT E(VD_RC v_X,VD_RC v_pi);
VT E(VD_RC v_X);
//方差(pi权重)
VT VAR(VD_RC v_X,VD_RC v_pi);
VT VAR(VD_RC v_X);
//协方差(pix权重，piy权重)
VT COV(VD_RC v_X,VD_RC v_Y,VD_RC v_pix ,VD_RC v_piy);
VT COV(VD_RC v_X,VD_RC v_Y);
//标准差（piy权重)
VT DVA(VD_RC v_X,VD_RC v_pix);
VT DVA(VD_RC v_X);
//求相关系数(pix权重，piy权重)
VT COR(VD_RC v_X,VD_RC v_Y,VD_RC v_pix,VD_RC v_piy);
VT COR(VD_RC v_X,VD_RC v_Y);

//---------------------------------------
//求和
VT SUM(VTArray vec,VTArraySize si);
//均值
VT AVG(VTArray vec,VTArraySize si);
//期望(pi权重)
VT E(VTArray v_X,VTArray v_pi,VTArraySize si);
VT E(VTArray v_X,VTArraySize si);
//方差(pi权重)
VT VAR(VTArray v_X,VTArray v_pi,VTArraySize si);
VT VAR(VTArray v_X,VTArraySize si);
//协方差(pix权重，piy权重)
VT COV(VTArray v_X,VTArray v_Y,VTArray v_pix ,VTArray v_piy,VTArraySize si);
VT COV(VTArray v_X,VTArray v_Y,VTArraySize si);
//标准差（piy权重)
VT DVA(VTArray v_X,VTArray v_pix,VTArraySize si);
VT DVA(VTArray v_X,VTArraySize si);
//求相关系数(pix权重，piy权重)
VT COR(VTArray v_X,VTArray v_Y,VTArray v_pix,VTArray v_piy,VTArraySize si);
VT COR(VTArray v_X,VTArray v_Y,VTArraySize si);

//计算一个数据列的分布情况；
//result的T1表示某一分布区间的中心值（非均值），T2表示分布频数（即出现次数）
template<class T1, class T2, class T3>void dataToDistribution(std::map<T1, T2> & result, const T3 * dataList, size_t dataCounts, size_t sectionCounts,
	T1 * savedLowerLimitValue = nullptr, T1 * savedUpperLimitValue = nullptr) {
	assert(dataList != nullptr && dataCounts != 0);
	result.clear();
	T1 vmax = dataList[0], vmin = dataList[0];
	for (size_t k = 0; k < dataCounts; k++) {
		if (dataList[k] > vmax)vmax = dataList[k];
		if (dataList[k] < vmin)vmin = dataList[k];
	}
	T1 sec = T1(vmax - vmin) / max(1, sectionCounts);
	for (size_t k = 0; k < dataCounts; ++k)
	{
		T1 i = dataList[k];
		if (i >= vmax) i -= sec;
		auto x = T1((double(size_t((i - vmin) / sec)) + 0.5) * sec + vmin);
		result[x] += T2(1);
	}
	if (savedLowerLimitValue)*savedLowerLimitValue = vmin;
	if (savedUpperLimitValue)*savedUpperLimitValue = vmax;
}
//计算一个数据列的分布情况；
//result的T1表示某一分布区间的中心值（非均值），T2表示分布频数（即出现次数）
template<class T1, class T2, class T3>void dataToDistribution(std::map<T1, T2>& result, const vector<T3>& dataList, size_t sectionCounts,
	T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	return dataToDistribution(result, dataList.data(), dataList.size(), sectionCounts, savedLowerLimitValue, savedUpperLimitValue);
}
//计算一个数据列的分布情况；
//resultDistRange表示某一分布区间的中心值（非均值），resultDistFrequency表示分布频数（即出现次数）
template<class T1, class T2, class T3>void dataToDistribution(std::vector<T1>& resultDistRange, std::vector<T2>& resultDistFrequency,
	const T3* dataList, size_t dataCounts, size_t sectionCounts,T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	std::map<T1, T2> ret;	
	dataToDistribution(ret, dataList, dataCounts, sectionCounts, savedLowerLimitValue, savedUpperLimitValue);
	resultDistRange.reserve(ret.size()); resultDistRange.clear();
	resultDistFrequency.reserve(ret.size()); resultDistFrequency.clear();
	for (auto& i: ret)
	{
		resultDistRange.push_back(i.first);
		resultDistFrequency.push_back(i.second);
	}
}
//计算一个数据列的分布情况；
//resultDistRange表示某一分布区间的中心值（非均值），resultDistFrequency表示分布频数（即出现次数）
template<class T1, class T2, class T3>void dataToDistribution(std::vector<T1>& resultDistRange, std::vector<T2>& resultDistFrequency,
	const vector<T3>& dataList, size_t sectionCounts, T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	return dataToDistribution(resultDistRange, resultDistFrequency, dataList.data(), dataList.size(),
		sectionCounts,savedLowerLimitValue ,savedUpperLimitValue);
}

#ifndef _CL_CLSPACE_DEF_
#define _CL_CLSPACE_DEF_
template<class Float>
class CLSpaceTemplate {
public:
	struct Point {
		Float x = 0, y = 0, z = 0;
	};
	typedef vector<Point> Polygon;
	struct Line {
		Point pts, pte;
	};
	struct Plane {
		Point pt1, pt2, pt3;
	};
	static Float add(Float v1, Float v2) {
		return v1 + v2;
	}
	static Float sub(Float v1, Float v2) {
		return v1 - v2;
	}
	static Float mul(Float v1, Float v2) {
		return v1 * v2;
	}
	static Float div(Float v1, Float v2) {
		return v1 / v2;
	}
	static bool double_Equal(Float num1, Float num2)
	{
		if ((num1 - num2 > -0.0000001) && (num1 - num2 < 0.0000001))
			return true;
		else
			return false;
	}
	/**
	* 判断当前线段是否包含给定的点
	* 即给定的点是否在当前边上
	*/
	static bool lineIsContainsPoint(const Line& line, const Point& point) {
		bool result = false;
		//判断给定点point与端点1构成线段的斜率是否和当前线段的斜率相同
		//给定点point与端点1构成线段的斜率k1
		Float k1 = 0;
		bool needjudgment = true;
		if (double_Equal(point.x, line.pts.x)) {
			//k1 = -DBL_MAX;
			needjudgment = false;
		}
		else {
			k1 = div(sub(point.y, line.pts.y), sub(point.x, line.pts.x));
		}
		//当前线段的斜率k2
		Float k2 = 0;
		if (double_Equal(line.pte.x, line.pts.x)) {
			//k2 = -DBL_MAX;
			needjudgment = false;
		}
		else {
			k2 = div(sub(line.pte.y, line.pts.y), sub(line.pte.x, line.pts.x));
		}

		if (needjudgment == true) {
			if (double_Equal(k1, k2)) {
				//若斜率相同，继续判断给定点point的x是否在pointA.x和pointB.x之间,若在 则说明该点在当前边上
				if (sub(point.x, line.pts.x) * sub(point.x, line.pte.x) < 0) {
					result = true;
				}
			}
		}
		return result;
	}
	//叉积
	static Float mult(const Point& a, const Point& b, const Point& c)
	{
		return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
	}
	/**
		* 给定线段是否与当前线段相交
		* 相交返回true, 不相交返回false
		*/
	static bool linesIsIntersect(const Line& line1, const Line& line2)
	{
		Point aa = line1.pts;
		Point bb = line1.pte;
		Point cc = line2.pts;
		Point dd = line2.pte;
		if (max(aa.x, bb.x) < min(cc.x, dd.x)) {
			return false;
		}
		if (max(aa.y, bb.y) < min(cc.y, dd.y)) {
			return false;
		}
		if (max(cc.x, dd.x) < min(aa.x, bb.x)) {
			return false;
		}
		if (max(cc.y, dd.y) < min(aa.y, bb.y)) {
			return false;
		}
		if (mult(cc, bb, aa) * mult(bb, dd, aa) < 0) {
			return false;
		}
		if (mult(aa, dd, cc) * mult(dd, bb, cc) < 0) {
			return false;
		}
		return true;
	}
	//多边形的各个点要是依次连接的输入的
	static bool pointIsInPolygon(const Point& point, const Polygon& poly) {
		bool result = false;
		int intersectCount = 0;
		// 判断依据：求解从该点向右发出的水平线射线与多边形各边的交点，当交点数为奇数，则在内部
		//不过要注意几种特殊情况：1、点在边或者顶点上;2、点在边的延长线上;3、点出发的水平射线与多边形相交在顶点上
		/**
		* 具体步骤如下：
		* 循环遍历各个线段：
		*  1、判断点是否在当前边上(斜率相同,且该点的x值在两个端口的x值之间),若是则返回true
		*  2、否则判断由该点发出的水平射线是否与当前边相交,若不相交则continue
		*  3、若相交,则判断是否相交在顶点上(边的端点是否在给定点的水平右侧).若不在,则认为此次相交为穿越,交点数+1 并continue
		*  4、若交在顶点上,则判断上一条边的另外一个端点与当前边的另外一个端点是否分布在水平射线的两侧.若是则认为此次相交为穿越,交点数+1.
		*/
		for (size_t i = 0; i < poly.size(); i++) {
			const Point& pointA = poly[i];
			Point pointB;
			Point pointPre;
			//若当前是第一个点,则上一点则是list里面的最后一个点
			if (i == 0) {
				pointPre = poly[poly.size() - 1];
			}
			else {
				pointPre = poly[i - 1];
			}
			//若已经循环到最后一个点,则与之连接的是第一个点
			if (i == (poly.size() - 1)) {
				pointB = poly[0];
			}
			else {
				pointB = poly[i + 1];
			}
			Line line = { pointA, pointB };
			//1、判断点是否在当前边上(斜率相同,且该点的x值在两个端口的x值之间),若是则返回true
			bool isAtLine = lineIsContainsPoint(line, point);
			if (isAtLine) {
				return true;
			}
			else {
				//2、若不在边上,判断由该点发出的水平射线是否与当前边相交,若不相交则continue
				//设置该射线的另外一个端点的x值=999,保证边的x永远不超过
				Point  radialPoint = { 180, point.y };
				Line radial = { point, radialPoint };
				//给定线段是否与当前线段相交 相交返回true
				bool isIntersect = linesIsIntersect(radial, line);
				if (!isIntersect) {
					continue;
				}
				else {
					//3、若相交,则判断是否相交在顶点上(边的端点是否在给定点的水平右侧).若不在,则认为此次相交为穿越,交点数+1 并continue
					if (!((pointA.x > point.x) && (double_Equal(pointA.y, point.y))
						|| (pointB.x > point.x) && (double_Equal(pointB.y, point.y)))) {
						intersectCount++;
						continue;
					}
					else {
						//4、若交在顶点上,则判断上一条边的另外一个端点与当前边的另外一个端点是否分布在水平射线的两侧.若是则认为此次相交为穿越,交点数+1
						if ((pointPre.y - point.y) * (pointB.y - point.y) < 0) {
							intersectCount++;
						}
					}
				}
			}
		}
		result = intersectCount % 2 == 1;
		return result;
	}
}; 
template class CLSpaceTemplate<float>;
typedef CLSpaceTemplate<float>  CLSpaceF;
template class  CLSpaceTemplate<double>;
typedef CLSpaceTemplate<double> CLSpace;
#endif

#endif

#define RANDDIV_MAX (ULLONG_MAX / 0xff) //本宏适用于生成原始数据的不可用
//#define RANDD_MAX (INT_MAX) //本宏适用于生成原始数据的不可用
#define RANDI_MAX (RANDDIV_MAX / 2 - 1) //最大随机数应该取一半
//设置随机种子，基于时间和额外算法（区别于srand）
void setRandomSeed();  
//产生一个随机数，0 ~ max ulonglong;不需要设置种子；
unsigned long long randi();
//设置随机种子，基于时间和额外算法（区别于srand）
#define RAND_SEED_EX()  
#define RAND_SEED()  
//#define RAND_SEED_EX()  (setRandomSeed())
//#define RAND_SEED()  (srand((unsigned)time(NULL)))


//取得(0,x)的随机整数：randi()%x；
#define RAND_I_0_X( x ) (randi()%(x))
//	取得(a,b)的随机整数：randi()%(b-a-1)+a+1；
#define RAND_I_A_B( a , b ) (randi()%((b)-(a)-1)+(a)+1)
//取得 ( a , RANDI_MAX ) 的随机整数；
#define RAND_I_A_MAX( a ) (RAND_I_A_B( a , RANDI_MAX ))
//	取得[a,b)的随机整数：randi()%(b-a)+a；
#define RAND_I_Ai_B( a , b ) (randi()%((b)-(a))+(a))
//	取得[a,b]的随机整数：randi()%(b-a+1)+a；
#define RAND_I_Ai_Bi( a , b ) (randi()%((b)-(a)+1)+(a))
//	取得(a,b]的随机整数：randi()%(b-a)+a+1；
#define RAND_I_A_Bi( a , b ) (randi()%((b)-(a))+(a)+1)
//	取得0-1之间的浮点数：randi()/double(RANDI_MAX)；
#define RAND_F_0_1() (randi()/double(RANDDIV_MAX))
//	取得(a,b)之间的浮点数：randi()/double(RANDI_MAX)*((b) - (a))+(a)；
#define RAND_F_A_B( a , b ) (RAND_F_0_1()*(double(b) - double(a))+double(a))

//检查是否是无效数，抛出异常
#define CLCheckNan(num) (_isnan((num)) ? throw std::invalid_argument("Value is not a number!"),TRUE:FALSE)
#define CLCheckInf(num) (isinf((num)) ? throw std::invalid_argument("Value is a infinite number!"),TRUE:FALSE)
#define CLCheckNanInf(num) \
	(_isnan((num)) ? (throw std::invalid_argument("Value is not a number!"),FALSE) :(isinf((num)) ? (throw std::invalid_argument("Value is a infinite number!"),FALSE):TRUE))
#define CLCheckNanMsg(num,msg) (_isnan((num)) ? (::MessageBox(NULL,_T(#msg),_T("Value Nan Exception Alert"),MB_ICONERROR),throw std::invalid_argument("Value is not a number!"),TRUE):FALSE)
#define CLCheckInfMsg(num,msg) (isinf((num)) ? (::MessageBox(NULL,_T(#msg),_T("Value Inf Exception Alert"),MB_ICONERROR),throw std::invalid_argument("Value is a infinite number!"),TRUE):FALSE)
#define CLCheckNanInfMsg(num,msg) \
	(_isnan((num)) ? (::MessageBox(NULL, _T(#msg), _T("Value Nan Exception Alert"), MB_ICONERROR), throw std::invalid_argument("Value is not a number!"),FALSE) : \
	isinf((num)) ? (::MessageBox(NULL, _T(#msg), _T("Value Inf Exception Alert"), MB_ICONERROR), throw std::invalid_argument("Value is a infinite number!"),FALSE):TRUE)

#include <map>
//定义映射模板类。该类可自动化处理线程同步。
template <typename A, typename B> 
class CLMapTemplate {
public:
	CLMapTemplate() :pCs(NULL) {};
	virtual ~CLMapTemplate() {
		EnterCriticalSection(GetInnerCS());
		GetMap().clear();
		LeaveCriticalSection(GetInnerCS());
		if (pCs != NULL) {
			DeleteCriticalSection(pCs);
			delete pCs;
			pCs = NULL;
		}
	};
	B GetValueFromMap(A first) {
		B p;
		//std::map<A, B>::iterator it;
		EnterCriticalSection(GetInnerCS());
		auto it = GetMap().find(first);
		p = ((it != GetMap().end()) ? it->second : (B)NULL);
		LeaveCriticalSection(GetInnerCS());
		return p;
	}
	B InsertPairToMap(A first, B second) {
		EnterCriticalSection(GetInnerCS());
		GetMap()[first] = second;
		LeaveCriticalSection(GetInnerCS());
		return second;
	}
	BOOL ErasePairFromMap(A first) {
		BOOL ret;
		//std::map<A, B>::iterator it;
		EnterCriticalSection(GetInnerCS());
		auto it = GetMap().find(first);
		ret = ((it != GetMap().end()) ? (GetMap().erase(it), TRUE) : FALSE);
		LeaveCriticalSection(GetInnerCS());
		return ret;
	}
protected:
	std::map<A, B> m_map;
	CRITICAL_SECTION* pCs;
	CRITICAL_SECTION* GetInnerCS() {
		if (pCs == NULL) {
			pCs = new CRITICAL_SECTION;
			if (pCs)InitializeCriticalSection(pCs);
		}
		return pCs;
	}
	inline std::map<A, B>& GetMap() { return m_map; }
};

template <class _ID> class _inlineData {
public:
	_ID m_data;
	size_t m_refCounts;

	_inlineData() {
		m_refCounts = 0;
		addRef();
	}
	virtual ~_inlineData() {
	}
	void addRef() {
		++m_refCounts;
	}
	void subRef() {
		if (m_refCounts == 0)
			free(this);
		else if (--m_refCounts == 0)
			free(this);
	}
	void operator delete(void *p)
	{
		((_inlineData<_ID>*)p)->subRef();
	}

	void operator delete(void *p, size_t size)
	{
		for (size_t i = 0; i < size; i++)((_inlineData<_ID>*)(p) + i)->subRef();
	}
};

template <class A> class CLMemeryManagerTemplate {
public:
	CLMemeryManagerTemplate(size_t nCounts = 0) {
		m_lst.reserve(nCounts);
		m_flag.reserve(nCounts);
	}
	virtual ~CLMemeryManagerTemplate() {
		size_t si = m_lst.size();
		for (size_t i = 0; i < si; i++)
		{
			delete m_lst[i];
			m_flag[i] = 0;
		}
		m_lst.clear();
		m_flag.clear();
	}
	A* newOne() {
		size_t si = m_lst.size();
		for (size_t i = 0; i < si; i++) {
			if (m_flag[i] == 0)
				return m_flag[i] = 1, &(m_lst[i]->m_data);
		}
		m_lst.push_back(new _inlineData<A>);
		m_flag.push_back(1);
		return &(m_lst[si]->m_data);
	}
	BOOL deleteOne(A*& p) {
		size_t si = m_lst.size();
		for (size_t i = 0; i < si; i++) {
			if (p == &(m_lst[i]->m_data)) {
				m_flag[i] = 0;
				p = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}
protected:
	std::vector<_inlineData<A>*> m_lst;
	std::vector<unsigned char> m_flag;
};

//释放std::vector的内存（而不是简单的清除元素）
template < class T >
void releaseStdVector(std::vector< T >& vt)
{
	std::vector< T > vtTemp;
	vtTemp.swap(vt);
}
//释放std::map的内存（而不是简单的清除元素）
template < class T1, class T2 >
void releaseStdMap(std::map< T1, T2>& vt)
{
	std::map< T1, T2> vtTemp;
	vtTemp.swap(vt);
}
//释放std::map的内存（而不是简单的清除元素）
template < class T1, class T2 >
void releaseStdUnorderedMap(std::unordered_map< T1, T2>& vt)
{
	std::unordered_map< T1, T2> vtTemp;
	vtTemp.swap(vt);
}
//释放std容器（而不是简单的清除元素）
template < class T1>
void releaseStd(T1& vt)
{
	T1 vtTemp;
	vtTemp.swap(vt);
}

//调试信息输出工具,请调用CLMiniDumper::create(int dumpType = 0)函数，dumpType可选MiniDumpNormal或MiniDumpWithFullMemory或其他minidump类型；
#ifndef _CL_DUMPER_H_
#define _CL_DUMPER_H_
class CLMiniDumper {
public:
	//创建调试信息输出管理工具
	static HRESULT create(int dumpType = 0);
	//释放调试信息输出管理工具,未创建则什么都不做
	static HRESULT release();

public:
	LONG writeMiniDump(EXCEPTION_POINTERS *pExceptionInfo);

private:
	void setMiniDumpFileName(void);
	BOOL getImpersonationToken(HANDLE* phToken);
	BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
	BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);

private:
	CLMiniDumper(int dumpType = 0);
	virtual ~CLMiniDumper(void);

private:
	TCHAR m_szMiniDumpPath[MAX_PATH] = { 0 };
	TCHAR m_szAppPath[MAX_PATH] = { 0 };
	int m_dumpType;
};
#endif

#ifndef _CL_CS_H_
#define _CL_CS_H_
//自定义原子自旋锁
typedef struct _at_cs {
	using typeName = _at_cs;
	_at_cs(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	LONG bLock;
	DWORD ownerThreadId;
	LONG lockCounts;
	_at_cs() noexcept;
	inline ~_at_cs() {
		bLock = FALSE;
	}
	void lock();
	void unlock();
	bool trylock();
}ATCS, * PATCS;
//Windows临界区
typedef struct _w_cs:CRITICAL_SECTION {
	using typeName = _w_cs;
	_w_cs(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	inline _w_cs() noexcept { InitializeCriticalSection(this); }
	inline ~_w_cs() { DeleteCriticalSection(this); }
	inline void lock() {EnterCriticalSection(this);	}
	inline void unlock() {LeaveCriticalSection(this);}
	inline bool trylock() {return TryEnterCriticalSection(this);}
}WCS, * PWCS;
//RWLock读写锁
typedef struct _rw_cs :SRWLOCK {
	using typeName = _rw_cs;
	_rw_cs(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	DWORD ownerThreadId;
	LONG lockCounts;
	_rw_cs() noexcept;
	void lock();
	void unlock();
	bool trylock();
	inline void lockShared() {::AcquireSRWLockShared(this);}
	inline void unlockShared() {::ReleaseSRWLockShared(this);}
	inline bool trylockShared() {return ::TryAcquireSRWLockShared(this);}
}RWCS, * PRWCS;
#include "mutex"
//c++STL锁
typedef struct _std_cs :protected std::mutex {
	using typeName = _std_cs;	
	_std_cs(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	inline _std_cs()noexcept :std::mutex() {}
	inline ~_std_cs() {}
	inline void lock() { std::mutex::lock(); }
	inline void unlock() { std::mutex::unlock(); }
	inline bool trylock() { return std::mutex::try_lock(); }
}STDCS, * PSTDCS;
//公用锁名称
using CLCS = RWCS;
#endif

//内部锁类，基类
#ifndef _CL_LOCKER_H_
#define _CL_LOCKER_H_
//内部锁类，基类
class CLCSLock
{
public:
	using cstype = RWCS;
	//using cstype = ATCS;
	//using cstype = WCS;
	//using cstype = STDCS;
	CLCSLock()noexcept{}
	CLCSLock(const CLCSLock& other){}
	CLCSLock& operator=(const CLCSLock&) {};
	~CLCSLock();	
	//主锁，请用于线程队列的增加删除操作
	inline void lock(void){
		m_cs.lock();
	}	
	inline void unlock(void){
		m_cs.unlock();
	}	
	//自由锁，可用于子类自由调用，csId在lock和unlock过程中前后必须一致
	void lock(int csId);
	//自由锁，可用于子类自由调用，csId在lock和unlock过程中前后必须一致
	void unlock(int csId);
	//做一次初始化操作，该函数是非必须的。
	//也可不调用该函数而直接使用lock(int)，但这样存在一定风险，
	//多线程同时第一次调用时可能存在访问未初始化的对象，造成同时初始化同意对象，破坏数据；
	inline void initLock(int csId) { lock(csId); unlock(csId); }
protected:
private:
	//cstype* m_cs = 0;
	cstype m_cs;
	std::map<int, cstype*>* m_csLst = 0;
};
#endif

//线程组管理类，基类
#ifndef _CL_TASKSVC_H_
#define _CL_TASKSVC_H_

//线程信息结构体
typedef class CLTaskSvcTrdSupport {
public:
	int nIndex;//线程组索引，从1开始
	int nTotals;//线程组总启动数
	HANDLE hThread;//线程真实句柄，由外部控制线程释放
	DWORD tId ;//线程id
	HANDLE quitEvent;//线程退出事件对象句柄,该句柄由本线程自身负责清除
	CLTaskSvcTrdSupport() { reset(); }
	void reset() { ZeroMemory(this,sizeof(CLTaskSvcTrdSupport)); }
}*PCLTaskSvcTrdSupport;
//线程参数结构体
typedef class CLTaskSvcTrdParam {
public:
	const CLTaskSvcTrdSupport info;;//保存该线程的在工作组内的相关信息
	DWORD_PTR extraData[10];//保存一些额外数据,用于后续函数间共享
	CLTaskSvcTrdParam(const CLTaskSvcTrdSupport& _info):info(_info){ reset(); }
	void reset() { ZeroMemory(extraData, sizeof(extraData)); }
}*PCLTaskSvcTrdParam;
//检查消息队列是否由WM_QUIT消息，并移除该消息
BOOL CHECK_WM_QUIT();
//线程组管理类，基类
class CLTaskSvc:public CLCSLock
{
public:
	//start用于激活一定数量的工作者线程，默认激活数量为1。返回当前线程队列大小。第二个参数isDefaultSuspend = TRUE 时候应该显示调用resume用于恢复挂起的线程组；
	//函数为非阻塞函数，在创建或启动线程组后将立即返回，若要实现线程组与调用线程同步，请在函数调用后执行wait操作等待线程组全部退出；
	//num = 0则启动cpu核心数量的线程；
	virtual size_t start(int num = 1,BOOL isDefaultSuspend = FALSE);
	//手动重启线程组，当start调用的第二个参数isDefaultSuspend = TRUE时候应该显示调用
	void resume();
	//手动挂起线程组
	void suspend();
	//手动挂起线程组
	virtual void pause() { suspend(); }
	//获取线程队列大小
	size_t getThreadsNum() const;
	//获取正在运行线程数
	size_t getActivateThreadsNum() const;
	//线程组是否正在运行
	BOOL isRunning() const {return getActivateThreadsNum() > 0 ? TRUE : FALSE;	}
	//取得cpu核心数
	static DWORD getCpuCoreCounts();	 
	void setWaitTimeToQuit(DWORD dt = INFINITE) { m_waitTimeToQuit = dt; }
	DWORD getWaitTimeToQuit() const {return m_waitTimeToQuit;}

	virtual ~CLTaskSvc();
	//close用于等待线程结束并关闭线程，退出线程由子类控制。(该方法内部使用3种不同方式)
	virtual void close();

	typedef std::map<DWORD, CLTaskSvcTrdSupport> ThreadsTable;
	const ThreadsTable& getThreadsTable() const { return vec_threads; }

	//外部调用线程调用此函数等待线程组退出，sensitivityMinsec表示一个等待判断的灵明度，毫秒记，越小表示一个等待判断周期越短
	//（该参数并不表示等待的最大时间而是判断间隔），返回等待总时间毫秒；
	ULONGLONG wait(UINT sensitivityMinsec = 100)const {
		ULONGLONG  sp = 0; 
		do {
			Sleep(sensitivityMinsec);
			sp += sensitivityMinsec;
		} while (isRunning());
		return sp;
	}
	//设置优先级，默认THREAD_PRIORITY_NORMAL
	void setPriority(int _nPriority = THREAD_PRIORITY_NORMAL) {
		nPriority = _nPriority;
	}
protected:
	//只有子类才可以构造父类，拒绝外部访问构造类实例
	CLTaskSvc();
	//线程启动时最先运行的内部初始化过程函数，必须返回TRUE，才会执行后续的run，否则直接进入exist
	virtual BOOL init(PCLTaskSvcTrdParam var); 
	//线程主工作体,纯虚函数需子类实现
	virtual DWORD run(PCLTaskSvcTrdParam var) = 0;
	//线程退出时的最后运行函数，线程返回值由该函数决定
	virtual DWORD exist(DWORD runReturnValue, PCLTaskSvcTrdParam var);

private:
	//工作者线程访问接口
	static DWORD workThread(LPVOID param);
	//线程队列表
	ThreadsTable vec_threads;

	DWORD m_waitTimeToQuit = INFINITE;

	int nPriority ;
};

//判断quit事件对象,有信号就退出循环
#define CHECK_EVENT_QUIT( hQuitEvent ) \
	((hQuitEvent) && ((hQuitEvent) != INVALID_HANDLE_VALUE) && (::WaitForSingleObject((hQuitEvent), 0) == WAIT_OBJECT_0))
//检查退出事件是否为通知状态，是则退出
#define WHEN_EVENT_QUIT_ARRIVE_BREAK_LOOP( hQuitEvent ) \
	if (CHECK_EVENT_QUIT( hQuitEvent ))break;

//判断WM_QUIT消息到来,有信号就退出循环
#define WHEN_WM_QUIT_ARRIVE_BREAK_LOOP() \
	if(CHECK_WM_QUIT())break;

//判断WM_QUIT消息和quit事件到来,有信号就退出循环
#define CHECK_WM_QUIT_OR_EVENT_QUIT_ARRIVE(hQuitEvent) \
	(CHECK_EVENT_QUIT( hQuitEvent ) || CHECK_WM_QUIT() )
#define IS_WM_QUIT_OR_EVENT_QUIT_ARRIVE() \
	(CHECK_EVENT_QUIT( var->info.quitEvent ) || CHECK_WM_QUIT() )

//由当前线程编号，重总数中分拣出本线程的执行区间下标编号，规则为：nIndexStart <= i < nIndexEnd;
#define GET_TRD_SECTION_TASK(var , nTotalSize , nIndexStart , nIndexEnd ) \
	size_t nIndexStart, nIndexEnd;\
	getTrdSection((nTotalSize), ((var)->info.nTotals), ((var)->info.nIndex - 1), (nIndexStart), (nIndexEnd));

#endif

//并行许可类
#ifndef _CL_PARALLELPASS_H_
#define _CL_PARALLELPASS_H_
//并行许可类，主要实现功能：
//多线程并行中，由“主线程”控制“分支线程组”的代码段的执行及自旋等待控制，如下：
//主线程：分发出许可后就自旋等待所有分支线程（执行直至结束相应代码）归还许可；
//分支线：程不断自旋等待获取到对应许可后（并返回主线程发出的附带指令orderKey）后续执行代码，直到主动归还以获取的对应许可；
//主线程：收到全部分支线程归还的许可后结束自旋等待，执行后续代码；
class CLParallelPass
{
public:
	CLParallelPass(UINT passCounts = 0);
	CLParallelPass(const CLParallelPass& bar) = delete;
	CLParallelPass& operator=(const CLParallelPass& bar) = delete;
	virtual ~CLParallelPass();
	CLParallelPass& init(UINT passCounts);
	UINT waitPass(UINT passIndex);
	void giveBackPass(UINT passIndex);
	void handOutPass(UINT orderKey = 0);
	void reclaimAllPass();
	UINT getPassCounts() const;
protected:
private:
	UINT m_counts = 0;
	UINT m_bufCounts = 0;
	UCHAR* flag = 0;
	UINT m_key = 0;
	const UINT m_core = CLTaskSvc::getCpuCoreCounts();
};
#endif

//原子操作类
#ifndef _CL_ATOMIC_H_
#define _CL_ATOMIC_H_
//默认状态下CLAtomic类对象的原子操作模式是否启动，原子操作在保证线程安全情况下将降低代码效率；
#define CLAtomic_bUseAtomc_def true

//原子操作类（模板）
template<class T>
class CLAtomic {	
	static_assert(sizeof(T) % 2 == 0, "CLAtomic: target type is not support!");
protected:
	T Target;
	bool bUseAtomc;

	/*inline static bool atomicCAS(CHAR* dest, CHAR newvalue, CHAR oldvalue)
	{
		SHORT cov = *reinterpret_cast<SHORT volatile*>(dest);cov &= 0xff00;
		SHORT new1 = newvalue;new1 |= cov;
		SHORT old = oldvalue;old |= cov;
		SHORT tem = InterlockedCompareExchange16(reinterpret_cast<SHORT volatile*>(dest),new1,old);
		return tem == old ? true : false;
	}*/
	/*inline static bool atomicCAS(UCHAR* dest, UCHAR newvalue, UCHAR oldvalue)
	{
		return atomicCAS(reinterpret_cast<CHAR volatile*>(dest), CHAR(newvalue), CHAR(oldvalue));
	}*/
	inline static bool atomicCAS(volatile SHORT* dest, SHORT newvalue, SHORT oldvalue)
	{
		return InterlockedCompareExchange16(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile USHORT* dest, USHORT newvalue, USHORT oldvalue)
	{
		return atomicCAS(reinterpret_cast<SHORT volatile*>(dest), SHORT(newvalue), SHORT(oldvalue));
	}
	inline static bool atomicCAS(volatile INT* dest, INT newvalue, INT oldvalue)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)newvalue, (LONG)oldvalue) == (LONG)oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile UINT* dest, UINT newvalue, UINT oldvalue)
	{
		return atomicCAS(reinterpret_cast<INT volatile*>(dest), INT(newvalue), INT(oldvalue));
	}
	inline static bool atomicCAS(volatile LONG* dest, LONG newvalue, LONG oldvalue)
	{
		return InterlockedCompareExchange(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile ULONG* dest, ULONG newvalue, ULONG oldvalue)
	{
		return atomicCAS(reinterpret_cast<LONG volatile*>(dest), LONG(newvalue), LONG(oldvalue));
	}
	inline static bool atomicCAS(volatile LONG64* dest, LONG64 newvalue, LONG64 oldvalue)
	{
		return InterlockedCompareExchange64(dest,newvalue,oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile ULONG64* dest, ULONG64 newvalue, ULONG64 oldvalue)
	{
		return atomicCAS(reinterpret_cast<LONG64 volatile*>(dest), LONG64(newvalue), LONG64(oldvalue));
	}
	inline static bool atomicCAS(volatile FLOAT* dest, FLOAT newvalue, FLOAT oldvalue)
	{
		auto old1 = *(LONG*)(void*)&oldvalue;
		return InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(dest), *(LONG*)(void*)&newvalue,old1) == old1 ? true : false;
	}
	inline static bool atomicCAS(volatile DOUBLE* dest, DOUBLE newvalue, DOUBLE oldvalue)
	{
		auto old1 = *(LONG64*)(void*)&oldvalue;
		return InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(dest), *(LONG64*)(void*)&newvalue,old1) == old1 ? true : false;
	}
	inline void set(T dest) {
		throw logic_error("CLAtomic::set is not impliment!");
		Target = dest;
	}
	inline T increment()
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old + T(1), old));
		return old;
	}
	inline T decrement()
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old - T(1), old));
		return old;
	}
	inline T add(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old + v, old));
		return old;
	}
	inline T mul(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old * v, old));
		return old;
	}
	inline T div(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old / v, old));
		return old;
	}
	inline T surplus(volatile int v)
	{
		T old;
		do {
			old = Target;
		} while (!CLAtomic::atomicCAS(&Target, old % int(v), old));
		return old;
	}
public:
	using value_type = T;

	inline CLAtomic():bUseAtomc(CLAtomic_bUseAtomc_def){
	}
	inline CLAtomic(T v, bool _bUseAtomc = CLAtomic_bUseAtomc_def)
		:Target(v), bUseAtomc(_bUseAtomc) {
	}
	inline CLAtomic(const CLAtomic& v)
		:Target(v.Target), bUseAtomc(v.bUseAtomc) {
	}
	template<class T2>
	inline CLAtomic(const CLAtomic<T2>& v)
		:Target(v()), bUseAtomc(v.isUseAtomic()) {
	}
	CLAtomic& setUseAtomic(bool _bUseAtomc = CLAtomic_bUseAtomc_def) {
		return bUseAtomc = _bUseAtomc,*this;
	}
	bool isUseAtomic() const {
		return bUseAtomc;
	}
	template<class T2>
	inline CLAtomic& operator=(const T2 v2) {
		return Target = v2, *this;
	}
	template<class T2>
	inline CLAtomic& operator=(const CLAtomic<T2>& v2) {
		return Target = v2(), *this;
	}
	inline CLAtomic& operator=(const CLAtomic& v2) {
		return Target = v2.Target, *this;
	}
	inline CLAtomic& operator=(const T& v2) {
		return Target = v2, *this;
	}
	//默认为原子操作++()，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator++() {
		return bUseAtomc ? increment() : ++Target, * this;
	}
	//默认为原子操作--()，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator--() {
		return bUseAtomc ? decrement() : --Target, * this;
	}
	//默认为原子操作()++，当setUseAtomic(false)时采用非原子操作方式；
	inline T operator++(int) {
		if (bUseAtomc) {
			return increment();
		}
		else {
			T old = Target;  
			++Target;
			return old;
		}
	}
	//默认为原子操作()--，当setUseAtomic(false)时采用非原子操作方式；
	inline T operator--(int) {
		if (bUseAtomc) {
			return decrement();
		}
		else {
			T old = Target;  
			--Target;
			return old;
		}
	}
	//默认为原子操作+=，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator+=(T v) {
		return  bUseAtomc ? add(v) : Target += v, * this;
	}
	//默认为原子操作-=，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator-=(T v) {
		return  bUseAtomc ? add(0 - v) : Target -= v, * this;
	}
	//默认为原子操作*=，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator*=(T v) {
		return  bUseAtomc ? mul(v) : Target *= v, * this;
	}
	//默认为原子操作/=，当setUseAtomic(false)时采用非原子操作方式；
	inline CLAtomic& operator/=(T v) {
		return  bUseAtomc ? div(v) : Target /= v, * this;
	}
	//默认为原子操作%=，当setUseAtomic(false)时采用非原子操作方式；
	inline  CLAtomic& operator%=(int v) {
		return  bUseAtomc ? surplus(v) : Target %= v, *this;
	}
	inline operator T() const noexcept { return Target; }
	inline operator T() const volatile noexcept { return Target; }
	inline operator T& () noexcept { return Target; }
	inline operator T& () volatile noexcept { return Target; }
	inline T operator()() const noexcept { return Target; }
	inline T operator()() const volatile noexcept { return Target; }
	template<class T2> inline operator T2() const { return (T2)Target; }
	template<class T2> inline T operator+(const T2 v) const { return Target + v; }
	template<class T2> inline T operator-(const T2 v) const { return Target - v; }
	template<class T2> inline T operator*(const T2 v) const { return Target * v; }
	template<class T2> inline T operator/(const T2 v) const { return Target / v; }
	template<class T2> inline T operator%(const T2 v) const { return Target % (int)v; }
};

template<class T,class T2> inline T operator+(const T v,const CLAtomic<T2>& tag){ 
	return v + tag; 
}
template<class T,class T2> inline T operator-(const T v,const CLAtomic<T2>& tag){ 
	return v - tag; 
}
template<class T,class T2> inline T operator*(const T v,const CLAtomic<T2>& tag){ 
	return v * tag; 
}
template<class T,class T2> inline T operator/(const T v,const CLAtomic<T2>& tag){ 
	return v / tag; 
}
template<class T,class T2> inline T operator%(const T v,const CLAtomic<T2>& tag){ 
	return v % (int)tag; 
}
#endif

//本类实现的效能较低，请使用c++11以上提供的unordered_map
#ifndef _CL_HASHMAP_H_
#define _CL_HASHMAP_H_

template<class Key, class Value>
class HashNode
{
public:
	Key    _key;
	HashNode *next;
	Value  _value;	

	HashNode(Key key, Value value)
	{
		_key = key;
		_value = value;
		next = NULL;
	}
	virtual ~HashNode()	{}
};

//本类实现的效能较低，请使用c++11以上提供的unordered_map
template <class Key, class Value, class HashFunc, class EqualKey>
class HashMap
{
public:
	HashMap(size_t size) : _size(size),ValueNULL(Value())
	{
		hash = HashFunc();
		equal = EqualKey();
		table = new HashNode<Key, Value>*[_size];
		ZeroMemory(table, _size * sizeof(HashNode<Key, Value>*));
	}

	virtual ~HashMap()
	{
		for (size_t i = 0; i < _size; i++)
		{
			HashNode<Key, Value> *currentNode = table[i];
			while (currentNode)
			{
				HashNode<Key, Value> *temp = currentNode;
				currentNode = currentNode->next;
				delete temp;
			}
		}
		delete table;
	}


	HashNode<Key, Value>& insert(const Key& key, const Value& value)
	{
		return _insert(key, value, _getIndex(key));
	}

	bool del(const Key& key)
	{
		size_t index = _getIndex(key);
		HashNode<Key, Value> * node = table[index];
		HashNode<Key, Value> * prev = NULL;
		while (node)
		{
			if (node->_key == key)
			{
				if (prev == NULL)
				{
					table[index] = node->next;
				}
				else
				{
					prev->next = node->next;
				}
				delete node;
				return true;
			}
			prev = node;
			node = node->next;
		}
		return false;
	}	

	Value* find(const Key& key)
	{
		return _find(key, _getIndex(key));
	}


	Value& operator [](const Key& key)
	{
		size_t index = _getIndex(key);
		Value* pv =  _find(key, index);
		if (pv)
			return *pv;
		else
			return _insert(key, ValueNULL,index)._value;
	}
private:

	HashNode<Key, Value> **table;
	size_t _size;
	const Value ValueNULL;	
	HashFunc hash;
	EqualKey equal;

	inline size_t _getIndex(const Key& key){
		return hash(key) % _size;
	}

	HashNode<Key, Value>& _insert(const Key& key, const Value& value, size_t index)
	{
		HashNode<Key, Value> * node = new HashNode<Key, Value>(key, value);
		node->next = table[index];
		table[index] = node;
		return *node;
	}
	Value* _find(const Key& key, size_t index)
	{
		if (table[index] == NULL)
			return NULL;
		else
		{
			HashNode<Key, Value> * node = table[index];
			while (node)
			{
				if (node->_key == key)
					return &node->_value;
				node = node->next;
			}
			return NULL;
		}
	}
};

//首先要定义hash函数与比较函数
class HashFunc_string
{
public:
	int operator()(const std::tstring & key)
	{
		int hash = 0;
		size_t si = key.length();
		for (size_t i = 0; i < si; ++i)
		{
			hash = hash << 7 ^ key[i];
		}
		return (hash & 0x7FFFFFFF);
	}
};
class EqualKey_string
{
public:
	bool operator()(const std::tstring & A, const std::tstring & B)
	{
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};

//key为string的哈希映射
//本类实现的效能较低，请使用c++11以上提供的unordered_map
template <typename classTag>
class CLHashmap :public HashMap<std::tstring, classTag, HashFunc_string, EqualKey_string> {
public:
	CLHashmap(size_t si) :HashMap<std::tstring, classTag, HashFunc_string, EqualKey_string>(si) {}
};

#endif

//显示com错误信息,yes表示抛异常
HRESULT checkComErrorManual();
//打开EXCEL文件，并转存为CSV格式文件,返回结果：S_OK，成功；S_FALSE，失败
HRESULT converXlsToCsv(LPCTSTR pSrcFileName, LPCTSTR pDesFileName);
/*将Url指向的地址的文件下载到save_as指向的本地文件*/
BOOL downloadUrlFile(LPCTSTR Url, LPCTSTR save_as);
/*将Url指向的地址的文件下载到buf，buf和nBufSize不为0则表示已经存在缓冲区,buf使用后需要delete删除，save_as指向的本地文件地址则保存到指定文件*/
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT char*& buf, IN OUT size_t & nBufSize, OUT size_t & nDataSize);
typedef std::vector<char> DataBuffer;//数据缓冲区
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT DataBuffer& buf);
//把缓冲区数据写入到文件
BOOL writeBufToFile(LPCTSTR saveFile, const char* buf, size_t nDataSize, LPDWORD writeNumber = 0);


//退行输出，返回本行输出字符长度，参数1：表示要退格回删的字符数（该值应该为上一次调用函数成功的返回值，以实现单行的回删覆盖显示）
int printfWithBackCover(int bakCoverChars, const TCHAR* szFormat, ...); 
#define printfbc printfWithBackCover //退行输出，返回本行输出字符长度，参数1：表示要退格回删的字符数（该值应该为上一次调用函数成功的返回值，以实现单行的回删覆盖显示）
//删除最后的一行输出，bakCoverChars表示最后一行字符个数，应该是printfWithBackCover调用的返回值
int printfWithBackCoverEnd(int bakCoverChars);
#define printfbce printfWithBackCoverEnd //删除最后的一行输出，bakCoverChars表示最后一行字符个数，应该是printfWithBackCover调用的返回值

//取得线程分段,ssi总数据量，totals分段数，index当前序号从0开始，is起始编号，ie结束编号（最后一个对象的后一个位置）
void getTrdSection(size_t ssi, size_t totals, size_t index, OUT size_t &is, OUT size_t &ie);

#ifndef __CL_WINDOWS_SYSTEM_H__
#define __CL_WINDOWS_SYSTEM_H__
#include <windows.h>
#include "tchar.h"
#include "shellapi.h"
#include <iostream>
#include <string>
#pragma comment(lib, "shell32")
#pragma comment(lib, "user32")
#pragma comment(lib, "Advapi32.lib")

//以普通权限启动进程
BOOL createProcessLow(TCHAR * lpApplicationName,
	TCHAR * lpCommandLine = NULL,
	TCHAR * lpDirectory = NULL,
	UINT nShow = SW_SHOWNORMAL);
//以管理员权限启动进程
BOOL createProcessHigh(TCHAR * strProcessName,
	TCHAR * strCommandLine = NULL,
	TCHAR * lpDirectory = NULL,
	UINT nShow = SW_SHOWNORMAL);

typedef BOOL(WINAPI *PCreateProcessWithToken)(
	__in          HANDLE hToken,
	__in          DWORD dwLogonFlags,
	__in          LPCTSTR lpApplicationName,
	__in          LPTSTR lpCommandLine,
	__in          DWORD dwCreationFlags,
	__in          LPVOID lpEnvironment,
	__in          LPCTSTR lpCurrentDirectory,
	__in          LPSTARTUPINFO lpStartupInfo,
	__out         LPPROCESS_INFORMATION lpProcessInfo
	);

HANDLE dupExplorerToken();
BOOL isVistaOrLater();
BOOL isAdminPrivilege();
BOOL isInstallService(LPCTSTR servName);
BOOL installService(LPCTSTR servName, LPCTSTR lpszBinaryPathName=0, LPCTSTR lpServiceDisplayName=0, LPCTSTR lpDescription=0);
BOOL uninstallService(LPCTSTR servName);
BOOL startService(LPCTSTR servName);
BOOL stopService(LPCTSTR servName);


//带定时功能的，消息框
int messageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, DWORD dwMilliseconds = INFINITE);
int messageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, DWORD dwMilliseconds = INFINITE);
#ifdef UNICODE
#define messageBoxTimeout messageBoxTimeoutW
#define messageBoxT messageBoxTimeoutW
#else
#define messageBoxTimeout messageBoxTimeoutA
#define messageBoxT messageBoxTimeoutA
#endif

 class CLString;
 //取得cpu完整信息，包括cpu型号名，cpu类型名，cpu核心线程数，cpu主频(GHz)
 void getCpuInfo(CLString& chProcessorName, CLString& chProcessorType, DWORD& dwCoreNum, DWORD& dwMaxClockSpeed);

 //取得控台窗口句柄
 inline HWND getConsoleHwnd(void)
 {
	 HWND hwndFound;         // This is what is returned to the caller.
	 CHAR pszNewWindowTitle[6000]; // Contains fabricated
	 // WindowTitle.
	 CHAR pszOldWindowTitle[6000]; // Contains original
	 if (GetConsoleTitleA(pszOldWindowTitle, 6000)) {
		 sprintf_s(pszNewWindowTitle, "Cmd temp names: %I64d/%d.", GetTickCount64(), GetCurrentProcessId());
		 SetConsoleTitleA(pszNewWindowTitle);
		 int i = 0;
		 do {
			 Sleep(10);
			 hwndFound = FindWindowA(NULL, pszNewWindowTitle);
			 if (hwndFound) {
				 SetConsoleTitleA(pszOldWindowTitle);
				 return hwndFound;
			 }
		 } while (++i < INT_MAX);
		 throw 0;
	 }
	 else return NULL;
 }

 class CLCpuDelay;
 //取得cpu当前使用率
 class CLCpuUsage
 {
 public:
	 CLCpuUsage();
	 ~CLCpuUsage();
	 int GetUsage();//取得cpu当前使用率
 private:
#define CLCDelay_DELAY_DIFF	200
#define CLCDelay_DATA_COUNT  (1000/CLCDelay_DELAY_DIFF)
	 static CLCpuDelay	s_delay;
	 static int		s_count;
	 static int      s_index;
	 static int		s_lastCpu;
	 static int      s_cpu[CLCDelay_DATA_COUNT];
	 static __int64 s_time;
	 static __int64 s_idleTime;
	 static __int64 s_kernelTime;
	 static __int64 s_userTime;
	 CRITICAL_SECTION m_lock;
 };

#ifndef __CL_TICK_DEF__
#define __CL_TICK_DEF__
 //高精度计时器类
 class CLTick {
 protected:
	 LARGE_INTEGER lis;
	 LARGE_INTEGER lie;
	 LARGE_INTEGER Freg;
 public:
	 CLTick() { 
		 timingStart();
	 }
	 //开始计时
	 CLTick& timingStart() {
		 QueryPerformanceFrequency(&Freg);
		 QueryPerformanceCounter(&lis);
		 return *this;
	 }
	 //取得从计时开始到当前的时间
	 double getSpendTime(bool saveToStart = false) {
		 QueryPerformanceCounter(&lie);
		 double rt = double(lie.QuadPart - lis.QuadPart) / double(Freg.QuadPart);
		 if (saveToStart)lis = lie;
		 return rt;
	 }
 };
#endif

#endif

#endif