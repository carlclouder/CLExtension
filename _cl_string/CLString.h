#pragma once
#ifndef __CL_CLSTRING_H__
#define __CL_CLSTRING_H__

//Design by cailuo. 2014-04-30
//自定义选择集文字管理类 CLString -----------------------------------------

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
#include <exception>
#pragma comment(lib, "ws2_32.lib")

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC//内存泄漏检查
#include <stdlib.h>
#include <crtdbg.h>

#define DEFINE_LEAKS _CrtMemState s1, s2, s3;
#define START_LEAKS  _CrtMemCheckpoint( &s1 );
#define END_LEAKS  _CrtMemCheckpoint( &s2 );	\
	if( _CrtMemDifference( &s3, &s1, &s2) ){   \
	_tprintf_s(_T("File= %s , Line= %d .\n"),__FILE__,__LINE__-1);  \
	_CrtMemDumpStatistics( &s3 );}
#endif

#ifdef WIN32
//#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#include <string>
#include "wininet.h"
#pragma comment(lib,"wininet.lib")//链接需要 wininet.lib

//CL字符串类定义
class CLString;
typedef CLString* PCLString;//类的指针类型
typedef const PCLString PCCLString;//类的常量指针类型
typedef CLString& CLStringR;//类的引用对象类型
typedef const CLStringR CLStringRC;//类的常量引用对象类型

enum EnCode {
	EnCode_UNKNOWN = 0, //encode unknown
	EnCode_ASCII,       //ASCII
	EnCode_UTF8,        //UTF-8
	EnCode_ULE,         //Unicode little_Endian(windows default code)
	EnCode_UBE,         //Unicode Big_Endian
};

//以下为函数CLString::findMidString的搜索模式和保存模式，以字符串"aabbaabbccbbccdd"为例，剪裁字符分别为"aa","cc"时候以下结果有不同（在保存模式为FMS_MAX时候）；
enum FMS_TYPE {
	FMS_LEFT = 11,  // 左边型，"aa12aa34cc56cc78"为例，剪裁字符"aa","cc"，结果 "aa12aa34cc";
	FMS_RIGHT = 22, // 左边型，"aa12aa34cc56cc78"为例，剪裁字符"aa","cc"，结果 "aa34cc56cc";
	FMS_MAX = 12,   // 左边型，"aa12aa34cc56cc78"为例，剪裁字符"aa","cc"，结果 "aa12aa34cc56cc";
	FMS_MIN = 21,   // 左边型，"aa12aa34cc56cc78"为例，剪裁字符"aa","cc"，结果 "aa34cc";
};

//CL超级字符串类，功能很强就对了
class CLString
{
protected:
	//字符串指针
	LPTSTR pHead;

	//缓冲区大小，以字符计（包含末尾0字符）
	LONG_PTR m_unitNumber;

	//字符串字符个数
	LONG_PTR m_strlen;

	//标记内容是否已经改变
	byte m_changeFlag;

	//保存的内容是否是宽字节格式，为1表示宽字节
	byte m_encoding;

	typedef struct _ClStringExData {

		//临时字符串指针
		union _cl_u_pointer {
			LPSTR pMultiByte;
			LPWSTR pWideChar;
		}m_pointer_;
		byte m_PtExSaveType_;//标明m_pointer_是否做过存储操作0为存储，1存过MultiByte，2存过WideChar
		//临时字符串缓冲区空间长度，以字符计（包含末尾0字符）
		LONG_PTR m_pointerBufSizeInByte_;

		//内部保存的打开文件的句斌
		HANDLE m_hFile_;

		byte m_fileType_;//标明内部打开的文件的http文件数据格式类型
		HINTERNET hInternet_, hConnect_, hRequest_;//变量用于保存http句柄
		//动态分配的时间变量指针，用于秒级的时间统计;
		SYSTEMTIME* m_sysTimeStart_, * m_sysTimeEnd_;
		//动态分类的毫秒时间变量指针，用微秒级的时间统计;
		LARGE_INTEGER* m_largeIntegerStart_, * m_largeIntegerEnd_;

		std::vector<LPCTSTR>* m_vtInnerStringVector_;//内部维护的字符串向量指针
		std::vector<LPTSTR>* m_vtStringStoreLst_;//用于内部存储的字符串向量指针
		std::vector<LONG_PTR>* m_vtStringStoreBufSizeLst_;//用于内部存储的字符串向量长度数据的指针
		//用于逐行读取文件内容的控制变量，
		byte m_isEndOnce_;
		//#define (makeDEx()->m_isEndOnce_) (makeDEx()->m_isEndOnce_)
			//用于文字拆分输出的当前输出位置标记，默认为-1表示未做拆分输出准备或已经完毕；
		long m_curSplitOutIndex_;
		//#define (makeDEx()->m_curSplitOutIndex_) (makeDEx()->m_curSplitOutIndex_)

		inline _ClStringExData() { init(); }
		inline ~_ClStringExData() {
			closeAndClear();
			assert(m_vtInnerStringVector_ == 0 && m_vtStringStoreLst_ == 0 && m_vtStringStoreBufSizeLst_ == 0 &&
				m_sysTimeStart_ == 0 && m_sysTimeEnd_ == 0 && m_largeIntegerStart_ == 0 && m_largeIntegerEnd_ == 0);
		}
		void init();
		void closeInternet();
		BOOL closeFile();
		void closeMemery();
		void closeAndClear();
	}CLStringExData, * PCLStringExData;

	PCLStringExData pDataEx;//额外扩展数据结构包，初始化未使用则不创建

	inline PCLStringExData makeDEx() { return pDataEx ? pDataEx : pDataEx = new CLStringExData; }

	//析构中，最后一个调用的函数cleanAllMemory中调用
	inline void deleteDEx() { delete pDataEx; pDataEx = 0; }

	//内部功能接口，未开放。
protected:
	//函数改变m_encoding
	void setEncode();
	//函数只能在析构函数调用唯一一次，仅清空所有分配内存
	void cleanAllMemory(void);
	//函数只能在构造函数调用唯一一次，并赋予pHead有效的指针初值
	//指正分配内存由nDefaultCharNumber以字符数为单位指定
	void initialize(LONG_PTR nDefaultCharNumber = 0);
	static BOOL _AddPathAnEnt(LPTSTR pResBuffer, INT EndsNmber = 1);
	static LPCTSTR _ExtendPathToQuality(LPCTSTR pResBuffer, LPTSTR pDesBuffer);
	static LPCTSTR _ExtendPathToNormal(LPCTSTR pResBuffer, LPTSTR pDesBuffer);
	static LPCTSTR _GetLastStrByPath(LPCTSTR strPath);
	static LPCTSTR _GetLastStrByPath2(LPCTSTR strPath);
	static LPTSTR _DeleteLastStrFromPath(LPTSTR strPath);
	static BOOL _FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);
	BOOL _RegSZ(HKEY mainKey, LPCTSTR mainPass, LPCTSTR optionName, LPTSTR& pString, BOOL isSet = TRUE, LONG_PTR storeSize = MAX_PATH);
	static inline  INT c2i(TCHAR m_char)
	{
		return ((INT)m_char - 48);
	}
	static inline  TCHAR i2c(LONG_PTR i)
	{
		return ((TCHAR)(i + 48));
	}
	static inline  double qcf(double a, LONG_PTR cf)
	{
		double sum = 1;
		for (INT i = 1; i <= cf; i++)
		{
			sum = a * sum;
		}
		if (a != 0)
		{
			for (INT i = -1; i >= cf; i--)
			{
				sum = 1 / a * sum;
			}
		}
		return sum;
	}

	static inline LONG_PTR _strlen(LPCTSTR _in_Ptr);

	LPTSTR _newbuffer(LONG_PTR nNeedSize, LONG_PTR nPosIndex);
	//在扩展数据中构造一个（含末尾0空间的）字符串buffer，参数为需要保存的字符串不含末尾0的字符个数,并设置存储标记；
	PCLStringExData makeNewTempStringBuf(size_t stringSizeNoEndZero, size_t charSize = sizeof(TCHAR));

