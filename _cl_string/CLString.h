#pragma once
#ifndef __CL_CLSTRING_H__
#define __CL_CLSTRING_H__

//Design by cailuo. 2014-04-30
//�Զ���ѡ�����ֹ����� CLString -----------------------------------------

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
//#include "../_cl_common/CLCommon.h"
#pragma comment(lib, "ws2_32.lib")

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC//�ڴ�й©���
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
#pragma comment(lib,"wininet.lib")//������Ҫ wininet.lib

//CL�ַ����ඨ��
class CLString;
typedef CLString* PCLString;//���ָ������
typedef const PCLString PCCLString;//��ĳ���ָ������
typedef CLString& CLStringR;//������ö�������
typedef const CLStringR CLStringRC;//��ĳ������ö�������


//CL�����ַ����࣬���ܺ�ǿ�Ͷ���
class CLString
{
protected:
	//�ַ���ָ��
	LPTSTR pHead;

	//��������С�����ַ��ƣ�����ĩβ0�ַ���
	LONG_PTR m_UnitNumber;	

	//�ַ����ַ�����
	LONG_PTR m_Strlen;

	//��������Ƿ��Ѿ��ı�
	byte m_changeFlag;

	//����������Ƿ��ǿ��ֽڸ�ʽ��Ϊ1��ʾ���ֽ�
	byte m_encoding;

#define EnCode_UNKNOWN 0 //encode unknown
#define EnCode_ASCII   1 //ASCII
#define EnCode_UTF8    2 //UTF-8
#define EnCode_ULE     3 //Unicode little_Endian(windows default code)
#define EnCode_UBE     4 //Unicode Big_Endian

typedef struct _ClStringExData{

	//��ʱ�ַ���ָ��
	union _cl_u_pointer{
		LPSTR pMultiByte;
		LPWSTR pWideChar;
	}m_pointer_;
//#define (makeDEx()->m_pointer_) (makeDEx()->m_pointer_)
	byte m_PtExSaveType_;//����m_pointer_�Ƿ������洢����0Ϊ�洢��1���MultiByte��2���WideChar
//#define (makeDEx()->m_PtExSaveType_) (makeDEx()->m_PtExSaveType_)
	//��ʱ�ַ����������ռ䳤�ȣ����ַ��ƣ�����ĩβ0�ַ���
	LONG_PTR m_pointerBufSizeInByte_;
//#define (makeDEx()->m_pointerBufSizeInByte_) (makeDEx()->m_pointerBufSizeInByte_)

	//�ڲ�����Ĵ��ļ��ľ��
	HANDLE m_hFile_;
//#define (makeDEx()->m_hFile_) (makeDEx()->m_hFile_)

	byte m_fileType_;//�����ڲ��򿪵��ļ���http�ļ����ݸ�ʽ���ͣ�
//#define (makeDEx()->m_fileType_) (makeDEx()->m_fileType_)
	HINTERNET hInternet_,hConnect_,hRequest_;//�������ڱ���http���
//#define (makeDEx()->hInternet_) (makeDEx()->hInternet_)
//#define (makeDEx()->hConnect_) (makeDEx()->hConnect_)
//#define (makeDEx()->hRequest_) (makeDEx()->hRequest_)
	//��̬�����ʱ�����ָ�룬�����뼶��ʱ��ͳ��;
	SYSTEMTIME *m_sysTimeStart_,*m_sysTimeEnd_;
//#define (makeDEx()->m_sysTimeStart_) (makeDEx()->m_sysTimeStart_)
//#define (makeDEx()->m_sysTimeEnd_) (makeDEx()->m_sysTimeEnd_)
	//��̬����ĺ���ʱ�����ָ�룬��΢�뼶��ʱ��ͳ��;
	LARGE_INTEGER *m_largeIntegerStart_,*m_largeIntegerEnd_;
//#define (makeDEx()->m_largeIntegerStart_) (makeDEx()->m_largeIntegerStart_)
//#define (makeDEx()->m_largeIntegerEnd_) (makeDEx()->m_largeIntegerEnd_)
	
	std::vector<LPCTSTR> *m_vtInnerStringVector_;//�ڲ�ά�����ַ�������ָ��
	std::vector<LPTSTR> *m_vtStringStoreLst_;//�����ڲ��洢���ַ�������ָ��
	std::vector<LONG_PTR> *m_vtStringStoreBufSizeLst_;//�����ڲ��洢���ַ��������������ݵ�ָ��
//#define (makeDEx()->m_vtInnerStringVector_) (makeDEx()->m_vtInnerStringVector_)
//#define (makeDEx()->m_vtStringStoreLst_) (makeDEx()->m_vtStringStoreLst_)
//#define (makeDEx()->m_vtStringStoreBufSizeLst_) (makeDEx()->m_vtStringStoreBufSizeLst_)
	//�������ж�ȡ�ļ����ݵĿ��Ʊ�����
	byte m_isEndOnce_;	
//#define (makeDEx()->m_isEndOnce_) (makeDEx()->m_isEndOnce_)
	//�������ֲ������ĵ�ǰ���λ�ñ�ǣ�Ĭ��Ϊ-1��ʾδ��������׼�����Ѿ���ϣ�
	long m_curSplitOutIndex_;
//#define (makeDEx()->m_curSplitOutIndex_) (makeDEx()->m_curSplitOutIndex_)

	inline _ClStringExData(){init();}
	inline ~_ClStringExData(){
		closeAndClear();
		assert( m_vtInnerStringVector_ == 0 && m_vtStringStoreLst_ == 0 && m_vtStringStoreBufSizeLst_ == 0 && 
			m_sysTimeStart_ == 0 && m_sysTimeEnd_ == 0 && m_largeIntegerStart_ == 0 && m_largeIntegerEnd_ == 0 );
	}
	void init();	
	void closeInternet();
	BOOL closeFile();
	void closeMemery();
	void closeAndClear();
	}CLStringExData,*PCLStringExData;
	PCLStringExData pDataEx;//������չ���ݽṹ������ʼ��δʹ���򲻴���
	inline PCLStringExData makeDEx(){return pDataEx ? pDataEx : pDataEx = new CLStringExData;}
	//�����У����һ�����õĺ���cleanAllMemory�е���
	inline void deleteDEx(){ if(pDataEx)delete pDataEx;pDataEx = 0;}	

	//�ڲ����ܽӿڣ�δ���š�
protected:
	//�����ı�m_encoding
	  VOID setEncode(VOID);
	//����ֻ����������������Ψһһ�Σ���������з����ڴ�
	  VOID cleanAllMemory(VOID);
	//����ֻ���ڹ��캯������Ψһһ�Σ�������pHead��Ч��ָ���ֵ
	//ָ�������ڴ���nDefaultCharNumber���ַ���Ϊ��λָ��
	  VOID initialize(LONG_PTR nDefaultCharNumber = 0);	

	static BOOL _AddPathAnEnt(LPTSTR pResBuffer,INT EndsNmber=1);
	static LPCTSTR _ExtendPathToQuality(LPCTSTR pResBuffer,LPTSTR pDesBuffer);
	static LPCTSTR _ExtendPathToNormal(LPCTSTR pResBuffer,LPTSTR pDesBuffer);
	static LPCTSTR _GetLastStrByPath(LPCTSTR strPath);
	static LPCTSTR _GetLastStrByPath2(LPCTSTR strPath);
	static LPTSTR _DeleteLastStrFromPath(LPTSTR strPath);
	static BOOL _FindFirstFileExists(LPCTSTR lpPath,DWORD dwFilter);
	BOOL _RegSZ(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName,LPTSTR & pString,BOOL isSet=TRUE,LONG_PTR storeSize=MAX_PATH);
	static inline  INT c2i(TCHAR m_char)
	{
		return ((INT)m_char-48);
	}
	static inline  TCHAR i2c(LONG_PTR i)
	{
		return ((TCHAR)(i+48));
	}
	static inline  double qcf(double a,LONG_PTR cf)
	{ 
		double sum=1;
		for(INT i=1;i<=cf;i++)
		{sum=a*sum;}
		if(a!=0)
		{for(INT i=-1;i>=cf;i--)
		{sum=1/a*sum;}
		}
		return sum;
	}
	
	static inline LONG_PTR _strlen(LPCTSTR _in_Ptr){
		if(!_in_Ptr) return 0;
		TCHAR c = *_in_Ptr;
		LONG_PTR t =0;
		while(c!=0){
			t++;
			c = *(_in_Ptr+t);
		}
		return t;
	}

	LPTSTR _newbuffer(LONG_PTR nNeedSize,LONG_PTR nPosIndex);
public:
	//ͬ����ָ��Ϊ��Ԫ��
	friend class CLString;

	//���캯����Ĭ�ϣ�
	//���캯����LPCTSTR�汾������һ����ʼ���ַ���ָ�롣
	//���캯����const CLString& �汾������һ����ʼ��CLString���󣬵��Ǹú������´��Դ�����������Ϣ������ʱ�䣬�ļ�������ַ��������������������ݡ�(��Ҫ������չ���������copyExData)
	//���캯����ָ����ʼ���ڴ��ַ������汾������׶ξ�����ָ�����ڴ������������ƽ�������ڴ档��δ���洢�ַ������࣬�������Զ����ݡ��ú������Զ����ǽ�β0�ַ��Ŀռ䡣
	//���캯����������汾�������ַ���ָ�룬����0��NULLָ����Ϊ���һ��������������β��ǣ��������޷�Ԥ�ϡ���һ�����������ڳ�ʼ�����󣬺�����ִ�����׷�ӵ�����β����
	CLString();
	CLString(LPCSTR pString);
	CLString(LPCWSTR pString);
	CLString(LPCTSTR pString1,LPCTSTR pString2,...);
	CLString(const CLString& mString);	
	explicit CLString(LONG_PTR nDefaultCharNumber);

