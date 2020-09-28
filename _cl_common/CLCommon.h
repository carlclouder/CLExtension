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

//����ȽϺ�
#define BETWEEN(x,a,b)  (( (a) <= (x) && (x) <= (b) ) || ( (b) <= (x) && (x) <= (a) ))
#define BETWEENO(x,a,b)  (( (a) < (x) && (x) < (b) ) || ( (b) < (x) && (x) < (a) ))

//�����������������õĺ�
#define _RETSELF_ \
	return *this;

//���������е����ֵ�ĺ�
#define listMax( var , lst , lstSize ) \
	(var) = (lst)[0];\
	for (size_t _ise231=1,_si = (lstSize);_ise231 < _si;_ise231++)if( (var) < (lst)[_ise231]) (var) = (lst)[_ise231];
#define listMaxI( var , lst , lstSize ,index ) \
	(var) = (lst)[(index) = 0];\
	for (size_t _ise231=1,_si = (lstSize);_ise231 < _si;_ise231++)if( (var) < (lst)[_ise231]) (var) = (lst)[(index) = _ise231];
//����vector�е����ֵ�ĺ�
#define vectorMax( var , lst ) listMax(var,lst,(lst).size())
#define vectorMaxI( var , lst ,index ) listMaxI(var,lst,(lst).size(),index)

//�������к�( ��ʱ���� �� Ŀ��������vec �� ԭ�������� �� ԭ�����Ա�� )
#define vectorAesOdr( t , m_vtOff , offset , nSi ) \
{\
	(m_vtOff).clear();\
	for(size_t ci=0;ci<(nSi);ci++)m_vtOff.push_back(offset[ci]);\
	for (size_t ci = 0;ci<(nSi);ci++){for (size_t cj = ci+1;cj<nSi;cj++){	if( (m_vtOff[ci]) > (m_vtOff[cj]) ){t = m_vtOff[ci];m_vtOff[ci] = m_vtOff[cj];m_vtOff[cj] = t;}}}\
}
//�������к�( ��ʱ���� �� Ŀ��������vec �� ԭ�������� �� ԭ�����Ա�� )
#define vectorDesOdr( t , m_vtOff , offset , nSi ) \
{\
	(m_vtOff).clear();\
	for(size_t ci=0;ci<(nSi);ci++)m_vtOff.push_back(offset[ci]);\
	for (size_t ci = 0;ci<(nSi);ci++){for (size_t cj = ci+1;cj<nSi;cj++){	if( (m_vtOff[ci]) < (m_vtOff[cj]) ){t = m_vtOff[ci];m_vtOff[ci] = m_vtOff[cj];m_vtOff[cj] = t;}}}\
}

#ifndef _CL_COMMON_MATH_DEF_
#define _CL_COMMON_MATH_DEF_

typedef double VT;
//VT����
typedef std::vector<VT> VD;
typedef VD& VD_R;
typedef const VD& VD_RC;
//VT����ѡ����
typedef std::vector<VT>::iterator VDI;
typedef VDI& VDIR;
typedef std::vector<VT>::const_iterator VDIC;
typedef VDIC& VDICR;
//VT����
typedef const VT* VTArray;
typedef size_t VTArraySize;

//���
VT SUM(VD_RC vec);
//��ֵ
VT AVG(VD_RC vec);
//����(piȨ��)
VT E(VD_RC v_X, VD_RC v_pi);
VT E(VD_RC v_X);
//����(piȨ��)
VT VAR(VD_RC v_X, VD_RC v_pi);
VT VAR(VD_RC v_X);
//Э����(pixȨ�أ�piyȨ��)
VT COV(VD_RC v_X, VD_RC v_Y, VD_RC v_pix, VD_RC v_piy);
VT COV(VD_RC v_X, VD_RC v_Y);
//��׼�piyȨ��)
VT DVA(VD_RC v_X, VD_RC v_pix);
VT DVA(VD_RC v_X);
//�����ϵ��(pixȨ�أ�piyȨ��)
VT COR(VD_RC v_X, VD_RC v_Y, VD_RC v_pix, VD_RC v_piy);
VT COR(VD_RC v_X, VD_RC v_Y);

//---------------------------------------
//���
VT SUM(VTArray vec, VTArraySize si);
//��ֵ
VT AVG(VTArray vec, VTArraySize si);
//����(piȨ��)
VT E(VTArray v_X, VTArray v_pi, VTArraySize si);
VT E(VTArray v_X, VTArraySize si);
//����(piȨ��)
VT VAR(VTArray v_X, VTArray v_pi, VTArraySize si);
VT VAR(VTArray v_X, VTArraySize si);
//Э����(pixȨ�أ�piyȨ��)
VT COV(VTArray v_X, VTArray v_Y, VTArray v_pix, VTArray v_piy, VTArraySize si);
VT COV(VTArray v_X, VTArray v_Y, VTArraySize si);
//��׼�piyȨ��)
VT DVA(VTArray v_X, VTArray v_pix, VTArraySize si);
VT DVA(VTArray v_X, VTArraySize si);
//�����ϵ��(pixȨ�أ�piyȨ��)
VT COR(VTArray v_X, VTArray v_Y, VTArray v_pix, VTArray v_piy, VTArraySize si);
VT COR(VTArray v_X, VTArray v_Y, VTArraySize si);