public:
	typedef CLString& ref;
	typedef const CLString& refc;

	//构造函数（默认）
	//构造函数（LPCTSTR版本）接收一个初始化字符串指针。
	//构造函数（const CLString& 版本）接收一个初始化CLString对象，但是该函数不会拷贝源对象的其他信息，比如时间，文件句柄，字符串向量，网络句柄等数据。(若要拷贝扩展数据请调用copyExData)
	//构造函数（指定初始化内存字符数量版本）构造阶段就申请指定的内存避免后面的扩容平凡申请内存。若未来存储字符数更多，则会继续自动扩容。该函数会自动考虑结尾0字符的空间。
	//构造函数（变参数版本）接收字符串指针，并以0或NULL指针作为最后一个参数，用作结尾标记，否则后果无法预料。第一个参数将用于初始化对象，后面的字串用于追加到对象尾部。
	CLString();
	CLString(LPCSTR pString);
	CLString(LPCWSTR pString);
	CLString(LPCTSTR pString1, LPCTSTR pString2, ...);
	CLString(const CLString& mString);
	explicit CLString(LONG_PTR nDefaultCharNumber);

	CLString(CLString&& right);
	CLString& operator=(CLString&& right);

	//析构函数
	virtual ~CLString();

	//交换两个对象的所有数据所有数据
	CLString& swap(CLString& other);

	CLString&& move() { return std::move(*this); }

	//设置文件的操作模式，参数为1时为C模式，为0时为windows API模式（注意：API模式的速度约为C模式的1.3倍）
	//BOOL setFileOpenedType(INT m_NewFileOpenedType = 1);

	//加法“+”函数重载
	friend CLString operator+(const CLString& str1, const CLString& str2);
	//加法“+”函数重载
	friend CLString operator+(LPCTSTR pStr1, const  CLString& str2);
	//加法“+”函数重载
	friend CLString operator+(const CLString& str1, LPCTSTR pStr2);

	//拷贝数据，包含扩展数据（扩展数据只拷贝：临时字符串缓冲，时间结构数据，字符串列表数据，拆分输出当前状态等数据，（不拷贝文件句柄和网络句柄））
	ref copy(CLStringRC str);
	//拷贝扩展数据（只拷贝：临时字符串缓冲，时间结构数据，字符串列表数据，拆分输出当前状态等数据，（不拷贝文件句柄和网络句柄））
	ref copyExData(CLStringRC str);


	//1、赋值函数。参数1：字符串指针。
	//2、赋值函数。参数：可变参数，参数接受，字符串指针。
	//   函数将参数1字串初始化对象，并用后面参数所指向的字符串追加到对象尾部。
	//   注意：可变参数最后一个参数必须为一个0或NULL指针作为结束标记；
	//   也就是说函数参数一旦遇到NULL指针将结束继续取参数；
	//   若没有0指针作为结束，则函数无限取参数下去，后果将不可预料。
	ref set(LPCSTR pString);
	ref set(LPCWSTR pString);
	ref set(LPCTSTR pString1, LPCTSTR pString2, ...);
	ref set(CHAR ch);
	ref set(WCHAR ch);
	//赋值目标中的n个字符到对象，在末尾增加末尾0；
	ref setn(LPCTSTR pStr, LONG_PTR nSiChar);

	//拷贝并连接另一个字符串到当前对象尾部。参数1：字符串指针。
	ref append(LPCSTR pString);
	ref append(LPCWSTR pString);

	//拷贝并连接另一个TCHAR对象到当前对象尾部。参数1：TCHAR对象。
	ref append(CHAR ch);
	ref append(WCHAR ch);

#ifndef CLSTRING_FTOSLIMIT
#define CLSTRING_FTOSLIMIT  26
#define CLSTRING_FTOSLIMIT_MAXACCURACY  15
#endif // !CLSTRING_FTOSLIMIT
#ifndef CLSTRING_LTOSLIMIT
#define CLSTRING_LTOSLIMIT  19
#endif // !CLSTRING_LTOSLIMIT


	//将数字转化成字符串后增加到对象末尾，若失败则增加空字符串。
	//参数3 保存一个结果输出的指针
	//原样输出数字，v可以是任何类型的数字
	//maxAccuracy表示小数显示精度（最大支持CLSTRING_FTOSLIMIT_MAXACCURACY位小数精确），若精度大于实际小数位函数会自动抹去末尾0，并且不会用科学计数法输出，全部为原样输出
	//nBufSizeInChar最小要求有ltos CLSTRING_LTOSLIMIT和 ftos CLSTRING_FTOSLIMIT个字符的空间，否则直接返回false
	ref appendl(LONG_PTR v, BOOL* _out_bIsSuccess = 0);
	ref appendf(double v, size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY, BOOL* _out_bIsSuccess = 0);

	//重载操作符<< 其优先级遵循C++原则
	ref operator<<(const CHAR ch);
	ref operator<<(const byte number);
	ref operator<<(const WCHAR ch);
	ref operator<<(LPCSTR pString);
	ref operator<<(LPCWSTR pString);
	ref operator<<(const CLString& mString);
	ref operator<<(const CLString* pString);
	ref operator<<(const long long number);
	ref operator<<(const unsigned long long number);
	ref operator<<(const long number);
	ref operator<<(const unsigned long number);
	ref operator<<(const int number);
	ref operator<<(const unsigned int number);
	ref operator<<(const short number);
	ref operator<<(const unsigned short number);
	ref operator<<(const float number);
	ref operator<<(const double number);
	ref operator<<(const long double number);
	ref operator<<(const bool number);

	//赋值函数。字符串，CLString对象，数字。
	template<typename _Class>ref set(_Class obj) {
		empty() << obj; return *this;
	}
	inline ref set(const CLString& mString) { set(mString.string()); return *this; }
	inline ref set(const CLString* pString) { set(pString ? pString->string() : (LPCTSTR)NULL); return *this; }
	//拷贝并连接另一个CLString对象到当前对象尾部。参数1：字符串，CLString对象，数字。
	template<typename _Class>ref append(_Class obj) {
		(*this) << obj; return *this;
	}
	inline ref append(const CLString& mString) { append(mString.string()); return *this; }
	inline ref append(const CLString* pString) { append(pString ? pString->string() : (LPCTSTR)NULL); return *this; }
	//赋值操作符重载。参数：字符串，CLString对象，数字。
	template<typename _Class>ref operator=(_Class obj) {
		set(obj); return *this;
	}
	inline ref operator=(const CLString& mString) { set(mString.string()); return *this; }
	inline ref operator=(const CLString* pString) { set(pString ? pString->string() : (LPCTSTR)NULL); return *this; }

	//重载一个 % 操作，用于符号运算中实现在最前端像赋值函数set()的功能。参数：字符串，对象，数字。
	//它有比 + 和 <<  = 等更高的优先级，从而可以在运算符操作串中的最前端实现首先做初始化的操作。
	//而不用像 = 操作一样是低优先级后置的，必须在后面的操作串结束后才执行。
	template<typename _Class>ref operator%(_Class obj) {
		empty() << obj; return *this;
	}
	inline ref operator%(const CLString& mString) { set(mString.string()); return *this; }
	inline ref operator%(const CLString* pString) { set(pString ? pString->string() : (LPCTSTR)NULL); return *this; }
	//操作符重载，拷贝并连接另一个字符串到当前对象尾部。参数：字符串，CLString对象，数字。
	template<typename _Class>ref operator+=(_Class obj) {
		append(obj); return *this;
	}
	inline ref operator+=(const CLString& mString) { append(mString.string()); return *this; }
	inline ref operator+=(const CLString* pString) { append(pString ? pString->string() : (LPCTSTR)NULL); return *this; }


	//按指定格式格式化对象，类似CString中的format函数功能。
	//特别注意，调用此函数会清空原内容，不能将自身字串指针作为字符串参数源。但是可以作为格式化字串的源。
	ref format(LONG_PTR maxStrlen, LPCTSTR szFormat, ...);
	ref format(LPCTSTR szFormat, ...);

	//将保存字符串输出到目标变量,使用前可先设置切分标记，调用operator*
	ref operator>>(char& ch);
	ref operator>>(unsigned char& number);
	ref operator>>(wchar_t& ch);

	ref operator>>(ref mString);
	ref operator>>(CLString* pString);
	ref operator>>(long long& number);
	ref operator>>(unsigned long long& number);
	ref operator>>(long& number);
	ref operator>>(unsigned long& number);
	ref operator>>(int& number);
	ref operator>>(unsigned int& number);
	ref operator>>(short& number);
	ref operator>>(unsigned short& number);
	ref operator>>(float& number);
	ref operator>>(double& number);
	ref operator>>(long double& number);
	ref operator>>(bool& number);

	//设置分割符号，并初始化分割工作变量，并做切分
	ref operator*(LPCTSTR lpSplit);