	//��������
	virtual ~CLString();

	//�����ļ��Ĳ���ģʽ������Ϊ1ʱΪCģʽ��Ϊ0ʱΪwindows APIģʽ��ע�⣺APIģʽ���ٶ�ԼΪCģʽ��1.3����
	BOOL setFileOpenedType(INT m_NewFileOpenedType = 1);

	//�ӷ���+����������
	friend CLString operator+(const CLString & str1,const CLString & str2);
	//�ӷ���+����������
	friend CLString operator+(LPCTSTR pStr1,const  CLString & str2);
	//�ӷ���+����������
	friend CLString operator+(const CLString & str1, LPCTSTR pStr2);

	//�������ݣ�������չ���ݣ���չ����ֻ��������ʱ�ַ������壬ʱ��ṹ���ݣ��ַ����б����ݣ���������ǰ״̬�����ݣ����������ļ����������������
	CLString& copy(CLStringRC str);
	//������չ���ݣ�ֻ��������ʱ�ַ������壬ʱ��ṹ���ݣ��ַ����б����ݣ���������ǰ״̬�����ݣ����������ļ����������������
	CLString& copyExData(CLStringRC str);


	//1����ֵ����������1���ַ���ָ�롣
	//2����ֵ�������������ɱ�������������ܣ��ַ���ָ�롣
	//   ����������1�ִ���ʼ�����󣬲��ú��������ָ����ַ���׷�ӵ�����β����
	//   ע�⣺�ɱ�������һ����������Ϊһ��0��NULLָ����Ϊ������ǣ�
	//   Ҳ����˵��������һ������NULLָ�뽫��������ȡ������
	//   ��û��0ָ����Ϊ��������������ȡ������ȥ�����������Ԥ�ϡ�
	CLString& set(LPCSTR pString);	
	CLString& set(LPCWSTR pString);	
	CLString& set(LPCTSTR pString1,LPCTSTR pString2,...);
	CLString& set(CHAR ch);
	CLString& set(WCHAR ch);
	//��ֵĿ���е�n���ַ���������ĩβ����ĩβ0��
	CLString& setn(LPCTSTR pStr,LONG_PTR nSiChar);

	//������������һ���ַ�������ǰ����β��������1���ַ���ָ�롣
	CLString& append(LPCSTR pString);
	CLString& append(LPCWSTR pString);

	//������������һ��TCHAR���󵽵�ǰ����β��������1��TCHAR����
	CLString& append(CHAR ch);
	CLString& append(WCHAR ch);

#ifndef CLSTRING_FTOSLIMIT
#define CLSTRING_FTOSLIMIT  26
#define CLSTRING_FTOSLIMIT_MAXACCURACY  15
#endif // !CLSTRING_FTOSLIMIT
#ifndef CLSTRING_LTOSLIMIT
#define CLSTRING_LTOSLIMIT  19
#endif // !CLSTRING_LTOSLIMIT


	//������ת�����ַ��������ӵ�����ĩβ����ʧ�������ӿ��ַ�����
	//����3 ����һ����������ָ��
	//ԭ��������֣�v�������κ����͵�����
	//maxAccuracy��ʾС����ʾ���ȣ����֧��CLSTRING_FTOSLIMIT_MAXACCURACYλС����ȷ���������ȴ���ʵ��С��λ�������Զ�Ĩȥĩβ0�����Ҳ����ÿ�ѧ�����������ȫ��Ϊԭ�����
	//nBufSizeInChar��СҪ����ltos CLSTRING_LTOSLIMIT�� ftos CLSTRING_FTOSLIMIT���ַ��Ŀռ䣬����ֱ�ӷ���false
	CLString& appendl(LONG_PTR v,BOOL *_out_bIsSuccess = 0);
	CLString& appendf(double v,size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY,BOOL *_out_bIsSuccess = 0);

	//���ز�����<< �����ȼ���ѭC++ԭ��
	CLString& operator<<(const CHAR ch);
	CLString& operator<<(const byte number);
	CLString& operator<<(const WCHAR ch);
	CLString& operator<<(LPCSTR pString);
	CLString& operator<<(LPCWSTR pString);
	CLString& operator<<(const CLString& mString);
	CLString& operator<<(const CLString* pString);
	CLString& operator<<(const long long number);
	CLString& operator<<(const unsigned long long number);
	CLString& operator<<(const long number);
	CLString& operator<<(const unsigned long number);
	CLString& operator<<(const int number);
	CLString& operator<<(const unsigned int number);
	CLString& operator<<(const short number);
	CLString& operator<<(const unsigned short number);
	CLString& operator<<(const float number);
	CLString& operator<<(const double number);
	CLString& operator<<(const long double number);
	CLString& operator<<(const bool number);

    //��ֵ�������ַ�����CLString�������֡�
	template<typename _Class>CLString& set (_Class obj){
		empty() << obj;return *this;
	}
	inline CLString& set(const CLString& mString){set(mString.string());return *this;}
	inline CLString& set(const CLString* pString){set(pString ? pString->string(): (LPCTSTR)NULL);return *this;}
	//������������һ��CLString���󵽵�ǰ����β��������1���ַ�����CLString�������֡�
	template<typename _Class>CLString& append (_Class obj){
		(*this) << obj;return *this;
	}
	inline CLString& append(const CLString& mString){append(mString.string());return *this;}
	inline CLString& append(const CLString* pString){append(pString ? pString->string(): (LPCTSTR)NULL);return *this;}
	//��ֵ���������ء��������ַ�����CLString�������֡�
	template<typename _Class>CLString& operator=(_Class obj){
		set(obj);return *this;
	}
	inline CLString& operator=(const CLString& mString){set(mString.string());return *this;}
	inline CLString& operator=(const CLString* pString){set(pString ? pString->string(): (LPCTSTR)NULL);return *this;}

	//����һ�� % ���������ڷ���������ʵ������ǰ����ֵ����set()�Ĺ��ܡ��������ַ������������֡�
	//���б� + �� <<  = �ȸ��ߵ����ȼ����Ӷ�������������������е���ǰ��ʵ����������ʼ���Ĳ�����
	//�������� = ����һ���ǵ����ȼ����õģ������ں���Ĳ������������ִ�С�
	template<typename _Class>CLString& operator%(_Class obj){
		empty() << obj; return *this;
	}
	inline CLString& operator%(const CLString& mString){set(mString.string());return *this;}
	inline CLString& operator%(const CLString* pString){set(pString ? pString->string(): (LPCTSTR)NULL);return *this;}
	//���������أ�������������һ���ַ�������ǰ����β�����������ַ�����CLString�������֡�
    template<typename _Class>CLString& operator+=(_Class obj){
		append(obj);return *this;
	}
	inline CLString& operator+=(const CLString& mString){append(mString.string());return *this;}
	inline CLString& operator+=(const CLString* pString){append(pString ? pString->string(): (LPCTSTR)NULL);return *this;}


	//��ָ����ʽ��ʽ����������CString�е�format�������ܡ�
	//�ر�ע�⣬���ô˺��������ԭ���ݣ����ܽ������ִ�ָ����Ϊ�ַ�������Դ�����ǿ�����Ϊ��ʽ���ִ���Դ��
	CLString& format(LONG_PTR maxStrlen,LPCTSTR szFormat,...);
	CLString& format(LPCTSTR szFormat,...);

	//�������ַ��������Ŀ�����,ʹ��ǰ���������зֱ�ǣ�����operator*
	CLString& operator>>(char& ch);
	CLString& operator>>(unsigned char& number);
	CLString& operator>>(wchar_t& ch);
	
	CLString& operator>>(CLString& mString);
	CLString& operator>>(CLString* pString);
	CLString& operator>>(long long& number);
	CLString& operator>>(unsigned long long& number);
	CLString& operator>>(long& number);
	CLString& operator>>(unsigned long& number);
	CLString& operator>>(int& number);
	CLString& operator>>(unsigned int& number);
	CLString& operator>>(short& number);
	CLString& operator>>(unsigned short& number);
	CLString& operator>>(float& number);
	CLString& operator>>(double& number);
	CLString& operator>>(long double& number);
	CLString& operator>>(bool& number);

	//���÷ָ���ţ�����ʼ���ָ�������������з�
	CLString& operator*(LPCTSTR lpSplit);
#define CLRet _T("\r\n")//windows����з�
#define CLComma _T(",")//����
#define CLBkSla _T("/")//��б�ܣ�����
#define CLSla _T("\\")//��б��
#define CLVerL _T("|")//����
#define CLDot  _T(".")//��
#define CLTab  _T("\t")//����
#define CLSpc  _T(" ")//�ո�


	//ԭ��������֣�v�������κ����͵�����
	//maxAccuracy��ʾС����ʾ���ȣ����֧��CLSTRING_FTOSLIMIT_MAXACCURACYλС����ȷ������maxAccuracy����ʵ��С��λ�������Զ�Ĩȥĩβ0������Դ��ڵľ���λ�������룬
	//��<=0�����С�������Ҳ����ÿ�ѧ�����������ȫ��Ϊԭ�����
	//nBufSizeInChar��СҪ����ltos CLSTRING_LTOSLIMIT�� ftos CLSTRING_FTOSLIMIT���ַ��Ŀռ䣬����ֱ�ӷ���false
	//����ֵ*_out_bIsSuccessΪfalseʱ��buf�е�ֵ���ǲ���Ԥ���ֵ��ֻ�е�����trueʱ���������Ч��
	static BOOL ltos(LONG_PTR v,LPTSTR lpBuf,size_t nBufSizeInChar = CLSTRING_LTOSLIMIT);
	static BOOL ftos(double v,LPTSTR lpBuf,size_t nBufSizeInChar = CLSTRING_FTOSLIMIT,size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY);
	CLString& ltos(LONG_PTR v,BOOL* _out_bIsSuccess = NULL);
	CLString& ftos(double v,size_t maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY,BOOL* _out_bIsSuccess = NULL);
	//�ַ���תdouble���ͣ�С��������ò�Ҫ����CLSTRING_FTOSLIMIT_MAXACCURACYλ����Чλ����������17λ���������ʧ�棨ѹ������Ϊatoi�ٶ�2.3����
	static double stof(LPCTSTR lpBuf);
	//�ַ���תLONG_PTR���ͣ���Чλ����������18λ���������ʧ�棨ѹ������Ϊatoi�ٶ�2.3����
	static long long stoll(LPCTSTR lpBuf);
	
	double stof()const;
	int stoi()const;
	long stol()const;
	long long stoll()const;
	inline unsigned char stob()const { int r = stoi(); return r < 0 ? (unsigned char)(0) : (r > 255 ? (unsigned char)(255) : (unsigned char)(r)); }
	inline unsigned int stoui()const { int r = stoi(); return r < 0 ? (unsigned int)(0) : (unsigned int)(r); }
	inline unsigned long stoul()const { long r = stol(); return r < 0 ? (unsigned long)(0) : (unsigned long)(r); }
	inline static unsigned char stob(LPCTSTR lpBuf) { int r = _ttoi(lpBuf); return r < 0 ? (unsigned char)(0) : (r > 255 ? (unsigned char)(255) : (unsigned char)(r));}
	inline static unsigned int stoui(LPCTSTR lpBuf) { int r = _ttoi(lpBuf); return r < 0 ? (unsigned int)(0) : (unsigned int)(r); }
	inline static unsigned long stoul(LPCTSTR lpBuf) { long r = _ttol(lpBuf); return r < 0 ? (unsigned long)(0) : (unsigned long)(r); }
	inline unsigned long long stoull()const { long long r = stoll(); return r < 0 ? (unsigned long long)(0) : (unsigned long long)(r); }
	inline static long long stoull(LPCTSTR lpBuf) { long long r = stoll(lpBuf); return r < 0 ? (unsigned long long)(0) : (unsigned long long)(r); }
	
	//ȡ�ö����ַ�����������ָ�롣����CLString�е�getBuffer�������ܡ�
	inline LPCTSTR string() const { assert(pHead != NULL); return pHead; }
	inline LPCSTR stringA() { assert(pHead != NULL); return getASCII(); }
	inline LPCWSTR stringW() { assert(pHead != NULL); return getUnicode(); }


	//���øú����󣬲���������һ�����õĳ�ʼ������ַ�������ָ�룬
	//�÷���ָ��ֵ����Ϊ�������������ͻ���ռ�ʹ�á�����1��ָ�������ô洢���ַ�����
	//�������ָ���ַ�����β0�Ŀռ䣬�������Զ��жϲ����ӡ�
	//�������Զ�����Ŀ���ڴ�ռ�iStoreMaxCharLen+1��Χ�ڵ��ڴ�Ϊ0����˲���Ҫ��Ϊ������ʼ��������
	//ע�⣺storeA��storeW��Ϊ������洢ʱ���ڵ��ú������saveExStore()�Ѵ洢����ת�����ַ����ռ��б��棬����������string,set,append���ַ����任���ܣ�������ܽ�����ԣ�
	//����ǰ�ṹ��������洢��storeA��storeW�ڹ����Ϻ�store���޲��Ҳ������ʾ����saveExStore()���洢��
	LPTSTR store(LONG_PTR iStoreMaxCharLen=MAX_PATH);
	LPSTR storeA(LONG_PTR iStoreMaxCharLen=MAX_PATH);
	LPWSTR storeW(LONG_PTR iStoreMaxCharLen=MAX_PATH);

	//�����ù�storeA��storeW�洢���ʽ���ݣ�����ݱ�ʾת���洢���ڲ����ַ�����ת���ɹ��������ʱ�����������δ�洢ʲôҲ����
	CLString& saveExStore();

	//���ض����ѷ����˵Ļ�����(���ֽڼ�)�Ĵ�С��
	LONG_PTR buflen(VOID);

	//���ض����ַ�������(���ַ���, ����ֵ������֮ǰ�ַ���Ŀ) �ı���m_changeFlagΪFALSE;
	LONG_PTR strlen(VOID); 
	inline LONG_PTR size() { return strlen(); };
	inline LONG_PTR size0() { return strlen()+1; };

	//���ض����ַ�������(���ֽڼ�, ����ֵ������֮ǰ�ַ����ֽ���Ŀ)
	LONG_PTR strlenInByte(VOID);
	LONG_PTR strlenInByte0(VOID) { return (strlen() + 1)*sizeof(TCHAR); }

	//�����󱣴���ַ�����һ����Ч·���������ڸý�β���ӷ�б��"\"��
	//����1�����ӵķ�б�ܸ�����
	CLString& addAnPathEnd(INT endNumber=1);

	//�����󱣴���ַ�����һ����Ч·�������԰��ִ���
	//���е�"\"��Ϊ"\\"������1���Ƿ񱣴���ֵ��
	//����ʱ��Ȼ���ر仯ǰ��ԭ�ַ�����
	CLString& extendPathToQuality();

	//�����󱣴���ַ�����һ����Ч·�������԰��ִ���
	//����˫"\\"��Ϊ"\"������1���Ƿ񱣴���ֵ��
	//����ʱ��Ȼ���ر仯ǰ��ԭ�ַ�����
	CLString& extendPathToNormal();

	//�����󱣴���ַ�����һ����Ч·�����򷵻����һ��"\"������ַ�����������"\"����
	LPCTSTR getLastStrByPath();

	//�����󱣴���ַ�����һ����Ч·������ɾ�����һ��"\"��������ַ����������أ�������ĩβ"\"����
	CLString& deleteLastStrFromPath();

	//�����󱣴���ַ����к������֣��ɰ�����1ָ����λ����ȡ�����֣��޷���ȡ�����ţ�������������Ϊ������
	//����double���ͷ��ء�����1����0Ϊ��ʼ�Ƶ�λ�������š�����1�����ַ���Ϊ��123.456����һ������������ȡ
	//123.456ʱ����1��������Ϊ0��2�����ַ���Ϊ��abc123.456����һ������������ȡ
	//123.456ʱ����1��������Ϊ1.3��������������������ִ���Χ���ִ������������֣�����������-1��
	double getUNegNumByPos(INT numPos=0);
	static double getUNegNumByPos(LPCTSTR lpStr,INT numPos = 0);

	//�����󱣴���ַ����к������֣��ɰ�����1ָ����λ����ȡ�����֣�����ȡ����������0��
	//����double���ͷ��ء�����1����0Ϊ��ʼ�Ƶ�λ�������š�
	//����1�����ַ���Ϊ��123.456����һ������������ȡ123.456ʱ����1��������Ϊ0��
	//2�����ַ���Ϊ��abc-123.456����һ������������ȡ-123.456ʱ����1��������Ϊ1��
	//3��������������������ִ���Χ���ִ������������֣�����������0��
	//4�����ַ���Ϊ��abc-.456����һ������.001��652��00075���������ȡ��-0.456��0.001��75������1������Ϊ1��2��4��
	//5�����ַ���Ϊ��abc.abc123��������һ����.�������ȡ������0.0��ռһ����������λ��������1������Ϊ1����Ҫ��ȡ123�������1��������Ϊ2��
	double getDbNumByPos(INT numPos = 0);
	static double getDbNumByPos(LPCTSTR lpStr, INT numPos = 0);

	//�Ƚ϶�����ָ���ַ����Ƿ���ͬ����ͬ�򷵻�TRUE������1��ָ�����ַ���ָ�롣
	BOOL isEqual(LPCTSTR pStrCmp);

	//�Ƚ϶�����ָ��CLString���������Ƿ���ͬ����ͬ�򷵻�TRUE������1����һ��CLString����
	BOOL isEqual(const CLString& strCmp);

	//���������أ��ж��ַ����Ƿ���ͬ��ͬ�򷵻�TRUE������1��ָ�����ַ���ָ�롣
	//BOOL operator==(LPCTSTR pStrCmp);

	//���������أ��ж�CLString���������Ƿ���ͬ����ͬ�򷵻�TRUE������1����һ��CLString����
	//BOOL operator==(const CLString& strCmp);

	//���������أ��ж��ַ����Ƿ�ͬ����ͬ�򷵻�TRUE������1��ָ�����ַ���ָ�롣
	//BOOL operator!=(LPCTSTR pStrCmp);

	//���������أ��ж�CLString���������Ƿ�ͬ����ͬ�򷵻�TRUE������1����һ��CLString����
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

	//����ֵΪ0����ʾ��ͬ
	INT compareNoCase(const CLString& strCmp);
	//����ֵΪ0����ʾ��ͬ
	INT compareNoCase(LPCTSTR pStrCmp);
	//����ֵΪ0����ʾ��ͬ
	INT compare(const CLString& strCmp);
	//����ֵΪ0����ʾ��ͬ
	INT compare(LPCTSTR pStrCmp);