//����һ�������еķֲ������
//result��T1��ʾĳһ�ֲ����������ֵ���Ǿ�ֵ����T2��ʾ�ֲ�Ƶ���������ִ�����,   block_%zd (%zd)��Ϊ0��ʾ��������Ӧƽ�����)
template<class T1, class T2, class T3>void dataToDistribution(std::map<T1, T2>& result, const T3* dataList, size_t dataCounts, size_t sectionCounts,
	T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	assert(dataList != nullptr && dataCounts != 0);
	typename std::map<T1, T2>::const_iterator i, i2;
	T1 vmax = dataList[0], vmin = dataList[0];
	size_t agtimes = sectionCounts > 0 ? 100 : 0;
	if (sectionCounts == 0)sectionCounts = 100;
	for (size_t k = 0; k < dataCounts; k++) {
		if (dataList[k] > vmax)vmax = dataList[k];
		if (dataList[k] < vmin)vmin = dataList[k];
	}
	if (savedLowerLimitValue)
		*savedLowerLimitValue = vmin;
	if (savedUpperLimitValue)
		*savedUpperLimitValue = vmax;

ag:
	result.clear();
	T1 sec = T1(vmax - vmin) / max(1, sectionCounts);
	for (size_t k = 0; k < dataCounts; ++k)
	{
		T1 i = dataList[k];
		if (i >= vmax) i -= sec;
		auto x = T1((double(size_t((i - vmin) / sec)) + 0.5) * sec + vmin);
		result[x] += T2(1);
	}
	if (++agtimes >= 100) //��������
		return;
	//����Ӧ
	sectionCounts = result.size();
	i2 = result.cbegin(); ++i2;
	for (i = result.cbegin(); i != result.cend() && i2 != result.cend(); ++i, ++i2)
	{
		if ((i->second * 0.6 / 1.6 >= 1 && i->second * 1.0 < i2->second * 0.6) || (i2->second * 0.6 / 1.6 >= 1 && i->second * 0.6 > i2->second * 1.0)) {
			sectionCounts *= 2;
			goto ag;
		}
	}
}
//����һ�������еķֲ������
//result��T1��ʾĳһ�ֲ����������ֵ���Ǿ�ֵ����T2��ʾ�ֲ�Ƶ���������ִ�����, sectionCounts�ֲ�������(��Ϊ0��ʾ��������Ӧƽ�����)
template<class T1, class T2, class T3>void dataToDistribution(std::map<T1, T2>& result, const vector<T3>& dataList, size_t sectionCounts,
	T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	return dataToDistribution(result, dataList.data(), dataList.size(), sectionCounts, savedLowerLimitValue, savedUpperLimitValue);
}
//����һ�������еķֲ������
//resultDistRange��ʾĳһ�ֲ����������ֵ���Ǿ�ֵ����resultDistFrequency��ʾ�ֲ�Ƶ���������ִ�����, sectionCounts�ֲ�������(��Ϊ0��ʾ��������Ӧƽ�����)
template<class T1, class T2, class T3>void dataToDistribution(std::vector<T1>& resultDistRange, std::vector<T2>& resultDistFrequency,
	const T3* dataList, size_t dataCounts, size_t sectionCounts, T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	std::map<T1, T2> ret;
	dataToDistribution(ret, dataList, dataCounts, sectionCounts, savedLowerLimitValue, savedUpperLimitValue);
	resultDistRange.reserve(ret.size()); resultDistRange.clear();
	resultDistFrequency.reserve(ret.size()); resultDistFrequency.clear();
	for (auto& i : ret)
	{
		resultDistRange.push_back(i.first);
		resultDistFrequency.push_back(i.second);
	}
}
//����һ�������еķֲ������
//resultDistRange��ʾĳһ�ֲ����������ֵ���Ǿ�ֵ����resultDistFrequency��ʾ�ֲ�Ƶ���������ִ�����, sectionCounts�ֲ�������(��Ϊ0��ʾ��������Ӧƽ�����)
template<class T1, class T2, class T3>void dataToDistribution(std::vector<T1>& resultDistRange, std::vector<T2>& resultDistFrequency,
	const vector<T3>& dataList, size_t sectionCounts, T1* savedLowerLimitValue = nullptr, T1* savedUpperLimitValue = nullptr) {
	return dataToDistribution(resultDistRange, resultDistFrequency, dataList.data(), dataList.size(),
		sectionCounts, savedLowerLimitValue, savedUpperLimitValue);
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
	* �жϵ�ǰ�߶��Ƿ���������ĵ�
	* �������ĵ��Ƿ��ڵ�ǰ����
	*/
	static bool lineIsContainsPoint(const Line& line, const Point& point) {
		bool result = false;
		//�жϸ�����point��˵�1�����߶ε�б���Ƿ�͵�ǰ�߶ε�б����ͬ
		//������point��˵�1�����߶ε�б��k1
		Float k1 = 0;
		bool needjudgment = true;
		if (double_Equal(point.x, line.pts.x)) {
			//k1 = -DBL_MAX;
			needjudgment = false;
		}
		else {
			k1 = div(sub(point.y, line.pts.y), sub(point.x, line.pts.x));
		}
		//��ǰ�߶ε�б��k2
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
				//��б����ͬ�������жϸ�����point��x�Ƿ���pointA.x��pointB.x֮��,���� ��˵���õ��ڵ�ǰ����
				if (sub(point.x, line.pts.x) * sub(point.x, line.pte.x) < 0) {
					result = true;
				}
			}
		}
		return result;
	}
	//���
	static Float mult(const Point& a, const Point& b, const Point& c)
	{
		return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
	}
	/**
		* �����߶��Ƿ��뵱ǰ�߶��ཻ
		* �ཻ����true, ���ཻ����false
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
	//����εĸ�����Ҫ���������ӵ������
	static bool pointIsInPolygon(const Point& point, const Polygon& poly) {
		bool result = false;
		int intersectCount = 0;
		// �ж����ݣ����Ӹõ����ҷ�����ˮƽ�����������θ��ߵĽ��㣬��������Ϊ�����������ڲ�
		//����Ҫע�⼸�����������1�����ڱ߻��߶�����;2�����ڱߵ��ӳ�����;3���������ˮƽ�����������ཻ�ڶ�����
		/**
		* ���岽�����£�
		* ѭ�����������߶Σ�
		*  1���жϵ��Ƿ��ڵ�ǰ����(б����ͬ,�Ҹõ��xֵ�������˿ڵ�xֵ֮��),�����򷵻�true
		*  2�������ж��ɸõ㷢����ˮƽ�����Ƿ��뵱ǰ���ཻ,�����ཻ��continue
		*  3�����ཻ,���ж��Ƿ��ཻ�ڶ�����(�ߵĶ˵��Ƿ��ڸ������ˮƽ�Ҳ�).������,����Ϊ�˴��ཻΪ��Խ,������+1 ��continue
		*  4�������ڶ�����,���ж���һ���ߵ�����һ���˵��뵱ǰ�ߵ�����һ���˵��Ƿ�ֲ���ˮƽ���ߵ�����.��������Ϊ�˴��ཻΪ��Խ,������+1.
		*/
		for (size_t i = 0; i < poly.size(); i++) {
			const Point& pointA = poly[i];
			Point pointB;
			Point pointPre;
			//����ǰ�ǵ�һ����,����һ������list��������һ����
			if (i == 0) {
				pointPre = poly[poly.size() - 1];
			}
			else {
				pointPre = poly[i - 1];
			}
			//���Ѿ�ѭ�������һ����,����֮���ӵ��ǵ�һ����
			if (i == (poly.size() - 1)) {
				pointB = poly[0];
			}
			else {
				pointB = poly[i + 1];
			}
			Line line = { pointA, pointB };
			//1���жϵ��Ƿ��ڵ�ǰ����(б����ͬ,�Ҹõ��xֵ�������˿ڵ�xֵ֮��),�����򷵻�true
			bool isAtLine = lineIsContainsPoint(line, point);
			if (isAtLine) {
				return true;
			}
			else {
				//2�������ڱ���,�ж��ɸõ㷢����ˮƽ�����Ƿ��뵱ǰ���ཻ,�����ཻ��continue
				//���ø����ߵ�����һ���˵��xֵ=999,��֤�ߵ�x��Զ������
				Point  radialPoint = { 180, point.y };
				Line radial = { point, radialPoint };
				//�����߶��Ƿ��뵱ǰ�߶��ཻ �ཻ����true
				bool isIntersect = linesIsIntersect(radial, line);
				if (!isIntersect) {
					continue;
				}
				else {
					//3�����ཻ,���ж��Ƿ��ཻ�ڶ�����(�ߵĶ˵��Ƿ��ڸ������ˮƽ�Ҳ�).������,����Ϊ�˴��ཻΪ��Խ,������+1 ��continue
					if (!((pointA.x > point.x) && (double_Equal(pointA.y, point.y))
						|| (pointB.x > point.x) && (double_Equal(pointB.y, point.y)))) {
						intersectCount++;
						continue;
					}
					else {
						//4�������ڶ�����,���ж���һ���ߵ�����һ���˵��뵱ǰ�ߵ�����һ���˵��Ƿ�ֲ���ˮƽ���ߵ�����.��������Ϊ�˴��ཻΪ��Խ,������+1
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

#define RANDDIV_MAX (ULLONG_MAX / 0xff) //��������������ԭʼ���ݵĲ�����
//#define RANDD_MAX (INT_MAX) //��������������ԭʼ���ݵĲ�����
#define RANDI_MAX (RANDDIV_MAX / 2 - 1) //��������Ӧ��ȡһ��
//����������ӣ�����ʱ��Ͷ����㷨��������srand��
void setRandomSeed();
//����һ���������0 ~ max ulonglong;����Ҫ�������ӣ�
unsigned long long randi();
//����������ӣ�����ʱ��Ͷ����㷨��������srand��
#define RAND_SEED_EX()  
#define RAND_SEED()  
//#define RAND_SEED_EX()  (setRandomSeed())
//#define RAND_SEED()  (srand((unsigned)time(NULL)))


//ȡ��(0,x)�����������randi()%x��
#define RAND_I_0_X( x ) (randi()%(x))
//	ȡ��(a,b)�����������randi()%(b-a-1)+a+1��
#define RAND_I_A_B( a , b ) (randi()%((b)-(a)-1)+(a)+1)
//ȡ�� ( a , RANDI_MAX ) �����������
#define RAND_I_A_MAX( a ) (RAND_I_A_B( a , RANDI_MAX ))
//	ȡ��[a,b)�����������randi()%(b-a)+a��
#define RAND_I_Ai_B( a , b ) (randi()%((b)-(a))+(a))
//	ȡ��[a,b]�����������randi()%(b-a+1)+a��
#define RAND_I_Ai_Bi( a , b ) (randi()%((b)-(a)+1)+(a))
//	ȡ��(a,b]�����������randi()%(b-a)+a+1��
#define RAND_I_A_Bi( a , b ) (randi()%((b)-(a))+(a)+1)
//	ȡ��0-1֮��ĸ�������randi()/double(RANDI_MAX)��
#define RAND_F_0_1() (randi()/double(RANDDIV_MAX))
//	ȡ��(a,b)֮��ĸ�������randi()/double(RANDI_MAX)*((b) - (a))+(a)��
#define RAND_F_A_B( a , b ) (RAND_F_0_1()*(double(b) - double(a))+double(a))

//����Ƿ�����Ч�����׳��쳣
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
//����ӳ��ģ���ࡣ������Զ��������߳�ͬ����
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
	void operator delete(void* p)
	{
		((_inlineData<_ID>*)p)->subRef();
	}

	void operator delete(void* p, size_t size)
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

//�ͷ�std::vector���ڴ棨�����Ǽ򵥵����Ԫ�أ�
template < class T >
void releaseStdVector(std::vector< T >& vt)
{
	std::vector< T > vtTemp;
	vtTemp.swap(vt);
}
//�ͷ�std::map���ڴ棨�����Ǽ򵥵����Ԫ�أ�
template < class T1, class T2 >
void releaseStdMap(std::map< T1, T2>& vt)
{
	std::map< T1, T2> vtTemp;
	vtTemp.swap(vt);
}
//�ͷ�std::map���ڴ棨�����Ǽ򵥵����Ԫ�أ�
template < class T1, class T2 >
void releaseStdUnorderedMap(std::unordered_map< T1, T2>& vt)
{
	std::unordered_map< T1, T2> vtTemp;
	vtTemp.swap(vt);
}
//�ͷ�std�����������Ǽ򵥵����Ԫ�أ�
template < class T1>
void releaseStd(T1& vt)
{
	T1 vtTemp;
	vtTemp.swap(vt);
}

//������Ϣ�������,�����CLMiniDumper::create(int dumpType = 0)������dumpType��ѡMiniDumpNormal��MiniDumpWithFullMemory������minidump���ͣ�
#ifndef _CL_DUMPER_H_
#define _CL_DUMPER_H_
class CLMiniDumper {
public:
	//����������Ϣ���������
	static HRESULT create(int dumpType = 0);
	//�ͷŵ�����Ϣ���������,δ������ʲô������
	static HRESULT release();

public:
	LONG writeMiniDump(EXCEPTION_POINTERS* pExceptionInfo);

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

#ifndef _CL_LOCK_H_
#define _CL_LOCK_H_

//����ԭ������Ԫ
typedef struct _atomic_lock_base {
	using typeName = _atomic_lock_base;
	_atomic_lock_base(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;

	LONG bLock;
	inline _atomic_lock_base() noexcept : bLock(0) {}
	inline void lock() {
		while (::InterlockedExchange(&bLock, 1))Sleep(0);
	}
	inline void unlock() {
		::InterlockedExchange(&bLock, 0);
	}
	inline bool trylock() {
		return !::InterlockedExchange(&bLock, 1);
	}
}ATLockBase, * PATLockBase;

//�Զ���ԭ��������
typedef struct _atomic_lock :protected ATLockBase
{
	using typeName = _atomic_lock;
	_atomic_lock(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;

	DWORD ownerThreadId;
	LONG lockCounts;
	inline _atomic_lock() noexcept :
		ownerThreadId(0), lockCounts(0) {}
	inline void lock() {
		auto cid = GetCurrentThreadId();
		if (ownerThreadId != cid) {
			ATLockBase::lock();
			ownerThreadId = cid;
		}
		++lockCounts;
	}

	inline void unlock() {
		if (GetCurrentThreadId() == ownerThreadId)
			if (--lockCounts == 0) {
				ownerThreadId = 0;
				ATLockBase::unlock();
			}
	}

	inline bool trylock() {
		auto cid = GetCurrentThreadId();
		if (ownerThreadId != cid) {
			if (!ATLockBase::trylock())
				return false;
			ownerThreadId = cid;
		}
		++lockCounts;
		return true;
	}
}ATLock, * PATLock;

//Windows�ٽ���
typedef struct _w_cs :CRITICAL_SECTION {
	using typeName = _w_cs;
	_w_cs(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	inline _w_cs() noexcept { InitializeCriticalSection(this); }
	inline ~_w_cs() { DeleteCriticalSection(this); }
	inline void lock() { EnterCriticalSection(this); }
	inline void unlock() { LeaveCriticalSection(this); }
	inline bool trylock() { return TryEnterCriticalSection(this); }
}WCS, * PWCS;

//RWLock��д��(windows ƽ̨)  
typedef struct _rw_lock :SRWLOCK {
	using typeName = _rw_lock;
	_rw_lock(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	DWORD ownerThreadId;
	LONG lockCounts;
	_rw_lock() noexcept;
	void lock();
	void unlock();
	bool trylock();
	inline void lockShared() { ::AcquireSRWLockShared(this); }
	inline void unlockShared() { ::ReleaseSRWLockShared(this); }
	inline bool trylockShared() { return ::TryAcquireSRWLockShared(this); }
}RWLock, * PRWLock;

#include "mutex"
//c++ STL��
typedef struct _std_lock :protected std::mutex {
	using typeName = _std_lock;
	_std_lock(const typeName&) = delete;
	typeName& operator=(const typeName&) = delete;
	inline _std_lock()noexcept :std::mutex() {}
	inline ~_std_lock() {}
	inline void lock() { std::mutex::lock(); }
	inline void unlock() { std::mutex::unlock(); }
	inline bool trylock() { return std::mutex::try_lock(); }
}STDLock, * PSTDLock;

//����������
using CLCS = RWLock;
#endif

//�ڲ����࣬����
#ifndef _CL_LOCKER_H_
#define _CL_LOCKER_H_
//�ڲ����࣬����
class CLCSLock
{
public:
	using cstype = RWLock;
	//using cstype = ATLock;
	//using cstype = WCS;
	//using cstype = STDLock;
	CLCSLock()noexcept {}
	CLCSLock(const CLCSLock& other) {}
	CLCSLock& operator=(const CLCSLock&) {};
	~CLCSLock();
	//�������������̶߳��е�����ɾ������
	inline void lock(void) {
		m_cs.lock();
	}
	inline void unlock(void) {
		m_cs.unlock();
	}
	//���������������������ɵ��ã�csId��lock��unlock������ǰ�����һ��
	void lock(int csId);
	//���������������������ɵ��ã�csId��lock��unlock������ǰ�����һ��
	void unlock(int csId);
	//��һ�γ�ʼ���������ú����ǷǱ���ġ�
	//Ҳ�ɲ����øú�����ֱ��ʹ��lock(int)������������һ�����գ�
	//���߳�ͬʱ��һ�ε���ʱ���ܴ��ڷ���δ��ʼ���Ķ������ͬʱ��ʼ��ͬ������ƻ����ݣ�
	inline void initLock(int csId) { lock(csId); unlock(csId); }
protected:
private:
	//cstype* m_cs = 0;
	cstype m_cs;
	std::map<int, cstype*>* m_csLst = 0;
};
#endif

//�߳�������࣬����
#ifndef _CL_TASKSVC_H_
#define _CL_TASKSVC_H_

//�߳���Ϣ�ṹ��
typedef class CLTaskSvcTrdSupport {
public:
	int nIndex;//�߳�����������1��ʼ
	int nTotals;//�߳�����������
	HANDLE hThread;//�߳���ʵ��������ⲿ�����߳��ͷ�
	DWORD tId;//�߳�id
	HANDLE quitEvent;//�߳��˳��¼�������,�þ���ɱ��߳����������
	CLTaskSvcTrdSupport() { reset(); }
	void reset() { ZeroMemory(this, sizeof(CLTaskSvcTrdSupport)); }
}*PCLTaskSvcTrdSupport;

//�̲߳����ṹ��
typedef class CLTaskSvcTrdParam {
public:
	const CLTaskSvcTrdSupport info;//������̵߳��ڹ������ڵ������Ϣ
	DWORD_PTR extraData[10];//����һЩ��������,���ں��������乲��
	CLTaskSvcTrdParam(const CLTaskSvcTrdSupport& _info) :info(_info) { reset(); }
	void reset() { ZeroMemory(extraData, sizeof(extraData)); }
}*PCLTaskSvcTrdParam;

//�����Ϣ�����Ƿ���WM_QUIT��Ϣ�����Ƴ�����Ϣ
BOOL CHECK_WM_QUIT();

//�߳�������࣬����
class CLTaskSvc :public CLCSLock
{
public:
	//start���ڼ���һ�������Ĺ������̣߳�Ĭ�ϼ�������Ϊ1�����ص�ǰ�̶߳��д�С���ڶ�������isDefaultSuspend = TRUE ʱ��Ӧ����ʾ����resume���ڻָ�������߳��飻
	//����Ϊ�������������ڴ����������߳�����������أ���Ҫʵ���߳���������߳�ͬ�������ں������ú�ִ��wait�����ȴ��߳���ȫ���˳���
	//num = 0������cpu�����������̣߳�
	virtual size_t start(int num = 1, BOOL isDefaultSuspend = FALSE);
	//�ֶ������߳��飬��start���õĵڶ�������isDefaultSuspend = TRUEʱ��Ӧ����ʾ����
	void resume();
	//�ֶ������߳���
	void suspend();
	//�ֶ������߳���
	virtual void pause() { suspend(); }
	//��ȡ�̶߳��д�С
	size_t getThreadsNum() const;
	//��ȡ���������߳���
	size_t getActivateThreadsNum() const;
	//�߳����Ƿ���������
	BOOL isRunning() const { return getActivateThreadsNum() > 0 ? TRUE : FALSE; }
	//ȡ��cpu������
	static DWORD getCpuCoreCounts();
	void setWaitTimeToQuit(DWORD dt = INFINITE) { m_waitTimeToQuit = dt; }
	DWORD getWaitTimeToQuit() const { return m_waitTimeToQuit; }

	virtual ~CLTaskSvc();
	//close���ڵȴ��߳̽������ر��̣߳��˳��߳���������ơ�(�÷����ڲ�ʹ��3�ֲ�ͬ��ʽ)
	virtual void close();

	typedef std::map<DWORD, CLTaskSvcTrdSupport> ThreadsTable;
	const ThreadsTable& getThreadsTable() const { return vec_threads; }

	//�ⲿ�����̵߳��ô˺����ȴ��߳����˳���sensitivityMinsec��ʾһ���ȴ��жϵ������ȣ�����ǣ�ԽС��ʾһ���ȴ��ж�����Խ��
	//���ò���������ʾ�ȴ������ʱ������жϼ���������صȴ���ʱ����룻
	ULONGLONG wait(UINT sensitivityMinsec = 100)const {
		ULONGLONG  sp = 0;
		do {
			Sleep(sensitivityMinsec);
			sp += sensitivityMinsec;
		} while (isRunning());
		return sp;
	}
	//�������ȼ���Ĭ��THREAD_PRIORITY_NORMAL
	void setPriority(int _nPriority = THREAD_PRIORITY_NORMAL) {
		nPriority = _nPriority;
	}
protected:
	//ֻ������ſ��Թ��츸�࣬�ܾ��ⲿ���ʹ�����ʵ��
	CLTaskSvc();
	//�߳�����ʱ�������е��ڲ���ʼ�����̺��������뷵��TRUE���Ż�ִ�к�����run������ֱ�ӽ���exist
	virtual BOOL init(PCLTaskSvcTrdParam var);
	//�߳���������,���麯��������ʵ��
	virtual DWORD run(PCLTaskSvcTrdParam var) = 0;
	//�߳��˳�ʱ��������к������̷߳���ֵ�ɸú�������
	virtual DWORD exist(DWORD runReturnValue, PCLTaskSvcTrdParam var);

private:
	//�������̷߳��ʽӿ�
	static DWORD workThread(LPVOID param);
	//�̶߳��б�
	ThreadsTable vec_threads;

	DWORD m_waitTimeToQuit = INFINITE;

	int nPriority;
};

//�ж�quit�¼�����,���źž��˳�ѭ��
#define CHECK_EVENT_QUIT( hQuitEvent ) \
	((hQuitEvent) && ((hQuitEvent) != INVALID_HANDLE_VALUE) && (::WaitForSingleObject((hQuitEvent), 0) == WAIT_OBJECT_0))
//����˳��¼��Ƿ�Ϊ֪ͨ״̬�������˳�
#define WHEN_EVENT_QUIT_ARRIVE_BREAK_LOOP( hQuitEvent ) \
	if (CHECK_EVENT_QUIT( hQuitEvent ))break;

//�ж�WM_QUIT��Ϣ����,���źž��˳�ѭ��
#define WHEN_WM_QUIT_ARRIVE_BREAK_LOOP() \
	if(CHECK_WM_QUIT())break;

//�ж�WM_QUIT��Ϣ��quit�¼�����,���źž��˳�ѭ��
#define CHECK_WM_QUIT_OR_EVENT_QUIT_ARRIVE(hQuitEvent) \
	(CHECK_EVENT_QUIT( hQuitEvent ) || CHECK_WM_QUIT() )
#define IS_WM_QUIT_OR_EVENT_QUIT_ARRIVE() \
	(CHECK_EVENT_QUIT( var->info.quitEvent ) || CHECK_WM_QUIT() )

//�ɵ�ǰ�̱߳�ţ��������зּ�����̵߳�ִ�������±��ţ�����Ϊ��nIndexStart <= i < nIndexEnd;
#define GET_TRD_SECTION_TASK(var , nTotalSize , nIndexStart , nIndexEnd ) \
	size_t nIndexStart, nIndexEnd;\
	getTrdSection((nTotalSize), ((var)->info.nTotals), ((var)->info.nIndex - 1), (nIndexStart), (nIndexEnd));

#endif

//���������
#ifndef _CL_PARALLELPASS_H_
#define _CL_PARALLELPASS_H_
//��������࣬��Ҫʵ�ֹ��ܣ�
//���̲߳����У��ɡ����̡߳����ơ���֧�߳��顱�Ĵ���ε�ִ�м������ȴ����ƣ����£�
//���̣߳��ַ�����ɺ�������ȴ����з�֧�̣߳�ִ��ֱ��������Ӧ���룩�黹��ɣ�
//��֧�ߣ��̲��������ȴ���ȡ����Ӧ��ɺ󣨲��������̷߳����ĸ���ָ��orderKey������ִ�д��룬ֱ�������黹�Ի�ȡ�Ķ�Ӧ��ɣ�
//���̣߳��յ�ȫ����֧�̹߳黹����ɺ���������ȴ���ִ�к������룻
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

//ԭ�Ӳ�����
#ifndef _CL_ATOMIC_H_
#define _CL_ATOMIC_H_
//Ĭ��״̬��__CLAtomic_Old������ԭ�Ӳ���ģʽ�Ƿ�������ԭ�Ӳ����ڱ�֤�̰߳�ȫ����½����ʹ���Ч�ʣ�
#define CLAtomic_bUseAtomc_def true

template  <class T, bool = false>
struct _atomic_value;

template  <class T>
struct _atomic_value<T, false> {
	/*alignas(sizeof(T))*/ mutable T _Value{};
	_atomic_value() = default;
	constexpr _atomic_value(const T Value) noexcept : _Value(Value) {}
	constexpr _atomic_value(T& Value, bool) noexcept : _Value(Value) {}	
};

template  <class T>
struct _atomic_value<T, true> {
	/*alignas(sizeof(T))*/ T& _Value;
	_atomic_value() :_Value(_getStaticValueRef()) {}
	constexpr _atomic_value(const T Value) noexcept : _Value(_getStaticValueRef()) {}
	constexpr _atomic_value(T& Value, bool) noexcept : _Value(Value) {}	
private:
	inline static T& _getStaticValueRef() {
		static T _g_atv = 0;
		return _g_atv;
	}
};
//ֵ�����ػ�
template <class T, bool isRef = false>
struct _atomic_storage :_atomic_value<T, isRef> {
protected:
	bool _bUseAtomc{ CLAtomic_bUseAtomc_def };

	using _Base = _atomic_value<T, isRef>;
	_atomic_storage() = default;
	constexpr _atomic_storage(const T Value) noexcept : _Base(Value) {}
	constexpr _atomic_storage(T& Value, bool) noexcept : _Base(Value, true) {}
	void store(const T Value) noexcept { this->_Base::_Value = Value; }
	T load() const noexcept { return this->_Base::_Value; }

	template<size_t = sizeof(T)>
	bool compare_exchange_strong(T& _Expected, const T _Desired) noexcept;
	template<>
	bool compare_exchange_strong<2>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(short) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(short*)(&_Expected);
		if (InterlockedCompareExchange(&reinterpret_cast<volatile short&>(_Value), *(short*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}
	template<>
	bool compare_exchange_strong<4>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(long) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(long*)(&_Expected);
		if (InterlockedCompareExchange(&reinterpret_cast<volatile long&>(_Value), *(long*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}
	template<>
	bool compare_exchange_strong<8>(T& _Expected, const T _Desired) noexcept {
		static_assert(sizeof(long long) == sizeof(T), "Tried to reinterpret memory is not same. ");
		auto old = *(long long*)(&_Expected);
		if (InterlockedCompareExchange64(&reinterpret_cast<volatile long long&>(_Value), *(long long*)(&_Desired), old) == old) {
			return true;
		}
		else {
			_Expected = load();
			return false;
		}
	}

public:
	//���ø�ԭ�Ӷ����״̬Ϊ��ִ��ԭ�Ӳ�������ִ�з�ԭ�Ӳ�����ԭ�Ӳ�������ʱ��
	void setUseAtomic(bool bUseAtomc = CLAtomic_bUseAtomc_def) noexcept {
		_bUseAtomc = bUseAtomc;
	}
	//�жϸö���״̬����ִ��ԭ�Ӳ���������true��
	bool isUseAtomic() const noexcept {
		return _bUseAtomc;
	}
};

template <class T, size_t = sizeof(T), bool = false>
struct _atomic_integral; // not defined

template <class _integral, class T>
volatile _integral* _atomic_address_as(T& _Source) noexcept {
	static_assert(is_integral_v<_integral>, "Tried to reinterpret memory as non-integral");
	return &reinterpret_cast<volatile _integral&>(_Source);
}

template <class T, bool isRef>
struct _atomic_integral<T, 1, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 1-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T Value) noexcept : _Base(Value) {}
	constexpr _atomic_integral(T& Value, bool) noexcept : _Base(Value, true) {}

	T self_add(T _Operand) noexcept {
		char _Result =
			InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_and(T _Operand) noexcept {
		char _Result =
			InterlockedAnd8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_or(T _Operand) noexcept {
		char _Result =
			InterlockedOr8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_xor(T _Operand) noexcept {
		char _Result =
			InterlockedXor8(_atomic_address_as<char>(this->_Value), static_cast<char>(_Operand));
		return static_cast<T>(_Result);
	}
	T operator++(int) noexcept {
		return static_cast<T>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), 1));
	}
	T operator++() noexcept {
		unsigned char _Before =
			static_cast<unsigned char>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), 1));
		++_Before;
		return static_cast<T>(_Before);
	}
	T operator--(int) noexcept {
		return static_cast<T>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), -1));
	}
	T operator--() noexcept {
		unsigned char _Before =
			static_cast<unsigned char>(InterlockedExchangeAdd8(_atomic_address_as<char>(this->_Value), -1));
		--_Before;
		return static_cast<T>(_Before);
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 2, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 2-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}
	T self_add(T _Operand) noexcept {
		short _Result =
			InterlockedExchangeAdd16(_atomic_address_as<short>(this->_Value),
				static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_and(T _Operand) noexcept {
		short _Result =
			InterlockedAnd16(_atomic_address_as<short>(this->_Value), static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_or(T _Operand) noexcept {
		short _Result =
			InterlockedOr16(_atomic_address_as<short>(this->_Value), static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T self_xor(T _Operand) noexcept {
		short _Result =
			InterlockedXor16(_atomic_address_as<short>(this->_Value),
				static_cast<short>(_Operand));
		return static_cast<T>(_Result);
	}
	T operator++(int) noexcept {
		unsigned short _After =
			static_cast<unsigned short>(InterlockedIncrement16(_atomic_address_as<short>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}
	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement16(_atomic_address_as<short>(this->_Value)));
	}
	T operator--(int) noexcept {
		unsigned short _After =
			static_cast<unsigned short>(InterlockedDecrement16(_atomic_address_as<short>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}
	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement16(_atomic_address_as<short>(this->_Value)));
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 4, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 4-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}

	T self_add(T _Operand) noexcept {
		long _Result =
			InterlockedExchangeAdd(_atomic_address_as<long>(this->_Value),
				static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_and(T _Operand) noexcept {
		long _Result =
			InterlockedAnd(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_or(T _Operand) noexcept {
		long _Result =
			InterlockedOr(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_xor(T _Operand) noexcept {
		long _Result =
			InterlockedXor(_atomic_address_as<long>(this->_Value), static_cast<long>(_Operand));
		return static_cast<T>(_Result);
	}

	T operator++(int) noexcept {
		unsigned long _After =
			static_cast<unsigned long>(InterlockedIncrement(_atomic_address_as<long>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}

	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement(_atomic_address_as<long>(this->_Value)));
	}

	T operator--(int) noexcept {
		unsigned long _After =
			static_cast<unsigned long>(InterlockedDecrement(_atomic_address_as<long>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}

	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement(_atomic_address_as<long>(this->_Value)));
	}
};

template <class T, bool isRef>
struct _atomic_integral<T, 8, isRef> : _atomic_storage<T, isRef> { // CLAtomic integral operations using 8-byte intrinsics
protected:
	using _Base = _atomic_storage<T, isRef>;
	_atomic_integral() = default;
	constexpr _atomic_integral(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral(T& _Value, bool) noexcept : _Base(_Value, true) {}

	T self_add(T _Operand) noexcept {
		long long _Result =
			InterlockedExchangeAdd64(
				_atomic_address_as<long long>(this->_Value), static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_and(T _Operand) noexcept {
		long long _Result =
			InterlockedAnd64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_or(T _Operand) noexcept {
		long long _Result =
			InterlockedOr64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T self_xor(T _Operand) noexcept {
		long long _Result =
			InterlockedXor64(_atomic_address_as<long long>(this->_Value),
				static_cast<long long>(_Operand));
		return static_cast<T>(_Result);
	}

	T operator++(int) noexcept {
		unsigned long long _After =
			static_cast<unsigned long long>(InterlockedIncrement64(_atomic_address_as<long long>(this->_Value)));
		--_After;
		return static_cast<T>(_After);
	}

	T operator++() noexcept {
		return static_cast<T>(InterlockedIncrement64(_atomic_address_as<long long>(this->_Value)));
	}

	T operator--(int) noexcept {
		unsigned long long _After =
			static_cast<unsigned long long>(InterlockedDecrement64(_atomic_address_as<long long>(this->_Value)));
		++_After;
		return static_cast<T>(_After);
	}

	T operator--() noexcept {
		return static_cast<T>(InterlockedDecrement64(_atomic_address_as<long long>(this->_Value)));
	}
};

template <class T, bool isRef = false>
struct _atomic_integral_facade : _atomic_integral<T, sizeof(T), isRef> {
protected:
	using _Base = _atomic_integral<T, sizeof(T), isRef>;
	using difference_type = T;
	_atomic_integral_facade() = default;
	constexpr _atomic_integral_facade(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_integral_facade(T& _Value, bool) noexcept : _Base(_Value, true) {}

public:
	T operator++(int)  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator++(0) : _Value++;
	}

	T operator++()  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator++() : ++_Value;
	}

	T operator--(int)  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator--(0) : _Value--;
	}

	T operator--()  noexcept {
		return isUseAtomic() ? const_cast<_atomic_integral_facade*>(this)->_Base::operator--() : --_Value;
	}

	T operator+=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_add(_Operand) + _Operand) : _Value += _Operand;
	}

	T operator-=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_add(T(0) - _Operand) - _Operand) : _Value -= _Operand;
	}

	T operator&=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_and(_Operand) & _Operand) : _Value &= _Operand;
	}

	T operator|=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_or(_Operand) | _Operand) : _Value |= _Operand;
	}

	T operator^=(T _Operand) noexcept {
		return isUseAtomic() ? static_cast<T>(this->_Base::self_xor(_Operand) ^ _Operand) : _Value ^= _Operand;
	}

	template<class T2>
	T operator*=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value *= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp * _Operand));
		return _rt;
	}
	template<class T2>
	T operator/=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value /= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp / _Operand));
		return _rt;
	}

	T operator%=(int _Operand) noexcept {
		if (!isUseAtomic())return _Value %= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp % _Operand));
		return _rt;
	}
};

template <class T, bool isRef = false>
struct _atomic_floating : _atomic_storage<T, isRef> {
protected:
	// provides CLAtomic floating-point operations
	using _Base = _atomic_storage<T, isRef>;
	using difference_type = T;
	_atomic_floating() = default;
	constexpr _atomic_floating(const T _Value) noexcept : _Base(_Value) {}
	constexpr _atomic_floating(T& _Value, bool) noexcept : _Base(_Value, true) {}

public:
	template<class T2>
	T operator+=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value += _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp + _Operand));
		return _rt;
	}
	template<class T2>
	T operator-=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value -= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp - _Operand));
		return _rt;
	}
	template<class T2>
	T operator*=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value *= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp * _Operand));
		return _rt;
	}
	template<class T2>
	T operator/=(T2 _Operand) noexcept {
		if (!isUseAtomic())return _Value /= _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp / _Operand));
		return _rt;
	}
	template<size_t = sizeof(T)>
	T operator%=(int _Operand) noexcept { throw runtime_error; return 0; }
	template<>
	T operator%=<4>(int _Operand) noexcept {
		if (!isUseAtomic())return _Value = long(_Value) % _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = long(_Temp) % _Operand));
		return _rt;
	}
	template<>
	T operator%=<8>(int _Operand) noexcept {
		if (!isUseAtomic())return _Value = long long(_Value) % _Operand;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = long long(_Temp) % _Operand));
		return _rt;
	}

	T operator++() noexcept {
		if (!isUseAtomic())return ++_Value;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp + T(1)));
		return _rt;
	}

	T operator--() noexcept {
		if (!isUseAtomic())return --_Value;
		T _Temp{ this->load() }; T _rt;
		while (!this->compare_exchange_strong(_Temp, _rt = _Temp - T(1)));
		return _rt;
	}

	T operator++(int) noexcept {
		if (!isUseAtomic())return _Value++;
		T _Temp{ this->load() };
		while (!this->compare_exchange_strong(_Temp, _Temp + T(1)));
		return _Temp;
	}

	T operator--(int) noexcept {
		if (!isUseAtomic())return _Value--;
		T _Temp{ this->load() };
		while (!this->compare_exchange_strong(_Temp, _Temp - T(1)));
		return _Temp;
	}
};