#define CLRet _T("\r\n")//windows风格换行符
#define CLComma _T(",")//逗号
#define CLBkSla _T("/")//反斜杠，除号
#define CLSla _T("\\")//正斜杠
#define CLVerL _T("|")//竖线
#define CLDot  _T(".")//点
#define CLTab  _T("\t")//跳格
#define CLSpc  _T(" ")//空格


	//原样输出数字，v可以是任何类型的数字
	//maxAccuracy表示小数显示精度（最大支持CLSTRING_FTOSLIMIT_MAXACCURACY位小数精确），若maxAccuracy大于实际小数位函数会自动抹去末尾0，否则对大于的精度位四舍五入，
	//若<=0则不输出小数，并且不会用科学计数法输出，全部为原样输出
	//nBufSizeInChar最小要求有ltos CLSTRING_LTOSLIMIT和 ftos CLSTRING_FTOSLIMIT个字符的空间，否则直接返回false
	//返回值*_out_bIsSuccess为false时，buf中的值将是不可预测的值，只有当返回true时，结果才有效。
	static BOOL ltos(LONG_PTR v, LPTSTR lpBuf, size_t nBufSizeInChar = CLSTRING_LTOSLIMIT);
	static BOOL ftos(double v, LPTSTR lpBuf, size_t nBufSizeInChar = CLSTRING_FTOSLIMIT, size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY);
	ref ltos(LONG_PTR v, BOOL* _out_bIsSuccess = NULL);
	ref ftos(double v, size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY, BOOL* _out_bIsSuccess = NULL);
	//字符串转double类型，小数精度最好不要超过CLSTRING_FTOSLIMIT_MAXACCURACY位，有效位总数不超过17位，否则可能失真（压力测试为atoi速度2.3倍）
	static double stof(LPCTSTR lpBuf);
	//字符串转LONG_PTR类型，有效位总数不超过18位，否则可能失真（压力测试为atoi速度2.3倍）
	static long long stoll(LPCTSTR lpBuf);

	double stof()const;
	int stoi()const;
	long stol()const;
	long long stoll()const;
	inline unsigned char stob()const { int r = stoi(); return r < 0 ? (unsigned char)(0) : (r > 255 ? (unsigned char)(255) : (unsigned char)(r)); }
	inline unsigned int stoui()const { int r = stoi(); return r < 0 ? (unsigned int)(0) : (unsigned int)(r); }
	inline unsigned long stoul()const { long r = stol(); return r < 0 ? (unsigned long)(0) : (unsigned long)(r); }
	inline static unsigned char stob(LPCTSTR lpBuf) { int r = _ttoi(lpBuf); return r < 0 ? (unsigned char)(0) : (r > 255 ? (unsigned char)(255) : (unsigned char)(r)); }
	inline static unsigned int stoui(LPCTSTR lpBuf) { int r = _ttoi(lpBuf); return r < 0 ? (unsigned int)(0) : (unsigned int)(r); }
	inline static unsigned long stoul(LPCTSTR lpBuf) { long r = _ttol(lpBuf); return r < 0 ? (unsigned long)(0) : (unsigned long)(r); }
	inline unsigned long long stoull()const { long long r = stoll(); return r < 0 ? (unsigned long long)(0) : (unsigned long long)(r); }
	inline static long long stoull(LPCTSTR lpBuf) { long long r = stoll(lpBuf); return r < 0 ? (unsigned long long)(0) : (unsigned long long)(r); }

	//取得对象字符串缓冲区的指针。类似CLString中的getBuffer函数功能。
	inline LPCTSTR string() const { assert(pHead != NULL); return pHead; }
	inline LPCSTR stringA() { assert(pHead != NULL); return getASCII(); }
	inline LPCWSTR stringW() { assert(pHead != NULL); return getUnicode(); }


	//调用该函数后，产生并返回一个可用的初始化后的字符缓冲区指针，
	//该返回指针值可作为其他函数参数和缓存空间使用。参数1：指定最大可用存储的字符数。
	//无需额外指定字符串结尾0的空间，函数会自动判断并增加。
	//函数会自动清理目标内存空间iStoreMaxCharLen+1范围内的内存为0，因此不需要人为的做初始化操作。
	//注意：storeA和storeW当为异形体存储时候，在调用后还需调用saveExStore()把存储内容转换到字符串空间中保存，才能做后续string,set,append等字符串变换功能，否则可能结果不对；
	//若当前结构非异形体存储则storeA和storeW在功能上和store并无差别，也无需显示调用saveExStore()做存储；
	LPTSTR store(LONG_PTR iStoreMaxCharLen = MAX_PATH);
	LPSTR storeA(LONG_PTR iStoreMaxCharLen = MAX_PATH);
	LPWSTR storeW(LONG_PTR iStoreMaxCharLen = MAX_PATH);

	//若调用过storeA或storeW存储异格式内容，则根据表示转换存储到内部到字符串，转换成功则归零临时缓冲区，如果未存储什么也不做
	ref saveExStore();

	//返回对象已分配了的缓冲区(以字节计)的大小。
	LONG_PTR buflen(void);

	//返回对象字符串长度(以字符计, 即空值结束符之前字符数目) 改变标记m_changeFlag为FALSE;
	LONG_PTR strlen(void);
	inline LONG_PTR size() { return strlen(); };
	inline LONG_PTR size0() { return strlen() + 1; };

	//返回对象字符串长度(以字节计, 即空值结束符之前字符的字节数目)
	LONG_PTR strlenInByte(void);
	LONG_PTR strlenInByte0(void) { return (strlen() + 1) * sizeof(TCHAR); }

	//若对象保存的字符串是一个有效路劲，可以在该结尾增加反斜杠"\"。
	//参数1：增加的反斜杠个数；
	ref addAnPathEnd(INT endNumber = 1);

	//若对象保存的字符串是一个有效路劲，可以把字串中
	//所有单"\"变为"\\"。参数1：是否保存新值。
	//方法时仍然返回变化前的原字符串。
	ref extendPathToQuality();

	//若对象保存的字符串是一个有效路劲，可以把字串中
	//所有双"\\"变为"\"。参数1：是否保存新值。
	//方法时仍然返回变化前的原字符串。
	ref extendPathToNormal();

	//若对象保存的字符串是一个有效路劲，则返回最后一个"\"后面的字符串（不包含"\"）。
	LPCTSTR getLastStrByPath();

	//若对象保存的字符串是一个有效路劲，则删掉最后一个"\"及后面的字符串，并返回（不包含末尾"\"）。
	ref deleteLastStrFromPath();

	//若对象保存的字符串中含有数字，可按参数1指定的位置提取出数字（无法提取出负号，故所有数都将为正数）
	//并以double类型返回。参数1：以0为起始计的位置索引号。例：1、若字符串为“123.456这是一串数”，则提取
	//123.456时参数1必须设置为0。2、若字符串为“abc123.456这是一串数”，则提取
	//123.456时参数1必须设置为1.3、若如果参数索引超出字串范围或字串本不存在数字，则函数返回数-1。
	double getUNegNumByPos(INT numPos = 0);
	static double getUNegNumByPos(LPCTSTR lpStr, INT numPos = 0);

	//若对象保存的字符串中含有数字，可按参数1指定的位置提取出数字（可提取正数负数和0）
	//并以double类型返回。参数1：以0为起始计的位置索引号。
	//例：1、若字符串为“123.456这是一串数”，则提取123.456时参数1必须设置为0；
	//2、若字符串为“abc-123.456这是一串数”，则提取-123.456时参数1必须设置为1；
	//3、若如果参数索引超出字串范围或字串本不存在数字，则函数返回数0；
	//4、若字符串为“abc-.456这是一串数，.001，652，00075”，则可提取出-0.456、0.001和75，参数1可设置为1、2和4；
	//5、若字符串为“abc.abc123”，单独一个“.”则可提取出数字0.0（占一个数字排序位），参数1可设置为1，若要提取123，则参数1必须设置为2；
	double getDbNumByPos(INT numPos = 0);
	static double getDbNumByPos(LPCTSTR lpStr, INT numPos = 0);

	//比较对象与指定字符串是否相同，相同则返回TRUE。参数1：指定的字符串指针。
	BOOL isEqual(LPCTSTR pStrCmp);

	//比较对象与指定CLString对象内容是否相同，相同则返回TRUE。参数1：另一个CLString对象。
	BOOL isEqual(const CLString& strCmp);

	//操作符重载，判断字符串是否相同相同则返回TRUE。参数1：指定的字符串指针。
	//BOOL operator==(LPCTSTR pStrCmp);

	//操作符重载，判断CLString对象内容是否相同，相同则返回TRUE。参数1：另一个CLString对象。
	//BOOL operator==(const CLString& strCmp);

	//操作符重载，判断字符串是否不同，不同则返回TRUE。参数1：指定的字符串指针。
	//BOOL operator!=(LPCTSTR pStrCmp);

	//操作符重载，判断CLString对象内容是否不同，不同则返回TRUE。参数1：另一个CLString对象。
	//BOOL operator!=(const CLString& strCmp);

	friend bool operator < (CLStringRC str1, LPCTSTR str2);
	friend bool operator < (LPCTSTR str1, CLStringRC str2);
	friend bool operator < (CLStringRC str1, CLStringRC str2);
	friend bool operator <= (CLStringRC str1, LPCTSTR str2);
	friend bool operator <= (LPCTSTR str1, CLStringRC str2);
	friend bool operator <= (CLStringRC str1, CLStringRC str2);
	friend bool operator > (CLStringRC str1, LPCTSTR str2);
	friend bool operator > (LPCTSTR str1, CLStringRC str2);
	friend bool operator > (CLStringRC str1, CLStringRC str2);
	friend bool operator >= (CLStringRC str1, LPCTSTR str2);
	friend bool operator >= (LPCTSTR str1, CLStringRC str2);
	friend bool operator >= (CLStringRC str1, CLStringRC str2);
	friend bool operator == (CLStringRC str1, LPCTSTR str2);
	friend bool operator == (LPCTSTR str1, CLStringRC str2);
	friend bool operator == (CLStringRC str1, CLStringRC str2);
	friend bool operator != (CLStringRC str1, LPCTSTR str2);
	friend bool operator != (LPCTSTR str1, CLStringRC str2);
	friend bool operator != (CLStringRC str1, CLStringRC str2);

	//返回值为0，表示相同
	INT compareNoCase(const CLString& strCmp);
	//返回值为0，表示相同
	INT compareNoCase(LPCTSTR pStrCmp);
	//返回值为0，表示相同
	INT compare(const CLString& strCmp);
	//返回值为0，表示相同
	INT compare(LPCTSTR pStrCmp);

	/*
	对象包含有以pStrCmp所指的字符，但不要求这些字符是连续的，只要些字字符前后顺序和pStrCmp保持一直即可。
	返回值: INT
	0， 表示全匹配
	>0，表示匹配的个数
	-1，指针错误(无任何匹配)
	*/
	INT haveCharNoCase(LPCTSTR pStrCmp);
	INT haveCharNoCase(const CLString& strCmp);

	//若对象保存的是以Unicode编码形式的字符串，则函数可以临时返回一个该字串的
	//多字节(EnCode_ASCII)版本的字串，而不影响或更改对象保存的原值。
	//注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	//CHAR* getMultiByte(void);
	//若对象保存的是以多字节(EnCode_ASCII)编码形式的字符串，则函数可以临时返回一个该字串的
	//Unicode编码版本的字串，而不影响或更改对象保存的原值。
	//注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	//WCHAR* getWideChar(void);

   //返回保存字符串的ascii版本，注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	LPCSTR getASCII();
	//返回保存字符串的EnCode_UTF8版本，注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	LPCSTR getUTF8();
	//返回保存字符串的UnicodeLittleEndian版本，注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	LPCWSTR getUnicodeLittleEndian();
	//返回保存字符串的UnicodeBigEndian版本，注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	LPCWSTR getUnicodeBigEndian();
	//返回保存字符串的Unicode版本，注意：返回值缓冲区内存由类来管理释放，不要调用delete或free。
	LPCWSTR getUnicode();

	//静态方法：成功则返回转换后的字符串缓冲区指针（该指针需要在使用后显式delete[]来释放），若转换失败则返回NULL指针。
	static LPSTR unicodeToAscii(LPCWSTR lpStr);
	static LPSTR unicodeToUtf8(LPCWSTR lpStr);
	static LPWSTR asciiToUnicode(LPCSTR lpStr);
	static LPSTR asciiToUtf8(LPCSTR lpStr);
	static LPWSTR utf8ToUnicode(LPCSTR lpStr);
	static LPSTR utf8ToAscii(LPCSTR lpStr);

	//成员方法：成功则返回转换后的字符串缓冲区指针（该指针保存在内部变量(makeDEx()->m_pointer_)中，不需要delete[]），若转换失败则返回NULL指针。
	LPCSTR unicodeToAsciiInner(LPCWSTR lpStr);
	LPCSTR unicodeToUtf8Inner(LPCWSTR lpStr);
	LPCWSTR asciiToUnicodeInner(LPCSTR lpStr);
	LPCSTR asciiToUtf8Inner(LPCSTR lpStr);
	LPCWSTR utf8ToUnicodeInner(LPCSTR lpStr);
	LPCSTR utf8ToAsciiInner(LPCSTR lpStr);

	//让用户有能力访问(makeDEx()->m_pointer_)临时指针数据
	inline LPCSTR getTmpBufMultiByte()const { return pDataEx ? pDataEx->m_pointer_.pMultiByte : NULL; }
	inline LPCWSTR getTmpBufWideChar()const { return pDataEx ? pDataEx->m_pointer_.pWideChar : NULL; }
	//让用户有能力访问(makeDEx()->m_pointer_)临时指针缓冲区的大小（字节记）
	inline LONG_PTR getTmpBufSize()const { return pDataEx ? pDataEx->m_pointerBufSizeInByte_ : 0; }

	//UNICODE字符串转换为多字节字符串；
	//static BOOL wideCharToMultiByte(LPSTR pDesMultiByteBuf, LPWSTR pSorWideChar, LONG_PTR nDesBufLen);
	//多字节字符串转换为UNICODE字符串；
	//static BOOL multiByteToWideChar(LPWSTR pDesWideCharBuf, LPSTR pSorMultiByteChar, LONG_PTR nDesBufLen);

	//若文字保存的是一个全路径或文件名，则替换调扩展名,带".xxx"形式的，如果没有扩展名则增加；
	ref replaceExtName(LPCTSTR newExtName);

	//注册表相关---------------------------------------------------------------------------------------------------------------------------------
	//将对象中字符串保存到指定注册表位置。
	//参数1：主键；参数2：子路劲；参数3：键名。
	BOOL setReg(HKEY mainKey, LPCTSTR mainPass, LPCTSTR optionName);

	//将指定的注册表位置的字符串读入到对象字符缓冲。
	//参数1：主键；参数2：子路劲；参数3：键名；参数4：设定的缓冲区大小。
	BOOL getReg(HKEY mainKey, LPCTSTR mainPass, LPCTSTR optionName, LONG_PTR stroeSize = MAX_PATH);

	//若对象保存的字符串是一个有效的注册表路劲，则可以删除该路劲下指定的键及键值。
	//参数1：主键；参数2：是否删除整个键及键值；参数3：要删除键的键名。
	//若参数2及参数3均为默认值则删除路径下的所有键及键值。
	//删除一整个路径项及某个Key项用isToDeleteKey=TRUE，调用RegDeleteKey；
	//删除某个Key项下的值用isToDeleteKey=FALSE，调用RegDeleteValue；
	BOOL asPathToDeleteReg(HKEY mainKey, BOOL isToDeleteKey = TRUE, LPCTSTR keyName = NULL);

	//若对象保存的字符串是一个有效的注册表路劲，则检索路劲下的子项，而不是子键(不是key-value对)，检索子健及键值请用asPathToEnumRegValue。
	//若枚举成功返回TRUE。该函数作为asPathToEnumRegKey函数在Win32/NT及以上的扩展版本使用。
	//参数1：主键；参数2：要枚举的项索引值；参数3：保存项名的缓冲区指针；
	//参数4：保存项名的缓冲区大小；参数5：键类型名字串指针；参数6：键类型名字串缓冲大小（字节计）；
	//参数7：键的写入时间信息结构体。
	BOOL asPathToEnumRegKeyEx(HKEY mainKey, DWORD dwIndex, LPTSTR subkey_name, DWORD subKeyLen = MAX_PATH,
		LPTSTR lpClass = NULL,     // address of buffer for class string
		LPDWORD lpcbClass = NULL,  // address for size of class buffer
		PFILETIME lpftLastWriteTime = NULL // address for time key last written to
	);

	//若对象保存的字符串是一个有效的注册表路劲，则可用来枚举该字串所示路劲下的项名。
	//若枚举成功返回TRUE。在Win32/NT及以上请采用扩展版本的asPathToEnumRegKeyEx函数。
	//参数1：主键；参数2：要枚举的项索引值；参数3：保存项名的缓冲区指针；
	//参数4：保存项名的缓冲区大小。
	BOOL asPathToEnumRegKey(HKEY mainKey, DWORD dwIndex, LPTSTR subkey_name, DWORD subKeyLen = MAX_PATH);

	//若对象保存的字符串是一个有效的注册表路劲，则可用来枚举该字串所示路劲下的键及其对应键值。
	//若枚举成功返回TRUE。参数1：主键；参数2：要枚举的项索引值；参数3：保存键名的缓冲区大小；
	//参数4：保存键名的缓冲区指针；参数5：保存值的缓冲区（或变量）的大小（以字节计）；
	//参数6：保存值的缓冲区（或变量）的指针；参数7：键值的类型码。
	BOOL asPathToEnumRegValue(
		HKEY mainKey,              // handle to key to query
		DWORD dwIndex,          // index of value to query		
		DWORD valueNameBufSize,  // address for size of value buffer	
		LPTSTR valueNameBuf,     // address of buffer for value string		
		DWORD dataSizeInByte,       // address for size of data buffer
		BYTE* dataBuffer,          // address of buffer for value data
		DWORD Type = REG_SZ        // address of buffer for type code
	);

	//若对象保存的字符串是一个有效的注册表路劲，则可用来获取该字串所示路劲下的key对应value值;
	//不需要索引index，但通过检索的是keyValueName名必须匹配（可不区分大小写）且valueType返回数据类型必须匹配（例如 REG_SZ,REG_DWORD），否则失败。
	//若枚举成功返回TRUE。参数1：主键；参数2：要枚举的项索引值；参数3：保存键名的缓冲区大小；
	//参数4：保存键名的缓冲区指针；参数5：保存值的缓冲区（或变量）的大小（以字节计）；
	//参数6：保存值的缓冲区（或变量）的指针；参数7：键值的类型码。
	BOOL asPathToGetRegValue(
		HKEY mainKey,              // handle to key to query	
		LPCTSTR valueName,     // address of buffer for value string		
		DWORD dataBufSizeInByte,       // address for size of data buffer
		LPVOID dataBuffer,          // address of buffer for value data	
		DWORD valueType = REG_SZ        // address of buffer for type code
	);


	//http及网络操作相关---------------------------------------------------------------------------------------------------------------------------------

	//Http Get请求,lpszObjectName表示参数字串，pHeaders指向保存要增加的报头字串的数组，nHeaderNum报头数量，
	//bU2G是否将结果由UTF转为多字节，openErrorAlert是否开启错误提示弹框。
	BOOL httpGet(LPCTSTR lpszObjectName = NULL, LPCTSTR pHeaders[] = NULL, LONG_PTR nHeaderNum = 0, BOOL bU2G = TRUE, BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL);
	//向内部保存的以打开的请求发送一个报头；	BOOL httpAddHeaders(LPTSTR pHeaderString = NULL);
	BOOL httpAddHeaders(LPTSTR pHeaderString = NULL);
	//调用Http操作前的初始化，及连接函数，serverDomain表示不带http：//的域名（或ip），port服务端口，
	//openErrorAlert是否开启错误提示弹框。
	BOOL httpInitGet(LPCTSTR serverDomain, INTERNET_PORT port = 80, BOOL openErrorAlert = TRUE);
	//把UTF字串转为多字节后增加到string对象尾部；
	static void U2G(const char* utf8, std::string& re);
	//释放所有打开的Http资源句柄
	ref httpClose();
	//将Url指向的地址文件内容保存到对象中,Url必须带http://头;urlEncode指定网络数据的格式。该函数不需要调用httpClose()
	BOOL httpOpenUrl(LPCTSTR Url, BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL, BYTE urlEncode = EnCode_ASCII);


	//文件系统相关---------------------------------------------------------------------------------------------------------------------------------

	//功能：[静态函数]枚举指定路径下的文件或文件夹的路劲字符串，返回到std::vector中；
	//返回值：无返回
	//参数：stringVector 保存路劲枚举结果的std::vector<LPCTSTR>对象的引用；注：结果字符串均为动态分配内存，需要由调用者逐一释放；
	//参数：lpPath 起始路劲，路径最后一个字符可以加入路径分隔符（'\\'或'/'），也可以不加；路径也支持相对路径及他们的组合形式，如 "./" "../" ".\\" ".\\..\\" 等；
	//参数：lpExName [文件模式]下要列举匹配的文件扩展名(必须要带.号)；[文件夹模式]下要匹配的最末级的文件夹的名称；当该参数值为 NULL,0,_T("")或_T('')时将匹配对应模式下的所有结果而不做任何过滤；
	//参数：bRecursion 是否采用递归调用（默认=TRUE），若采TRUE，则遍历搜索目录下的文件夹及他们所有的子文件夹；
	//参数：bEnumFiles 是否为文件模式（默认=TRUE）；若采TRUE为文件模式，结果只检索并保存文件的路径结果；否则为文件夹模式，只检索并保存文件夹的路径；	
	//参数：bClearVectorBefoteStart 是否在枚举前清空结果集对象并释放对应指向的动态内存（默认=TRUE 表示清空）；若，结果集有数据并且字符串指针指向的是非动态申请的内存空间，当参数为TRUE时将产生释放错误；
	//参数：bJustStoreEndName 是否仅仅存储末尾名而不是全路径（默认=FALSE 表示保存全路径）
	//注意：文件夹模式下的路径结果字符串的末尾字符不会含有路径分隔符（'\\'或'/'）
	//注意：通常系统返回的文件或文件夹路劲结果集std::vector是字符按升序排列的（小->大）
	static void fileEnumeration(std::vector<LPCTSTR>& stringVector, LPCTSTR lpPath, LPCTSTR lpExName,
		BOOL bRecursion = TRUE, BOOL bEnumFiles = TRUE, BOOL bClearVectorBefoteStart = TRUE, BOOL bJustStoreEndName = FALSE);

	//功能：枚举指定路径下的文件或文件夹的路劲字符串，返回到对象的std::vector结果集变量中；
	//返回值：保存有检索到的文件或文件夹路径的对象内部结果集变量std::vector的常量引用；
	//参数：lpPath 起始路劲，路径最后一个字符可以加入路径分隔符（'\\'或'/'），也可以不加；路径也支持相对路径及他们的组合形式，如 "./" "../" ".\\" ".\\..\\" 等；
	//参数：lpExName [文件模式]下要列举匹配的文件扩展名(必须要带.号)；[文件夹模式]下要匹配的最末级的文件夹的名称；当该参数值为 NULL,0,_T("")或_T('')时将匹配对应模式下的所有结果而不做任何过滤；
	//参数：bRecursion 是否采用递归调用（默认=TRUE），若采TRUE，则遍历搜索目录下的文件夹及他们所有的子文件夹；
	//参数：bEnumFiles 是否为文件模式（默认=TRUE）；若采TRUE为文件模式，结果只检索并保存文件的路径结果；否则为文件夹模式，只检索并保存文件夹的路径；	
	//参数：bClearVectorBefoteStart 是否在枚举前清空结果集对象（默认=TRUE 表示清空）；该函数内存由对象本身控制，任何情况下设置该参数将不引起何释放错误的产生；
	//参数：bJustStoreEndName 是否仅仅存储末尾名而不是全路径（默认=FALSE 表示保存全路径）
	//注意：文件夹模式下的路径结果字符串的末尾字符不会含有路径分隔符（'\\'或'/'）
	//注意：通常系统返回的文件或文件夹路劲结果集std::vector是字符按升序排列的（小->大）
	//注意：该结果集的后续可通过显式调用对象的getVT()方法从新获得其常量引用;
	const std::vector<LPCTSTR>& fileEnumeration(LPCTSTR lpPath, LPCTSTR lpExName, BOOL bRecursion = TRUE,
		BOOL bEnumFiles = TRUE, BOOL bClearVectorBefoteStart = TRUE, BOOL bJustStoreEndName = FALSE);

	//创建多级目录路径。注意：路劲以“/”或“\\”结尾均可,目录分隔符可以使任意个“/”或“\\”的组合。
	//比如 "c://\\123\\/abc////"  代表生成 "c:\123\abc" 目录
	//路劲存在或创建成功返回true，路劲创建失败返回false。
	static BOOL createDirectory(LPCTSTR lpszPath);
	//由文件名，创建多级目录路径。注意：路劲以“/”或“\\”作为分割均可,目录分隔符可以使任意个“/”或“\\”的组合。
	//以下三种形式均会生成目录：
	//比如 "c://\\123\\/abc////file1.txt"  代表生成 "c:\123\abc" 目录
	//比如 "c://\\123\\/abc////file1"  代表生成 "c:\123\abc" 目录
	//比如 "c://\\123\\/abc////"  代表生成 "c:\123\abc" 目录
	//路劲存在或创建成功返回true，路劲创建失败返回false。
	static BOOL createDirectoryByFileName(LPCTSTR lpszFileName);
	BOOL createDirectory(void);
	//取得系统特殊对象或文件夹的路劲保存到对象字符串。参数详API函数：SHGetSpecialFolderPath。
	//默认参数下将取得桌面文件夹的全路劲 nFolder=CSIDL_DESKTOP。
	ref getSpecialFolderPath(INT nFolder = CSIDL_DESKTOP, INT storeLen = MAX_PATH, HWND hwndOwner = NULL, BOOL fCreate = FALSE);
	//若对象保存的字符串是一个文件或文件夹的路径（绝对路径、相对路径，文件或文件夹均可），
	//则检查该文件或文件夹是否存在。存在则返回TRUE。
	//注意：路劲不能以“/”或“\\”结尾。
	BOOL filePathExists(void);
	//若对象保存的字符串是一个文件夹的路径（绝对路径、相对路径均可），则检查该文件夹是否存在。存在则返回TRUE。
	//注意：路劲不能以“/”或“\\”结尾。
	BOOL folderExists(void);
	//若对象保存的字符串是一个文件夹的路径（绝对路径、相对路径均可），则检查该文件夹是否存在。存在则返回TRUE。
	//注意：路劲不能以“/”或“\\”结尾。
	BOOL findFirstFileExists(DWORD dwFilter = FILE_ATTRIBUTE_DIRECTORY);
	// 检查一个文件是否存在（绝对路径、相对路径，文件或文件夹均可）。注意：路劲不能以“/”或“\\”结尾。
	BOOL fileExists(void);

	//若指定了pFileDir为NULL：对象保存的字符串内容写入对象内部已维护一个文件句柄所指向的文件中，该句柄对象若为打开则当对象析构时被自动释放，也可以显式调用closeFile()函数主动关闭打开的文件句柄；
	//若指定了pFileDir文件路径：则关闭已有的文件句柄，并打开指定的新文件，若打开不成功则原维护的可用句柄不再可用；
	//C模式采用"w+"模式打开文件，API模式以读写方式打开文件（该模式更快）；
	//dwOpenMethod = OPEN_ALWAYS表示创建，并追加
	//lDistanceToMove Long，字节偏移量
	//lpDistanceToMoveHigh Long，指定一个长整数变量，其中包含了要使用的一个高双字偏移。可设为零(将声明变为ByVal)，表示只使用lDistanceToMove;
	//dwMoveMethod Long，下述常数之一
	//FILE_BEGIN lOffset将新位置设为从文件起始处开始算的起的一个偏移
	//FILE_CURRENT lOffset将新位置设为从当前位置开始计算的一个偏移
	//FILE_END lOffset将新位置设为从文件尾开始计算的一个偏移
	BOOL writeToFile(LPCTSTR pFileDir = NULL, DWORD dwOpenMethod = OPEN_ALWAYS, DWORD dwMoveMethod = FILE_END, LONG lDistanceToMove = 0, PLONG lpDistanceToMoveHigh = NULL, BOOL openAlertMsg = TRUE);
	BOOL writeLineToFile(LPCTSTR pFileDir = NULL, DWORD dwMoveMethod = FILE_END, LONG lDistanceToMove = 0, PLONG lpDistanceToMoveHigh = NULL, BOOL openAlertMsg = TRUE);

	//读取文本的一行（不包括末尾换行符），下次调用将跳入下一行读取。
	//返回值 >=0 表示当前读取行存在并且数值表示读取的字符数（空行任然为一行）
	//返回值 -1  表示当前行已经到达文件末尾，没有可读取的行数据
	LONG_PTR readLineFromFile(LPCTSTR  pFileDir = NULL, BOOL openAlertMsg = TRUE);
	//读取文本(从最后一行开始)的一行（不包括末尾换行符），下次调用将跳入下一行读取。
	//返回值 >=0 表示当前读取行存在并且数值表示读取的字符数（空行任然为一行）
	//返回值 -1  表示当前行已经到达文件末尾，没有可读取的行数据
	LONG_PTR readLineFromFileEnd(LPCTSTR pFileDir = NULL, BOOL openAlertMsg = TRUE);
	//关闭内部保存的已打开的文件句柄，并会更新文件的操作模式标记；
	BOOL closeFile();

	//读取一个文件的所有内容到对象中，调用后马上关闭文件句柄，读取失败返回NULL，否则返回文件内容。
	//该函数不会保存文件句柄，使用后不用调用closeFile()函数。
	LPCTSTR readFile(LPCTSTR pFilePath, BOOL openAlertMsg = TRUE);


	//用函数判断系统是Big Endian还是Little Endian
	//big-endian，   返回true;
	//little-endian，返回false
	inline static BOOL isBigEndian()
	{
		unsigned short test = 0x1122;
		if (*((unsigned char*)&test) == 0x11) return TRUE;
		else return FALSE;
	}
	//函数需要一个已打开的可读文件句柄，返回文件的编码格式。
	//目前只能判断ascii，Unicode bigEndian/littleEndian，UTF-8的文本。
	//参数2指定为true表示对文件做判断后将文件指针移动到有效数据起始位置（不包括最前端的标头字节）
	static byte checkTextFileEncode(HANDLE hFile, BOOL isMovePinterToBeging = FALSE);

	//转换原缓冲区中的文本格式，输出到lpOutTagbuf指向的指针。
	static BOOL swapEncode(const LPBYTE lpResbuf, LONG_PTR nResbufSizeInByte, byte uResEncode,
		LPBYTE& lpOutTagbuf, LONG_PTR& nOutTagbufSizeInByte, byte uTagEncode, LPBYTE lpDefaultBuf = 0, LONG_PTR nDefaultBufSizeInByte = 0);

	//字串转换操作类---------------------------------------------------------------------------------------------------------------------------------

	//把保存的字符串按szSplitChar分割符号(一个字符)拆分到一个std::vector中,
	//bClearVectorBefoteStart指定是否在执行前清空内部的std::vector对象，若不清空恻把新数据追加到std::vector尾部。
	//说明：多个分隔符并列将视为风格符之间任然是只有空终止字符的字符串。
	//函数不会对返回的vt做任何排序。
	//若原字符不含有分割字符串,则把整个原字符放入std::vector中（即无论分割符是否存在，总是能分出一个字符串）。
	std::vector<LPCTSTR>& split(const TCHAR szSplitChar = _T(','), BOOL bClearVectorBefoteStart = TRUE);
	std::vector<LPCTSTR>& split(LPCTSTR szSplitCharString, BOOL bClearVectorBefoteStart = TRUE);
	//清空对象内部保存的std::vector但不清除内存（释放std::vector管理的内存给系统请用clearInnerStringVectorMemery()）
	void clearInnerStringVector();
	//释放std::vector管理的内存给系统
	void clearInnerStringVectorMemery();
	//清空std::vector中保存的动态字符串内存
	static void clearStringVector(std::vector<LPCTSTR>& _v_str);
	//取得内部保存的字符串拆分std::vector对象的引用
	const std::vector<LPCTSTR>& getVT();
	//将std::vector降序排列（大->小），区分大小写。
	const std::vector<LPCTSTR>& makeVtDescendingOrder();
	//将std::vector降序排列（大->小），不区分大小写。
	const std::vector<LPCTSTR>& makeVtDescendingOrderI();
	//将std::vector升序排列（小->大），区分大小写。
	const std::vector<LPCTSTR>& makeVtAscendingOrder();
	//将std::vector升序排列（小->大），不区分大小写。
	const std::vector<LPCTSTR>& makeVtAscendingOrderI();

	inline size_t vtSize() { return getVT().size(); }//快速访问当前vt单元个数
	inline LPCTSTR vtAt(size_t i) { return getVT().at(i); }//快速访问vt字串

	//快速访问vt字串的ascii版本,特别注意：返回的是一个非持久型指针，在下一次调用后内容将会改变；
	LPCSTR vtAtA(size_t i);
	//快速访问vt字串的unicode版本,特别注意：返回的是一个非持久型指针，在下一次调用后内容将会改变；
	LPCWSTR vtAtW(size_t i);



	//系统操作类---------------------------------------------------------------------------------------------------------------------------------
	static HWND getConsoleHwnd(void); //取得当前控台程序的窗口句柄

	//计算windows下的时间差，需要两个存储时间的指针变量；
	static double calcTimeDelta(const SYSTEMTIME* beforetime, const SYSTEMTIME* behandtime);

	//调用系统API创建一个GUID值，反回GUID
	static GUID createGuid(void);
	//把一个GUID值格式化为字符串保存在对象内部，并返回字符串；
	LPCTSTR guidToString(const GUID& guid);

	//调用系统API创建一个GUID值，并把这个GUID值格式化为字符串保存在对象内部，并返回字符串；
	//若干参数pResGuid指向一个GUID变量，则创建的GUID会保存到指向的变量，若为NULL则不反回GUID值；
	LPCTSTR createGuidToString(GUID* pResGuid = NULL);

	//开始计时（计时启动），不首先调用此函数Timing系列函数将会出错；Ex函数采用微妙级别计数。
	void timingStart(void);
	void timingStartEx(void);
	//取得自计时开始起到当前调用时刻的系统消耗时间，以秒为单位；Ex函数采用微妙级别计数。
	double timingGetSpentTime(BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL);
	double timingGetSpentTimeEx(BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL);
	//取得自计时开始起到当前调用时刻的系统消耗时间，以秒为单位的字符串表述；
	//若无法计算返回NULL，成功则返回字符串表述，并且字串保存在对象中；
	LPCTSTR timingGetSpentTimeString(BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL);


	//取得windows系统GetLastError错误代码格式化字符串
	LPCTSTR getLastErrorString(DWORD nLastError);
	ref getLastErrorStringR(DWORD nLastError);
	static DWORD  getLastErrorString(DWORD nLastError, LPTSTR lpszOutPut, LONG_PTR nSize);
	//取得windows系统GetLastError错误代码对应格式化字符串的MessageBox
	DWORD getLastErrorMessageBox(DWORD nLastError, HWND hParentWnd = 0, LPCTSTR pTitle = _T("LastError"), UINT uType = MB_ICONWARNING, LPCTSTR pInsetStr = NULL, LPCTSTR pEndStr = NULL);
	//取得windows系统GetLastError错误代码（除了ERROR_SUCCEED）对应格式化字符串的MessageBox
	DWORD getLastErrorMessageBoxExceptSucceed(DWORD nLastError, HWND hParentWnd = 0, LPCTSTR pTitle = _T("LastError"), UINT uType = MB_ICONWARNING, LPCTSTR pInsetStr = NULL, LPCTSTR pEndStr = NULL);
	//用MessageBox显示当前保存值的内容
	INT messageBox(UINT nStyle = MB_OK, HWND hParentWnd = 0);
	ref messageBoxRef(UINT nStyle = MB_OK, HWND hParentWnd = 0);
	INT messageBoxTime(LPCTSTR boxTitle = 0, UINT nStyle = MB_OK, DWORD dwMilliseconds = INFINITE, HWND hParentWnd = 0);
	ref messageBoxTimeRef(LPCTSTR boxTitle = 0, UINT nStyle = MB_OK, DWORD dwMilliseconds = INFINITE, HWND hParentWnd = 0);
	INT messageBox(LPCTSTR boxTitle, UINT nStyle, HWND hParentWnd = 0);
	ref messageBoxRef(LPCTSTR boxTitle, UINT nStyle, HWND hParentWnd = 0);
	//临时通过一个MessageBox显示出当前保存的字符串内容。
	void showContent();
	//向cmd控制台输出字符串内容。
	ref logout();
	//向cmd控制台输出字符串内容。
	ref printf();
	//退行输出，返回本行输出字符长度，参数1：表示要退格回删的字符数（该值应该为上一次调用函数成功的返回值，以实现单行的回删覆盖显示）
	int printfBackCover(int bakCoverChars);
	//用于在控制台程序输入字符串使用。函数返回值：0 （按下Esc键），1 （按下回车键）。
	//参数1：输出到的字符串缓冲区指针，参数2：缓冲区大小以字符为单位，参数3：前显字符串指针，当为NULL时候将没有任何前向显示。
	//函数为阻塞函数，只有当按下回车或Esc键，函数才会退出。
	//函数最大只能输入nBufferSizeInChar-1个字符。
	//函数会自动对lpBuffer做初始化。
	static int getCharCmd(LPTSTR lpBuffer, LONG_PTR nBufferSizeInChar, LPCTSTR lpPreString = NULL);
	//全局函数：取得windows消息码所对应的消息定义字符串，Release模式下返回一行表述函数已弃用的提示字符串，而不是NULL指针。
	static LPCTSTR getWindowsMsgStringFromMsgID(UINT messageId);

	//由内部字符串产生一个异常，并抛出