	/*
	�����������pStrCmp��ָ���ַ�������Ҫ����Щ�ַ��������ģ�ֻҪЩ���ַ�ǰ��˳���pStrCmp����һֱ���ɡ�
	����ֵ: INT  
	0�� ��ʾȫƥ��
	>0����ʾƥ��ĸ���
	-1��ָ�����(���κ�ƥ��)
	*/
	INT haveCharNoCase(LPCTSTR pStrCmp);
	INT haveCharNoCase(const CLString& strCmp);

	//�����󱣴������Unicode������ʽ���ַ���������������ʱ����һ�����ִ���
	//���ֽ�(EnCode_ASCII)�汾���ִ�������Ӱ�����Ķ��󱣴��ԭֵ��
	//ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	//CHAR* getMultiByte(VOID);
	//�����󱣴�����Զ��ֽ�(EnCode_ASCII)������ʽ���ַ���������������ʱ����һ�����ִ���
	//Unicode����汾���ִ�������Ӱ�����Ķ��󱣴��ԭֵ��
	//ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	//WCHAR* getWideChar(VOID);

   //���ر����ַ�����ascii�汾��ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	LPCSTR getASCII();
//���ر����ַ�����EnCode_UTF8�汾��ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	LPCSTR getUTF8();
//���ر����ַ�����UnicodeLittleEndian�汾��ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	LPCWSTR getUnicodeLittleEndian();
//���ر����ַ�����UnicodeBigEndian�汾��ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	LPCWSTR getUnicodeBigEndian();
//���ر����ַ�����Unicode�汾��ע�⣺����ֵ�������ڴ������������ͷţ���Ҫ����delete��free��
	LPCWSTR getUnicode();

	//��̬�������ɹ��򷵻�ת������ַ���������ָ�루��ָ����Ҫ��ʹ�ú���ʽdelete[]���ͷţ�����ת��ʧ���򷵻�NULLָ�롣
	static LPSTR unicodeToAscii(LPCWSTR lpStr);
	static LPSTR unicodeToUtf8(LPCWSTR lpStr);
	static LPWSTR asciiToUnicode(LPCSTR lpStr);
	static LPSTR asciiToUtf8(LPCSTR lpStr);
	static LPWSTR utf8ToUnicode(LPCSTR lpStr);
	static LPSTR utf8ToAscii(LPCSTR lpStr);

	//��Ա�������ɹ��򷵻�ת������ַ���������ָ�루��ָ�뱣�����ڲ�����(makeDEx()->m_pointer_)�У�����Ҫdelete[]������ת��ʧ���򷵻�NULLָ�롣
	LPCSTR unicodeToAsciiInner(LPCWSTR lpStr);
	LPCSTR unicodeToUtf8Inner(LPCWSTR lpStr);
	LPCWSTR asciiToUnicodeInner(LPCSTR lpStr);
	LPCSTR asciiToUtf8Inner(LPCSTR lpStr);
	LPCWSTR utf8ToUnicodeInner(LPCSTR lpStr);
	LPCSTR utf8ToAsciiInner(LPCSTR lpStr);
	
	//���û�����������(makeDEx()->m_pointer_)��ʱָ������
	inline LPCSTR getTmpBufMultiByte()const{ return pDataEx ?  pDataEx->m_pointer_.pMultiByte : NULL;}
	inline LPCWSTR getTmpBufWideChar()const{ return pDataEx ?  pDataEx->m_pointer_.pWideChar: NULL;}
	//���û�����������(makeDEx()->m_pointer_)��ʱָ�뻺�����Ĵ�С���ֽڼǣ�
	inline LONG_PTR getTmpBufSize()const{ return pDataEx ? pDataEx->m_pointerBufSizeInByte_:0;}

	//UNICODE�ַ���ת��Ϊ���ֽ��ַ�����
	//static BOOL wideCharToMultiByte(LPSTR pDesMultiByteBuf, LPWSTR pSorWideChar, LONG_PTR nDesBufLen);
	//���ֽ��ַ���ת��ΪUNICODE�ַ�����
	//static BOOL multiByteToWideChar(LPWSTR pDesWideCharBuf, LPSTR pSorMultiByteChar, LONG_PTR nDesBufLen);

	//�����ֱ������һ��ȫ·�����ļ��������滻����չ��,��".xxx"��ʽ�ģ����û����չ�������ӣ�
	CLString& replaceExtName(LPCTSTR newExtName);

	//ע������---------------------------------------------------------------------------------------------------------------------------------
	//���������ַ������浽ָ��ע���λ�á�
	//����1������������2����·��������3��������
	BOOL setReg(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName);

	//��ָ����ע���λ�õ��ַ������뵽�����ַ����塣
	//����1������������2����·��������3������������4���趨�Ļ�������С��
	BOOL getReg(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName,LONG_PTR stroeSize=MAX_PATH);

	//�����󱣴���ַ�����һ����Ч��ע���·���������ɾ����·����ָ���ļ�����ֵ��
	//����1������������2���Ƿ�ɾ������������ֵ������3��Ҫɾ�����ļ�����
	//������2������3��ΪĬ��ֵ��ɾ��·���µ����м�����ֵ��
	//ɾ��һ����·���ĳ��Key����isToDeleteKey=TRUE������RegDeleteKey��
	//ɾ��ĳ��Key���µ�ֵ��isToDeleteKey=FALSE������RegDeleteValue��
	BOOL asPathToDeleteReg(HKEY mainKey,BOOL isToDeleteKey=TRUE,LPCTSTR keyName=NULL);

	//�����󱣴���ַ�����һ����Ч��ע���·���������·���µ�����������Ӽ�(����key-value��)�������ӽ�����ֵ����asPathToEnumRegValue��
	//��ö�ٳɹ�����TRUE���ú�����ΪasPathToEnumRegKey������Win32/NT�����ϵ���չ�汾ʹ�á�
	//����1������������2��Ҫö�ٵ�������ֵ������3�����������Ļ�����ָ�룻
	//����4�����������Ļ�������С������5�����������ִ�ָ�룻����6�����������ִ������С���ֽڼƣ���
	//����7������д��ʱ����Ϣ�ṹ�塣
	BOOL asPathToEnumRegKeyEx(HKEY mainKey,DWORD dwIndex,LPTSTR subkey_name,DWORD subKeyLen=MAX_PATH,
		LPTSTR lpClass=NULL,     // address of buffer for class string
		LPDWORD lpcbClass=NULL,  // address for size of class buffer
		PFILETIME lpftLastWriteTime=NULL // address for time key last written to
		);

	//�����󱣴���ַ�����һ����Ч��ע���·�����������ö�ٸ��ִ���ʾ·���µ�������
	//��ö�ٳɹ�����TRUE����Win32/NT�������������չ�汾��asPathToEnumRegKeyEx������
	//����1������������2��Ҫö�ٵ�������ֵ������3�����������Ļ�����ָ�룻
	//����4�����������Ļ�������С��
	BOOL asPathToEnumRegKey(HKEY mainKey,DWORD dwIndex,LPTSTR subkey_name,DWORD subKeyLen=MAX_PATH);

	//�����󱣴���ַ�����һ����Ч��ע���·�����������ö�ٸ��ִ���ʾ·���µļ������Ӧ��ֵ��
	//��ö�ٳɹ�����TRUE������1������������2��Ҫö�ٵ�������ֵ������3����������Ļ�������С��
	//����4����������Ļ�����ָ�룻����5������ֵ�Ļ���������������Ĵ�С�����ֽڼƣ���
	//����6������ֵ�Ļ����������������ָ�룻����7����ֵ�������롣
	BOOL asPathToEnumRegValue(
		HKEY mainKey,              // handle to key to query
		DWORD dwIndex,          // index of value to query		
		DWORD valueNameBufSize,  // address for size of value buffer	
		LPTSTR valueNameBuf,     // address of buffer for value string		
		DWORD dataSizeInByte ,       // address for size of data buffer
		BYTE* dataBuffer,          // address of buffer for value data
		DWORD Type=REG_SZ        // address of buffer for type code
		);

	//�����󱣴���ַ�����һ����Ч��ע���·�������������ȡ���ִ���ʾ·���µ�key��Ӧvalueֵ;
	//����Ҫ����index����ͨ����������keyValueName������ƥ�䣨�ɲ����ִ�Сд����valueType�����������ͱ���ƥ�䣨���� REG_SZ,REG_DWORD��������ʧ�ܡ�
	//��ö�ٳɹ�����TRUE������1������������2��Ҫö�ٵ�������ֵ������3����������Ļ�������С��
	//����4����������Ļ�����ָ�룻����5������ֵ�Ļ���������������Ĵ�С�����ֽڼƣ���
	//����6������ֵ�Ļ����������������ָ�룻����7����ֵ�������롣
	BOOL asPathToGetRegValue(
		HKEY mainKey,              // handle to key to query	
		LPCTSTR valueName,     // address of buffer for value string		
		DWORD dataBufSizeInByte,       // address for size of data buffer
		LPVOID dataBuffer,          // address of buffer for value data	
		DWORD valueType=REG_SZ        // address of buffer for type code
	);


	//http������������---------------------------------------------------------------------------------------------------------------------------------