//���ͻ�����ѡ��
template <class T>
using _choose_atomic_base_t =
typename _Select<is_integral_v<T> && !is_same_v<bool, T>>::template _Apply<_atomic_integral_facade<T>,
	typename _Select<is_floating_point_v<T>>::template _Apply<_atomic_floating<T>,
	_atomic_storage<T>>>;

template <class T>
using _choose_atomic_ref_t =
typename _Select<is_integral_v<T> && !is_same_v<bool, T>>::template _Apply<_atomic_integral_facade<T, true>,
	typename _Select<is_floating_point_v<T>>::template _Apply<_atomic_floating<T, true>,
	_atomic_storage<T, true>>>;

//���ͻ�����ѡ��ѡ�����û�ֵ����ԭ�Ӳ������ࣩ
template <class T, bool isRef = false>
using _choose_atomic_base_or_ref_t =
typename _Select<isRef>::template _Apply<_choose_atomic_ref_t<T>, _choose_atomic_base_t<T>>;

//ԭ�Ӳ����ࣨģ�壩
template <class T>
struct CLAtomic : _choose_atomic_base_or_ref_t<T> { // CLAtomic value
private:
	using _Base = _choose_atomic_base_or_ref_t<T>;

public:
	// clang-format off
	static_assert(is_trivially_copyable_v<T>&& is_copy_constructible_v<T>&& is_move_constructible_v<T>
		&& is_copy_assignable_v<T>&& is_move_assignable_v<T>,
		"CLAtomic<T> requires T to be trivially copyable, copy constructible, move constructible, copy assignable, "
		"and move assignable.");
	// clang-format on