#define CLSTRING_MAKE_EXCEPTION(logicName) void throw_##logicName() throw(std::logicName);
#pragma warning( disable : 4290 )
	CLSTRING_MAKE_EXCEPTION(exception);//异常
	CLSTRING_MAKE_EXCEPTION(logic_error);//逻辑错误
	CLSTRING_MAKE_EXCEPTION(domain_error); //域错误
	CLSTRING_MAKE_EXCEPTION(invalid_argument); //非法参数
	CLSTRING_MAKE_EXCEPTION(length_error); //通常是创建对象是给出的尺寸太大
	CLSTRING_MAKE_EXCEPTION(out_of_range); //访问超界

	CLSTRING_MAKE_EXCEPTION(runtime_error);//运行时错误
	CLSTRING_MAKE_EXCEPTION(range_error); //边界错误
	CLSTRING_MAKE_EXCEPTION(overflow_error);//上溢
	CLSTRING_MAKE_EXCEPTION(underflow_error);//下溢

	//类CLString方法实现---------------------------------------------------------------------------------------------------------------------------------
	BOOL isEmpty();//是否为空字符

	ref empty();//强制对象置为空,仅字符串相关数据清空，其他数据不做任何修改

	TCHAR getAt(LONG_PTR nIndex);//返回字串指定位置的字符，索引从0开始,参数可为任意值，若参数越界则返回0不会奔溃
	TCHAR operator [](LONG_PTR nIndex);//返回字串指定位置的字符，索引从0开始;参数可为任意值，若参数越界则返回0不会奔溃

	BOOL setAt(LONG_PTR nIndex, TCHAR ch);//设置给定位置上的字符 

	//重载的(LPCTSTR)操作符像访问一个C风格的字符串一样，直接访问保存在一个CLString对象中的字符。
	inline operator LPCTSTR() const { return this->string(); }
	inline operator LPCSTR() { return this->getASCII(); }
	inline operator LPCWSTR() { return this->getUnicode(); }
	//重载的()操作符像访问一个C风格的字符串一样，直接访问保存在一个CLString对象中的字符。
	inline LPCTSTR operator()(void) const { return this->string(); }


	//此成员函数从此CLString对象中提取一个长度为nCount个字符的子串（从nFirst（从零开始的索引）指定的位置开始）到目标对象中，返回保存目标对象的引用。
	ref mid(ref storeTagObj, LONG_PTR nCount);
	ref mid(ref storeTagObj, LONG_PTR nFirst, LONG_PTR nCount);
	ref left(ref storeTagObj, LONG_PTR nCount);
	ref right(ref storeTagObj, LONG_PTR nCount);

	//此成员函数从此CLString对象中提取一个长度为nCount个字符的子串，从nFirst（从零开始的索引）指定的位置开始。此函数返回截取操作后的类对象自身的引用。
	ref midSave(LONG_PTR nFirst, LONG_PTR nCount);
	ref leftSave(LONG_PTR nCount);
	ref rightSave(LONG_PTR nCount);
	ref leftCut(LONG_PTR nCount) { return rightSave(this->strlen() - nCount); }
	ref rightCut(LONG_PTR nCount) { return leftSave(this->strlen() - nCount); }

	ref makeUpper();//对象转换为一个大写字符串
	ref makeLower();//对象转换为一个小写字符串。
	ref makeReverse();//对象中的字符的顺序颠倒过来。
	//此成员函数用一个字符替换另一个字符。函数的第一个原形在字符串中用chNew现场替换chOld。
	//函数的第二个原形用lpszNew指定的字符串替换lpszOld指定的子串。
	//在替换之后，该字符串有可能增长或缩短；那是因为lpszNew和lpszOld的长度不需要是相等的。两种版本形式都进行区分大小写的匹配。
	//返回值：返回被替换的字符数。如果这个字符串没有改变则返回零。
	LONG_PTR replaceRN(TCHAR chOld, TCHAR chNew);
	ref replace(TCHAR chOld, TCHAR chNew);
	LONG_PTR replaceRN(LPCTSTR lpszOld, LPCTSTR lpszNew);
	ref replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	LONG_PTR removeRN(TCHAR ch);//将ch实例从字符串中移走。返回值：返回从字符串中移走的字符数。如果字符串没有改变则返回零。
	ref remove(TCHAR ch);

	//在字符串中的给定索引处插入一个单个字符或一个子字符串。返回值：返回被改变的字符串的长度。
	LONG_PTR insertRN(LONG_PTR nIndex, TCHAR ch);
	ref insert(LONG_PTR nIndex, TCHAR ch);
	LONG_PTR insertRN(LONG_PTR nIndex, LPCTSTR pstr);
	ref insert(LONG_PTR nIndex, LPCTSTR pstr);
	//从一个字符串中从nIndex开始的地方删除一个或多个字符。如果nCount比此字符串还要长，则字符串的其余部分都将被删除。
	LONG_PTR deleteCharRN(LONG_PTR nIndex, LONG_PTR nCount = 1);
	ref deleteChar(LONG_PTR nIndex, LONG_PTR nCount = 1);
	//这个成员函数的没有参数的版本用来将字符串最前面和最后面的“换行符，空格和tab字符”修整掉。
	//这个成员函数的需要参数的版本用来将一个特定的字符或一群特定的字符从字符串的开始和结尾处删除。
	ref trim();
	ref trim(TCHAR chTarget);
	ref trim(LPCTSTR lpszTargets);
	//这个成员函数的没有参数的版本用来将字符串最前面的“换行符，空格和tab字符”修整掉。
	//这个成员函数的需要参数的版本用来将一个特定的字符或一群特定的字符从字符串的开始处删除。
	ref trimLeft();
	ref trimLeft(TCHAR chTarget);
	ref trimLeft(LPCTSTR lpszTargets);
	//这个成员函数的没有参数的版本用来将字符串最后面的“换行符，空格和tab字符”修整掉。
	//这个成员函数的需要参数的版本用来将一个特定的字符或一群特定的字符从字符串的结尾处删除。
	ref trimRight();
	ref trimRight(TCHAR chTarget);
	ref trimRight(LPCTSTR lpszTargets);


	LPCSTR strStr(LPCSTR lpszSub) { return StrStrA(getASCII(), lpszSub); };
	LPCWSTR strStr(LPCWSTR lpszSub) { return StrStrW(getUnicode(), lpszSub); };
	LPCSTR strStrI(LPCSTR lpszSub) { return StrStrIA(getASCII(), lpszSub); };
	LPCWSTR strStrI(LPCWSTR lpszSub) { return StrStrIW(getUnicode(), lpszSub); };

	//在字符串中查找子字符串，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nStart表示搜索起始位置索引。
	static LONG_PTR  find(LPCTSTR lpszTag, LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//在字符串中查找字符，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nStart表示搜索起始位置索引。
	LONG_PTR  find(TCHAR ch, LONG_PTR nStart = 0);
	//在字符串中查找子字符串，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nStart表示搜索起始位置索引。
	LONG_PTR  find(LPCTSTR lpszSub, LONG_PTR nStart = 0);

	//将搜索结果保存到内部临时存储空间，并返回给外部,该临时对象在下一次覆盖操作前将会有效，
	//需要临时独立对象保存结果，则需要调用findStringObj()函数；
	LPCTSTR findString(LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//将搜索结果保存到返回对象；
	CLString findStringObj(LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//将搜索结果保存到对象本身；
	ref findStringSave(LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//将反向搜索结果保存到内部临时存储空间，并返回给外部,该临时对象在下一次覆盖操作前将会有效；
	//需要临时独立对象保存结果，则需要调用rfindStringObj()函数；
	LPCTSTR rfindString(LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//将反向搜索结果保存到返回对象；
	CLString rfindStringObj(LPCTSTR lpszSub, LONG_PTR nStart = 0);
	//将反向搜索结果保存到对象本身；
	ref rfindStringSave(LPCTSTR lpszSub, LONG_PTR nStart = 0);

	//用两个前后剪裁字符串剪裁对象字符串，返回剪裁结果（即中间）字符串；
	//lpszSubLeft、lpszSubRight参数：是左右剪裁字符串，对目标字符串从前后两个方向搜索剪裁字符串，找到后区中间的字符串作为结果字符串；
	// 	   当两个或其中一个为null或""时候，参数仍然有效，此时表示，忽略该参数限定的范围，该侧的所有字符串都匹配搜索条件；
	//cutType参数：若剪裁字符串在字串中可能存在多个，则应该明确指定cutType参数，设定具体剪裁模式（默认为最小化剪裁FMS_MIN）
	// 其他模式详见枚举类型FMS_TYPE，其中：
	// FMS_MAX：  表示采用最大化剪裁，若剪裁字符串出现多次，则取第一次出现的左剪裁字符串，到最后一次出现的右剪裁字符串，之间的字符串；该模式，两个前后剪裁字符串之间可能出现其他左和右剪裁字符串；
	// FMS_MIN：  表示采用最小化剪裁，若剪裁字符串出现多次，则取左右剪裁字符串之间不再出现任何一个剪裁字符串的那部分字符串，若满足这种情况的区间出现多次，则取最靠前的那个区间；
	// FMS_LEFT： 表示采用靠左优先剪裁，若剪裁字符串出现多次，则取第一次出现的左剪裁字符串，到第一次出现的右剪裁字符串，之间的字符串；该模式，两个前后剪裁字符串之间可能出现其他左剪裁字符串；
	// FMS_RIGHT：表示采用靠右优先剪裁，若剪裁字符串出现多次，则取最后一次出现的左剪裁字符串，到最后一次出现的右剪裁字符串，之间的字符串；该模式，两个前后剪裁字符串之间可能出现其他右剪裁字符串；
	// 注意：1、若左右剪裁字符串不同，且在字符串中左右剪裁字符串仅仅各出现一次，则以上四种模式的结果相同；
	//       2、若左右剪裁字符串相同，且字符串又存在满足搜索结果的可能区间，则应该明确指定模式，避免搜索结果错误；	
	//saveType参数：表示搜索结果字串是否要包含剪裁字符串本身（默认为最小化剪裁FMS_MIN，即不包含剪裁字符串本身），其中：
	// FMS_MAX：  结果字符串中包含左右两个剪裁字符串；
	// FMS_MIN：  结果字符串中不包含剪裁字符串；
	// FMS_LEFT： 结果字符串中只包含左剪裁字符串；
	// FMS_RIGHT：结果字符串中只包含右剪裁字符串；
	//nStart参数：表示左剪裁字符串的搜索起始点（左侧起算，从0开始的索引）；
	//nRevStart参数：表示右剪裁字符串的反向搜索起始点（右侧起算，从0开始的索引）；
	//注意：nStart，nRevStart本质上是减小原字符串的搜索范围大小，因此当搜索字符串的范围长度压缩为0长度后，返回结果将为""空字符串（但不会是null）；
	//e.g  CLString str = "file=[D:/document/test.txt]";
	//     str.findMidString("[","]");                    //返回结果："D:/document/test.txt"
	//     str.findMidString("/","",FMS_RIGHT);           //返回结果："test.txt"
	//     str.findMidString("/","",FMS_LEFT,FMS_LEFT);   //返回结果："/document/test.txt"
	//     str.findMidString("/","/",FMS_MAX,FMS_MAX);    //返回结果："/document/"
	LPCTSTR findMidString(LPCTSTR lpszSubLeft, LPCTSTR lpszSubRight, FMS_TYPE cutType = FMS_MIN, FMS_TYPE saveType = FMS_MIN,
		LONG_PTR nStart = 0, LONG_PTR nRevStart = 0);
	//用两个前后剪裁字符串剪裁对象字符串，返回剪裁结果（即中间）字符串到CLString对象；参数意义详见CLString::findMidString()函数；
	CLString findMidStringObj(LPCTSTR lpszSubLeft, LPCTSTR lpszSubRight, FMS_TYPE cutType = FMS_MIN, FMS_TYPE saveType = FMS_MIN,
		LONG_PTR nStart = 0, LONG_PTR nRevStart = 0);
	//用两个前后剪裁字符串剪裁对象字符串，返回剪裁结果（即中间）字符串保存到CLString对象本身（覆盖掉原字符串）；参数意义详见CLString::findMidString()函数；
	ref findMidStringSave(LPCTSTR lpszSubLeft, LPCTSTR lpszSubRight, FMS_TYPE cutType = FMS_MIN, FMS_TYPE saveType = FMS_MIN,
		LONG_PTR nStart = 0, LONG_PTR nRevStart = 0);

	//在字符串中反向查找子字符串，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nRevStart表示反向搜索正序起始位置索引。
	static LONG_PTR  rfind(LPCTSTR lpszTag, LPCTSTR lpszSub, LONG_PTR nRevStart = 0);
	//在字符串中反向查找字符，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nRevStart表示反向搜索正序起始位置索引。
	LONG_PTR  rfind(TCHAR ch, LONG_PTR nRevStart = 0);
	//在字符串中反向查找子字符串，返回第一次完全匹配的起始位置索引；如果没有找到子字符串或字符则返回-1。nRevStart表示反向搜索正序起始位置索引。
	LONG_PTR  rfind(LPCTSTR lpszSub, LONG_PTR nRevStart = 0);

	//字符串中搜索与lpszCharSet中任意字符匹配的第一个字符。
	//返回此字符串中第一个在lpszCharSet中也包括的字符的从零开始的索引。
	LONG_PTR  findFirstOneOf(LPCTSTR lpszCharSet);

	//重字符串队列种找到标签，并排序
	static void findFlagAndSort(std::vector<TCHAR*>& sortRet, const TCHAR* lpFlagString, const TCHAR** tagStringList, size_t tagStringCounts);

	//加密及解密字符串，nDepth表示加密的深度(默认为1层，一般情况不需要增加就满足要求)，
	//加密及解密的对应深度必须相同才能解密。函数同时支持UNICODE和ASCII。
	//对速度有要求的可以使用，传递buf1，buf2，nCharCounts的版本（不需要动态申请内存），
	//其中nCharCounts代表加密字串字符数(注意：ASCII一个汉字为2个字符数),不是buf1，buf2的大小,buf1，buf2的大小必须>=nCharCounts，否则访问会越界。
	//函数压力测试：字符数100个，加解密单次，深度100万层，耗时1.36秒(传buf版)和1.36秒。
	//字符数100个，加解密10万次，每次2层，耗时2.5秒(传buf版)和3.5秒。
	//测试环境win7 x64 A10 6G
	static CLStringR encrypteString(CLStringR outputString, LPCTSTR inputString, size_t nDepth = 1);
	static CLStringR unEncrypteString(CLStringR outputString, LPCTSTR inputString, size_t nDepth = 1);
	static CLStringR encrypteString(CLStringR outputString, LPCTSTR inputString, int* buf1, int* buf2, size_t nCharCounts, size_t nDepth = 1);
	static CLStringR unEncrypteString(CLStringR outputString, LPCTSTR inputString, int* buf1, int* buf2, size_t nCharCounts, size_t nDepth = 1);
	CLStringR encrypteString(LPCTSTR inputString, size_t nDepth = 1);
	CLStringR unEncrypteString(LPCTSTR inputString, size_t nDepth = 1);
	CLStringR encrypteString(LPCTSTR inputString, int* buf1, int* buf2, size_t nCharCounts, size_t nDepth = 1);
	CLStringR unEncrypteString(LPCTSTR inputString, int* buf1, int* buf2, size_t nCharCounts, size_t nDepth = 1);

	//-----------------------------其他静态全局函数---------------------------------------------------------------

		//findStringInPair函数的结果信息结构
	typedef struct {
		LPCTSTR pS1, pS2, pV1, pV2, pE1, pE2;//分别为标签1的前后指针，数据前后指针，标签2的前后指针
		size_t nS, nV, nE;//标签1长度，数据长度，标签2长度
	}FSIP_INF, * PFSIP_INF;//findStringInPair函数的结果信息结构
	//找到任意指定两个标签字串间的字串，找到返回TRUE
	//lpOrg查找的源字符串；
	//lpBeginFlag、lpEndFlag为标签字符串，他们可以是、0、NULL、_T("\0")、或其他任意字符串
	//pInfOut输出信息结构体指针，该结构体传入前不需要初始化
	//orgMaxCheckCounts 表示从源开始要查找的最大源字符数量，超过却没找到将返回False，默认为 0 （0表示不设查找字符最大数限制）
	static BOOL findStringInPair(LPCTSTR lpOrg, LPCTSTR lpBeginFlag, LPCTSTR lpEndFlag, OUT CLString::PFSIP_INF pInfOut = 0, size_t orgMaxCheckCounts = 0);
	BOOL findStringInPair(LPCTSTR lpBeginFlag, LPCTSTR lpEndFlag, OUT PFSIP_INF pInfOut = 0, size_t orgMaxCheckCounts = 0);

	//#define USE_CLSTRING_TIME_CHECK //要关闭效能测试，请注释掉此行
#ifdef USE_CLSTRING_TIME_CHECK
#define MAX_DBG 10
public:
	LPCTSTR _tick_ni;
	//CLTime _tick_time;
	double _tick_s;
#ifdef UNICODE
	std::map<std::wstring, double> _tick_lst;
#else
	std::map<std::string, double> _tick_lst;
#endif
	CLStringR initCheck() { _tick_lst.clear(); return *this; }
#define _INIT_CHECK_ { initCheck();}
#define _CHECK_START_( STR ) { _tick_ni = _T(#STR); timingStartEx();}
#define _CHECK_END_ {  _tick_s = timingGetSpentTimeEx(); _tick_lst[_tick_ni] += _tick_s;	}
	CLStringR checkLogout() {
		for (auto i = _tick_lst.begin(); i != _tick_lst.end(); i++)\
			_tprintf(_T(" %s = %.15lf s\n"), i->first.c_str(), i->second); return *this;
	}
#define _CHECK_LOGINF_ {checkLogout();}
#else
#define _INIT_CHECK_
#define _CHECK_START_( STR )
#define _CHECK_END_
#define _CHECK_LOGINF_
#endif

};

CLString operator+(const CLString& str1, const CLString& str2);
CLString operator+(LPCTSTR pStr1, const CLString& str2);
CLString operator+(const CLString& str1, LPCTSTR pStr2);
template<typename _class> CLString operator+(const CLString& str1, _class obj2) {
	CLString temp;
	temp << str1 << obj2;
	return  std::move(temp);
}
template<typename _class> CLString operator+(_class obj1, const CLString& str2) {
	CLString temp;
	temp << obj1 << str2;
	return  std::move(temp);
}


bool operator < (CLStringRC str1, LPCTSTR str2);
bool operator < (LPCTSTR str1, CLStringRC str2);
bool operator < (CLStringRC str1, CLStringRC str2);
bool operator <= (CLStringRC str1, LPCTSTR str2);
bool operator <= (LPCTSTR str1, CLStringRC str2);
bool operator <= (CLStringRC str1, CLStringRC str2);
bool operator > (CLStringRC str1, LPCTSTR str2);
bool operator > (LPCTSTR str1, CLStringRC str2);
bool operator > (CLStringRC str1, CLStringRC str2);
bool operator >= (CLStringRC str1, LPCTSTR str2);
bool operator >= (LPCTSTR str1, CLStringRC str2);
bool operator >= (CLStringRC str1, CLStringRC str2);
bool operator == (CLStringRC str1, LPCTSTR str2);
bool operator == (LPCTSTR str1, CLStringRC str2);
bool operator == (CLStringRC str1, CLStringRC str2);
bool operator != (CLStringRC str1, LPCTSTR str2);
bool operator != (LPCTSTR str1, CLStringRC str2);
bool operator != (CLStringRC str1, CLStringRC str2);

//区间比较宏
//#define BETWEEN(x,a,b)  (( (a) <= (x) && (x) <= (b) ) || ( (b) <= (x) && (x) <= (a) ))
//#define BETWEENO(x,a,b)  (( (a) < (x) && (x) < (b) ) || ( (b) < (x) && (x) < (a) ))

//去掉文字对象后面的注释（“#”及其后的内容）和前后的空字符
#define CLRemoveNotes( x )  {LONG_PTR  ind_x_023dn = (x).find(_T("#"));ind_x_023dn = (ind_x_023dn == -1 ? (x).strlen() : ind_x_023dn);(x).midSave(0,ind_x_023dn);(x).trim();}
//取得全局0字串指针
LPTSTR getCLString0();

//临时对象化转换
#define CLStrA( lpstr ) (CLString((lpstr)).getASCII())
#define CLStrW( lpstr ) (CLString((lpstr)).getUnicode())

#ifdef UNICODE
#define CLStr( lpstr ) CLStrW( lpstr )
#else
#define CLStr( lpstr ) CLStrA( lpstr )
#endif

#endif