	//Http Get����,lpszObjectName��ʾ�����ִ���pHeadersָ�򱣴�Ҫ���ӵı�ͷ�ִ������飬nHeaderNum��ͷ������
	//bU2G�Ƿ񽫽����UTFתΪ���ֽڣ�openErrorAlert�Ƿ���������ʾ����
	BOOL httpGet(LPCTSTR lpszObjectName = NULL,LPCTSTR pHeaders[] = NULL,LONG_PTR nHeaderNum = 0,BOOL bU2G = TRUE,	BOOL openErrorAlert = TRUE,LPTSTR pOutErrStr = NULL);
	//���ڲ�������Դ򿪵�������һ����ͷ��	BOOL httpAddHeaders(LPTSTR pHeaderString = NULL);
	BOOL httpAddHeaders(LPTSTR pHeaderString = NULL);
	//����Http����ǰ�ĳ�ʼ���������Ӻ�����serverDomain��ʾ����http��//����������ip����port����˿ڣ�
	//openErrorAlert�Ƿ���������ʾ����
	BOOL httpInitGet(LPCTSTR serverDomain,INTERNET_PORT port = 80,BOOL openErrorAlert = TRUE);
	//��UTF�ִ�תΪ���ֽں����ӵ�string����β����
	static void U2G(const char* utf8,std::string& re);
	//�ͷ����д򿪵�Http��Դ���
	CLString& httpClose();
	//��Urlָ��ĵ�ַ�ļ����ݱ��浽������,Url�����http://ͷ;urlEncodeָ���������ݵĸ�ʽ���ú�������Ҫ����httpClose()
	BOOL httpOpenUrl(LPCTSTR Url,BOOL openErrorAlert = TRUE, LPTSTR pOutErrStr = NULL,BYTE urlEncode = EnCode_ASCII);


	//�ļ�ϵͳ���---------------------------------------------------------------------------------------------------------------------------------

	//���ܣ�[��̬����]ö��ָ��·���µ��ļ����ļ��е�·���ַ��������ص�std::vector�У�
	//����ֵ���޷���
	//������stringVector ����·��ö�ٽ����std::vector<LPCTSTR>��������ã�ע������ַ�����Ϊ��̬�����ڴ棬��Ҫ�ɵ�������һ�ͷţ�
	//������lpPath ��ʼ·����·�����һ���ַ����Լ���·���ָ�����'\\'��'/'����Ҳ���Բ��ӣ�·��Ҳ֧�����·�������ǵ������ʽ���� "./" "../" ".\\" ".\\..\\" �ȣ�
	//������lpExName [�ļ�ģʽ]��Ҫ�о�ƥ����ļ���չ��(����Ҫ��.��)��[�ļ���ģʽ]��Ҫƥ�����ĩ�����ļ��е����ƣ����ò���ֵΪ NULL,0,_T("")��_T('')ʱ��ƥ���Ӧģʽ�µ����н���������κι��ˣ�
	//������bRecursion �Ƿ���õݹ���ã�Ĭ��=TRUE��������TRUE�����������Ŀ¼�µ��ļ��м��������е����ļ��У�
	//������bEnumFiles �Ƿ�Ϊ�ļ�ģʽ��Ĭ��=TRUE��������TRUEΪ�ļ�ģʽ�����ֻ�����������ļ���·�����������Ϊ�ļ���ģʽ��ֻ�����������ļ��е�·����	
	//������bClearVectorBefoteStart �Ƿ���ö��ǰ��ս���������ͷŶ�Ӧָ��Ķ�̬�ڴ棨Ĭ��=TRUE ��ʾ��գ�����������������ݲ����ַ���ָ��ָ����ǷǶ�̬������ڴ�ռ䣬������ΪTRUEʱ�������ͷŴ���
	//������bJustStoreEndName �Ƿ�����洢ĩβ��������ȫ·����Ĭ��=FALSE ��ʾ����ȫ·����
	//ע�⣺�ļ���ģʽ�µ�·������ַ�����ĩβ�ַ����Ậ��·���ָ�����'\\'��'/'��
	//ע�⣺ͨ��ϵͳ���ص��ļ����ļ���·�������std::vector���ַ����������еģ�С->��
	static void fileEnumeration(std::vector<LPCTSTR>& stringVector ,LPCTSTR lpPath,LPCTSTR lpExName, 
		BOOL bRecursion = TRUE, BOOL bEnumFiles = TRUE,BOOL bClearVectorBefoteStart = TRUE,BOOL bJustStoreEndName = FALSE);
	
	//���ܣ�ö��ָ��·���µ��ļ����ļ��е�·���ַ��������ص������std::vector����������У�
	//����ֵ�������м��������ļ����ļ���·���Ķ����ڲ����������std::vector�ĳ������ã�
	//������lpPath ��ʼ·����·�����һ���ַ����Լ���·���ָ�����'\\'��'/'����Ҳ���Բ��ӣ�·��Ҳ֧�����·�������ǵ������ʽ���� "./" "../" ".\\" ".\\..\\" �ȣ�
	//������lpExName [�ļ�ģʽ]��Ҫ�о�ƥ����ļ���չ��(����Ҫ��.��)��[�ļ���ģʽ]��Ҫƥ�����ĩ�����ļ��е����ƣ����ò���ֵΪ NULL,0,_T("")��_T('')ʱ��ƥ���Ӧģʽ�µ����н���������κι��ˣ�
	//������bRecursion �Ƿ���õݹ���ã�Ĭ��=TRUE��������TRUE�����������Ŀ¼�µ��ļ��м��������е����ļ��У�
	//������bEnumFiles �Ƿ�Ϊ�ļ�ģʽ��Ĭ��=TRUE��������TRUEΪ�ļ�ģʽ�����ֻ�����������ļ���·�����������Ϊ�ļ���ģʽ��ֻ�����������ļ��е�·����	
	//������bClearVectorBefoteStart �Ƿ���ö��ǰ��ս��������Ĭ��=TRUE ��ʾ��գ����ú����ڴ��ɶ��������ƣ��κ���������øò�������������ͷŴ���Ĳ�����
	//������bJustStoreEndName �Ƿ�����洢ĩβ��������ȫ·����Ĭ��=FALSE ��ʾ����ȫ·����
	//ע�⣺�ļ���ģʽ�µ�·������ַ�����ĩβ�ַ����Ậ��·���ָ�����'\\'��'/'��
	//ע�⣺ͨ��ϵͳ���ص��ļ����ļ���·�������std::vector���ַ����������еģ�С->��
	//ע�⣺�ý�����ĺ�����ͨ����ʽ���ö����getVT()�������»���䳣������;
	const std::vector<LPCTSTR>& fileEnumeration(LPCTSTR lpPath,LPCTSTR lpExName, BOOL bRecursion = TRUE, 
		BOOL bEnumFiles = TRUE,BOOL bClearVectorBefoteStart = TRUE,BOOL bJustStoreEndName = FALSE);
	
	//�����༶Ŀ¼·����ע�⣺·���ԡ�/����\\����β����,Ŀ¼�ָ�������ʹ�������/����\\������ϡ�
	//���� "c://\\123\\/abc////"  �������� "c:\123\abc" Ŀ¼
	//·�����ڻ򴴽��ɹ�����true��·������ʧ�ܷ���false��
	static BOOL createDirectory(LPCTSTR lpszPath);
	//���ļ����������༶Ŀ¼·����ע�⣺·���ԡ�/����\\����Ϊ�ָ����,Ŀ¼�ָ�������ʹ�������/����\\������ϡ�
	//����������ʽ��������Ŀ¼��
	//���� "c://\\123\\/abc////file1.txt"  �������� "c:\123\abc" Ŀ¼
	//���� "c://\\123\\/abc////file1"  �������� "c:\123\abc" Ŀ¼
	//���� "c://\\123\\/abc////"  �������� "c:\123\abc" Ŀ¼
	//·�����ڻ򴴽��ɹ�����true��·������ʧ�ܷ���false��
	static BOOL createDirectoryByFileName(LPCTSTR lpszFileName);
	BOOL createDirectory(void);
	//ȡ��ϵͳ���������ļ��е�·�����浽�����ַ�����������API������SHGetSpecialFolderPath��
	//Ĭ�ϲ����½�ȡ�������ļ��е�ȫ·�� nFolder=CSIDL_DESKTOP��
	CLString& getSpecialFolderPath(INT nFolder=CSIDL_DESKTOP,INT storeLen=MAX_PATH,HWND hwndOwner=NULL,BOOL fCreate=FALSE);
	//�����󱣴���ַ�����һ���ļ����ļ��е�·��������·�������·�����ļ����ļ��о��ɣ���
	//������ļ����ļ����Ƿ���ڡ������򷵻�TRUE��
	//ע�⣺·�������ԡ�/����\\����β��
	BOOL filePathExists(VOID);
	//�����󱣴���ַ�����һ���ļ��е�·��������·�������·�����ɣ���������ļ����Ƿ���ڡ������򷵻�TRUE��
	//ע�⣺·�������ԡ�/����\\����β��
	BOOL folderExists(VOID);
	//�����󱣴���ַ�����һ���ļ��е�·��������·�������·�����ɣ���������ļ����Ƿ���ڡ������򷵻�TRUE��
	//ע�⣺·�������ԡ�/����\\����β��
	BOOL findFirstFileExists(DWORD dwFilter=FILE_ATTRIBUTE_DIRECTORY);
	// ���һ���ļ��Ƿ���ڣ�����·�������·�����ļ����ļ��о��ɣ���ע�⣺·�������ԡ�/����\\����β��
	BOOL fileExists(VOID);