	using value_type = T;

	//��׼���죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�
	constexpr CLAtomic(const T _Value) noexcept : _Base(_Value) {}

	//Ĭ�Ϲ��죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�
	constexpr CLAtomic() noexcept(is_nothrow_default_constructible_v<T>) : _Base() {}

	//�������죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	CLAtomic(const CLAtomic& v)
		: _Base(v.load()) {
	}
	using _Base::load;
	using _Base::store;
	//�������죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	template<class T2>
	CLAtomic(const CLAtomic<T2>& v)
		: _Base(v.load()) {
	}
	T operator=(const T v2) {
		return _Value = v2;
	}
	template<class T2>
	T operator=(const T2 v2) {
		return _Value = v2;
	}

	//��ֵ��������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	T operator=(const CLAtomic& v2) {
		return _Value = v2.load();
	}

	//��ֵ��������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	template<class T2>
	T operator=(const CLAtomic<T2>& v2) {
		return _Value = v2.load();
	}

	operator T() const noexcept {
		return this->load();
	}
};

//ԭ�Ӳ��������ࣨģ�壩�����ཫ�ڳ�ʼ���׶�Ψһ�İ�һ����Ӧ���͵���������
template <class T>
struct CLAtomicRef : _choose_atomic_base_or_ref_t<T, true> { // CLAtomic value
private:
	using _Base = _choose_atomic_base_or_ref_t<T, true>;

public:
	// clang-format off
	static_assert(is_trivially_copyable_v<T>&& is_copy_constructible_v<T>&& is_move_constructible_v<T>
		&& is_copy_assignable_v<T>&& is_move_assignable_v<T>,
		"CLAtomic<T> requires T to be trivially copyable, copy constructible, move constructible, copy assignable, "
		"and move assignable.");
	// clang-format on

	using value_type = T;

	//��׼���죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�
	constexpr CLAtomicRef(T& _Value) noexcept : _Base(_Value, true) {}

	//Ĭ�Ϲ��죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�
	constexpr CLAtomicRef() noexcept(is_nothrow_default_constructible_v<T>) = delete;

	//�������죬��ʼ״̬��Ƕ���Ϊִ��ԭ�Ӳ����ģ�������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	CLAtomicRef(const CLAtomicRef& v)
		: _Base(v._Value, true) {
	}

	using _Base::load;
	using _Base::store;

	T operator=(const T v2) {
		return _Value = v2;
	}
	template<class T2>
	T operator=(const T2 v2) {
		return _Value = v2;
	}

	//��ֵ��������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	T operator=(const CLAtomicRef& v2) {
		return _Value = v2.load();
	}
	template<class T2>
	T operator=(const CLAtomicRef<T2>& v2) {
		return _Value = v2.load();
	}
	T operator=(const CLAtomic<T>& v2) {
		return _Value = v2.load();
	}
	//��ֵ��������ԭ�����ԭ�Ӳ������״̬�����踴�ƣ�
	template<class T2>
	T operator=(const CLAtomic<T2>& v2) {
		return _Value = v2.load();
	}
	operator T() const noexcept {
		return this->load();
	}
};