	//��ָ����pFileDirΪNULL�����󱣴���ַ�������д������ڲ���ά��һ���ļ������ָ����ļ��У��þ��������Ϊ���򵱶�������ʱ���Զ��ͷţ�Ҳ������ʽ����closeFile()���������رմ򿪵��ļ������
	//��ָ����pFileDir�ļ�·������ر����е��ļ����������ָ�������ļ������򿪲��ɹ���ԭά���Ŀ��þ�����ٿ��ã�
	//Cģʽ����"w+"ģʽ���ļ���APIģʽ�Զ�д��ʽ���ļ�����ģʽ���죩��
	//dwOpenMethod = OPEN_ALWAYS��ʾ��������׷��
	//lDistanceToMove Long���ֽ�ƫ����
	//lpDistanceToMoveHigh Long��ָ��һ�����������������а�����Ҫʹ�õ�һ����˫��ƫ�ơ�����Ϊ��(��������ΪByVal)����ʾֻʹ��lDistanceToMove;
	//dwMoveMethod Long����������֮һ
	//FILE_BEGIN lOffset����λ����Ϊ���ļ���ʼ����ʼ������һ��ƫ��
	//FILE_CURRENT lOffset����λ����Ϊ�ӵ�ǰλ�ÿ�ʼ�����һ��ƫ��
	//FILE_END lOffset����λ����Ϊ���ļ�β��ʼ�����һ��ƫ��
	BOOL writeToFile(LPCTSTR pFileDir = NULL,DWORD dwOpenMethod = OPEN_ALWAYS,DWORD dwMoveMethod = FILE_END, LONG lDistanceToMove = 0, PLONG lpDistanceToMoveHigh = NULL,BOOL openAlertMsg = TRUE);
	BOOL writeLineToFile(LPCTSTR pFileDir = NULL,DWORD dwMoveMethod = FILE_END, LONG lDistanceToMove = 0, PLONG lpDistanceToMoveHigh = NULL,BOOL openAlertMsg = TRUE);

	//��ȡ�ı���һ�У�������ĩβ���з������´ε��ý�������һ�ж�ȡ��
	//����ֵ >=0 ��ʾ��ǰ��ȡ�д��ڲ�����ֵ��ʾ��ȡ���ַ�����������ȻΪһ�У�
	//����ֵ -1  ��ʾ��ǰ���Ѿ������ļ�ĩβ��û�пɶ�ȡ��������
	LONG_PTR readLineFromFile(LPCTSTR  pFileDir = NULL, BOOL openAlertMsg = TRUE);
	//��ȡ�ı�(�����һ�п�ʼ)��һ�У�������ĩβ���з������´ε��ý�������һ�ж�ȡ��
	//����ֵ >=0 ��ʾ��ǰ��ȡ�д��ڲ�����ֵ��ʾ��ȡ���ַ�����������ȻΪһ�У�
	//����ֵ -1  ��ʾ��ǰ���Ѿ������ļ�ĩβ��û�пɶ�ȡ��������
	LONG_PTR readLineFromFileEnd(LPCTSTR pFileDir = NULL, BOOL openAlertMsg = TRUE);
	//�ر��ڲ�������Ѵ򿪵��ļ��������������ļ��Ĳ���ģʽ��ǣ�
	BOOL closeFile();

	//��ȡһ���ļ����������ݵ������У����ú����Ϲر��ļ��������ȡʧ�ܷ���NULL�����򷵻��ļ����ݡ�
	//�ú������ᱣ���ļ������ʹ�ú��õ���closeFile()������
	LPCTSTR readFile(LPCTSTR pFilePath, BOOL openAlertMsg = TRUE);


	//�ú����ж�ϵͳ��Big Endian����Little Endian
	//big-endian��   ����true;
	//little-endian������false
	inline static BOOL isBigEndian()
	{
		unsigned short test = 0x1122;
		if(*( (unsigned char*) &test ) == 0x11) return TRUE;
		else return FALSE;
	}
	//������Ҫһ���Ѵ򿪵Ŀɶ��ļ�����������ļ��ı����ʽ��
	//Ŀǰֻ���ж�ascii��Unicode bigEndian/littleEndian��UTF-8���ı���
	//����2ָ��Ϊtrue��ʾ���ļ����жϺ��ļ�ָ���ƶ�����Ч������ʼλ�ã���������ǰ�˵ı�ͷ�ֽڣ�
	static byte checkTextFileEncode(HANDLE hFile,BOOL isMovePinterToBeging = FALSE);

	//ת��ԭ�������е��ı���ʽ�������lpOutTagbufָ���ָ�롣
	static BOOL swapEncode(const LPBYTE lpResbuf,LONG_PTR nResbufSizeInByte,byte uResEncode,
		LPBYTE &lpOutTagbuf,LONG_PTR &nOutTagbufSizeInByte,byte uTagEncode,LPBYTE lpDefaultBuf = 0,LONG_PTR nDefaultBufSizeInByte = 0);

	//�ִ�ת��������---------------------------------------------------------------------------------------------------------------------------------

	//�ѱ�����ַ�����szSplitChar�ָ����(һ���ַ�)��ֵ�һ��std::vector��,
	//bClearVectorBefoteStartָ���Ƿ���ִ��ǰ����ڲ���std::vector���������������������׷�ӵ�std::vectorβ����
	//˵��������ָ������н���Ϊ����֮����Ȼ��ֻ�п���ֹ�ַ����ַ�����
	//��������Է��ص�vt���κ�����
	//��ԭ�ַ������зָ��ַ���,�������ԭ�ַ�����std::vector�У������۷ָ���Ƿ���ڣ������ֳܷ�һ���ַ�������
	std::vector<LPCTSTR>& split(const TCHAR szSplitChar = _T(','),BOOL bClearVectorBefoteStart = TRUE);
	std::vector<LPCTSTR>& split(LPCTSTR szSplitCharString,BOOL bClearVectorBefoteStart = TRUE);
	//��ն����ڲ������std::vector��������ڴ棨�ͷ�std::vector�������ڴ��ϵͳ����clearInnerStringVectorMemery()��
	void clearInnerStringVector();
	//�ͷ�std::vector�������ڴ��ϵͳ
	void clearInnerStringVectorMemery();
	//���std::vector�б���Ķ�̬�ַ����ڴ�
	static void clearStringVector(std::vector<LPCTSTR>& _v_str);	
	//ȡ���ڲ�������ַ������std::vector���������
	const std::vector<LPCTSTR>& getVT();
	//��std::vector�������У���->С�������ִ�Сд��
	const std::vector<LPCTSTR>& makeVtDescendingOrder();
	//��std::vector�������У���->С���������ִ�Сд��
	const std::vector<LPCTSTR>& makeVtDescendingOrderI();
	//��std::vector�������У�С->�󣩣����ִ�Сд��
	const std::vector<LPCTSTR>& makeVtAscendingOrder();
	//��std::vector�������У�С->�󣩣������ִ�Сд��
	const std::vector<LPCTSTR>& makeVtAscendingOrderI();

	inline size_t vtSize(){ return getVT().size();}//���ٷ��ʵ�ǰvt��Ԫ����
	inline LPCTSTR vtAt(size_t i){ return getVT().at(i);}//���ٷ���vt�ִ�

	//���ٷ���vt�ִ���ascii�汾,�ر�ע�⣺���ص���һ���ǳ־���ָ�룬����һ�ε��ú����ݽ���ı䣻
	LPCSTR vtAtA(size_t i);
	//���ٷ���vt�ִ���unicode�汾,�ر�ע�⣺���ص���һ���ǳ־���ָ�룬����һ�ε��ú����ݽ���ı䣻
	LPCWSTR vtAtW(size_t i);



	//ϵͳ������---------------------------------------------------------------------------------------------------------------------------------
	static HWND getConsoleHwnd(void); //ȡ�õ�ǰ��̨����Ĵ��ھ��

	//����windows�µ�ʱ����Ҫ�����洢ʱ���ָ�������
	static double calcTimeDelta(const SYSTEMTIME* beforetime,const SYSTEMTIME* behandtime);

	//����ϵͳAPI����һ��GUIDֵ������GUID
	static GUID createGuid(void);
	//��һ��GUIDֵ��ʽ��Ϊ�ַ��������ڶ����ڲ����������ַ�����
	LPCTSTR guidToString(const GUID &guid);	
	
	//����ϵͳAPI����һ��GUIDֵ���������GUIDֵ��ʽ��Ϊ�ַ��������ڶ����ڲ����������ַ�����
	//���ɲ���pResGuidָ��һ��GUID�������򴴽���GUID�ᱣ�浽ָ��ı�������ΪNULL�򲻷���GUIDֵ��
	LPCTSTR createGuidToString(GUID* pResGuid = NULL);

	//��ʼ��ʱ����ʱ�������������ȵ��ô˺���Timingϵ�к������������Ex��������΢��������
	void timingStart(void);
	void timingStartEx(void);
	//ȡ���Լ�ʱ��ʼ�𵽵�ǰ����ʱ�̵�ϵͳ����ʱ�䣬����Ϊ��λ��Ex��������΢��������
	double timingGetSpentTime(BOOL openErrorAlert = TRUE,LPTSTR pOutErrStr = NULL);
	double timingGetSpentTimeEx(BOOL openErrorAlert = TRUE,LPTSTR pOutErrStr = NULL );
	//ȡ���Լ�ʱ��ʼ�𵽵�ǰ����ʱ�̵�ϵͳ����ʱ�䣬����Ϊ��λ���ַ���������
	//���޷����㷵��NULL���ɹ��򷵻��ַ��������������ִ������ڶ����У�
	LPCTSTR timingGetSpentTimeString(BOOL openErrorAlert = TRUE,LPTSTR pOutErrStr = NULL);	
	