//�ɵ�ԭ�Ӳ����ࣨģ��,�Ǹ�������ִ���ٶ�����
template<class T>
class __CLAtomic_Old {
	static_assert(sizeof(T) % 2 == 0, "__CLAtomic_Old: target type is not support!");
protected:
	T Target;
	bool bUseAtomc;


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
		return InterlockedCompareExchange64(dest, newvalue, oldvalue) == oldvalue ? true : false;
	}
	inline static bool atomicCAS(volatile ULONG64* dest, ULONG64 newvalue, ULONG64 oldvalue)
	{
		return atomicCAS(reinterpret_cast<LONG64 volatile*>(dest), LONG64(newvalue), LONG64(oldvalue));
	}
	inline static bool atomicCAS(volatile FLOAT* dest, FLOAT newvalue, FLOAT oldvalue)
	{
		auto old1 = *(LONG*)(void*)&oldvalue;
		return InterlockedCompareExchange(reinterpret_cast<LONG volatile*>(dest), *(LONG*)(void*)&newvalue, old1) == old1 ? true : false;
	}
	inline static bool atomicCAS(volatile DOUBLE* dest, DOUBLE newvalue, DOUBLE oldvalue)
	{
		auto old1 = *(LONG64*)(void*)&oldvalue;
		return InterlockedCompareExchange64(reinterpret_cast<LONG64 volatile*>(dest), *(LONG64*)(void*)&newvalue, old1) == old1 ? true : false;
	}
	inline void set(T dest) {
		throw logic_error("__CLAtomic_Old::set is not impliment!");
		Target = dest;
	}
	inline T increment()
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old + T(1), old));
		return old;
	}
	inline T decrement()
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old - T(1), old));
		return old;
	}
	inline T add(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old + v, old));
		return old;
	}
	inline T mul(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old * v, old));
		return old;
	}
	inline T div(volatile T v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old / v, old));
		return old;
	}
	inline T surplus(volatile int v)
	{
		T old;
		do {
			old = Target;
		} while (!__CLAtomic_Old::atomicCAS(&Target, old % int(v), old));
		return old;
	}