	//ȡ��windowsϵͳGetLastError��������ʽ���ַ���
	LPCTSTR getLastErrorString(DWORD nLastError);
	CLString& getLastErrorStringR(DWORD nLastError);
	static DWORD  getLastErrorString(DWORD nLastError,LPTSTR lpszOutPut,LONG_PTR nSize);
	//ȡ��windowsϵͳGetLastError��������Ӧ��ʽ���ַ�����MessageBox
	DWORD getLastErrorMessageBox(DWORD nLastError,HWND hParentWnd = 0,LPCTSTR pTitle = _T("LastError"),UINT uType = MB_ICONWARNING,LPCTSTR pInsetStr = NULL,LPCTSTR pEndStr = NULL);
	//ȡ��windowsϵͳGetLastError������루����ERROR_SUCCEED����Ӧ��ʽ���ַ�����MessageBox
	DWORD getLastErrorMessageBoxExceptSucceed(DWORD nLastError,HWND hParentWnd = 0,LPCTSTR pTitle = _T("LastError"),UINT uType = MB_ICONWARNING,LPCTSTR pInsetStr = NULL,LPCTSTR pEndStr = NULL);
	//��MessageBox��ʾ��ǰ����ֵ������
	INT messageBox(UINT nStyle = MB_OK,HWND hParentWnd = 0);
	CLString& messageBoxRef(UINT nStyle = MB_OK,HWND hParentWnd = 0);
	INT messageBoxTimeout(UINT nStyle = MB_OK, HWND hParentWnd = 0);
	INT messageBox(LPCTSTR boxTitle,UINT nStyle,HWND hParentWnd = 0);
	CLString& messageBoxRef(LPCTSTR boxTitle,UINT nStyle,HWND hParentWnd = 0);
	//��ʱͨ��һ��MessageBox��ʾ����ǰ������ַ������ݡ�
	void showContent();
	//��cmd����̨����ַ������ݡ�
	CLString& logout();
	//��cmd����̨����ַ������ݡ�
	CLString& printf();
	//������������ر�������ַ����ȣ�����1����ʾҪ�˸��ɾ���ַ�������ֵӦ��Ϊ��һ�ε��ú����ɹ��ķ���ֵ����ʵ�ֵ��еĻ�ɾ������ʾ��
	int printfBackCover(int bakCoverChars); 
	//�����ڿ���̨���������ַ���ʹ�á���������ֵ��0 ������Esc������1 �����»س�������
	//����1����������ַ���������ָ�룬����2����������С���ַ�Ϊ��λ������3��ǰ���ַ���ָ�룬��ΪNULLʱ��û���κ�ǰ����ʾ��
	//����Ϊ����������ֻ�е����»س���Esc���������Ż��˳���
	//�������ֻ������nBufferSizeInChar-1���ַ���
	//�������Զ���lpBuffer����ʼ����
	static int getCharCmd(LPTSTR lpBuffer,LONG_PTR nBufferSizeInChar,LPCTSTR lpPreString = NULL);
	//ȫ�ֺ�����ȡ��windows��Ϣ������Ӧ����Ϣ�����ַ�����Releaseģʽ�·���һ�б������������õ���ʾ�ַ�����������NULLָ�롣
	static LPCTSTR getWindowsMsgStringFromMsgID(UINT messageId);

	//���ڲ��ַ�������һ���쳣�����׳�
#define CLSTRING_MAKE_EXCEPTION(logicName) void throw_##logicName() throw(std::logicName);
#pragma warning( disable : 4290 )
	CLSTRING_MAKE_EXCEPTION(exception);//�쳣
	CLSTRING_MAKE_EXCEPTION(logic_error);//�߼�����
	CLSTRING_MAKE_EXCEPTION(domain_error); //�����
	CLSTRING_MAKE_EXCEPTION(invalid_argument); //�Ƿ�����
	CLSTRING_MAKE_EXCEPTION(length_error); //ͨ���Ǵ��������Ǹ����ĳߴ�̫��
	CLSTRING_MAKE_EXCEPTION(out_of_range); //���ʳ���

	CLSTRING_MAKE_EXCEPTION(runtime_error);//����ʱ����
	CLSTRING_MAKE_EXCEPTION(range_error); //�߽����
	CLSTRING_MAKE_EXCEPTION(overflow_error);//����
	CLSTRING_MAKE_EXCEPTION(underflow_error);//����

	//��CLString����ʵ��---------------------------------------------------------------------------------------------------------------------------------
	BOOL isEmpty();//�Ƿ�Ϊ���ַ�

	CLString& empty();//ǿ�ƶ�����Ϊ��,���ַ������������գ��������ݲ����κ��޸�

	TCHAR getAt(LONG_PTR nIndex);//�����ִ�ָ��λ�õ��ַ���������0��ʼ,������Ϊ����ֵ��������Խ���򷵻�0���ᱼ��
	TCHAR operator []( LONG_PTR nIndex );//�����ִ�ָ��λ�õ��ַ���������0��ʼ;������Ϊ����ֵ��������Խ���򷵻�0���ᱼ��

	BOOL setAt( LONG_PTR nIndex, TCHAR ch );//���ø���λ���ϵ��ַ� 

	//���ص�(LPCTSTR)�����������һ��C�����ַ���һ����ֱ�ӷ��ʱ�����һ��CLString�����е��ַ���
	inline operator LPCTSTR() const { return this->string(); }
	inline operator LPCSTR() { return this->getASCII(); }
	inline operator LPCWSTR() { return this->getUnicode(); }
	//���ص�()�����������һ��C�����ַ���һ����ֱ�ӷ��ʱ�����һ��CLString�����е��ַ���
	inline LPCTSTR operator()(void) const { return this->string(); }


	//�˳�Ա�����Ӵ�CLString��������ȡһ������ΪnCount���ַ����Ӵ�����nFirst�����㿪ʼ��������ָ����λ�ÿ�ʼ����Ŀ������У����ر���Ŀ���������á�
	CLString& mid( CLString& storeTagObj, LONG_PTR nCount ) ;
	CLString& mid( CLString& storeTagObj, LONG_PTR nFirst, LONG_PTR nCount ) ;
	CLString& left( CLString& storeTagObj,  LONG_PTR nCount ) ;
	CLString& right( CLString& storeTagObj,  LONG_PTR nCount ) ;

	//�˳�Ա�����Ӵ�CLString��������ȡһ������ΪnCount���ַ����Ӵ�����nFirst�����㿪ʼ��������ָ����λ�ÿ�ʼ���˺������ؽ�ȡ���������������������á�
	CLString& midSave(LONG_PTR nFirst, LONG_PTR nCount );	
	CLString& leftSave(LONG_PTR nCount);
	CLString& rightSave(LONG_PTR nCount);
	CLString& leftCut(LONG_PTR nCount) { return rightSave(this->strlen() - nCount); }
	CLString& rightCut(LONG_PTR nCount) { return leftSave(this->strlen() - nCount); }
	
	CLString& makeUpper( );//����ת��Ϊһ����д�ַ���
	CLString& makeLower( );//����ת��Ϊһ��Сд�ַ�����
	CLString& makeReverse( );//�����е��ַ���˳��ߵ�������
	//�˳�Ա������һ���ַ��滻��һ���ַ��������ĵ�һ��ԭ�����ַ�������chNew�ֳ��滻chOld��
	//�����ĵڶ���ԭ����lpszNewָ�����ַ����滻lpszOldָ�����Ӵ���
	//���滻֮�󣬸��ַ����п������������̣�������ΪlpszNew��lpszOld�ĳ��Ȳ���Ҫ����ȵġ����ְ汾��ʽ���������ִ�Сд��ƥ�䡣
	//����ֵ�����ر��滻���ַ������������ַ���û�иı��򷵻��㡣
	LONG_PTR replaceRN( TCHAR chOld, TCHAR chNew );
	CLString& replace(TCHAR chOld, TCHAR chNew);	
	LONG_PTR replaceRN( LPCTSTR lpszOld, LPCTSTR lpszNew );
	CLString& replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	LONG_PTR removeRN ( TCHAR ch );//��chʵ�����ַ��������ߡ�����ֵ�����ش��ַ��������ߵ��ַ���������ַ���û�иı��򷵻��㡣
	CLString& remove(TCHAR ch);

	//���ַ����еĸ�������������һ�������ַ���һ�����ַ���������ֵ�����ر��ı���ַ����ĳ��ȡ�
	LONG_PTR insertRN( LONG_PTR nIndex, TCHAR ch );
	CLString& insert(LONG_PTR nIndex, TCHAR ch);
	LONG_PTR insertRN( LONG_PTR nIndex, LPCTSTR pstr );
	CLString& insert(LONG_PTR nIndex, LPCTSTR pstr);
	//��һ���ַ����д�nIndex��ʼ�ĵط�ɾ��һ�������ַ������nCount�ȴ��ַ�����Ҫ�������ַ��������ಿ�ֶ�����ɾ����
	LONG_PTR deleteCharRN( LONG_PTR nIndex, LONG_PTR nCount = 1);
	CLString& deleteChar(LONG_PTR nIndex, LONG_PTR nCount = 1);
	//�����Ա������û�в����İ汾�������ַ�����ǰ��������ġ����з����ո��tab�ַ�����������
	//�����Ա��������Ҫ�����İ汾������һ���ض����ַ���һȺ�ض����ַ����ַ����Ŀ�ʼ�ͽ�β��ɾ����
	CLString& trim();
	CLString& trim(TCHAR chTarget);
	CLString& trim(LPCTSTR lpszTargets);
	//�����Ա������û�в����İ汾�������ַ�����ǰ��ġ����з����ո��tab�ַ�����������
	//�����Ա��������Ҫ�����İ汾������һ���ض����ַ���һȺ�ض����ַ����ַ����Ŀ�ʼ��ɾ����
	CLString& trimLeft( );
	CLString& trimLeft( TCHAR chTarget );
	CLString& trimLeft( LPCTSTR lpszTargets );
	//�����Ա������û�в����İ汾�������ַ��������ġ����з����ո��tab�ַ�����������
	//�����Ա��������Ҫ�����İ汾������һ���ض����ַ���һȺ�ض����ַ����ַ����Ľ�β��ɾ����
	CLString& trimRight( );
	CLString& trimRight( TCHAR chTarget );
	CLString& trimRight( LPCTSTR lpszTargets );