public:
	using value_type = T;

	inline __CLAtomic_Old() :bUseAtomc(CLAtomic_bUseAtomc_def) {
	}
	inline __CLAtomic_Old(T v, bool _bUseAtomc = CLAtomic_bUseAtomc_def)
		: Target(v), bUseAtomc(_bUseAtomc) {
	}
	inline __CLAtomic_Old(const __CLAtomic_Old& v)
		: Target(v.Target), bUseAtomc(v.bUseAtomc) {
	}
	template<class T2>
	inline __CLAtomic_Old(const __CLAtomic_Old<T2>& v)
		: Target(v()), bUseAtomc(v.isUseAtomic()) {
	}
	template<class T2>
	inline __CLAtomic_Old(T2 v)
		: Target(T(v)), bUseAtomc(CLAtomic_bUseAtomc_def) {
	}
	__CLAtomic_Old& setUseAtomic(bool _bUseAtomc = CLAtomic_bUseAtomc_def) {
		return bUseAtomc = _bUseAtomc, *this;
	}
	bool isUseAtomic() const {
		return bUseAtomc;
	}
	inline __CLAtomic_Old& operator=(T v2) {
		return Target = v2, *this;
	}
	inline __CLAtomic_Old& operator=(const __CLAtomic_Old& v2) {
		return Target = v2.Target, *this;
	}
	template<class T2>
	inline __CLAtomic_Old& operator=(T2 v2) {
		return Target = v2, *this;
	}
	template<class T2>
	inline __CLAtomic_Old& operator=(const __CLAtomic_Old<T2>& v2) {
		return Target = v2(), *this;
	}
	//Ĭ��Ϊԭ�Ӳ���++()����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator++() {
		return bUseAtomc ? increment() : ++Target, * this;
	}
	//Ĭ��Ϊԭ�Ӳ���--()����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator--() {
		return bUseAtomc ? decrement() : --Target, * this;
	}
	//Ĭ��Ϊԭ�Ӳ���()++����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
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
	//Ĭ��Ϊԭ�Ӳ���()--����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
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
	//Ĭ��Ϊԭ�Ӳ���+=����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator+=(T v) {
		return  bUseAtomc ? add(v) : Target += v, *this;
	}
	//Ĭ��Ϊԭ�Ӳ���-=����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator-=(T v) {
		return  bUseAtomc ? add(0 - v) : Target -= v, *this;
	}
	//Ĭ��Ϊԭ�Ӳ���*=����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator*=(T v) {
		return  bUseAtomc ? mul(v) : Target *= v, *this;
	}
	//Ĭ��Ϊԭ�Ӳ���/=����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline __CLAtomic_Old& operator/=(T v) {
		return  bUseAtomc ? div(v) : Target /= v, *this;
	}
	//Ĭ��Ϊԭ�Ӳ���%=����setUseAtomic(false)ʱ���÷�ԭ�Ӳ�����ʽ��
	inline  __CLAtomic_Old& operator%=(int v) {
		return  bUseAtomc ? surplus(v) : Target %= v, *this;
	}
	inline operator T() const noexcept { return Target; }
	inline operator T& () noexcept { return Target; }
	inline T operator()() const noexcept { return Target; }
	template<class T2> inline T operator+(const T2 v) const { return Target + v; }
	template<class T2> inline T operator-(const T2 v) const { return Target - v; }
	template<class T2> inline T operator*(const T2 v) const { return Target * v; }
	template<class T2> inline T operator/(const T2 v) const { return Target / v; }
	template<class T2> inline T operator%(const T2 v) const { return Target % (int)v; }
};

#endif

//����ʵ�ֵ�Ч�ܽϵͣ���ʹ��c++11�����ṩ��unordered_map
#ifndef _CL_HASHMAP_H_
#define _CL_HASHMAP_H_

template<class Key, class Value>
class HashNode
{
public:
	Key    _key;
	HashNode* next;
	Value  _value;

	HashNode(Key key, Value value)
	{
		_key = key;
		_value = value;
		next = NULL;
	}
	virtual ~HashNode() {}
};

//����ʵ�ֵ�Ч�ܽϵͣ���ʹ��c++11�����ṩ��unordered_map
template <class Key, class Value, class HashFunc, class EqualKey>
class HashMap
{
public:
	HashMap(size_t size) : _size(size), ValueNULL(Value())
	{
		hash = HashFunc();
		equal = EqualKey();
		table = new HashNode<Key, Value> * [_size];
		ZeroMemory(table, _size * sizeof(HashNode<Key, Value>*));
	}

	virtual ~HashMap()
	{
		for (size_t i = 0; i < _size; i++)
		{
			HashNode<Key, Value>* currentNode = table[i];
			while (currentNode)
			{
				HashNode<Key, Value>* temp = currentNode;
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
		HashNode<Key, Value>* node = table[index];
		HashNode<Key, Value>* prev = NULL;
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
		Value* pv = _find(key, index);
		if (pv)
			return *pv;
		else
			return _insert(key, ValueNULL, index)._value;
	}
private:

	HashNode<Key, Value>** table;
	size_t _size;
	const Value ValueNULL;
	HashFunc hash;
	EqualKey equal;

	inline size_t _getIndex(const Key& key) {
		return hash(key) % _size;
	}

	HashNode<Key, Value>& _insert(const Key& key, const Value& value, size_t index)
	{
		HashNode<Key, Value>* node = new HashNode<Key, Value>(key, value);
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
			HashNode<Key, Value>* node = table[index];
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

//����Ҫ����hash������ȽϺ���
class HashFunc_string
{
public:
	int operator()(const std::tstring& key)
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
	bool operator()(const std::tstring& A, const std::tstring& B)
	{
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};

//keyΪstring�Ĺ�ϣӳ��
//����ʵ�ֵ�Ч�ܽϵͣ���ʹ��c++11�����ṩ��unordered_map
template <typename classTag>
class CLHashmap :public HashMap<std::tstring, classTag, HashFunc_string, EqualKey_string> {
public:
	CLHashmap(size_t si) :HashMap<std::tstring, classTag, HashFunc_string, EqualKey_string>(si) {}
};

#endif

//��ʾcom������Ϣ,yes��ʾ���쳣
HRESULT checkComErrorManual();
//��EXCEL�ļ�����ת��ΪCSV��ʽ�ļ�,���ؽ����S_OK���ɹ���S_FALSE��ʧ��
HRESULT converXlsToCsv(LPCTSTR pSrcFileName, LPCTSTR pDesFileName);
/*��Urlָ��ĵ�ַ���ļ����ص�save_asָ��ı����ļ�*/
BOOL downloadUrlFile(LPCTSTR Url, LPCTSTR save_as);
/*��Urlָ��ĵ�ַ���ļ����ص�buf��buf��nBufSize��Ϊ0���ʾ�Ѿ����ڻ�����,bufʹ�ú���Ҫdeleteɾ����save_asָ��ı����ļ���ַ�򱣴浽ָ���ļ�*/
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT char*& buf, IN OUT size_t& nBufSize, OUT size_t& nDataSize);
typedef std::vector<char> DataBuffer;//���ݻ�����
BOOL downloadUrl(LPCTSTR Url, LPCTSTR save_as, IN OUT DataBuffer& buf);
//�ѻ���������д�뵽�ļ�
BOOL writeBufToFile(LPCTSTR saveFile, const char* buf, size_t nDataSize, LPDWORD writeNumber = 0);


//������������ر�������ַ����ȣ�����1����ʾҪ�˸��ɾ���ַ�������ֵӦ��Ϊ��һ�ε��ú����ɹ��ķ���ֵ����ʵ�ֵ��еĻ�ɾ������ʾ��
int printfWithBackCover(int bakCoverChars, const TCHAR* szFormat, ...);
#define printfbc printfWithBackCover //������������ر�������ַ����ȣ�����1����ʾҪ�˸��ɾ���ַ�������ֵӦ��Ϊ��һ�ε��ú����ɹ��ķ���ֵ����ʵ�ֵ��еĻ�ɾ������ʾ��
//ɾ������һ�������bakCoverChars��ʾ���һ���ַ�������Ӧ����printfWithBackCover���õķ���ֵ
int printfWithBackCoverEnd(int bakCoverChars);
#define printfbce printfWithBackCoverEnd //ɾ������һ�������bakCoverChars��ʾ���һ���ַ�������Ӧ����printfWithBackCover���õķ���ֵ

//ȡ���̷ֶ߳�,ssi����������totals�ֶ�����index��ǰ��Ŵ�0��ʼ��is��ʼ��ţ�ie������ţ����һ������ĺ�һ��λ�ã�
void getTrdSection(size_t ssi, size_t totals, size_t index, OUT size_t& is, OUT size_t& ie);

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

//����ͨȨ����������
BOOL createProcessLow(TCHAR* lpApplicationName,
	TCHAR* lpCommandLine = NULL,
	TCHAR* lpDirectory = NULL,
	UINT nShow = SW_SHOWNORMAL);
//�Թ���ԱȨ����������
BOOL createProcessHigh(TCHAR* strProcessName,
	TCHAR* strCommandLine = NULL,
	TCHAR* lpDirectory = NULL,
	UINT nShow = SW_SHOWNORMAL);

typedef BOOL(WINAPI* PCreateProcessWithToken)(
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
BOOL installService(LPCTSTR servName, LPCTSTR lpszBinaryPathName = 0, LPCTSTR lpServiceDisplayName = 0, LPCTSTR lpDescription = 0);
BOOL uninstallService(LPCTSTR servName);
BOOL startService(LPCTSTR servName);
BOOL stopService(LPCTSTR servName);


//����ʱ���ܵģ���Ϣ��
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
//ȡ��cpu������Ϣ������cpu�ͺ�����cpu��������cpu�����߳�����cpu��Ƶ(GHz)
void getCpuInfo(CLString& chProcessorName, CLString& chProcessorType, DWORD& dwCoreNum, DWORD& dwMaxClockSpeed);

//ȡ�ÿ�̨���ھ��
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
//ȡ��cpu��ǰʹ����
class CLCpuUsage
{
public:
	CLCpuUsage();
	~CLCpuUsage();
	int GetUsage();//ȡ��cpu��ǰʹ����
private:
#define CLCDelay_DELAY_DIFF   200
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
//�߾��ȼ�ʱ����
class CLTick {
protected:
	LARGE_INTEGER lis;
	LARGE_INTEGER lie;
	LARGE_INTEGER Freg;
public:
	CLTick() {
		timingStart();
	}
	//��ʼ��ʱ
	CLTick& timingStart() {
		QueryPerformanceFrequency(&Freg);
		QueryPerformanceCounter(&lis);
		return *this;
	}
	//ȡ�ôӼ�ʱ��ʼ����ǰ��ʱ��
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