	//�ַ������������ַ����ĵ�һ��ƥ����ַ���
	//���ش˶���������Ҫ�����ַ������ַ�ƥ��ĵ�һ���ַ��Ĵ��㿪ʼ�����������û���ҵ����ַ������ַ��򷵻�-1��
	LONG_PTR  find( TCHAR ch, LONG_PTR nStart = 0 ) ;
	LONG_PTR  find( LPCTSTR lpszSub, LONG_PTR nStart = 0 ) ;

	LPCSTR strStr(LPCSTR lpszSub) { return StrStrA(getASCII(), lpszSub); };
	LPCWSTR strStr(LPCWSTR lpszSub) { return StrStrW(getUnicode(), lpszSub); };
	LPCSTR strStrI(LPCSTR lpszSub) { return StrStrIA(getASCII(), lpszSub); };
	LPCWSTR strStrI(LPCWSTR lpszSub) { return StrStrIW(getUnicode(), lpszSub); };

	//�ַ������������ַ����ĵ�һ��ƥ����ַ���
	//���ش˶���������Ҫ�����ַ������ַ�ƥ��ĵ�һ���ַ��Ĵ��㿪ʼ�����������û���ҵ����ַ������ַ��򷵻�-1��
	static LONG_PTR  findString(LPCTSTR lpszTag,LPCTSTR lpszSub, LONG_PTR nStart = 0);

	//������һ���Ӵ�ƥ������һ���ַ����˺�������������ʱ����strrchr.
	//���ش˶�������Ҫ����ַ�ƥ������һ���ַ������������û���ҵ���Ҫ���ַ��򷵻�-1��
	LONG_PTR  reverseFind( TCHAR ch ) ;
	//�ַ�����������lpszCharSet�������ַ�ƥ��ĵ�һ���ַ���
	//���ش��ַ����е�һ����lpszCharSet��Ҳ�������ַ��Ĵ��㿪ʼ��������
	LONG_PTR  findFirstOneOf( LPCTSTR lpszCharSet ) ;

	//���ַ����������ҵ���ǩ��������
	static void findFlagAndSort(std::vector<TCHAR*>& sortRet, const TCHAR* lpFlagString, const TCHAR** tagStringList, size_t tagStringCounts);

	//���ܼ������ַ�����nDepth��ʾ���ܵ����(Ĭ��Ϊ1�㣬һ���������Ҫ���Ӿ�����Ҫ��)��
    //���ܼ����ܵĶ�Ӧ��ȱ�����ͬ���ܽ��ܡ�����ͬʱ֧��UNICODE��ASCII��
	//���ٶ���Ҫ��Ŀ���ʹ�ã�����buf1��buf2��nCharCounts�İ汾������Ҫ��̬�����ڴ棩��
	//����nCharCounts���������ִ��ַ���(ע�⣺ASCIIһ������Ϊ2���ַ���),����buf1��buf2�Ĵ�С,buf1��buf2�Ĵ�С����>=nCharCounts��������ʻ�Խ�硣
	//����ѹ�����ԣ��ַ���100�����ӽ��ܵ��Σ����100��㣬��ʱ1.36��(��buf��)��1.36�롣
	//�ַ���100�����ӽ���10��Σ�ÿ��2�㣬��ʱ2.5��(��buf��)��3.5�롣
	//���Ի���win7 x64 A10 6G
	static CLStringR encrypteString(CLStringR outputString,LPCTSTR inputString,size_t nDepth = 1);	
	static CLStringR unEncrypteString(CLStringR outputString,LPCTSTR inputString,size_t nDepth = 1);
	static CLStringR encrypteString(CLStringR outputString,LPCTSTR inputString,int* buf1,int* buf2,size_t nCharCounts,size_t nDepth = 1);	
	static CLStringR unEncrypteString(CLStringR outputString,LPCTSTR inputString,int* buf1,int* buf2,size_t nCharCounts,size_t nDepth = 1);
	CLStringR encrypteString(LPCTSTR inputString,size_t nDepth = 1);
	CLStringR unEncrypteString(LPCTSTR inputString,size_t nDepth = 1);
	CLStringR encrypteString(LPCTSTR inputString,int* buf1,int* buf2,size_t nCharCounts,size_t nDepth = 1);
	CLStringR unEncrypteString(LPCTSTR inputString,int* buf1,int* buf2,size_t nCharCounts,size_t nDepth = 1);

//-----------------------------������̬ȫ�ֺ���---------------------------------------------------------------

	//findStringInPair�����Ľ����Ϣ�ṹ
	typedef struct {
		LPCTSTR pS1,pS2,pV1,pV2,pE1,pE2;//�ֱ�Ϊ��ǩ1��ǰ��ָ�룬����ǰ��ָ�룬��ǩ2��ǰ��ָ��
		size_t nS,nV,nE;//��ǩ1���ȣ����ݳ��ȣ���ǩ2����
	}FSIP_INF,*PFSIP_INF;//findStringInPair�����Ľ����Ϣ�ṹ
	//�ҵ�����ָ��������ǩ�ִ�����ִ����ҵ�����TRUE
	//lpOrg���ҵ�Դ�ַ�����
    //lpBeginFlag��lpEndFlagΪ��ǩ�ַ��������ǿ����ǡ�0��NULL��_T("\0")�������������ַ���
    //pInfOut�����Ϣ�ṹ��ָ�룬�ýṹ�崫��ǰ����Ҫ��ʼ��
    //orgMaxCheckCounts ��ʾ��Դ��ʼҪ���ҵ����Դ�ַ�����������ȴû�ҵ�������False��Ĭ��Ϊ 0 ��0��ʾ��������ַ���������ƣ�
	static BOOL findStringInPair(LPCTSTR lpOrg,LPCTSTR lpBeginFlag,LPCTSTR lpEndFlag,OUT PFSIP_INF pInfOut = 0,size_t orgMaxCheckCounts=0);
	BOOL findStringInPair(LPCTSTR lpBeginFlag,LPCTSTR lpEndFlag,OUT PFSIP_INF pInfOut = 0,size_t orgMaxCheckCounts=0);

//#define USE_CLSTRING_TIME_CHECK //Ҫ�ر�Ч�ܲ��ԣ���ע�͵�����
#ifdef USE_CLSTRING_TIME_CHECK
#define MAX_DBG 10
	public: 
		LPCTSTR _tick_ni;
		//CLTime _tick_time;
		double _tick_s;
#ifdef UNICODE
		std::map<std::wstring,double> _tick_lst;
#else
		std::map<std::string,double> _tick_lst;
#endif
	CLStringR initCheck(){_tick_lst.clear(); return *this;}
#define _INIT_CHECK_ { initCheck();}
#define _CHECK_START_( STR ) { _tick_ni = _T(#STR); timingStartEx();}
#define _CHECK_END_ {  _tick_s = timingGetSpentTimeEx(); _tick_lst[_tick_ni] += _tick_s;	}
	CLStringR checkLogout() { for(auto i = _tick_lst.begin();i != _tick_lst.end();i++)\
		_tprintf(_T(" %s = %.15lf s\n"),i->first.c_str(),i->second);return *this;}
#define _CHECK_LOGINF_ {checkLogout();}
#else
#define _INIT_CHECK_
#define _CHECK_START_( STR )
#define _CHECK_END_
#define _CHECK_LOGINF_
#endif

};

CLString operator+(const CLString& str1,const CLString& str2);
CLString operator+(LPCTSTR pStr1,const CLString& str2);
CLString operator+(const CLString& str1 ,LPCTSTR pStr2);
template<typename _class> CLString operator+(const CLString& str1 ,_class obj2){
	CLString temp;
	temp << str1 << obj2;
	return (temp);
}
template<typename _class> CLString operator+(_class obj1 ,const CLString&  str2){
	CLString temp;
	temp << obj1 << str2;
	return (temp);
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

//����ȽϺ�
//#define BETWEEN(x,a,b)  (( (a) <= (x) && (x) <= (b) ) || ( (b) <= (x) && (x) <= (a) ))
//#define BETWEENO(x,a,b)  (( (a) < (x) && (x) < (b) ) || ( (b) < (x) && (x) < (a) ))

//ȥ�����ֶ�������ע�ͣ���#�����������ݣ���ǰ��Ŀ��ַ�
#define CLRemoveNotes( x )  {LONG_PTR  ind_x_023dn = (x).find(_T("#"));ind_x_023dn = (ind_x_023dn == -1 ? (x).strlen() : ind_x_023dn);(x).midSave(0,ind_x_023dn);(x).trim();}
//ȡ��ȫ��0�ִ�ָ��
LPTSTR getCLString0();

//��ʱ����ת��
#define CLStrA( lpstr ) (CLString((lpstr)).getASCII())
#define CLStrW( lpstr ) (CLString((lpstr)).getUnicode())

#ifdef UNICODE
#define CLStr( lpstr ) CLStrW( lpstr )
#else
#define CLStr( lpstr ) CLStrA( lpstr )
#endif

#endif