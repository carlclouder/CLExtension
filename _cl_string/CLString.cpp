//自定义选择集文字管理类 CLString -----------------------------------------
#include "CLString.h"
#include "../_cl_common/CLCommon.h"

using namespace std;

#ifdef _DEBUG
LONG_PTR $Debug_alloc = 0;		   //内存分配次数计数器
BOOL $clstring_open_destroyflag = 0; //析构函数调用情况开关
#endif

#define BUFFERSIZE 16384 //16k


LPTSTR getCLString0(){
	static TCHAR $char0 = 0;//初始化常量值不能修改其值
	assert( $char0 == 0 );
	return &$char0;
}

  void CLString::setEncode()
{
#ifdef UNICODE
	 if(isBigEndian()) m_encoding = EnCode_UBE;
	 else m_encoding = EnCode_ULE;
#else
	 m_encoding = EnCode_ASCII;
#endif   
}
  void CLString::cleanAllMemory(void)/*会清除内部Vector*/
{
	assert(string() != NULL);
	if(string() != getCLString0() && string() != NULL)
		delete[] pHead;
	pHead = getCLString0();
	m_unitNumber = 0;
	m_changeFlag = TRUE;
	deleteDEx();
}
  void CLString::initialize(LONG_PTR nDefaultCharNumber)
{
	setEncode();
	//让指针有效
	if(nDefaultCharNumber < 1){
		pHead = getCLString0();
		m_unitNumber = 0;
	}
	else{ 
		pHead = new TCHAR[ m_unitNumber = nDefaultCharNumber];
		pHead[0] = 0;
	}
	m_strlen = 0;
	m_changeFlag = FALSE;
	pDataEx = 0;
}

  
  CLString& CLString::set(LPCSTR pString)
{
	if (!pString)
	{
		//ZeroMemory(pHead,m_unitNumber*sizeof(TCHAR));
		if(string() && string() != getCLString0())
			*pHead = 0;
		m_changeFlag = TRUE;
		return *this;
	}

	if(m_encoding == EnCode_ASCII)
    {
		LONG_PTR _targetLen = std::strlen(pString);
		if ((!( pString + _targetLen + 1 < (LPCSTR)pHead || pString > (LPCSTR)pHead + m_unitNumber )) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCSTR>(getCLString0()))) )
		{
			::MessageBoxA(NULL,"字符串指针范围重叠！", "CLString对象错误", MB_ICONERROR);
			throw std::logic_error("字符串指针范围重叠！");
		}
		if (_targetLen+1 <= m_unitNumber)
		{
			//ZeroMemory(pHead +_targetLen,(m_unitNumber -_targetLen)*sizeof(TCHAR));
			strcpy_s((LPSTR)pHead, _targetLen + 1, pString);
		}
		else {
		   if(string() != getCLString0())delete[] pHead;	
		   pHead = (LPTSTR)(new char[ m_unitNumber = _targetLen + 1]);  
		   strcpy_s((LPSTR)pHead, m_unitNumber, pString);
		}
		m_changeFlag = TRUE;
	}
	else{
		LONG_PTR _targetLen1 = std::strlen(pString);
		if ((!( pString + _targetLen1 + 1 < (LPCSTR)pHead || pString > (LPCSTR)((LPWSTR)pHead + m_unitNumber))) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCSTR>(getCLString0()))) )
		{
			::MessageBoxW(NULL,L"字符串指针范围重叠！", L"CLString对象错误", MB_ICONERROR);
			throw std::logic_error("字符串指针范围重叠！");
		}		
		LONG_PTR _targetLen;
		LPWSTR pStringW;
		BOOL rt = swapEncode((const LPBYTE)pString,_targetLen1,EnCode_ASCII,(LPBYTE&)pStringW,_targetLen,EnCode_ULE,(LPBYTE)pHead,m_unitNumber*sizeof(WCHAR));
		if(!rt)
			empty();
		else if(_targetLen > m_unitNumber*(LONGLONG)sizeof(WCHAR))
		{
			if(string() != getCLString0())delete[] pHead;
			pHead = (LPTSTR)pStringW;
			m_unitNumber = _targetLen/sizeof(WCHAR);
		}		
		m_changeFlag = TRUE;		
	}
	return *this;
}
  CLString& CLString::set(LPCWSTR pString)
  {
	  if (!pString)
	  {
		  //ZeroMemory(pHead,m_unitNumber*sizeof(TCHAR));
		  if(string() && string() != getCLString0())*pHead = 0;
		  m_changeFlag = TRUE;
		  return *this;
	  }
	  if(m_encoding == EnCode_ASCII){
		  LONG_PTR _targetLen1 = std::wcslen(pString);
		  if ((!( pString + _targetLen1 + 1 < (LPCWSTR)pHead || pString > (LPCWSTR)((LPCSTR)pHead + m_unitNumber) ) ) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCWSTR>(getCLString0()))))
		  {
			  ::MessageBoxA(NULL,"字符串指针范围重叠！", "CLString对象错误", MB_ICONERROR);
			  throw std::logic_error("字符串指针范围重叠！");
		  }		
		  LONG_PTR _targetLen;
		  LPSTR pStringA;
		  BOOL rt = swapEncode((const LPBYTE)pString,(_targetLen1)*sizeof(WCHAR),EnCode_ULE,(LPBYTE&)pStringA,_targetLen,EnCode_ASCII,(LPBYTE)pHead,m_unitNumber*sizeof(CHAR));
		  if(!rt)
			  empty();
		  else if(_targetLen > m_unitNumber*(LONG_PTR)sizeof(CHAR))
		  {
			  if(string() != getCLString0())delete[] pHead;
			  pHead = (LPTSTR)pStringA;
			  m_unitNumber = _targetLen/sizeof(CHAR);
		  }		
		  m_changeFlag = TRUE;	
	  }
	  else{
		  LONG_PTR _targetLen = std::wcslen(pString);
		  if ((!( pString + _targetLen + 1 < (LPCWSTR)pHead || pString > ((LPCWSTR)pHead + m_unitNumber) ) ) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCWSTR>(getCLString0()))))
		  {
			  ::MessageBoxW(NULL, L"字符串指针范围重叠！", L"CLString对象错误", MB_ICONERROR);
			  throw std::logic_error("字符串指针范围重叠！");
		  }
		  if (_targetLen+1 <= m_unitNumber)
		  {
			  wcscpy_s((LPWSTR)pHead, _targetLen + 1, pString);
		  }
		  else {
			  if(string() != getCLString0())delete[] pHead;	
			  pHead = (LPTSTR)(new WCHAR[ m_unitNumber = _targetLen + 1]);  
			  wcscpy_s((LPWSTR)pHead, m_unitNumber, pString);
		  }
		  m_changeFlag = TRUE;	 
	  }
	return *this;
}
  CLString& CLString::set(LPCTSTR pString1,LPCTSTR pString2,...)
  {
	  
	  LPCTSTR p = pString1;	  
	  LONG_PTR n = 0;
	  LONG_PTR nSize = p ? n = 1,std::_tcslen(p):0;
	  va_list ap;
	  va_start(ap,pString2);
	  for(p = pString2;p;nSize += std::_tcslen(p),p = va_arg(ap,LPCTSTR),n++);
	  va_end(ap);
	  store(nSize);
	  LONG_PTR i = 1;
	  set(pString1);
	  va_start(ap,pString2);
	  for (p = pString2;i < n;append(p),p = va_arg(ap,LPCTSTR),i++);
	  va_end(ap);
	  return *this;
  }
  CLString& CLString::set(CHAR ch){
	  CHAR chr[2]={ch,0};
	  return set(chr);
  }
  CLString& CLString::set(WCHAR ch){
	  WCHAR chr[2]={ch,0};
	  return set(chr);
  }

BOOL CLString::_AddPathAnEnt(LPTSTR pResBuffer,INT EndsNmber)
{
	LONG index=-1;
	for (LONG i=0;pResBuffer[i];i++)
	{
		index=i;
	}
	if (pResBuffer[0]!=NULL)
	{
		INT j=0;
		for (j=0;j<EndsNmber;j++)
		{
			pResBuffer[index+1+j]=92;
		}
		pResBuffer[index+1+j]=0;
		return TRUE;
	}
	else return FALSE;
}
LPCTSTR CLString::_ExtendPathToQuality(LPCTSTR pResBuffer,LPTSTR pDesBuffer)
{
	INT i,j;
	for(i=0,j=0;pResBuffer[i];i++,j++)
	{
		if(pResBuffer[i]==_T('\\'))
		{
			pDesBuffer[j]=_T('\\');pDesBuffer[j+1]=_T('\\');
			j+=1;
		}
		else
		{	
			pDesBuffer[j]=pResBuffer[i];
		}
	}
	pDesBuffer[j]=0;
	return pDesBuffer;
}
LPCTSTR CLString::_ExtendPathToNormal(LPCTSTR pResBuffer,LPTSTR pDesBuffer)
{
	if(!pResBuffer)
		return pDesBuffer;
	INT i,j;
	BOOL isFlag = FALSE;
	for(i=0,j=0;pResBuffer[i];i++)
	{
		if((pResBuffer[i]==_T('\\'))&&(pResBuffer[i+1]==_T('\\')))
		{
			if(isFlag == FALSE ){ 
				pDesBuffer[j++]=_T('\\');
				isFlag = TRUE;
				i+=1;
			}
		}else if (pResBuffer[i]==_T('/'))
		{
			if(isFlag == FALSE ){ 
				pDesBuffer[j++]=_T('\\');
				isFlag = TRUE;
			}
		}else if(pResBuffer[i]==_T('\\')){
			if(isFlag == FALSE ){ 
				pDesBuffer[j++]=_T('\\');
				isFlag = TRUE;
			}
		}else{
			pDesBuffer[j++]=pResBuffer[i];
			isFlag = FALSE;
		}
	}
	pDesBuffer[j]=0;
	return pDesBuffer;
}
LPCTSTR CLString::_GetLastStrByPath2(LPCTSTR strPath)   //get file name
{ 	
	LPCTSTR p1 = _tcschr(strPath,_T('\\'));
	LPCTSTR p2 = _tcschr(strPath,_T('/'));
	LPCTSTR m_ret=strPath;
	LPCTSTR p = p1 > p2 ? p1 : p2;
	while (p)
	{
		p+=1;
		m_ret=p;
		p1=_tcschr(p,_T('\\'));
		p2 = _tcschr(p,_T('/'));
		p = p1 > p2 ? p1 : p2;

	}
	return m_ret;
}
LPCTSTR CLString::_GetLastStrByPath(LPCTSTR strPath)   //get file name
{
	assert(strPath!=NULL);
	LPCTSTR p = strPath;
	while (*p)
		p++;//to the end
	while (--p != strPath) {
		if (*p == _T('\\') || *p == _T('/')) {
			return p + 1;
		}
	}
	return p;
}
LPTSTR CLString::_DeleteLastStrFromPath(LPTSTR strPath)   //get file name
{ 	
	auto pc = _GetLastStrByPath(strPath);
	LPTSTR pTag = LPTSTR(pc);
	pTag[0] = 0;
	return strPath;
}
BOOL CLString::_FindFirstFileExists(LPCTSTR lpPath,DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath,&fd);
	BOOL bFilter = (BOOL)((FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter);
	BOOL RetValue = (BOOL)(((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE);
	FindClose(hFind);
	return RetValue;
}
BOOL CLString::_RegSZ(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName,LPTSTR & pString,BOOL isSet,LONG_PTR storeSize)
{
	BOOL m_ret=FALSE;
	HKEY hKey=NULL;
	LSTATUS pBOOL=0;
	DWORD m_access;
	if(isSet)m_access=KEY_ALL_ACCESS;
	else m_access=KEY_QUERY_VALUE;
	DWORD dwDisposition=REG_CREATED_NEW_KEY;
	try{
		pBOOL=RegCreateKeyEx(mainKey,(LPTSTR)mainPass,NULL,(LPTSTR)optionName,REG_OPTION_NON_VOLATILE,m_access,NULL,&hKey,&dwDisposition);
		if(ERROR_SUCCESS==pBOOL)
		{	
			if (isSet)
			{
				pBOOL=RegSetValueEx(hKey,(LPTSTR)optionName,NULL,REG_SZ,(const byte*)pString,(DWORD)sizeof(TCHAR)*((::_tcsclen(pString))+1));
				if (ERROR_SUCCESS==pBOOL)
				{
					m_ret=TRUE;
				}
			}
			else
			{
				DWORD inType=(DWORD)REG_SZ;
				LONG_PTR buffsize=sizeof(TCHAR)*storeSize;	
				pBOOL=RegQueryValueEx(hKey,(LPTSTR)optionName,NULL,&inType,(LPBYTE)pString,(LPDWORD)&buffsize);
				if (ERROR_SUCCESS==pBOOL)
				{
					m_ret=TRUE;
				}
			}	
			RegCloseKey(hKey);
		}}
	catch(...)
	{}
	return m_ret;
}
LPTSTR CLString::_newbuffer(LONG_PTR nNeedSize,LONG_PTR nPosIndex)
{
	LPTSTR p = NULL;
	if(nNeedSize <= 0)
		return NULL;
	if(!(makeDEx()->m_vtStringStoreBufSizeLst_))
		(makeDEx()->m_vtStringStoreBufSizeLst_) =new std::vector<LONG_PTR>;
	if(!(makeDEx()->m_vtStringStoreLst_))
		(makeDEx()->m_vtStringStoreLst_) = new std::vector<LPTSTR>;
	if((LONG_PTR)(makeDEx()->m_vtStringStoreLst_)->size() <= nPosIndex){//超出范围
		p = NULL;
		for (LONG_PTR i = (makeDEx()->m_vtStringStoreLst_)->size(); i<= nPosIndex ;i++)
		{
			(makeDEx()->m_vtStringStoreLst_)->push_back(p = (new TCHAR[nNeedSize]));
			(makeDEx()->m_vtStringStoreBufSizeLst_)->push_back(nNeedSize);
		}
		return p;
	}
	else { //范围内
		if((makeDEx()->m_vtStringStoreBufSizeLst_)->at(nPosIndex) >= nNeedSize)
			return (makeDEx()->m_vtStringStoreLst_)->at(nPosIndex);
		else {
			delete[] (makeDEx()->m_vtStringStoreLst_)->at(nPosIndex);
			(makeDEx()->m_vtStringStoreBufSizeLst_)->at(nPosIndex) = nNeedSize;
			return (makeDEx()->m_vtStringStoreLst_)->at(nPosIndex) = new TCHAR[nNeedSize];			
		}
	}
}

CLString::CLString()
{
	initialize();
}
CLString::CLString(LPCSTR pString)
{
	initialize( pString ? std::strlen(pString) : 0);
	set(pString);
}
CLString::CLString(LPCWSTR pString)
{
	initialize( pString ? std::wcslen(pString) : 0);
	set(pString);
}
CLString::CLString(const CLString& mString)
{
	initialize(std::_tcslen(mString.string()));
	set(mString.string());
}
CLString::CLString(LONG_PTR nDefaultCharNumber)
{
	initialize(nDefaultCharNumber);
}

CLString::CLString(LPCTSTR pString1,LPCTSTR pString2,...)
{
	LPCTSTR p = (pString1? &pString1[0]:0);
	LONG_PTR n = 0;
	LONG_PTR nSize = p ? std::_tcslen(p) : 0;
	va_list ap;
	va_start(ap,pString2);
	for (p = pString2; p != NULL ; ) {
		++n;
		nSize += std::_tcslen(p);
		p = va_arg(ap, LPCTSTR);
	}
	va_end(ap);
	initialize(nSize);
	set(pString1);
	LONG_PTR i = 0;
	va_start(ap,pString2);
	for (p = pString2; i < n;) {
		append(p); 
		p = va_arg(ap, LPCTSTR); 
		i++;
	}
	va_end(ap);
}
CLString::~CLString()
{
#ifdef _DEBUG	
	if ($clstring_open_destroyflag)
	{
		::MessageBox(NULL,string(),_T("析构 ~CLString"),MB_OK);
	}
#endif
	cleanAllMemory();
}

CLString& CLString::append(LPCSTR pString)
{
	if ((!pString)||((*pString)==0))
	   return (*this);
	if(m_encoding == EnCode_ASCII){
    LONG_PTR _targetLen = std::strlen(pString);
    if ((!( pString + _targetLen + 1 < (LPCSTR)pHead || pString > (LPCSTR)pHead + m_unitNumber ) ) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCSTR>(getCLString0()))))
        {
            ::MessageBox(NULL, _T("字符串指针范围重叠！"), _T("CLString对象错误"), MB_ICONERROR);
            throw std::logic_error("字符串指针范围重叠！");
        }
    if (_targetLen + strlen() + 1 <= m_unitNumber)
    {
       strcpy_s((LPSTR)pHead + strlen(), _targetLen + 1, pString);
    }
    else {
       LPSTR pHeade = new CHAR[ m_unitNumber = strlen() + _targetLen + 1];            
       strcpy_s(pHeade, strlen() + 1, (LPCSTR)pHead);
       strcpy_s(pHeade + strlen(), _targetLen + 1, pString);
       if(string() != getCLString0())delete[] pHead;
       pHead = (LPTSTR)pHeade;
    }
	}else{
		LPWSTR pTag;
		LONG_PTR _targetLen;
		if(!swapEncode((const LPBYTE)pString,std::strlen(pString),EnCode_ASCII,(LPBYTE&)pTag,_targetLen,EnCode_ULE))
			return *this;
		_targetLen = _targetLen/sizeof(WCHAR) -1;
		if (_targetLen + strlen() + 1 <= m_unitNumber)
		{
			wcscpy_s((LPWSTR)pHead + strlen(), _targetLen + 1, pTag);
		}
		else {
			LPWSTR pHeade = new WCHAR[ m_unitNumber = strlen() + _targetLen + 1];            
			wcscpy_s(pHeade, strlen() + 1, (LPCWSTR)pHead);
			wcscpy_s(pHeade + strlen(), _targetLen + 1, pTag);
			if(string() != getCLString0())delete[] pHead;
			pHead = (LPTSTR)pHeade;
		}
	}
	m_changeFlag = TRUE;
	return (*this);
}
CLString& CLString::append(LPCWSTR pString)
{
	if ((!pString)||((*pString)==0))
		return (*this);
	if(m_encoding != EnCode_ASCII){
		LONG_PTR _targetLen = std::wcslen(pString);
		if ((!( pString + _targetLen + 1 < (LPCWSTR)pHead || pString > (LPCWSTR)pHead + m_unitNumber ) ) && (!(pHead == getCLString0() || pString == reinterpret_cast<LPCWSTR>(getCLString0()))))
		{
			::MessageBox(NULL, _T("字符串指针范围重叠！"), _T("CLString对象错误"), MB_ICONERROR);
			throw std::logic_error("字符串指针范围重叠！");
		}
		if (_targetLen + strlen() + 1 <= m_unitNumber)
		{
			wcscpy_s((LPWSTR)pHead + strlen(), _targetLen + 1, pString);
		}
		else {
			LPWSTR pHeade = new WCHAR[ m_unitNumber = strlen() + _targetLen + 1];            
			wcscpy_s(pHeade, strlen() + 1, (LPCWSTR)pHead);
			wcscpy_s(pHeade + strlen(), _targetLen + 1, pString);
			if(string() != getCLString0())delete[] pHead;
			pHead = (LPTSTR)pHeade;
		}
	}
	else{
		LPSTR pTag;
		LONG_PTR _targetLen;
		if(!swapEncode((const LPBYTE)pString,(std::wcslen(pString))*sizeof(WCHAR),EnCode_ULE,(LPBYTE&)pTag,_targetLen,EnCode_ASCII))
			return *this;
		_targetLen = _targetLen/sizeof(CHAR) -1;
		if (_targetLen + strlen() + 1 <= m_unitNumber)
		{
			strcpy_s((LPSTR)pHead + strlen(), _targetLen + 1, pTag);
		}
		else {
			LPSTR pHeade = new CHAR[ m_unitNumber = strlen() + _targetLen + 1];            
			strcpy_s(pHeade, strlen() + 1, (LPCSTR)pHead);
			strcpy_s(pHeade + strlen(), _targetLen + 1, pTag);
			if(string() != getCLString0())delete[] pHead;
			pHead = (LPTSTR)pHeade;
		}
	}
	m_changeFlag = TRUE;
	return (*this);
}
CLString& CLString::append(CHAR ch)
{
	CHAR chr[2] = {ch,0};
	return append(chr);
}
CLString& CLString::append(WCHAR ch)
{
	WCHAR chr[2] = {ch,0};
	return append(chr);
}
CLString& CLString::appendl(LONG_PTR v,BOOL *_out_bIsSuccess /*= 0*/)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return CLString::ltos(v,str) ? 
		(_out_bIsSuccess ? (*_out_bIsSuccess = TRUE):FALSE,append(str)) : 
		(_out_bIsSuccess ? (*_out_bIsSuccess = FALSE):TRUE,*this);
}
CLString& CLString::appendf(double v,size_t maxAccuracy /*= 15*/,BOOL *_out_bIsSuccess /*= 0*/)
{
	TCHAR str[CLSTRING_FTOSLIMIT];
	return CLString::ftos(v,str, CLSTRING_FTOSLIMIT,maxAccuracy) ?
		(_out_bIsSuccess ? (*_out_bIsSuccess = TRUE):FALSE,append(str)) : 
		(_out_bIsSuccess ? (*_out_bIsSuccess = FALSE):TRUE,*this);
}

CLString& CLString::operator<<(LPCSTR pString)
{
	return append(pString);
}
CLString& CLString::operator<<(LPCWSTR pString)
{
	return append(pString);
}
CLString& CLString::operator<<(const CLString& mString)
{
	return append(mString.string());
}
CLString& CLString::operator<<(const CLString* pString)
{
	return append(pString ? pString->string() : (LPCTSTR)NULL);
}
CLString& CLString::operator<<(const CHAR ch)
{
	return append(ch);
}
CLString& CLString::operator<<(const byte number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append(CLString::ltos(number, str) ? str : CLString(6).format(6, _T("%d"), int(number)).string());
}
CLString& CLString::operator<<(const WCHAR ch)
{
	return append(ch);
}
CLString& CLString::operator<<(const long long number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(23).format(23,_T("%lld"),number).string());
}
CLString& CLString::operator<<(const long double number)
{
	TCHAR str[CLSTRING_FTOSLIMIT];
	return append( CLString::ftos(number,str) ? str : CLString(46).format(46,_T("%g"),number).string());
}
CLString& CLString::operator<<(const double number)
{
	TCHAR str[CLSTRING_FTOSLIMIT];
	return append( CLString::ftos(number,str) ? str : CLString(46).format(46,_T("%g"),number).string());
}
CLString& CLString::operator<<(const float number)
{
	TCHAR str[CLSTRING_FTOSLIMIT];
	return append( CLString::ftos(number,str, CLSTRING_FTOSLIMIT,6) ? str : CLString(23).format(23,_T("%g"),number).string());
}
CLString& CLString::operator<<(const unsigned short number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(6).format(6,_T("%d"),number).string());
}
CLString& CLString::operator<<(const short number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(6).format(6,_T("%d"),number).string());
}
CLString& CLString::operator<<(const unsigned int number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(12).format(12,_T("%d"),number).string());
}
CLString& CLString::operator<<(const int number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(12).format(12,_T("%d"),number).string());
}
CLString& CLString::operator<<(const unsigned long number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(12).format(12,_T("%d"),number).string());
}
CLString& CLString::operator<<(const long number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(12).format(12,_T("%d"),number).string());
}
CLString& CLString::operator<<(const unsigned long long number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append( CLString::ltos(number,str) ? str : CLString(23).format(23,_T("%lld"),number).string());
}
CLString& CLString::operator<<(const bool number)
{
	TCHAR str[CLSTRING_LTOSLIMIT];
	return append(CLString::ltos((number ? 1 : 0), str) ? str : CLString(23).format(23, _T("%d"), (number ? 1:0)).string());
}

CLString& CLString::format(LONG_PTR maxStrlen,LPCTSTR szFormat,...)
{
	if(!szFormat || *szFormat == 0)
	{
		return empty();
	}
	LONG_PTR _szFormatlen = std::_tcslen(szFormat) + 1;
	if( szFormat + _szFormatlen < pHead  ||	szFormat > pHead + m_unitNumber )//指针重叠判断
	{
		va_list pArgs;
		va_start(pArgs,szFormat);
		_vstprintf_s(store(maxStrlen),maxStrlen + 1,szFormat,pArgs);
		va_end(pArgs);
	}else{
		LPTSTR tmp = new TCHAR[maxStrlen + 1];
		va_list pArgs;
		va_start(pArgs,szFormat);
		_vstprintf_s(tmp,maxStrlen + 1,szFormat,pArgs);
		va_end(pArgs);
		set(tmp);
		delete[] tmp;
	}
	return (*this);
}	
CLString& CLString::format(LPCTSTR szFormat,...)
{
	if(!szFormat || *szFormat == 0)
	{
		return empty();
	}
	LONG_PTR _szFormatlen = std::_tcslen(szFormat) + 1;
	
	if( szFormat + _szFormatlen < pHead  ||	szFormat > pHead + m_unitNumber )//指针重叠判断
	{
		va_list pArgs;
		va_start(pArgs,szFormat);
		LONG_PTR maxStrlen =  
#ifdef UNICODE
			 _vscwprintf( szFormat, pArgs );
#else
			 _vscprintf( szFormat, pArgs );
#endif		
		if(maxStrlen <= 0){va_end(pArgs);return empty();}
		_vstprintf_s(store(maxStrlen),maxStrlen + 1,szFormat,pArgs);
		va_end(pArgs);
	}else{
		va_list pArgs;
		va_start(pArgs,szFormat);
		LONG_PTR maxStrlen =  
#ifdef UNICODE
			_vscwprintf( szFormat, pArgs );
#else
			_vscprintf( szFormat, pArgs );
#endif		
		if(maxStrlen <= 0){va_end(pArgs);return empty();}
		LPTSTR tmp = new TCHAR[maxStrlen + 1];		
		_vstprintf_s(tmp,maxStrlen + 1,szFormat,pArgs);
		va_end(pArgs);
		set(tmp);
		delete[] tmp;
	}
	return (*this);
}	

BOOL CLString::ltos(LONG_PTR v,LPTSTR lpBuf,size_t nBufSizeInChar){
	if (!lpBuf || nBufSizeInChar < CLSTRING_LTOSLIMIT) {
		CLString().format(_T("CLString::ltos bufSize = %d < CLSTRING_LTOSLIMIT (%d)"), (int)nBufSizeInChar, (int)CLSTRING_LTOSLIMIT)
			.messageBoxRef(_T("CLString Alert"), MB_ICONERROR).throw_runtime_error();
		return FALSE;
	}

	LONGLONG ts = abs(v);
	size_t wei = 1;
	while ((ts /= 10) > 0) {
		wei++;
		if (wei > CLSTRING_FTOSLIMIT_MAXACCURACY) {
			_stprintf_s(lpBuf, nBufSizeInChar, _T("%lld"), LONGLONG(v));
			return TRUE;
		}
	}

	BOOL isUp = v >= 0 ? TRUE : FALSE;
	LONG_PTR n = ((LONG_PTR)(isUp ? v : (-1)*v));
	int ni = 0;
	LPTSTR pc = lpBuf;
	size_t uesd = 1;
	if(n > 0)//填入整数部分
	{
		do{
			if( (++uesd > nBufSizeInChar) ||  uesd > 18)
				return FALSE;
			ni ++;
			*pc++ = (TCHAR)(n%10 + 48);*pc = 0;
			n /= 10;
		}while( n > 0 );		
	}else {
		if( ++uesd > nBufSizeInChar)
			return FALSE;
		ni++;
		*pc++ = _T('0');*pc = 0;		
	}
	if(isUp == FALSE){//整数部分增加负号
		if( ++uesd > nBufSizeInChar)
			return FALSE;
		ni++;
		*pc++ = _T('-');*pc = 0;
	}
	if(ni >= 2) //整数部分反向
	{
		TCHAR t;
		int mi = ni/2,in;
		for (int i = 0 ; i < mi ; i++)
		{
			t = lpBuf[i];
			lpBuf[i] = lpBuf[ in = (ni - 1 - i) ];
			lpBuf[ in ] = t;
		}
	}
	return TRUE;
}
CLString& CLString::ltos(LONG_PTR v,BOOL* _out_bIsSuccess)
{
	CLString::ltos(v,this->store(CLSTRING_LTOSLIMIT),CLSTRING_LTOSLIMIT) ?
		( _out_bIsSuccess ? *_out_bIsSuccess=TRUE:TRUE) : 
		(this->empty(),( _out_bIsSuccess ? *_out_bIsSuccess=FALSE:FALSE));
	return *this;
}

BOOL CLString::ftos(double sv,LPTSTR buf,size_t bufSize,size_t maxAccuracy)
{
	if (!buf || bufSize < CLSTRING_FTOSLIMIT) {
		CLString().format(_T("CLString::ftos bufSize = %d < CLSTRING_FTOSLIMIT (%d)"), (int)bufSize, (int)CLSTRING_FTOSLIMIT)
			.messageBoxRef(_T("CLString Alert"),MB_ICONERROR).throw_runtime_error();
		return FALSE;
	}
	double ts = sv;
	size_t wei = 0;
	while (abs(ts) > 1.0) {
		wei++;
		ts *= 0.1;
		if (wei > CLSTRING_FTOSLIMIT_MAXACCURACY) {
			_stprintf_s(buf, bufSize, _T("%.0f"), sv);
			return TRUE;
		}
	}
norm:
	size_t acc = min(maxAccuracy, CLSTRING_FTOSLIMIT_MAXACCURACY);
	auto ret = buf;
	TCHAR szZs[30], * pzs = &szZs[29];
	TCHAR szXs[30], * pxs = szXs;;
	bool isUp = sv < 0 ? false : true;
	auto stt = sv * (-1.0);
	ULONGLONG zs = isUp ? ULONGLONG(sv) : ULONGLONG(sv * (-1.0));
	double xs = isUp ? sv - double(zs) : (sv + double(zs)) * (-1.0);
	//size_t zsw = 0;
	while (zs > 0 && pzs > szZs) {
		*pzs = TCHAR(zs % 10 + 48);
		//zsw++;
		zs /= 10;
		pzs--;
	}
	if (pzs == szZs + 29)*pzs-- = _T('0');
	if (!isUp)*pzs-- = _T('-');
	while (++pzs <= szZs + 29) {
		*buf++ = *pzs;
	}
	*buf = 0;

	if (acc > 0) {
		size_t xsw = 0;
		while (xs > 1e-15 /*&& pxs < szXs + 20*/ && (++xsw + wei) < 20 && xsw <= acc) {
			xs *= 10.0;
			auto c = ULONGLONG(xs);
			*pxs++ = TCHAR(c) + 48;
			xs -= double(c);
		}
		*pxs = 0;
		if (pxs > szXs) {
			*buf++ = _T('.');
			auto pc = szXs;
			while (pc < pxs)
				*buf++ = *pc++;
			*buf = 0;
			while (--buf >= ret && (*buf == _T('0') || *buf == _T('.')))
				*buf = 0;
		}
	}
	return TRUE;

	//下列代码有效，但精度不够暂时弃用
	//if(!lpBuf || nBufSizeInChar < CLSTRING_FTOSLIMIT)
	//	return FALSE;
	//if(maxAccuracy > CLSTRING_FTOSLIMIT_MAXACCURACY) maxAccuracy = CLSTRING_FTOSLIMIT_MAXACCURACY;
	//BOOL isUp = v >= 0 ? TRUE : FALSE;
	//unsigned long long n = ((unsigned long long)(isUp ? v : (-1.0)*v));
	//double _t = ((double)n);
	//double _w = isUp ? (v - _t) : (v + _t)*(-1.0);
	//if (_w >= 1.0)
	//	_w = 0;
	//int ni = 0;
	//LPTSTR pc = lpBuf;
	//size_t uesd = 1;
	//if(n > 0)//填入整数部分
	//{
	//	do{
	//		if( ++uesd > nBufSizeInChar ||  uesd > CLSTRING_FTOSLIMIT-2)
	//			return FALSE;
	//		ni ++;
	//		*pc++ = (TCHAR)(n%10 + 48);*pc = 0;
	//		n /= 10;			
	//	}while( n > 0 );		
	//}else {
	//	if( ++uesd > nBufSizeInChar)
	//		return FALSE;
	//	ni++;
	//	*pc++ = _T('0');*pc = 0;		
	//}
	//if(isUp == FALSE){//整数部分增加负号
	//	if( ++uesd > nBufSizeInChar)
	//		return FALSE;
	//	ni++;
	//	*pc++ = _T('-');*pc = 0;
	//}
	//if(ni >= 2) //整数部分反向
	//{
	//	TCHAR t;
	//	int mi = ni/2,in;
	//	for (int i = 0 ; i < mi ; i++)
	//	{
	//		t = lpBuf[i];
	//		lpBuf[i] = lpBuf[ in = (ni - 1 - i) ];
	//		lpBuf[ in ] = t;
	//	}
	//}
	//if(_w > 1e-15 && maxAccuracy >= 0 ){//加入小数部分
	//	if( ++uesd > nBufSizeInChar)
	//		return TRUE;
	//	*pc++ = _T('.');*pc = 0;
	//	ULONGLONG i,c;
	//	for (i = 0; i< (ULONGLONG)maxAccuracy ;)
	//	{
	//		_w*=10.0;
	//		c = (((ULONGLONG)_w)%10);
	//		_w -= (double)c;
	//		if( ++uesd > nBufSizeInChar)
	//			return TRUE;			
	//		*pc++ = (TCHAR)(c+48);
	//		*pc = 0; //
	//		i++;
	//		if (_w < 1e-15)
	//			break;
	//		//if(uesd == CLSTRING_LTOSLIMIT)
	//		//	break;
	//	}
	//	

	//	_w*=10.0;
	//	c = ((ULONGLONG)_w)%10;
	//	_w -= (double)c;

	//	if( c >= 5){//处理四舍五入
	//		TCHAR* pCend=pc,*pCfirst=0;//保存末尾位置指针
	//		BOOL isDot = TRUE;//isNeedMove = FALSE;
	//		while( (--pc) >= lpBuf ){
	//			if(*pc == _T('9'))
	//			{ 
	//				if(isDot){
	//					(*pc) = 0;
	//					pCend = pc;
	//				}
	//				else{
	//					(*pc) = _T('0');
	//					if(pc == lpBuf)
	//						{//isNeedMove = TRUE;
	//							pCfirst=pc;}
	//				}
	//			}
	//			else if(*pc == _T('.')){
	//				if(isDot)
	//				{
	//					isDot=FALSE;
	//					(*pc) = 0;
	//					pCend = pc;
	//				}
	//				continue;
	//			}
	//			else{
	//				if( *pc >= _T('0') && *pc <= _T('8'))
	//				{
	//					*pc = *pc+1;
	//				}
	//				else if(*pc == _T('-')){
	//					//isNeedMove = TRUE;
	//					pCfirst = pc + 1;
	//				}
	//				break;
	//			}
	//		}
	//		if(pCfirst){//处理最高位超过的向后一位移动
	//			*(pCend+1)=0;
	//			while(pCend > pCfirst){
	//				*pCend = *(pCend - 1);
	//				pCend--;
	//			}
	//			*pCfirst = _T('1');
	//		}
	//	}else{
	//		while( ((--pc) >= lpBuf) && ((*pc) == _T('0') || (*pc) == _T('.'))){
	//			(*pc) = 0;
	//		}
	//	}
	//}
	//return TRUE;
}

CLString& CLString::ftos(double v,size_t maxAccuracy,BOOL* _out_bIsSuccess)
{
	CLString::ftos(v,this->store(CLSTRING_FTOSLIMIT), CLSTRING_FTOSLIMIT,maxAccuracy) ?
	(_out_bIsSuccess ? *_out_bIsSuccess=TRUE:TRUE) : 
	(this->empty(),(_out_bIsSuccess ? *_out_bIsSuccess=FALSE:FALSE));
	return *this;
}

double CLString::stof(LPCTSTR lpBuf)
{
	if(!lpBuf)return 0;
	double s= 0.0;  

	double d = 0.1;  	

	bool falg=false;  
	LPCTSTR str = lpBuf;

	while( *str==32 || *str==9 || *str==10 || *str==13 )  
	{  
		str++;  
	}  

	if(*str== _T('-'))//记录数字正负  
	{  
		falg=true;  
		str++;  
	}else if(*str== _T('+')){
		falg=false;  
		str++; 
	} 

	if(!(( *str>= _T('0') && *str<= _T('9')) || (*str== _T('.'))))//如果一开始非数字则退出，返回0.0  
		return 0;  
	while(*str== _T('0'))str++;
	int used = 0;
	while(*str>= _T('0') && *str<= _T('9') && *str!= _T('.') )//计算小数点前整数部分  
	{  
		s= s*10.0 + (double)(*str - _T('0'));  
		str++;  
		if( ++used > CLSTRING_FTOSLIMIT ) return 0;
	}  

	if(*str== _T('.'))//以后为小数部分  
		str++;  
	int wused = 0;
	double _w = 0;
	while(*str>=_T('0')&&*str<=_T('9'))//计算小数部分  
	{  
		_w += ((double)(*str-_T('0')))*d;  
		d*=0.1;  
		str++;  
		if( ++wused >= CLSTRING_FTOSLIMIT_MAXACCURACY || ++used >= CLSTRING_FTOSLIMIT)
			break;
	}  
	if(_w != 0)
	{
		_w += 5e-16;
		_w *= 1e15;
		_w = (double)(LONG_PTR)_w;
		_w *= 1e-15;
		s += _w;
	}

	if(*str==_T('e')||*str==_T('E'))//考虑科学计数法  
	{  
		str++;  
		int jishu=0;  
		if(*str== _T('+'))  
		{  
			str++;  
			while(*str>=_T('0')&&*str<=_T('9'))  
			{  
				jishu=jishu*10+*str-_T('0');  
				str++;  
			}  
			while(jishu>0)  
			{  
				s*=10;  
				jishu--;  
			}  
		}  
		if(*str==_T('-'))  
		{  
			str++;  
			while(*str>=_T('0')&&*str<=_T('9'))  
			{  
				jishu=jishu*10+*str-_T('0');  
				str++;  
			}  
			while(jishu>0)  
			{  
				s/=10;  
				jishu--;  
			}  
		}  
	}  

	return (falg ? -1.0*s : s); 
}
double CLString::stof() const
{
	return CLString::stof(this->string());
}
long long CLString::stoll(LPCTSTR lpBuf)
{
	if(!lpBuf)return 0;
	long long s= 0;

	bool falg=false;  
	LPCTSTR str = lpBuf;

	while( *str==32 || *str==9 || *str==10 || *str==13 )  
	{  
		str++;  
	}  

	if(*str== _T('-'))//记录数字正负  
	{  
		falg=true;  
		str++;  
	}else if(*str== _T('+')){
		falg=false;  
		str++; 
	} 

	if(!(( *str>= _T('0') && *str<= _T('9')) ))//如果一开始非数字则退出，返回0.0  
		return 0;  
	while(*str== _T('0'))str++;
	int used = 0;
	while(*str>= _T('0') && *str<= _T('9'))//计算小数点前整数部分  
	{  
		s= s*10 + (long long)(*str - _T('0'));
		str++;  
		if( ++used > 18 ) return 0;
	} 
	if(*str==_T('e')||*str==_T('E'))//考虑科学计数法  
	{  
		str++;  
		int jishu = 0;
		if(*str== _T('+'))  
		{  
			str++;  
			while(*str>=_T('0')&&*str<=_T('9'))  
			{  
				jishu=jishu*10+*str-_T('0');  
				str++;  
			}  
			while(jishu>0)  
			{  
				s*=10;  
				jishu--;  
			}  
		}  
		if(*str==_T('-'))  
		{  
			str++;  
			while(*str>=_T('0')&&*str<=_T('9'))  
			{  
				jishu=jishu*10+*str-_T('0');  
				str++;  
			}  
			while(jishu>0)  
			{  
				s/=10;  
				jishu--;  
			}  
		}  
	}  

	return (falg? (-1)*s:s); 
}
int CLString::stoi()const
{
	return _ttoi(this->string());
}
long CLString::stol() const
{
	return _ttol(this->string());
}
long long CLString::stoll() const
{
	return CLString::stoll(this->string());
}

LPTSTR CLString::store(LONG_PTR iStoreMaxCharLen)
{
	assert(string() != NULL);
	if(iStoreMaxCharLen < 0)iStoreMaxCharLen = 0;
	m_changeFlag=TRUE;
	if( iStoreMaxCharLen+1 > m_unitNumber)
	{
		if(string() != getCLString0())delete[] pHead;
		pHead = new TCHAR[ m_unitNumber = iStoreMaxCharLen + 1 ];
	}
	ZeroMemory(pHead,sizeof(TCHAR)*(iStoreMaxCharLen+1));
	m_strlen = 0;
	return pHead;
}
LONG_PTR CLString::buflen(void)
{
	return m_unitNumber*sizeof(TCHAR);
} 
LONG_PTR CLString::strlen(void)
{
	if (m_changeFlag)
	{
		assert(string() != NULL);
		m_strlen = std::_tcslen(string());
		m_changeFlag = FALSE;
	}
	return m_strlen;
} 
LONG_PTR CLString::strlenInByte(void)
{
	return strlen()*sizeof(TCHAR);
} 
void CLString::showContent()
{
	::MessageBox(NULL,string(),_T("CLString content:"),MB_OK);
}
int CLString::getCharCmd(LPTSTR lpBuffer,LONG_PTR nBufferSizeInChar,LPCTSTR lpPreString /*= NULL*/)
{
	assert( lpBuffer != NULL );
	ZeroMemory(lpBuffer,nBufferSizeInChar*sizeof(TCHAR));
	fflush(stdin);
	system("cls");
	std::_tprintf(_T("%s"),lpPreString ? lpPreString : _T(""));
	int ip = 0;
	while(1){
		if (_kbhit())
		{
			TCHAR a = _getch();
			if(a == 0x1b){ 
				fflush(stdin);
				return 0;
			}else if(a == 0x0d){
				fflush(stdin);				
				return 1;
			}
			else if(a == 0x08 && ip > 0){			
				lpBuffer[--ip] = 0;				
				system("cls");
				std::_tprintf(_T("%s%s"),lpPreString ? lpPreString : _T("") , lpBuffer);
			}
			else if( a >= 0x20 && a <= 0x7e && ip < nBufferSizeInChar - 1){
				lpBuffer[ip++] = a;
				std::_tprintf(_T("%c"),a);
			}
			fflush(stdin);
		}
	}
	return 0;
}

CLString& CLString::addAnPathEnd(INT endNumber)
{	
	LONG_PTR N = (strlen()+1)*2;
	if(N <= m_unitNumber)
		N = m_unitNumber;
	LPTSTR tmp=new TCHAR[N];
	//ZeroMemory(tmp,sizeof(TCHAR)*N);
	_tcscpy_s(tmp,N,string());
	if(_AddPathAnEnt(tmp,endNumber)){
		if(string() != getCLString0())delete[] pHead;
		pHead = tmp;
		m_unitNumber = N;
		m_changeFlag = TRUE;
	}else{
		delete[] tmp;
	}		
	return *this;
}
CLString& CLString::extendPathToQuality()
{
	LONG_PTR N = (strlen()+1)*2;
	if(N <= m_unitNumber)
		N = m_unitNumber;
	LPTSTR tmp=new TCHAR[N];
	ZeroMemory(tmp,sizeof(TCHAR)*N);
	_ExtendPathToQuality(string(),tmp);
	if(string() != getCLString0())delete[] pHead;
	pHead = tmp;
	m_unitNumber = N;
	m_changeFlag = TRUE;
	return *this;
}
CLString& CLString::extendPathToNormal()
{
	LPTSTR tmp;	
	if(strlen()+1 > MAX_PATH)
	{	
		tmp=new TCHAR[m_unitNumber];
		ZeroMemory(tmp,sizeof(TCHAR)*m_unitNumber);
		_ExtendPathToNormal(pHead,tmp);
		if(string() != getCLString0())delete[] pHead;
		pHead = tmp;
		m_changeFlag = TRUE;
	}
	else{
		TCHAR tmpbuf[MAX_PATH] = {0};
		tmp = tmpbuf;
		_ExtendPathToNormal(pHead,tmp);
		set(tmp);
	}	
	return *this;
}
LPCTSTR CLString::getLastStrByPath()
{
	TCHAR tmp[MAX_PATH] = {0};
	_tcscpy_s(tmp,MAX_PATH,_GetLastStrByPath(pHead));
	set(tmp);
	return string();
}
CLString& CLString::deleteLastStrFromPath()
{	
	extendPathToNormal();
	if(!_tcschr(pHead,_T('\\')))
		return *this;
	_DeleteLastStrFromPath(pHead);
	m_changeFlag = TRUE;
	return *this;
}
CLString& CLString::replaceExtName(LPCTSTR newExtName)
{
	//auto idot = reverseFind(_T('.'));
	//auto idot1 = reverseFind(_T('\\'));
	//auto idot2 = reverseFind(_T('/'));
	long long ci = size() - 1;
	long long nsi = 0;
	while (ci >= 0 )
	{
		TCHAR ch = pHead[ci];
		if (ch == _T('.')) {
			if (ci <= 0)
				return *this;
			ch = pHead[ci - 1];
			if (!(ch != _T('\\') && ch != _T('/') && ch != _T('.') && ch != _T(':')
				&& ch != _T('*') && ch != _T('\"') && ch != _T('?') && ch != _T('|') 
				&& ch != _T('<') && ch != _T('>') ))
				return *this;
			if (!(ch >= 33 && ch <= 126))
				return *this;
			return leftSave(ci).append(newExtName);
		}
		else if (ch == _T('\\') || ch == _T('/'))
		{
			if (nsi == 0)
				return *this;
			ch = pHead[ci + 1];
			if (!(ch != _T('\\') && ch != _T('/') && ch != _T('.') && ch != _T(':')
				&& ch != _T('*') && ch != _T('\"') && ch != _T('?') && ch != _T('|')
				&& ch != _T('<') && ch != _T('>')))
				return *this;
			if (!(ch >= 33 && ch <= 126))
				return *this;
			return trim().append(newExtName);
		}
		ci--;
		nsi++;
	}
	return trim().append(newExtName);
}
BOOL CLString::setReg(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName)
{
	return _RegSZ(mainKey,mainPass,optionName,pHead,TRUE,0);
}
BOOL CLString::getReg(HKEY mainKey,LPCTSTR mainPass,LPCTSTR optionName,LONG_PTR stroeSize)
{
	if(m_unitNumber < stroeSize ){
		if(string() != getCLString0())delete[] pHead;
		pHead = new TCHAR[ m_unitNumber = stroeSize ];
	}
	//ZeroMemory(pHead,sizeof(TCHAR)*m_unitNumber);
	if(string() && string() != getCLString0())*pHead = 0;
	m_changeFlag = TRUE;
	if(_RegSZ(mainKey,mainPass,optionName,pHead,FALSE,stroeSize))
	{
		return TRUE;
	}
	else {	
		//ZeroMemory(pHead,sizeof(TCHAR)*m_unitNumber);
		if(string() && string() != getCLString0())*pHead = 0;
		return FALSE;
	}
}
CLString& CLString::getSpecialFolderPath(INT nFolder,INT storeLen,HWND hwndOwner,BOOL fCreate)
{
	if(m_unitNumber < storeLen ){
		if(string() != getCLString0())delete[] pHead;
		pHead = new TCHAR[ m_unitNumber = storeLen ];
	}
	//ZeroMemory(pHead,sizeof(TCHAR)*m_unitNumber);
	m_changeFlag = TRUE;
	if(!SHGetSpecialFolderPath(hwndOwner,pHead,nFolder,fCreate))	
		//ZeroMemory(pHead,sizeof(TCHAR)*m_unitNumber);	
		if(string() && string() != getCLString0())*pHead = 0;
	return *this;
}
BOOL CLString::fileExists(void)// 检查一个文件是否存在（绝对路径、相对路径，文件或文件夹均可）
{
	return CLString::filePathExists();
}
BOOL CLString::filePathExists(void)// 检查一个路径是否存在（绝对路径、相对路径，文件或文件夹均可）
{
	return CLString::_FindFirstFileExists(string(), FALSE);
}
BOOL CLString::folderExists(void)// 检查一个文件夹是否存在（绝对路径、相对路径均可）
{
	return CLString::_FindFirstFileExists(string(), FILE_ATTRIBUTE_DIRECTORY);
}
BOOL CLString::findFirstFileExists(DWORD dwFilter)
{
	return CLString::_FindFirstFileExists(string(),dwFilter);
}

double CLString::getUNegNumByPos(INT numPos)
{
	return getUNegNumByPos(string(),numPos);
}
double CLString::getUNegNumByPos(LPCTSTR lpStr,INT numPos/*=0*/)
{
	INT in_place=numPos;
	if (0>in_place){return -1;}		
	if (!lpStr)return -1;
	LPCTSTR p = lpStr;
	if (p==NULL){return -1;}
	double m_num=0;
	INT m_n1=0,m_on_off=1,m_mchar=0;
	INT m_char;
	LPCTSTR pCur=p;
	LONG_PTR len = CLString::_strlen(lpStr);
	LPCTSTR pstart=pCur,pend=NULL,pdot=NULL;				
	while(m_n1 < len)
	{
		pCur=p+m_n1;
		m_char=(INT)*pCur;
		m_n1+=1;					
		if (((m_char>=48)&&(m_char<=57))||(m_char==46))
		{
			if (m_on_off==1)
			{
				pstart=pCur;
				pend=pCur;
				m_on_off=0;
				pdot=NULL;
				if (m_char==46)
				{
					pdot=pCur;
				}
			}
			else if (m_on_off==0)
			{
				if (m_char==46)
				{
					if(pdot==NULL)
					{	pdot=pCur;pend=pCur;}
					else
					{	
						m_mchar+=1;
						if (m_mchar<=in_place)
						{
							pstart=pCur;
							pend=pCur;
							pdot=pCur;
						}
					}
				}
				else pend=pCur;
			}
		}
		else 
		{ 
			if((m_on_off==0)||(m_n1==1))
			{
				m_mchar+=1;
			}
			m_on_off=1;	
			if (m_mchar>in_place)
			{
				break;
			}	
		}
	}				
	LONG_PTR cf=1;
	if(((m_n1==len)&&(m_on_off==1)&&(in_place!=(m_mchar-1)))||(m_mchar<in_place))
	{
		pend=pstart=NULL;
	}
	if((pstart!=NULL)&&(pend!=NULL))
	{
		m_num=0;
		if (pdot!=NULL)
		{   
			len = pend + 1-pstart;
		}
		else
		{
			pdot=pend+1;
			len=pdot-pstart;
		}
		for (INT i=0;i<len;i++)
		{
			pCur=pstart+i;
			cf=pdot-pCur;
			if(cf>0)
			{						
				m_num=m_num+c2i(*pCur)*qcf(10,cf-1);
			}
			else if (cf<0)
			{  m_num=m_num+c2i(*pCur)*qcf(10,cf);
			}	
		}
	}
	else m_num=-1;
	return m_num;		
}

double CLString::getDbNumByPos(INT numPos)
{
	return getDbNumByPos(string(), numPos);
}

double CLString::getDbNumByPos(LPCTSTR lpStr, INT numPos)
{
#define DBN_SUB (1)	// has -
#define DBN_DOT (2) // has .
#define DBN_ZER (16) // has 0 end
#define DBN_UP  (4) // has x.
#define DBN_DOWN (8) // has x.x
	if (lpStr == NULL || numPos < 0)return 0.0;
	TCHAR temp[256] = {0};
	LPTSTR pc = temp;
	BYTE flag = 0;
	INT cind = -1;
	while (*lpStr)
	{
		if (*lpStr == _T('.')) { 
			if (flag & DBN_DOT) { // has . again
				if (cind >= numPos)break;
				flag = 0; pc = temp;
				*pc++ = _T('0'); *pc++ = _T('.'); *pc = _T('0'); *(pc+1) = 0; 
				cind++; 
				flag |= ( DBN_UP | DBN_DOT );
			}
			else if (flag & DBN_UP) { // fas up
					*pc++ = _T('.');  *pc = _T('0'); *(pc + 1) = 0; flag |= DBN_DOT;
				}
			else { // no up
				*pc++ = _T('0'); *pc++ = _T('.'); *pc = _T('0'); *(pc + 1) = 0; 
				cind++; flag |= (DBN_UP | DBN_DOT);
			}
		}
		else if (*lpStr == _T('-')) { 
			
			if (flag & DBN_UP) { // has up
				if (cind >= numPos)break;
				flag = 0;  pc = temp;
				*pc++ = _T('-'); *(pc) = 0; flag |= DBN_SUB;
			}
			else if (flag & DBN_ZER) {
				*pc++ = _T('0'); *pc = 0; flag |= DBN_UP; cind++; 
				if (cind >= numPos)break;
				flag = 0;  pc = temp;
				*pc++ = _T('-'); *(pc) = 0; flag |= DBN_SUB;
			} //just gas -
			else if (flag & DBN_SUB) {} //just gas -
			else if (flag == 0) { *pc++ = _T('-'); *pc = _T('0'); *(pc + 1) = 0; flag |= DBN_SUB; } // no any
			else {} // do no
		}
		else if (*lpStr == _T('0')) {
			if (flag & DBN_UP) { *pc++ = *lpStr; *(pc) = 0; }
			else if( flag & DBN_ZER) {  }
			else if ((flag & DBN_SUB) || (flag == 0)) { 
				flag |= DBN_ZER; }
			else {} // do no
		}
		else if (*lpStr >= _T('1') && *lpStr <= _T('9')) { 
			 *pc++ = *lpStr; *(pc) = 0;
			 if (flag & DBN_DOT) flag |= DBN_DOWN;
			 else if (flag & DBN_UP) {}
			 else {
				 cind++;  flag |= DBN_UP;
				 if (flag & DBN_ZER) {
					 flag ^= DBN_ZER; 
				 }
			 }
		}
		else { // is not number
			if (cind < 0) { // first is not number
				cind = 0; flag |= DBN_UP; temp[0] = 0;
			}
			if (flag & DBN_UP) { // has up
				 if (cind >= numPos)break;
			}
			else if (flag & DBN_ZER) {
				*pc++ = _T('0'); *pc = 0; flag |= DBN_UP; if (cind >= numPos)break;
			}
			flag = 0;  pc = temp; *pc = 0;
		}
		lpStr++;
	}
	if (cind < numPos)temp[0] = 0;
	return _ttof(temp);
}

BOOL CLString::asPathToDeleteReg(HKEY mainKey,BOOL isToDeleteKey,LPCTSTR keyName)
{
	// 删除一整个路径项及某个Key项用isToDeleteKey=TRUE，调用RegDeleteKey；
	// 删除某个Key项下的值用isToDeleteKey=FALSE，调用RegDeleteValue；
	BOOL m_ret=FALSE;
	HKEY hKey=NULL;	
	LONG pBOOL=0;
	DWORD m_access=KEY_ALL_ACCESS;
	TCHAR ownerPass[MAX_PATH+1]={0};
	TCHAR keyOrValueName[MAX_PATH+1]={0};
	_tcscpy_s(ownerPass,pHead);	
	try
	{
		if (isToDeleteKey)
		{
			_DeleteLastStrFromPath(ownerPass);
			_tcscpy_s(keyOrValueName,pHead);
			_GetLastStrByPath(keyOrValueName);
		}
		else
		{
			_tcscpy_s(keyOrValueName,keyName);
		}
		pBOOL=RegOpenKeyEx(mainKey,ownerPass,NULL,m_access,&hKey);
		if(ERROR_SUCCESS==pBOOL)
		{
			if (isToDeleteKey)
			{
				pBOOL=RegDeleteKey(hKey,keyOrValueName);
				if (ERROR_SUCCESS==pBOOL)
				{m_ret=TRUE;}
			}
			else
			{
				pBOOL=RegDeleteValue(hKey,keyOrValueName);
				if (ERROR_SUCCESS==pBOOL)
				{m_ret=TRUE;}
			}	
			RegCloseKey(hKey);	
		}
	}
	catch(...)
	{}
	return m_ret;
}
BOOL CLString::asPathToEnumRegKeyEx(HKEY mainKey,DWORD dwIndex,LPTSTR subkey_name,DWORD subKeyLen,
	LPTSTR lpClass,     // address of buffer for class string
	LPDWORD lpcbClass,  // address for size of class buffer
	PFILETIME lpftLastWriteTime // address for time key last written to
		)
{
	// 该函数用来 检索路劲下的子路劲，而不是子键，检索子健及键值请用asPathToEnumRegValue。
	BOOL m_ret=FALSE;
	HKEY hKey=NULL;	
	LONG pBOOL=0;
	DWORD m_access=KEY_READ;
	DWORD subkey_len=subKeyLen;
	LPTSTR ownerPass= pHead;
	try
	{			
		pBOOL=RegOpenKeyEx(mainKey,(LPTSTR)ownerPass,NULL,m_access,&hKey);
		if(ERROR_SUCCESS==pBOOL)
		{
			pBOOL=RegEnumKeyExA(hKey, dwIndex, subkey_name, &subkey_len, 0, lpClass, lpcbClass, lpftLastWriteTime );
			if (ERROR_SUCCESS==pBOOL)
			{
				m_ret=TRUE;
			}
			else {
				CLString().getLastErrorMessageBoxExceptSucceed(pBOOL);
			}
				
			RegCloseKey(hKey);	
		}
	}
	catch(...)
	{}
	return m_ret;
}
BOOL CLString::asPathToEnumRegKey(HKEY mainKey,DWORD dwIndex,LPTSTR subkey_name,DWORD subKeyLen)
{
	BOOL m_ret=FALSE;
	HKEY hKey=NULL;	
	LONG pBOOL=0;
	DWORD m_access=KEY_READ;
	DWORD subkey_len=subKeyLen;
	LPTSTR ownerPass=pHead;
	try
	{			
		pBOOL=RegOpenKeyEx(mainKey,(LPTSTR)ownerPass,NULL,m_access,&hKey);
		if(ERROR_SUCCESS==pBOOL)
		{
			pBOOL=RegEnumKey(hKey, dwIndex, subkey_name, subkey_len);
			if (ERROR_SUCCESS==pBOOL)
			{m_ret=TRUE;}
			RegCloseKey(hKey);	
		}
	}
	catch(...)
	{}
	return m_ret;
}
BOOL CLString::asPathToEnumRegValue(
	HKEY mainKey,              // handle to key to query
	DWORD dwIndex,          // index of value to query		
	DWORD valueNameBufSize,  // address for size of value buffer	
	LPTSTR valueNameBuf,     // address of buffer for value string		
	DWORD dataSizeInByte,       // address for size of data buffer
	BYTE* dataBuffer,          // address of buffer for value data
	DWORD Type        // address of buffer for type code
	)
{
	// 该函数用来 检索路劲下的子健及键值，而不是子路劲，检索子路劲请用asPathToEnumRegKeyEx。
	BOOL m_ret=FALSE;
	HKEY hKey=NULL;	
	LONG pBOOL=0;
	DWORD m_access=KEY_READ;
	LPTSTR ownerPass=pHead;

	LPTSTR lpValueName= valueNameBuf;    // address of buffer for value string
	DWORD lpcbValueName= valueNameBufSize;   // address for size of value buffer

	DWORD lpType= Type;          // address of buffer for type code
	BYTE* lpData= dataBuffer;           // address of buffer for value data
	DWORD lpcbData = dataSizeInByte;        
	try
	{			
		pBOOL=RegOpenKeyEx(mainKey,(LPTSTR)ownerPass,NULL,m_access,&hKey);
		if(ERROR_SUCCESS==pBOOL)
		{
			pBOOL=RegEnumValue(hKey, dwIndex, lpValueName,&lpcbValueName, 0, &lpType, lpData, &lpcbData);
			if (ERROR_SUCCESS==pBOOL)
			{m_ret=TRUE;}
			RegCloseKey(hKey);	
		}
	}
	catch(...)
	{}
	return m_ret;
}
BOOL CLString::asPathToGetRegValue(
	HKEY mainKey,              // handle to key to query	
	LPCTSTR valueName,     // address of buffer for value string		
	DWORD dataBufSizeInByte,       // address for size of data buffer
	LPVOID dataBuffer,          // address of buffer for value data	
	DWORD valueType        // address of buffer for type code
)
{
	BOOL m_ret = FALSE;
	HKEY hKey = NULL;
	LONG pBOOL = 0;
	DWORD m_access = KEY_READ;
	DWORD dataSizeInByte = dataBufSizeInByte;
	LPCTSTR ownerPass = string();

	DWORD lpType = REG_NONE;          // address of buffer for type code

	pBOOL = RegOpenKeyEx(mainKey, ownerPass, NULL, m_access, &hKey);
	if (ERROR_SUCCESS == pBOOL)
	{
		//pBOOL = RegGetValue(mainKey, ownerPass, valueName, RRF_RT_ANY, &lpType, dataBuffer, &dataSizeInByte);
		pBOOL = RegQueryValueEx(hKey, valueName,0, &lpType, (LPBYTE)dataBuffer, &dataSizeInByte);
		if (ERROR_SUCCESS == pBOOL)
		{
			if (lpType == valueType) // type match
				m_ret = TRUE;
		}
		RegCloseKey(hKey);
	}
		
	return m_ret;
}
BOOL CLString::isEqual(LPCTSTR pStrCmp)
{
	if (_tcscmp(pHead,pStrCmp)==0)
	{
		return TRUE;
	}
	else return FALSE;
}
BOOL CLString::isEqual(const CLString& strCmp)
{
	if (_tcscmp(pHead,strCmp.string())==0)
	{
		return TRUE;
	}
	else return FALSE;
}

INT CLString::compareNoCase(const CLString& strCmp)
{
	return _tcsicmp(pHead, strCmp.string());
}
INT CLString::compareNoCase(LPCTSTR pStrCmp)
{
	return _tcsicmp(pHead, pStrCmp);
}
INT CLString::compare(const CLString& strCmp)
{
	return _tcscmp(pHead, strCmp.string());    
}
INT CLString::compare(LPCTSTR pStrCmp)
{
	return _tcscmp(pHead, pStrCmp);
}
INT CLString::haveCharNoCase(LPCTSTR pStrCmp)
{
	LPCTSTR i = pStrCmp;
	LPCTSTR j = pHead;
	TCHAR a, b;
	if (!j || !i)return -1;
	INT _match = 0, _len = 0;
	while (*i)
	{
		while (*j)
		{
			if (*j >= 65 && *j <= 90)
			{
				a = (*j) + 32;
			}
			else a = *j;

			if (*i >= 65 && *i <= 90)
			{
				b = (*i) + 32;
			}
			else b = *i;

			if( a == b )
			{
				j++;
				_match++;
				break;
			}
			else j++;
		}
		i++;
		_len++;
	}
	if ((_match < _len) && (_match > 0)) return _match; //返回匹配个数
	else if(_match == _len) return 0;
	else return -1;
}
INT CLString::haveCharNoCase(const CLString& strCmp)
{
	return haveCharNoCase(strCmp.string());
}

DWORD   CLString::getLastErrorString(DWORD nLastError,LPTSTR lpszOutPut,LONG_PTR nSize){
	LPTSTR lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,nLastError,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&lpMsgBuf,0, NULL);	
	assert(lpszOutPut != NULL);
	if(!lpMsgBuf)
		_stprintf_s(lpszOutPut,nSize,_T("<LastError: %d> 该错误码无法解析！"),nLastError);
	else 
	{
		_stprintf_s(lpszOutPut,nSize,_T("<LastError: %d> %s"),nLastError,lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
	return nLastError;
}
LPCTSTR CLString::getLastErrorString(DWORD nLastError)
{	
	TCHAR inf[MAX_PATH] ={0};
	getLastErrorString(nLastError,inf,MAX_PATH);
	set(inf);
	return pHead;
}
CLString& CLString::getLastErrorStringR(DWORD nLastError)
{
	getLastErrorString(nLastError);
	return *this;
}
DWORD CLString::getLastErrorMessageBox(DWORD nLastError,HWND hParentWnd ,LPCTSTR pTitle,UINT uType,LPCTSTR pInsetStr,LPCTSTR pEndStr)
{
	(*this).getLastErrorStringR(nLastError).insert(0,pInsetStr).append(pEndStr);
    if (nLastError == ERROR_SUCCESS)
		::messageBoxT(hParentWnd, string(), pTitle, MB_ICONINFORMATION,5000);
    else 
		::messageBoxT(hParentWnd, string(),pTitle,uType, 5000);
	return nLastError;
}
DWORD CLString::getLastErrorMessageBoxExceptSucceed(DWORD nLastError,HWND hParentWnd,LPCTSTR pTitle,UINT uType,LPCTSTR pInsetStr,LPCTSTR pEndStr)
{
    (*this).getLastErrorStringR(nLastError).insert(0,pInsetStr).append(pEndStr);
    if (nLastError != ERROR_SUCCESS)
    {
        ::messageBoxT(hParentWnd, string(),pTitle,uType, 5000);
    }
    return nLastError;
}
INT CLString::messageBox(UINT nStyle,HWND hParentWnd)
{
	return ::MessageBox(hParentWnd,string(),_T("Msg Box"), nStyle);
}

CLString& CLString::messageBoxRef(UINT nStyle, HWND hParentWnd)
{
	return messageBox(nStyle, hParentWnd), * this;
}

INT CLString::messageBoxTime(LPCTSTR boxTitle,UINT nStyle, DWORD dwMilliseconds, HWND hParentWnd)
{
	return ::messageBoxTimeout(hParentWnd, this->string(), boxTitle ? boxTitle : _T("Msg Box"), nStyle, dwMilliseconds);
}

CLString& CLString::messageBoxTimeRef(LPCTSTR boxTitle, UINT nStyle, DWORD dwMilliseconds, HWND hParentWnd)
{
	return ::messageBoxTimeout(hParentWnd, this->string(), boxTitle ? boxTitle : _T("Msg Box"), nStyle, dwMilliseconds),*this;
}

INT CLString::messageBox(LPCTSTR boxTitle,UINT nStyle,HWND hParentWnd)
{
	return ::MessageBox(hParentWnd,string(),boxTitle ? boxTitle: _T("Msg Box"), nStyle);
}

CLString& CLString::messageBoxRef(LPCTSTR boxTitle, UINT nStyle, HWND hParentWnd)
{
	return messageBox(boxTitle, nStyle, hParentWnd), * this;
}

CLString& CLString::logout()
{
	_tprintf_s(string());
	return *this;
}

CLString& CLString::printf()
{
	return logout();
}

int CLString::printfBackCover(int bakCoverChars)
{
	return printfWithBackCover(bakCoverChars, _T("%s"), string()); 
}

LPCSTR CLString::getASCII(){
	if (m_encoding == EnCode_ASCII)
		return (LPCSTR)string();
	else {
		if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
		int length = ::WideCharToMultiByte (CP_ACP,0, (LPCWSTR)string(), -1,NULL, 0,NULL, 0 )+1;

		if ((makeDEx()->m_pointerBufSizeInByte_) < length)//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pMultiByte)
				delete[] (makeDEx()->m_pointer_).pMultiByte;
			(makeDEx()->m_pointer_).pMultiByte = new char[ (makeDEx()->m_pointerBufSizeInByte_) = length];
		}		
		
		::WideCharToMultiByte ( CP_ACP,                // ANSI 代码页
			0, // 检查重音字符
			(LPCWSTR)string(),         // 原Unicode 串
			-1,                    // -1 意思是串以0x00结尾
			(makeDEx()->m_pointer_).pMultiByte,          // 目的char字符串
			length,  // 缓冲大小
			NULL,                  // 肥缺省字符串
			0 );                // 忽略这个参数
		return (makeDEx()->m_pointer_).pMultiByte;
	}
}
LPCSTR CLString::getUTF8(){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if (m_encoding == EnCode_ASCII){	
		LPCWSTR p;
		LONG_PTR nSize;
		if(!swapEncode((const LPBYTE)string(),strlen(),EnCode_ASCII,(LPBYTE&)p,nSize,EnCode_ULE))
			return NULL;
		int length = ::WideCharToMultiByte (CP_UTF8,0, p, -1,NULL, 0,NULL, 0 )+1;
		if ((makeDEx()->m_pointerBufSizeInByte_) < length)//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pMultiByte)
				delete[] (makeDEx()->m_pointer_).pMultiByte;
			(makeDEx()->m_pointer_).pMultiByte = new char[ (makeDEx()->m_pointerBufSizeInByte_) = length];
		}	
		::WideCharToMultiByte ( CP_UTF8,0,p,-1,(makeDEx()->m_pointer_).pMultiByte,length,NULL,0 );                                                        
		delete[] p;
		return (makeDEx()->m_pointer_).pMultiByte;   
	}
	else {
		int length = ::WideCharToMultiByte (CP_UTF8,0, (LPCWSTR)string(), -1,NULL, 0,NULL, 0 )+1;
		if ((makeDEx()->m_pointerBufSizeInByte_) < length)//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pMultiByte)
				delete[] (makeDEx()->m_pointer_).pMultiByte;
			(makeDEx()->m_pointer_).pMultiByte = new char[ (makeDEx()->m_pointerBufSizeInByte_) = length];
		}	
		::WideCharToMultiByte ( CP_UTF8, 0,(LPCWSTR)string(),-1,(makeDEx()->m_pointer_).pMultiByte, length,NULL,0 );
		return (makeDEx()->m_pointer_).pMultiByte;
	}
}
LPSTR CLString::unicodeToAscii(LPCWSTR lpStr){
	if(lpStr == 0) return NULL;
	int length = ::WideCharToMultiByte (CP_ACP,0, lpStr, -1,NULL, 0,NULL, 0 );
	if(length <= 0)return NULL;
	LPSTR lp = new CHAR[++length];	
	::WideCharToMultiByte ( CP_ACP,0,lpStr,-1,lp,length,NULL,0);
	lp[length-1] = 0;
	return lp;
}
LPSTR CLString::unicodeToUtf8(LPCWSTR lpStr){
	if(lpStr == 0) return NULL;
	int length = ::WideCharToMultiByte (CP_UTF8,0, lpStr, -1,NULL, 0,NULL, 0 );
	if(length <= 0)return NULL;
	LPSTR lp = new CHAR[++length];	
	::WideCharToMultiByte ( CP_UTF8,0,lpStr,-1,lp,length,NULL,0);
	lp[length-1] = 0;
	return lp;
}
LPWSTR CLString::asciiToUnicode(LPCSTR lpStr){
	if(lpStr == 0) return NULL;
	int length = ::MultiByteToWideChar (CP_ACP,0, lpStr, -1,NULL, 0);
	if(length <= 0)return NULL;
	LPWSTR lp = new WCHAR[++length];	
	::MultiByteToWideChar ( CP_ACP,0,lpStr,-1,lp,length);
	lp[length-1] = 0;
	return lp;
}
LPSTR CLString::asciiToUtf8(LPCSTR lpStr){
	if(lpStr == 0) return NULL;
	LPWSTR lpw = asciiToUnicode(lpStr);
	if(lpw){
		LPSTR lpRet = unicodeToUtf8(lpw);
		delete[] lpw;
		return lpRet;
	}
	else return NULL;
}
LPWSTR CLString::utf8ToUnicode(LPCSTR lpStr){
	if(lpStr == 0) return NULL;
	int length = ::MultiByteToWideChar (CP_UTF8,0, lpStr, -1,NULL, 0);
	if(length <= 0)return NULL;
	LPWSTR lp = new WCHAR[++length];	
	::MultiByteToWideChar ( CP_UTF8,0,lpStr,-1,lp,length);
	lp[length-1] = 0;
	return lp;
}
LPSTR CLString::utf8ToAscii(LPCSTR lpStr){
	if(lpStr == 0) return NULL;
	LPWSTR lpw = utf8ToUnicode(lpStr);
	if(lpw){
		LPSTR lpRet = unicodeToAscii(lpw);
		delete[] lpw;
		return lpRet;
	}
	else return NULL;
}
LPCSTR CLString::unicodeToAsciiInner(LPCWSTR lpStr){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if(lpStr == 0) return NULL;
	int length = ::WideCharToMultiByte (CP_ACP,0, lpStr, -1,NULL, 0,NULL, 0 );
	if(length <= 0)return NULL;
	++length;
	LPSTR lp = (getTmpBufSize() >= length*(LONG_PTR)sizeof(char)) ? 
		((makeDEx()->m_pointer_).pMultiByte) : (((makeDEx()->m_pointer_).pMultiByte ? delete[] (makeDEx()->m_pointer_).pMultiByte,0 : NULL),(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(char),(makeDEx()->m_pointer_).pMultiByte = new CHAR[length]);	
	::WideCharToMultiByte ( CP_ACP,0,lpStr,-1,lp,length,NULL,0);
	lp[length-1] = 0;
	return lp;
}
LPCSTR CLString::unicodeToUtf8Inner(LPCWSTR lpStr){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if(lpStr == 0) return NULL;
	int length = ::WideCharToMultiByte (CP_UTF8,0, lpStr, -1,NULL, 0,NULL, 0 );
	if(length <= 0)return NULL;
	++length;
	LPSTR lp = (getTmpBufSize() >= length*(LONG_PTR)sizeof(char)) ? 
		((makeDEx()->m_pointer_).pMultiByte) : (((makeDEx()->m_pointer_).pMultiByte ? delete[] (makeDEx()->m_pointer_).pMultiByte,0 : NULL),(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(char),(makeDEx()->m_pointer_).pMultiByte = new CHAR[length]);	
	::WideCharToMultiByte ( CP_UTF8,0,lpStr,-1,lp,length,NULL,0);
	lp[length-1] = 0;
	return lp;
}
LPCWSTR CLString::asciiToUnicodeInner(LPCSTR lpStr){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if(lpStr == 0) return NULL;
	int length = ::MultiByteToWideChar (CP_ACP,0, lpStr, -1,NULL, 0);
	if(length <= 0)return NULL;
	++length;
	LPWSTR lp = (getTmpBufSize() >= length*(LONG_PTR)sizeof(WCHAR)) ? 
		((makeDEx()->m_pointer_).pWideChar) : ( ((makeDEx()->m_pointer_).pWideChar ? (delete[] (makeDEx()->m_pointer_).pWideChar,0): NULL),(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR),(makeDEx()->m_pointer_).pWideChar = new WCHAR[length]);		
	::MultiByteToWideChar ( CP_ACP,0,lpStr,-1,lp,length);
	lp[length-1] = 0;
	return lp;
}
LPCSTR CLString::asciiToUtf8Inner(LPCSTR lpStr){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if(lpStr == 0) return NULL;
	LPWSTR lpw = asciiToUnicode(lpStr);
	if(lpw){
		LPCSTR lpRet = unicodeToUtf8Inner(lpw);
		delete[] lpw;
		return lpRet;
	}
	else return NULL;
}
LPCWSTR CLString::utf8ToUnicodeInner(LPCSTR lpStr){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if(lpStr == 0) return NULL;
	int length = ::MultiByteToWideChar (CP_UTF8,0, lpStr, -1,NULL, 0);
	if(length <= 0)return NULL;
	++length;
	LPWSTR lp = (getTmpBufSize() >= length*(LONG_PTR)sizeof(WCHAR)) ? 
		((makeDEx()->m_pointer_).pWideChar) : (((makeDEx()->m_pointer_).pWideChar ? delete[] (makeDEx()->m_pointer_).pWideChar,0 : NULL),(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR),(makeDEx()->m_pointer_).pWideChar = new WCHAR[length]);		
	::MultiByteToWideChar ( CP_UTF8,0,lpStr,-1,lp,length);
	lp[length-1] = 0;
	return lp;
}
LPCSTR CLString::utf8ToAsciiInner(LPCSTR lpStr){
	if(lpStr == 0) return NULL;
	LPWSTR lpw = utf8ToUnicode(lpStr);
	if(lpw){
		LPCSTR lpRet = unicodeToAsciiInner(lpw);
		delete[] lpw;
		return lpRet;
	}
	else return NULL;
}
LPCWSTR CLString::getUnicodeLittleEndian(){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if (m_encoding == EnCode_ASCII){		
		int length = ::MultiByteToWideChar (CP_ACP, 0,(LPCSTR)string(), -1,NULL, 0)+1;  
		if ((makeDEx()->m_pointerBufSizeInByte_) < (LONG_PTR)(length*sizeof(WCHAR)))//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pWideChar)
				delete[] (makeDEx()->m_pointer_).pWideChar;
			(makeDEx()->m_pointer_).pWideChar = new WCHAR[length];
			(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR);
		}
		::MultiByteToWideChar ( CP_ACP, 0,(LPCSTR)string(),-1, (makeDEx()->m_pointer_).pWideChar,length );   
		*((makeDEx()->m_pointer_).pWideChar + length - 1) = 0;
		return (makeDEx()->m_pointer_).pWideChar;
	}
	else if(m_encoding == EnCode_UBE){
		LONG_PTR length = strlen()+1;
		if ((makeDEx()->m_pointerBufSizeInByte_) < (LONG_PTR)(length*sizeof(WCHAR)))//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pWideChar)
				delete[] (makeDEx()->m_pointer_).pWideChar;
			(makeDEx()->m_pointer_).pWideChar = new WCHAR[length];
			(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR);
		}
		for(LONG_PTR i = 0; i < length; *((makeDEx()->m_pointer_).pWideChar + i)= ntohs(*((LPCWSTR)string() + i)),i++);
		*((makeDEx()->m_pointer_).pWideChar + length - 1) = 0;
		return (makeDEx()->m_pointer_).pWideChar;
	}
	else 
		return (LPCWSTR)string();
}
LPCWSTR CLString::getUnicodeBigEndian(){
	if(pDataEx)pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	if (m_encoding == EnCode_ASCII){		
		int length = ::MultiByteToWideChar (CP_ACP, 0,(LPCSTR)string(), -1,NULL, 0)+1;  
		if ((makeDEx()->m_pointerBufSizeInByte_) < (LONG_PTR)(length*sizeof(WCHAR)))//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pWideChar)
				delete[] (makeDEx()->m_pointer_).pWideChar;
			(makeDEx()->m_pointer_).pWideChar = new WCHAR[length];
			(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR);
		}
		::MultiByteToWideChar ( CP_ACP, 0,(LPCSTR)string(),-1, (makeDEx()->m_pointer_).pWideChar,length );   
		for(LONG_PTR i = 0; i < length; *((makeDEx()->m_pointer_).pWideChar + i)= htons(*((makeDEx()->m_pointer_).pWideChar + i)),i++);
		*((makeDEx()->m_pointer_).pWideChar + length - 1) = 0;
		return (makeDEx()->m_pointer_).pWideChar;
	}
	else if(m_encoding == EnCode_ULE){
		LONG_PTR length = strlen()+1;
		if ((makeDEx()->m_pointerBufSizeInByte_) < (LONG_PTR)(length*sizeof(WCHAR)))//上一次保存有记录就清空
		{
			if((makeDEx()->m_pointer_).pWideChar)
				delete[] (makeDEx()->m_pointer_).pWideChar;
			(makeDEx()->m_pointer_).pWideChar = new WCHAR[length];
			(makeDEx()->m_pointerBufSizeInByte_) = length*sizeof(WCHAR);
		}
		for(LONG_PTR i = 0; i < length; *((makeDEx()->m_pointer_).pWideChar + i)= htons(*((LPCWSTR)string() + i)),i++);
		*((makeDEx()->m_pointer_).pWideChar + length - 1) = 0;
		return (makeDEx()->m_pointer_).pWideChar;
	}
	else 
		return (LPCWSTR)string();
}
LPCWSTR CLString::getUnicode(){
	if(isBigEndian())
		return getUnicodeBigEndian();
	else 
		return getUnicodeLittleEndian();
}

BOOL CLString::writeLineToFile(LPCTSTR  pFileDir, DWORD dwMoveMethod,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,BOOL openAlertMsg){
	
	//CLString szTmp(*this);
	append(_T("\r\n"));
	BOOL rt = writeToFile(pFileDir,OPEN_ALWAYS,dwMoveMethod,lDistanceToMove,lpDistanceToMoveHigh,openAlertMsg);
	//set(szTmp);
	leftSave(strlen()-2);
	return rt;
	}
BOOL CLString::writeToFile(LPCTSTR  pFileDir, DWORD dwOpenMethod,DWORD dwMoveMethod,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,BOOL openAlertMsg)
{
	if (!pFileDir && !((makeDEx()->m_hFile_)))
	{
		if (openAlertMsg)::MessageBox(NULL, _T("第一次写入文件，请指定文件路劲！"), _T("CLString writeToFile Error"), MB_ICONERROR);
		return FALSE;
	}
	if (pFileDir)//有路径
	{
		if (makeDEx()->m_hFile_)//已打开文件
			if (!closeFile())
				return FALSE;
		if (!createDirectoryByFileName(pFileDir)) //检查路径
			return FALSE;
		HANDLE hTmp = ::CreateFile(pFileDir, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, dwOpenMethod, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hTmp || hTmp == INVALID_HANDLE_VALUE) {
			if (openAlertMsg) {
				DWORD le = GetLastError();
				CLString().getLastErrorMessageBoxExceptSucceed(le);
			}
			return FALSE;
		}
		else (makeDEx()->m_hFile_) = hTmp;
	}
	DWORD dwWrites;   //定义实际写入字节数
	DWORD _position;

	_position = SetFilePointer((makeDEx()->m_hFile_), lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
	return ::WriteFile((makeDEx()->m_hFile_), string(), strlen() * sizeof(TCHAR), &dwWrites, NULL);
}
LONG_PTR CLString::readLineFromFile(LPCTSTR  pFileDir, BOOL openAlertMsg)
{
	BOOL rs = FALSE;
	if (!pFileDir && !(makeDEx()->m_hFile_))//没路径，没打开文件
	{
		if(openAlertMsg)::MessageBox(NULL,_T("第一次读取文件，请指定文件路劲！"),_T("CLString readLineFromFile Error"),MB_ICONERROR);
		return -1;
	} 
	if (pFileDir)//有路径，有打开文件
	{ 
		(makeDEx()->m_isEndOnce_) = FALSE;
		if (makeDEx()->m_hFile_)
			if (!closeFile())
				return NULL;
		HANDLE hTmp = ::CreateFile(pFileDir, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hTmp || hTmp == INVALID_HANDLE_VALUE ){
			if(openAlertMsg){
				DWORD le = GetLastError();
				CLString().getLastErrorMessageBoxExceptSucceed(le);}
			return -1;
		}
		else (makeDEx()->m_hFile_) = hTmp;
	}
		DWORD dwRead = 0;   //定义实际写入字节数
		DWORD _position,i = 0;
		BOOL haveRET = FALSE , isBegin = FALSE;
		_position = SetFilePointer((makeDEx()->m_hFile_), 0, 0, FILE_CURRENT);		
		TCHAR a = 0,b = 0;
		while(::ReadFile((makeDEx()->m_hFile_), &a, sizeof(TCHAR), &dwRead, NULL) && dwRead > 0){
			if(b == 13 && a == 10){
				i -= sizeof(TCHAR);
				haveRET = TRUE;
				break;
			}
			b = a;
			i += sizeof(TCHAR);
			haveRET = FALSE;
			isBegin = TRUE;
		}
		DWORD _size = GetFileSize((makeDEx()->m_hFile_), NULL);
		if(i == 0 && isBegin == FALSE) {
			set(_T("\0"));
			if(_position >= _size){
				if((makeDEx()->m_isEndOnce_) == FALSE) 
					(makeDEx()->m_isEndOnce_) = TRUE;
				else 
					return -1;
			}		
			return 0;
		}		
		_position = SetFilePointer((makeDEx()->m_hFile_), _position, 0, FILE_BEGIN);
		if(!::ReadFile((makeDEx()->m_hFile_), (LPBYTE)store(i/sizeof(TCHAR)), i, &dwRead, NULL))
		{
			return -1;
		}
		if (haveRET)//以换行结尾，调节指针
		{
			_position = SetFilePointer((makeDEx()->m_hFile_), 2*sizeof(TCHAR), 0, FILE_CURRENT);
			if(_position == _size){
				(makeDEx()->m_isEndOnce_) = FALSE;
			}
		}else{
			(makeDEx()->m_isEndOnce_) = TRUE;
		}
	return strlen();
}
LONG_PTR CLString::readLineFromFileEnd(LPCTSTR  pFileDir, BOOL openAlertMsg)
{
	BOOL rs = FALSE;
	if (!pFileDir && !(makeDEx()->m_hFile_))//没路径，没打开文件
	{
		if(openAlertMsg)::MessageBox(NULL,_T("第一次读取文件，请指定文件路劲！"),_T("CLString readLineFromFile Error"),MB_ICONERROR);
		return -1;
	} 
	if (pFileDir)//有路径，有打开文件
	{ 
		(makeDEx()->m_isEndOnce_) = FALSE;
		if (makeDEx()->m_hFile_)
			if (!closeFile())
				return NULL;
		HANDLE hTmp = ::CreateFile(pFileDir, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(!hTmp || hTmp == INVALID_HANDLE_VALUE ){
			if(openAlertMsg){
				DWORD le = GetLastError();
				CLString().getLastErrorMessageBoxExceptSucceed(le);}
			return -1;
		}
		(makeDEx()->m_hFile_) = hTmp;
		SetFilePointer((makeDEx()->m_hFile_),0,0,FILE_END);
	}
	LONG_PTR _position,i = 0;
	_position = SetFilePointer((makeDEx()->m_hFile_), 0, 0, FILE_CURRENT);		
	if( _position < sizeof(TCHAR)){
		if((makeDEx()->m_isEndOnce_)){
			(makeDEx()->m_isEndOnce_) = FALSE;
			set(_T(""));
			return strlen();
		}
		else return -1;
	}
	SetFilePointer((makeDEx()->m_hFile_), (long)sizeof(TCHAR)*(-1), 0, FILE_CURRENT);
	DWORD dwRead = 0;   //定义实际写入字节数	
	BOOL haveRET = FALSE , isBegin = TRUE,isEnd=FALSE;		
	TCHAR a = 0,b = 0;
	while(::ReadFile((makeDEx()->m_hFile_), &a, sizeof(TCHAR), &dwRead, NULL) && dwRead > 0 ){
		if(b == 10 && a == 13){
			i -= sizeof(TCHAR);
			haveRET = TRUE;
			_position +=sizeof(TCHAR);
			break;
		}		
		b = a;
		i += sizeof(TCHAR);
		haveRET = FALSE;
		isBegin = TRUE;	
		_position = SetFilePointer((makeDEx()->m_hFile_), (long)sizeof(TCHAR)*(-1), 0, FILE_CURRENT);
		if( _position < sizeof(TCHAR) ){
			isEnd=TRUE;
			break;
		}	
		SetFilePointer((makeDEx()->m_hFile_), (long)sizeof(TCHAR)*(-1), 0, FILE_CURRENT);
	}	
	SetFilePointer((makeDEx()->m_hFile_), _position, 0, FILE_BEGIN);
	if(!::ReadFile((makeDEx()->m_hFile_), (LPBYTE)store(i/sizeof(TCHAR)), i, &dwRead, NULL))
		return -1;
	if(isEnd)
		SetFilePointer((makeDEx()->m_hFile_), 0, 0, FILE_BEGIN);
	if (haveRET)//以换行结尾，调节指针
	{
		(makeDEx()->m_isEndOnce_) = TRUE;
		SetFilePointer((makeDEx()->m_hFile_), _position-2*sizeof(TCHAR), 0, FILE_BEGIN);
	}else{
		if(_position == 0){
			(makeDEx()->m_isEndOnce_) = FALSE;
		}
	}
	return strlen();
}
LPCTSTR CLString::readFile(LPCTSTR pFilePath, BOOL openAlertMsg)
{
	closeFile();

	HANDLE _hFile = ::CreateFile(pFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	empty(); //先开后清除

	if(!_hFile || (_hFile == INVALID_HANDLE_VALUE)){
		DWORD le = GetLastError();
		CLString().getLastErrorStringR(le).messageBox(_T("CLString readFile Error"),MB_ICONERROR);
		return NULL;	
	}

	byte ec = checkTextFileEncode(_hFile,TRUE);

	if(ec == EnCode_UNKNOWN){
		::CloseHandle(_hFile);
		return NULL;
	}	

	DWORD nNum = 0, nSizeH = 0, nSizeL = GetFileSize(_hFile,&nSizeH)+sizeof(TCHAR)+2;
	if( m_unitNumber*sizeof(TCHAR) < nSizeL){
		if(string() != getCLString0())delete[] pHead;
		pHead = new TCHAR[m_unitNumber = nSizeL/sizeof(TCHAR)];
		m_changeFlag = TRUE;
	}
	//LPBYTE p1 = new byte[nSizeL];
	ReadFile(_hFile,(LPBYTE)pHead,m_unitNumber*sizeof(TCHAR),&nNum,0);
	ZeroMemory((LPBYTE)pHead + nNum,m_unitNumber*sizeof(TCHAR) - nNum);
	::CloseHandle(_hFile);
	
	if(m_encoding != ec){
		LPBYTE pTag;
		LONG_PTR nSize;
		if(swapEncode((const LPBYTE)pHead,nNum,ec,pTag,nSize,m_encoding)){
			set((LPCTSTR)pTag);
			delete[] pTag;
		}
	}
	return string();
}
BOOL CLString::closeFile() {
	if(pDataEx == NULL)
		return TRUE;
	else
		return pDataEx->closeFile();
}

CLString operator+(const CLString& str1,const  CLString& str2)
{
    CLString strResult(str1.string());

    strResult.append(str2.string());

    return(strResult);
}
CLString operator+(LPCTSTR pStr1,const  CLString & str2)
{
    CLString strResult(pStr1);

    strResult.append(str2.string());

    return(strResult);
}
CLString operator+(const CLString & str1, LPCTSTR pStr2)
{
    CLString strResult(str1.string());

    strResult.append(pStr2);

    return(strResult);
}
double CLString::calcTimeDelta(const SYSTEMTIME* beforetime,const SYSTEMTIME* behandtime)
{
    int dyear = (int)behandtime->wYear - (int)beforetime->wYear;
    int dmonth = (int)behandtime->wMonth - (int)beforetime->wMonth + dyear * 12;
    int dday = (int)behandtime->wDay - (int)beforetime->wDay + dmonth * 30;
    int dhour = (int)behandtime->wHour - (int)beforetime->wHour + dday * 24;
    int dminute = (int)behandtime->wMinute - (int)beforetime->wMinute + dhour * 60;
    int dsecond = (int)behandtime->wSecond - (int)beforetime->wSecond + dminute * 60;
    double dmillisecond = (double)behandtime->wMilliseconds - (double)beforetime->wMilliseconds + dsecond * 1000.0;
    return dmillisecond / 1000.0;
}
void CLString::timingStart(void){
	if(!(makeDEx()->m_sysTimeStart_))
		(makeDEx()->m_sysTimeStart_) = new SYSTEMTIME;
	GetSystemTime((makeDEx()->m_sysTimeStart_));
}
void CLString::timingStartEx(void){
	if(!(makeDEx()->m_largeIntegerStart_))
		(makeDEx()->m_largeIntegerStart_) = new LARGE_INTEGER;
	QueryPerformanceCounter((makeDEx()->m_largeIntegerStart_));
}
double CLString::timingGetSpentTime(BOOL openErrorAlert  /*= TRUE*/,LPTSTR pOutErrStr ){
	if (!(makeDEx()->m_sysTimeStart_))
	{	
		if (openErrorAlert)
			::MessageBox(NULL,_T("没有启动计时，获取时间差失败！"),_T("timingGetSpentTime Error"),MB_ICONERROR);
		if(pOutErrStr)
			_tcscpy_s(pOutErrStr,256,_T("没有启动计时，获取时间差失败！"));
		return -1;
	}
	if(!(makeDEx()->m_sysTimeEnd_))
		(makeDEx()->m_sysTimeEnd_) = new SYSTEMTIME;
	GetSystemTime((makeDEx()->m_sysTimeEnd_));
	return CLString::calcTimeDelta((makeDEx()->m_sysTimeStart_),(makeDEx()->m_sysTimeEnd_));
}
double CLString::timingGetSpentTimeEx(BOOL openErrorAlert  /*= TRUE*/,LPTSTR pOutErrStr ){
	
	if (!(makeDEx()->m_largeIntegerStart_))
	{	
		if (openErrorAlert)
			::MessageBox(NULL,_T("没有启动计时，获取时间差失败！"),_T("timingGetSpentTimeEx Error"),MB_ICONERROR);
		if(pOutErrStr)
			_tcscpy_s(pOutErrStr,256,_T("没有启动计时，获取时间差失败！"));
		return -1;
	}	
	if(!(makeDEx()->m_largeIntegerEnd_))
		(makeDEx()->m_largeIntegerEnd_) = new LARGE_INTEGER;
	QueryPerformanceCounter((makeDEx()->m_largeIntegerEnd_));
	LARGE_INTEGER Freg;
	QueryPerformanceFrequency(&Freg);
	return (double)((makeDEx()->m_largeIntegerEnd_)->QuadPart - (makeDEx()->m_largeIntegerStart_)->QuadPart) / (double)Freg.QuadPart; 
}
LPCTSTR CLString::timingGetSpentTimeString(BOOL openErrorAlert /*= TRUE*/,LPTSTR pOutErrStr)
{	
	double s = timingGetSpentTime(openErrorAlert,pOutErrStr);
	if (s == -1) 
		return NULL;
	_stprintf_s(store(100),101,_T("%.3f秒"),s);
	return pHead;
}
GUID CLString::createGuid()
{
	GUID guid;
#ifdef WIN32
	CoCreateGuid(&guid);
#else
	uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#endif
	return guid;
}
LPCTSTR CLString::guidToString(const GUID &guid)
{
	TCHAR buf[64] = {0};
#ifdef __GNUC__
	sntprintf(
#else // MSVC
	_sntprintf_s(
#endif
		buf,
		sizeof(TCHAR)*64,
		_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	set(buf);
	return pHead;
}
LPCTSTR CLString::createGuidToString(GUID* pResGuid)
{
	GUID guid;
#ifdef WIN32
	HRESULT hr = CoCreateGuid(&guid);
	if (!pResGuid&& hr == S_OK) memcpy(pResGuid,&guid,sizeof(GUID));
#else
	uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#endif
	TCHAR buf[64] = {0};
#ifdef __GNUC__
	sntprintf(
#else // MSVC
	_sntprintf_s(
#endif
		buf,
		sizeof(TCHAR)*64,
		_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	set(buf);
	return pHead;
}
CLString& CLString::httpClose(){
	if(pDataEx)
		pDataEx->closeInternet();
	return *this;
}
BOOL CLString::httpInitGet(LPCTSTR serverDomain,INTERNET_PORT port ,BOOL openErrorAlert){
	httpClose();
	(makeDEx()->hInternet_) = InternetOpen(_T("WinInetGet/0.1"),INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!(makeDEx()->hInternet_))
	{
		if(openErrorAlert)
			::MessageBox(NULL,_T("初始化Internet Get对象失败！"),_T("HttpInitial Error"),MB_ICONERROR);
		return FALSE;
	}
	(makeDEx()->hConnect_) = InternetConnect((makeDEx()->hInternet_), serverDomain, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if((makeDEx()->hConnect_) == NULL){		
		if(openErrorAlert)
			::MessageBox(NULL,CLString().getLastErrorString(GetLastError()),_T("Http Get Connect Error"),MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}
BOOL CLString::httpGet(LPCTSTR lpszObjectName,LPCTSTR pHeaders[],LONG_PTR nHeaderNum,BOOL bU2G,BOOL openErrorAlert,LPTSTR pOutErrStr)
{
	
	DWORD dwOpenRequestFlags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
		INTERNET_FLAG_KEEP_CONNECTION |
		INTERNET_FLAG_NO_AUTH |
		INTERNET_FLAG_NO_COOKIES |
		INTERNET_FLAG_NO_UI |
		//设置启用HTTPS
		INTERNET_FLAG_SECURE |
		INTERNET_FLAG_RELOAD;

	//初始化Request
	(makeDEx()->hRequest_) = (HINTERNET)HttpOpenRequest((makeDEx()->hConnect_), _T("GET"), lpszObjectName, NULL,	NULL, NULL,
		dwOpenRequestFlags, 0);
	if((makeDEx()->hRequest_) == NULL){	
		CLString err;
		if(openErrorAlert)
			::MessageBox(NULL,err.getLastErrorString(GetLastError()),_T("Http Get Request Error"),MB_ICONERROR);
		if(pOutErrStr)
			_tcscpy_s(pOutErrStr,256,err.getLastErrorString(GetLastError()));
		return FALSE;
	}
	if (pHeaders != NULL)//循环增加报头
	{
		LONG_PTR i = 0;
		do 
		{
			HttpAddRequestHeaders((makeDEx()->hRequest_),pHeaders[i],-1, HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);
			i++;
		} while ((nHeaderNum > (LONG_PTR)0) && (i < nHeaderNum));
	}
	//发送Request
	BOOL bResult = HttpSendRequest((makeDEx()->hRequest_), NULL, 0, NULL, 0);
	//链接失败，对应的错误码可以从微软网站http://support.microsoft.com/kb/193625 查询
	if(!bResult) {
		CLString err;
		if(openErrorAlert)
			::MessageBox(NULL,err.getLastErrorString(GetLastError()),_T("Http Get SendRequest Error"),MB_ICONERROR);
		if(pOutErrStr)
			_tcscpy_s(pOutErrStr,256,err.getLastErrorString(GetLastError()));
		return FALSE;
	}
	DWORD httpcode;
	DWORD dwSizeOfRq = sizeof(DWORD);
	HttpQueryInfo((makeDEx()->hRequest_), HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &httpcode, &dwSizeOfRq, NULL);
	//获取HTTP Response 的 Body.
	DWORD dwBytesAvailable;
	//BYTE *pMessageBody = (BYTE *)malloc(BUFFERSIZE);
	BYTE messageBody[BUFFERSIZE];
	DWORD dwBytesRead;
	std::string Result = "";	
	while(InternetQueryDataAvailable((makeDEx()->hRequest_), &dwBytesAvailable, 0, 0)) {		
		BOOL bResult = InternetReadFile((makeDEx()->hRequest_), messageBody,
			dwBytesAvailable, &dwBytesRead);
		if(!bResult) {
			return FALSE;
		}
		if(dwBytesRead == 0)
			break;
		messageBody[dwBytesRead] = _T('\0');
		if(bU2G)
			U2G((char*)messageBody,Result);
		else
			Result+=(char*)messageBody;
	}
	if(httpcode==HTTP_STATUS_DENIED)
	{	
		set((LPCTSTR)Result.c_str());
		return FALSE;
	}
	//处理其他API返回值
	if(httpcode >= 200 && httpcode < 300){
		set((LPCTSTR)Result.c_str());
		return TRUE;
	}
	return FALSE;
}
BOOL CLString::httpAddHeaders(LPTSTR pHeaderString /*= NULL*/)
{
	if (!(makeDEx()->hRequest_))
	{
		::MessageBox(NULL,_T("还没有获得Http连接请求句柄，增加Http头失败！"),_T("HttpRequest Error"),MB_ICONERROR);
		return FALSE;
	}
	if (!pHeaderString)
	{
		return HttpAddRequestHeaders((makeDEx()->hRequest_), pHeaderString,    -1, HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE);
	}else return TRUE;
}
//UTF-8到GB2312的转换，json格式和新闻API都是UTF-8格式，需要转为GB2312；csv格式直接是GB2312，不需要转化。
void CLString::U2G(const char* utf8,std::string& re)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	re+=str;
	if(str) delete[] str;
}
void  CLString::fileEnumeration(std::vector<LPCTSTR>& m_vtInnerStringVector_,LPCTSTR lpPath,LPCTSTR lpExName,BOOL bRecursion,
	BOOL bEnumFiles,BOOL bClearVectorBefoteStart,BOOL bJustStoreEndName)
{
	try {
		//-------------------------------------------------------------------------  
		if(bClearVectorBefoteStart)
			clearStringVector(m_vtInnerStringVector_);

		LONG_PTR len = std::_tcslen(lpPath);
		if (lpPath == NULL || len <= 0) return;

		TCHAR path[MAX_PATH] = {0};
		_tcscpy_s(path, lpPath);
		if (lpPath[len - 1] != _T('\\')) _tcscat_s(path, _T("\\"));
		_tcscat_s(path, _T("*"));

		WIN32_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFile(path, &fd);
		if (hFindFile == INVALID_HANDLE_VALUE)
		{
			::FindClose(hFindFile); return;
		}

		TCHAR tempPath[MAX_PATH]; 
		BOOL bUserReture = TRUE; 
		BOOL bIsDirectory;

		BOOL bFinish = FALSE;
		while (!bFinish)
		{
			_tcscpy_s(tempPath, lpPath);
			if (lpPath[len - 1] != _T('\\')) _tcscat_s(tempPath, _T("\\"));
			_tcscat_s(tempPath, fd.cFileName);

			bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

			//如果是.或..  
			if (bIsDirectory
				&& (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0))
			{
				bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
				continue;
			}

			if ( bEnumFiles != bIsDirectory)
			{				
				if ( bEnumFiles == TRUE){
					LPCTSTR pdot;
					if( (lpExName == 0)
						|| (*lpExName == 0)
						|| ( lpExName && (pdot = _tcsrchr(tempPath, _T('.')))  &&  _tcsicmp(pdot, lpExName) == 0) )
					{
						size_t _nii = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
						LPTSTR p = new TCHAR[_nii];
						_tcscpy_s(p,_nii,(bJustStoreEndName ? fd.cFileName : tempPath));
						m_vtInnerStringVector_.push_back(p);
					}
				}
				else {
					if(lpExName == NULL){
						size_t _nii = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
						LPTSTR p = new TCHAR[_nii];
						_tcscpy_s(p,_nii,(bJustStoreEndName ? fd.cFileName : tempPath));
						m_vtInnerStringVector_.push_back(p);
					}else{
						LPCTSTR pdot;
						if( *lpExName == 0 || ((pdot = _tcsrchr(tempPath, _T('\\')))  &&  _tcsicmp(pdot+1, lpExName) == 0) )
						{
							size_t _nii = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
							LPTSTR p = new TCHAR[_nii];
							_tcscpy_s(p,_nii,(bJustStoreEndName ? fd.cFileName : tempPath));
							m_vtInnerStringVector_.push_back(p);
						}
					}
				}
			}

			if (bIsDirectory && bRecursion) //是子目录  
			{
				fileEnumeration(m_vtInnerStringVector_, tempPath, lpExName,bRecursion, bEnumFiles,FALSE);
			}

			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
		}

		::FindClose(hFindFile);

		//-------------------------------------------------------------------------  
	}
	catch (...) { return; }
}
const std::vector<LPCTSTR>&  CLString::fileEnumeration(LPCTSTR lpPath,LPCTSTR lpExName,BOOL bRecursion, 
	BOOL bEnumFiles,BOOL bClearVectorBefoteStart,BOOL bJustStoreEndName){	
		(makeDEx()->m_curSplitOutIndex_) = -1;
	if(!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	if(bClearVectorBefoteStart)
		clearInnerStringVector();
	try {
		//-------------------------------------------------------------------------  	

		LONG_PTR len = std::_tcslen(lpPath);
		if (lpPath == NULL || len <= 0) 
			return *(makeDEx()->m_vtInnerStringVector_);

		TCHAR path[MAX_PATH] = {0};
		_tcscpy_s(path, lpPath);
		if (lpPath[len - 1] != _T('\\')) _tcscat_s(path, _T("\\"));
		_tcscat_s(path, _T("*"));

		WIN32_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFile(path, &fd);
		if (hFindFile == INVALID_HANDLE_VALUE)
		{
			::FindClose(hFindFile); 
			return *(makeDEx()->m_vtInnerStringVector_);
		}

		TCHAR tempPath[MAX_PATH]; 
		BOOL bUserReture = TRUE; 
		BOOL bIsDirectory;

		BOOL bFinish = FALSE;
		while (!bFinish)
		{
			_tcscpy_s(tempPath, lpPath);
			if (lpPath[len - 1] != _T('\\')) _tcscat_s(tempPath, _T("\\"));
			_tcscat_s(tempPath, fd.cFileName);

			bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

			//如果是.或..  
			if (bIsDirectory
				&& (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0))
			{
				bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
				continue;
			}

			if ( bEnumFiles != bIsDirectory)
			{				
				if ( bEnumFiles == TRUE){
					LPCTSTR pdot;
					if( (lpExName == 0)
						|| (*lpExName == 0)
						|| ( lpExName && (pdot = _tcsrchr(tempPath, _T('.')))  &&  _tcsicmp(pdot, lpExName) == 0) )
					{						
						size_t _iin = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
						LPTSTR p = _newbuffer(_iin,(makeDEx()->m_vtInnerStringVector_)->size());
						_tcscpy_s(p,_iin,(bJustStoreEndName ? fd.cFileName : tempPath));
						(makeDEx()->m_vtInnerStringVector_)->push_back(p);
					}
				}
				else {
					if(lpExName == NULL){
						size_t _iin = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
						LPTSTR p = _newbuffer(_iin,(makeDEx()->m_vtInnerStringVector_)->size());
						_tcscpy_s(p,_iin,(bJustStoreEndName ? fd.cFileName : tempPath));
						(makeDEx()->m_vtInnerStringVector_)->push_back(p);
					}else{
						LPCTSTR pdot;
						if( *lpExName == 0 || ((pdot = _tcsrchr(tempPath, _T('\\')))  &&  _tcsicmp(pdot+1, lpExName) == 0) )
						{
							size_t _iin = (bJustStoreEndName ? std::_tcslen(fd.cFileName)+1 : MAX_PATH);
							LPTSTR p = _newbuffer(_iin,(makeDEx()->m_vtInnerStringVector_)->size());
							_tcscpy_s(p,_iin,(bJustStoreEndName ? fd.cFileName : tempPath));
							(makeDEx()->m_vtInnerStringVector_)->push_back(p);
						}
					}
				}
			}

			if (bIsDirectory && bRecursion) //是子目录  
			{
				fileEnumeration(tempPath, lpExName,bRecursion, bEnumFiles,FALSE);
			}

			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
		}

		::FindClose(hFindFile);

		//-------------------------------------------------------------------------  
	}
	catch (...) {}
	return *(makeDEx()->m_vtInnerStringVector_);
}
void CLString::clearStringVector(std::vector<LPCTSTR>& _v_str)
{
	for (size_t i = 0 ; i < _v_str.size() ; i++)
	{
		delete[] _v_str.at(i);
	}
	_v_str.clear();
}
void CLString::clearInnerStringVector()
{
	(makeDEx()->m_curSplitOutIndex_) = -1;
	if((makeDEx()->m_vtInnerStringVector_)) {
		(makeDEx()->m_vtInnerStringVector_)->clear();
	}
}
void CLString::clearInnerStringVectorMemery()
{
	(makeDEx()->m_curSplitOutIndex_) = -1;
	if((makeDEx()->m_vtInnerStringVector_)) {
		(makeDEx()->m_vtInnerStringVector_)->clear();
	}
	if((makeDEx()->m_vtStringStoreBufSizeLst_)){
		(makeDEx()->m_vtStringStoreBufSizeLst_)->clear();
	}
	if((makeDEx()->m_vtStringStoreLst_)){
		clearStringVector((std::vector<LPCTSTR>&)*(makeDEx()->m_vtStringStoreLst_));
	}
}

byte CLString::checkTextFileEncode(HANDLE hFile,BOOL isMovePinterToBeging)
{
	byte c[3] = {0};
	DWORD nNum = 0;
	byte ret = EnCode_UNKNOWN;

	LARGE_INTEGER li = {0},liN = {0};
	isMovePinterToBeging ? 0 : SetFilePointerEx(hFile,li,&liN,FILE_CURRENT);

	SetFilePointer(hFile,0,0,FILE_BEGIN);
	for (int i = 0;i<3;i++)
		ReadFile(hFile,&c[i],1,&nNum,0);

	if( c[0] == 0xEF && c[1] == 0xBB && c[2] == 0xBF )
	{
		ret = EnCode_UTF8;
		isMovePinterToBeging ? SetFilePointer(hFile,3,0,FILE_BEGIN) : 0 ;
	}
	else if( c[0] == 0xFF && c[1] == 0xFE)
	{
		ret = EnCode_ULE;
		isMovePinterToBeging ? SetFilePointer(hFile,2,0,FILE_BEGIN) : 0 ;
	}
	else if( c[0] == 0xFE && c[1] == 0xFF)
	{
		ret = EnCode_UBE;
		isMovePinterToBeging ? SetFilePointer(hFile,2,0,FILE_BEGIN) : 0 ;
	}
	else if( c[0] != 0xFE && c[0] != 0xFF && c[0] != 0xEF)
	{
		ret = EnCode_ASCII;
		isMovePinterToBeging ? SetFilePointer(hFile,0,0,FILE_BEGIN) : 0 ;
	}
	else{
		isMovePinterToBeging ? SetFilePointer(hFile,0,0,FILE_BEGIN) : 0 ;
	}
	isMovePinterToBeging ? 0 : SetFilePointerEx(hFile,liN,&li,FILE_BEGIN);

	return ret;
}
BOOL CLString::swapEncode(const LPBYTE lpResbuf,LONG_PTR nResbufSizeInByte,byte uResEncode, LPBYTE &lpOutTagbuf,LONG_PTR &nOutTagbufSizeInByte,byte uTagEncode,LPBYTE lpDefaultBuf,LONG_PTR nDefaultBufSizeInByte)
{
	assert( lpResbuf != NULL );
	assert( uResEncode <= EnCode_UBE &&  uTagEncode <= EnCode_UBE && uResEncode != uTagEncode);
	assert( lpResbuf != lpDefaultBuf );

	if(uResEncode == EnCode_ASCII && uTagEncode == EnCode_ULE ){
		LONG_PTR n = ::MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpResbuf,nResbufSizeInByte,0,0) + 1;
		LPWSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpResbuf,nResbufSizeInByte,p,n) > 0)
		{lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(WCHAR);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR)) delete[] p; 		
	}
	else if(uResEncode == EnCode_ASCII && uTagEncode == EnCode_UBE ){
		LONG_PTR n = ::MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpResbuf,nResbufSizeInByte,0,0) + 1;
		LPWSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::MultiByteToWideChar(CP_ACP,0,(LPCSTR)lpResbuf,nResbufSizeInByte,p,n) > 0){
			for(LONG_PTR i = 0; i < n; *(p+i)=htons(*(p+i)),i++);lpOutTagbuf = (LPBYTE)p;
			nOutTagbufSizeInByte = n*sizeof(WCHAR);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(WCHAR)) delete[] p; 
	}
	else if(uResEncode == EnCode_ASCII && uTagEncode == EnCode_UTF8 ){
		LPBYTE p1;
		LONG_PTR nSize;
		if (!swapEncode(lpResbuf,nResbufSizeInByte,EnCode_ASCII,p1,nSize,EnCode_ULE))
			return FALSE;
		LONG_PTR n = ::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)p1,nSize/sizeof(WCHAR),0,0,0,0) + 1;
		LPSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(char)) ? (new char[n]) : (LPSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)p1,nSize/sizeof(WCHAR),p,n,0,0) > 0)
		{ delete[] p1;lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(char);return TRUE;}
		else { delete[] p1;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)(LONG_PTR)sizeof(char)) delete[] p; 
	}
	else if(uResEncode == EnCode_ULE && uTagEncode == EnCode_ASCII ){
		LONG_PTR n = ::WideCharToMultiByte(CP_ACP,0,(LPWSTR)lpResbuf,nResbufSizeInByte/sizeof(WCHAR),0,0,0,0) + 1;
		LPSTR p =(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(char)) ? (new char[n]) : (LPSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::WideCharToMultiByte(CP_ACP,0,(LPWSTR)lpResbuf,nResbufSizeInByte/sizeof(WCHAR),p,n,0,0) > 0)
		{lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(char);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(char))delete[] p; 
	}
	else if(uResEncode == EnCode_ULE && uTagEncode == EnCode_UBE ){
		LONG_PTR n = nResbufSizeInByte/sizeof(WCHAR)+1;
		LPWSTR p =(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(n > 1)
		{for(LONG_PTR i = 0; i < n; *(p+i)=htons(*(((LPCWSTR)lpResbuf)+i)),i++);
		lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(WCHAR);return TRUE;}
		if (!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(WCHAR))delete[] p; 
	}
	else if(uResEncode == EnCode_ULE && uTagEncode == EnCode_UTF8 ){
		LONG_PTR n = ::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)lpResbuf,nResbufSizeInByte/sizeof(WCHAR),0,0,0,0) + 1;
		LPSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(char)) ? (new char[n]) : (LPSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)lpResbuf,nResbufSizeInByte/sizeof(WCHAR),p,n,0,0) > 0)
		{lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(char);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < (LONG_PTR)n*(LONG_PTR)sizeof(char))delete[] p; 
	}
	else if(uResEncode == EnCode_UBE && uTagEncode == EnCode_ASCII ){
		LPBYTE p1;
		LONG_PTR nSize;
		if (!swapEncode(lpResbuf,nResbufSizeInByte,EnCode_UBE,p1,nSize,EnCode_ULE))
			return FALSE;
		int n = ::WideCharToMultiByte(CP_ACP,0,(LPWSTR)p1,nSize/sizeof(WCHAR),0,0,0,0) + 1;
		LPSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(char)) ? (new char[n]) : (LPSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::WideCharToMultiByte(CP_ACP,0,(LPWSTR)p1,nSize/sizeof(WCHAR),p,n,0,0) > 0)
		{ delete[] p1;lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(char);return TRUE;}
		else { delete[] p1;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(char))delete[] p; 
	}
	else if(uResEncode == EnCode_UBE && uTagEncode == EnCode_ULE ){
		LONG_PTR n = nResbufSizeInByte/sizeof(WCHAR)+1;
		LPWSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(n > 1)
		{for(LONG_PTR i = 0; i < n; *(p+i)=ntohs(*(((LPCWSTR)lpResbuf)+i)),i++);
		lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(WCHAR);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR))delete[] p; 
	}
	else if(uResEncode == EnCode_UBE && uTagEncode == EnCode_UTF8 ){
		LPBYTE p1;
		LONG_PTR nSize;
		if (!swapEncode(lpResbuf,nResbufSizeInByte,EnCode_UBE,p1,nSize,EnCode_ULE))
			return FALSE;
		int n = ::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)p1,nSize/sizeof(WCHAR),0,0,0,0) + 1;
		LPSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(char)) ? (new char[n]) : (LPSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::WideCharToMultiByte(CP_UTF8,0,(LPWSTR)p1,nSize/sizeof(WCHAR),p,n,0,0) > 0)
		{ delete[] p1;lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(char);return TRUE;}
		else { delete[] p1;}
		if (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(char))delete[] p; 
	}
	else if(uResEncode == EnCode_UTF8 && uTagEncode == EnCode_ASCII ){
		LPBYTE p1;
		LONG_PTR nSize;
		if (!swapEncode(lpResbuf,nResbufSizeInByte,EnCode_UTF8,p1,nSize,EnCode_ULE))
			return FALSE;
		if(!swapEncode(p1,nSize,EnCode_ULE,lpOutTagbuf,nOutTagbufSizeInByte,EnCode_ASCII,lpDefaultBuf,nDefaultBufSizeInByte))
			{delete[] p1; return FALSE;}
		else {delete[] p1;return TRUE;}
	}
	else if(uResEncode == EnCode_UTF8 && uTagEncode == EnCode_ULE ){
		LONG_PTR n = ::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)lpResbuf,nResbufSizeInByte,0,0) + 1;
		LPWSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)lpResbuf,nResbufSizeInByte,p,n) > 0)
		{lpOutTagbuf = (LPBYTE)p;nOutTagbufSizeInByte = n*sizeof(WCHAR);return TRUE;}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR))delete[] p; 
	}
	else if(uResEncode == EnCode_UTF8 && uTagEncode == EnCode_UBE ){
		LONG_PTR n = ::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)lpResbuf,nResbufSizeInByte,0,0) + 1;
		LPWSTR p = (!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR)) ? (new WCHAR[n]) : (LPWSTR)lpDefaultBuf;
		*(p + n - 1) = 0;
		if(::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)lpResbuf,nResbufSizeInByte,p,n) > 0)
		{
			for(LONG_PTR i = 0; i < n; *(p+i)=htons(*(p+i)),i++);
			lpOutTagbuf = (LPBYTE)p;
			nOutTagbufSizeInByte = n*sizeof(WCHAR);
			return TRUE;
		}
		if(!lpDefaultBuf || nDefaultBufSizeInByte < n*(LONG_PTR)sizeof(WCHAR)) delete[] p; 
	}

	lpOutTagbuf = NULL ;
	nOutTagbufSizeInByte = 0;
	return FALSE;
}

std::vector<LPCTSTR>& CLString::split(const TCHAR szSplitChar,BOOL bClearVectorBefoteStart)
{
	(makeDEx()->m_curSplitOutIndex_) = -1;
	if(bClearVectorBefoteStart)
		clearInnerStringVector();
	LONG_PTR si = strlen()+1;
	LPCTSTR t1 = pHead,t2 = NULL;
	LONG_PTR nLeft = si;
	if(!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	while (t2 = _tcschr(t1,szSplitChar))
	{
		si = t2 - t1;
		//LPTSTR pNew = new TCHAR[si+1];
		LPTSTR pNew = _newbuffer(si+1,(makeDEx()->m_vtInnerStringVector_)->size());
		pNew[si] = 0;
		memcpy_s(pNew,(si+1)*sizeof(TCHAR),t1,si*sizeof(TCHAR));
		(makeDEx()->m_vtInnerStringVector_)->push_back(pNew);
		t2++;
		t1 = t2;
		nLeft = nLeft - (si + 1);
	}
	if(*t1 != 0){
		//LPTSTR pNew = new TCHAR[nLeft];
		LPTSTR pNew = _newbuffer(nLeft,(makeDEx()->m_vtInnerStringVector_)->size());
		pNew[nLeft-1] = 0;
		memcpy_s(pNew,nLeft*sizeof(TCHAR),t1,(nLeft-1)*sizeof(TCHAR));
		(makeDEx()->m_vtInnerStringVector_)->push_back(pNew);
	}
	//delete[] p;
	return *(makeDEx()->m_vtInnerStringVector_);
}
std::vector<LPCTSTR>& CLString::split(LPCTSTR szSplitCharString,BOOL bClearVectorBefoteStart)
{
	(makeDEx()->m_curSplitOutIndex_) = -1;
	if(bClearVectorBefoteStart)
		clearInnerStringVector();
	
	LONG_PTR si = strlen()+1;
	//LPCTSTR t1 = szSplitCharString;
	LONG_PTR t1 = 0, t2 = 0;
	LONG_PTR nLeft = si;
	if(!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	if(!szSplitCharString)
		return *(makeDEx()->m_vtInnerStringVector_);
	LONG_PTR nTag = std::_tcslen(szSplitCharString);
	while ((t2 = find(szSplitCharString,t1)) >= 0)
	{
		si = t2 - t1;
		LPTSTR pNew = _newbuffer(si+1,(makeDEx()->m_vtInnerStringVector_)->size());
		pNew[si] = 0;
		memcpy_s(pNew,(si+1)*sizeof(TCHAR),pHead + (LONG_PTR)t1,si*sizeof(TCHAR));
		(makeDEx()->m_vtInnerStringVector_)->push_back(pNew);
		t1 = t2 + nTag;
		nLeft = nLeft - (si + nTag);
	}
	if(*(pHead + t1) != 0){
		//LPTSTR pNew = new TCHAR[nLeft];
		LPTSTR pNew = _newbuffer(nLeft,(makeDEx()->m_vtInnerStringVector_)->size());
		pNew[nLeft-1] = 0;
		memcpy_s(pNew,nLeft*sizeof(TCHAR),pHead + (LONG_PTR)t1,(nLeft-1)*sizeof(TCHAR));
		(makeDEx()->m_vtInnerStringVector_)->push_back(pNew);
	}
	//delete[] p;
	return *(makeDEx()->m_vtInnerStringVector_);
}
BOOL CLString::httpOpenUrl(LPCTSTR Url, BOOL openErrorAlert, LPTSTR pOutErrStr,BYTE urlEncode)/*将Url指向的地址的文件下载到save_as指向的本地文件*/
{
	assert(urlEncode > EnCode_UNKNOWN && urlEncode <= EnCode_UBE);	
	
	//byte Temp[BUFFERSIZE] = {0};	
	BOOL rs;
	try
	{
		_CHECK_START_(1.InternetOpen)
		if(!(makeDEx()->hInternet_))
			(makeDEx()->hInternet_) = InternetOpen(_T("RookIE/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			//(makeDEx()->hInternet_) = InternetOpen(_T("IE6.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		_CHECK_END_
	}
	catch(...){	goto _Final;}

	if ((makeDEx()->hInternet_) == NULL)goto _Final;

	if((makeDEx()->hConnect_)){
		InternetCloseHandle((makeDEx()->hConnect_));
			(makeDEx()->hConnect_) = NULL;
	}
	_CHECK_START_(2.InternetOpenUrl)
	(makeDEx()->hConnect_) = InternetOpenUrl((makeDEx()->hInternet_), Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	_CHECK_END_
		if ((makeDEx()->hConnect_) != NULL)
		{

#define _CMAX 65538
#define _CMAX2 65536
			_CHECK_START_(3.empty)
			empty();
			_CHECK_END_
			DWORD nFst = 0;	
			byte fst[_CMAX];
			DWORD Number = 0;
			byte temp[_CMAX];
			_CHECK_START_(4.InternetReadFile)
			while(rs = InternetReadFile((makeDEx()->hConnect_),&(fst[nFst]),_CMAX2-nFst,&Number)){				
				if(Number == 0)break;
				nFst+=Number;
				if (nFst == _CMAX2)break;
				//Number = 0;
			};
			if(rs == TRUE){
				if(nFst > 0){
					if(Number == 0){
						ZeroMemory(&(fst[nFst]),2);
						if(urlEncode != m_encoding){
							LPBYTE ptag;LONG_PTR itag;
							swapEncode(fst,nFst+2,urlEncode,ptag,itag,m_encoding,(LPBYTE)store(nFst),nFst + sizeof(TCHAR));
						}
						else append((LPCTSTR)fst);
						_CHECK_END_
						return TRUE;
					}
					else{ /*跳到长过程*/ }		//第二次采集 Number	> 0 情况		
				}
				else{ return TRUE;}
			}
			else{ return FALSE;}

			/*长过程*/
			std::vector<LPBYTE> bylst;
			std::vector<DWORD> dwlst;
			DWORD all = nFst;
			Number = 0;

			while(rs = InternetReadFile((makeDEx()->hConnect_), temp,_CMAX2, &Number)){
				if(Number == 0 )break;
				LPBYTE p = new byte[Number];
				memcpy_s(p,Number,temp,Number);
				bylst.push_back(p);
				dwlst.push_back(Number);
				all += Number;
				Number = 0;
			}
			if(rs == FALSE)return FALSE;
			else if(all == 0)return TRUE;
			size_t si = dwlst.size();
			LPBYTE p = new byte[all + 2];
			LPBYTE pc = p;
			memcpy_s(pc,nFst,fst,nFst);
			pc += nFst;
			for(size_t i=0;i<si;i++){
				memcpy_s(pc,dwlst.at(i),bylst.at(i),dwlst.at(i));
				pc += dwlst.at(i);
			}
			ZeroMemory(pc,2);
			for(size_t i =0;i<si;i++) delete[] (bylst.at(i));
			if(urlEncode != m_encoding){
				LPBYTE ptag;LONG_PTR itag;
				swapEncode(p,all+2,urlEncode,ptag,itag,m_encoding,(LPBYTE)store(all),all + sizeof(TCHAR));
			}
			else append((LPCTSTR)p);
			delete[] p;	
			_CHECK_END_
			return TRUE;
		}
_Final:
		empty();
		if(openErrorAlert)
			::MessageBox(NULL,CLString().getLastErrorString(GetLastError()),_T("HttpOpenUrl Error"),MB_ICONERROR);
		if(pOutErrStr)
			_tcscpy_s(pOutErrStr,256,CLString().getLastErrorString(GetLastError()));
		httpClose();
		return FALSE;
}
HWND CLString::getConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
	HWND hwndFound;         // This is what is returned to the caller.
	TCHAR pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
	// WindowTitle.
	TCHAR pszOldWindowTitle[MY_BUFSIZE]; // Contains original
	GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
	_stprintf_s(pszNewWindowTitle,_T("%d/%d"),GetTickCount(),GetCurrentProcessId());
	SetConsoleTitle(pszNewWindowTitle);
	Sleep(10);
	hwndFound=FindWindow(NULL, pszNewWindowTitle);
	SetConsoleTitle(pszOldWindowTitle);
	return(hwndFound);
}
BOOL CLString::isEmpty()
{
	return (strlen() == 0) ? TRUE : FALSE;
}
CLString& CLString::empty()
{
	return set((LPCTSTR)NULL);
}
TCHAR CLString::getAt(LONG_PTR nIndex) 
{
	if(nIndex > strlen() || nIndex < 0)
		return 0;
	return *(pHead + nIndex);
}
TCHAR CLString::operator[](LONG_PTR nIndex)
{
	return getAt(nIndex);
}
BOOL CLString::setAt(LONG_PTR nIndex, TCHAR ch)
{
	if( (nIndex > ((LONG_PTR)strlen() - 2 ))|| (nIndex < 0))
		return FALSE;
	(*(pHead + nIndex)) = ch;
	return TRUE;
}

CLString& CLString::mid( CLString& storeTagObj,LONG_PTR nFirst) 
{
	return mid(storeTagObj , nFirst , strlen() - nFirst);
}
CLString& CLString::mid( CLString& storeTagObj, LONG_PTR nFirst, LONG_PTR nCount ) 
{
	if(nFirst > strlen())nFirst = strlen();
	if (nFirst < 0)	nFirst = 0;
	if (nCount < 0)	nCount = 0;
	if( nFirst + nCount > strlen())nCount =  strlen() - nFirst;
	memcpy_s(storeTagObj.store(nCount),(nCount + 1)*sizeof(TCHAR),(const LPVOID)(pHead + nFirst), nCount*sizeof(TCHAR));
	return storeTagObj;
}
CLString& CLString::midSave(LONG_PTR nFirst, LONG_PTR nCount ){
	if (strlen() < nFirst)
		nFirst = strlen();
	if(nFirst < 0)
		nFirst = 0;
	if(nCount < 0)
		nCount = 0;
	if(strlen() < nFirst + nCount)
		nCount = strlen() - nFirst;
	m_changeFlag = TRUE;
	LPTSTR p = pHead;
	LONG_PTR i = 0;
	for (;i < nCount;i++,nFirst++)
		*(p+i)=*(p+nFirst);
	*(p+i) = 0;
	return *this;
}
CLString& CLString::leftSave(LONG_PTR nCount){	
	if(strlen() <= nCount)
		return *this;
	if(nCount < 0)
		nCount = 0;
	m_changeFlag =TRUE;
	*(pHead + nCount) = 0;
	return *this;
}
CLString& CLString::rightSave(LONG_PTR nCount){
	if(nCount < 0)
		nCount = 0;
	return midSave( strlen() >= nCount ? strlen() - nCount : 0 ,strlen() >= nCount ? nCount : strlen());
}
CLString& CLString::left(CLString& storeTagObj, LONG_PTR nCount)
{
	if(nCount < 0)
		nCount = 0;
	return mid(storeTagObj,0,nCount);
}
CLString& CLString::right(CLString& storeTagObj, LONG_PTR nCount)
{
	if(nCount < 0)
		nCount = 0;
	return mid(storeTagObj , strlen() - nCount,nCount);
}
CLString& CLString::makeUpper()
{
	if(strlen() <= 0)
		return *this;
	LPTSTR p = pHead;
	LONG_PTR i = 0;  
	while(*(p + i) && i < strlen())  
	{
		*(p + i) = toupper(*(p + i));
		i++;
	}
	return *this;
}
CLString& CLString::makeLower()
{
	if(strlen() <= 0)
		return *this;
	LPTSTR p = pHead;
	LONG_PTR i = 0;  
	while(*(p + i) && i < strlen())  
	{
		*(p + i) = tolower(*(p + i));
		i++;
	}
	return *this;
}
CLString& CLString::makeReverse()
{
	if(strlen() <= 0)
		return *this;
	LONG_PTR newLen = strlen() + 1;
	LPTSTR tmp = new TCHAR[newLen];//
	//ZeroMemory(tmp , newLen*sizeof(TCHAR));
	LPCTSTR p = pHead;
	for (LONG_PTR i = 0 ; i < strlen() ; i++)
	{
		*(tmp + i) = *(p + strlen() - 1 - i);
	}
	*(tmp + strlen()) = 0;
	set(tmp);
	delete[] tmp;
	return *this;
}
LONG_PTR CLString::replaceRN(TCHAR chOld, TCHAR chNew)
{
	LPTSTR p = pHead,t1 = p,t2 = t1;
	LONG_PTR i = 0;
	while( t2 = _tcschr(t1,chOld)){
		*t2 = chNew ;
		i++;		
		t2++;
		t1 = t2;
	}
	return i;
}
CLString& CLString::replace(TCHAR chOld, TCHAR chNew){
	replaceRN(chOld,chNew);
	return *this;
}
LONG_PTR CLString::replaceRN(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	if(!lpszOld || !lpszNew) 
		return 0;
	BOOL isChange = FALSE;
	LONG_PTR  pos = -1;
	CLString tmp = pHead,res,tmp2;//modify---------
	LONG_PTR i = 0, len = std::_tcslen(lpszOld);
	while( (pos = tmp.find(lpszOld)) >= 0){
		res << tmp.left(tmp2,pos);
		res << lpszNew;
		tmp.rightSave(tmp.strlen()-pos-std::_tcslen(lpszOld));
		i += len;
		isChange = TRUE;
	}
	res << tmp;
	if(isChange)
		set(res);
	return i;
}
CLString& CLString::replace(LPCTSTR lpszOld, LPCTSTR lpszNew){
	replaceRN(lpszOld,lpszNew);
	return *this;
}
LONG_PTR CLString::removeRN(TCHAR ch)
{
	LPTSTR tmp = new TCHAR[strlen()+1];//
	ZeroMemory(tmp,(strlen()+1)*sizeof(TCHAR));
	LPTSTR p = pHead;
	LONG_PTR i = 0,res = 0;
	while(*p){
		if (*p == ch)
		{
			p++;
			res++;
			continue;
		}
		*(tmp + i) = *p;
		i++;
		p++;
	}
	if(res)
		set(tmp);
	delete[] tmp;
	return res;
}
CLString& CLString::remove(TCHAR ch){
	removeRN(ch);
	return *this;
}
LONG_PTR CLString::insertRN(LONG_PTR nIndex, TCHAR ch)
{
	TCHAR tmp[] = {ch,0};
	return insertRN(nIndex,tmp);
}
CLString& CLString::insert(LONG_PTR nIndex, TCHAR ch)
{
	insertRN( nIndex,ch);
	return *this;
}
LONG_PTR CLString::insertRN(LONG_PTR  nIndex, LPCTSTR pstr)
{	
	if(nIndex > strlen())
		nIndex = strlen();
	if(nIndex < 0)
		nIndex = 0;
	CLString m_sztmp; //modify---------
	right(m_sztmp,strlen() - nIndex);
	leftSave(nIndex);
	append(pstr);
	append(m_sztmp);
	return strlen();
}
CLString& CLString::insert(LONG_PTR nIndex, LPCTSTR pstr)
{
	insertRN(nIndex, pstr);
	return *this;
}
LONG_PTR CLString::deleteCharRN(LONG_PTR nIndex, LONG_PTR nCount /*= 1*/)
{
	if(nIndex > 0 && nIndex < strlen() && nCount > 0){
		if(nCount > strlen() - nIndex)
			nCount = strlen() - nIndex;
		CLString m_sztmp; //modify---------
		right(m_sztmp,strlen() - nIndex - nCount);
		leftSave(nIndex);
		append(m_sztmp);
	}
	return strlen();
}
CLString& CLString::deleteChar(LONG_PTR nIndex, LONG_PTR nCount /*= 1*/)
{
	deleteCharRN(nIndex,nCount);
	return *this;
}
CLString& CLString::trim()
{
	return trimRight().trimLeft();
}
CLString& CLString::trim(TCHAR chTarget)
{
	return trimRight(chTarget).trimLeft(chTarget);
}
CLString& CLString::trim(LPCTSTR lpszTargets)
{
	return trimRight(lpszTargets).trimLeft(lpszTargets);
}
CLString& CLString::trimLeft()
{
	/*TCHAR tmp[] = {9,10,32};
	while ( getAt(0) == tmp[0] || getAt(0) == tmp[1] ||getAt(0) == tmp[2] )
	{
		switch (getAt(0))
		{
		case 9:			trimLeft(tmp[0]);			break;
		case 10:			trimLeft(tmp[1]);			break;
		case 32:			trimLeft(tmp[2]);			break;
		default:			break;
		}
	}*/
	TCHAR* p1 = pHead;
	while( *p1 >= 0 && *p1 <= 32 ){
		if(*p1 == 0){ break;}
		else{ p1++; }
	}
	if(p1>pHead)
	{
		size_t n =  pHead + strlen() - p1;
		size_t i =0; 
		for(;i<=n;){ pHead[i] = *p1;p1++;i++;}		
		m_changeFlag = TRUE;
	}
	return *this;
}
CLString& CLString::trimLeft(TCHAR chTarget)
{
	TCHAR tmp[] = {chTarget,0};
	while ( getAt(0) == chTarget )
	{
		trimLeft(tmp);
	}
	return *this;
}
CLString& CLString::trimLeft(LPCTSTR lpszTargets)
{
	LONG_PTR nSi =0;
	if (!lpszTargets ||  (nSi = std::_tcslen(lpszTargets)) == 0 || nSi > strlen())
		return *this;
	BOOL dif = FALSE;
	LONG_PTR  same=0,tm=0;
	for(LONG_PTR  i=0,j=0;i<strlen() && (!dif);i++){
		if( lpszTargets[j++] == pHead[i] ){
			tm++;
		}
		else dif = TRUE;
		if( j >= nSi) j=0;
		if(tm == nSi){same += nSi;tm=0;}
	}
	for (LONG_PTR  i=0,j=same;j<strlen()+1;j++,i++)
	{
		pHead[i]=pHead[j];
	}
	m_changeFlag = TRUE;
	return *this;
}
CLString& CLString::trimRight()
{
	/*TCHAR tmp[] = {9,10,32};
	while ( getAt(strlen()-1) == tmp[0] || getAt(strlen()-1) == tmp[1] ||getAt(strlen()-1) == tmp[2] )
	{
	switch (getAt(strlen()-1))
	{
	case 9:
	trimRight(tmp[0]);
	break;
	case 10:
	trimRight(tmp[1]);
	break;
	case 32:
	trimRight(tmp[2]);
	break;
	default:	
	break;
	}
	}*/
	TCHAR* p1 = pHead + strlen();
	while(p1 > pHead){
		p1--;
		if( *p1 >= 0 && *p1 <= 32){*p1=0;m_changeFlag = TRUE;}
		else return *this;
	}	
	return *this;
}
CLString& CLString::trimRight(TCHAR chTarget)
{
	TCHAR tmp[] = {chTarget,0};
	while ( getAt(strlen()-1) == chTarget )
	{
		trimRight(tmp);
	}
	return *this;
}
CLString& CLString::trimRight(LPCTSTR lpszTargets)
{
	LONG_PTR nSi =0;
	if (!lpszTargets ||  (nSi = std::_tcslen(lpszTargets)) == 0 || nSi > strlen())
		return *this;
	BOOL dif = FALSE;
	LONG_PTR  same=strlen(),tm=0;
	for(LONG_PTR i=strlen()-1,j=nSi-1;i>=0 && (!dif);i--){
		if( lpszTargets[j--] == pHead[i] ){
			tm++;
		}
		else dif = TRUE;
		if( j < 0) j=nSi-1;
		if(tm == nSi){same -= nSi;tm=0;}
	}
	pHead[same]=0;
	m_changeFlag = TRUE;
	return *this;
}
LONG_PTR CLString::find(TCHAR ch, LONG_PTR nStart) 
{
	if(nStart < 0 || nStart >= strlen())
		return -1;
	LPCTSTR p = _tcschr((pHead + nStart),ch);
	if (p)
		return (p - pHead)/sizeof(TCHAR);
	else
		return -1;
}
LONG_PTR  CLString::find(LPCTSTR lpszSub, LONG_PTR nStart) 
{
	return findString(pHead,lpszSub,nStart);
}
LONG_PTR  CLString::findString(LPCTSTR lpszTag,LPCTSTR lpszSub, LONG_PTR nStart) 
{
	if(!lpszTag)
		return -1;
	if(nStart < 0 || nStart >= (LONG_PTR)std::_tcslen(lpszTag))
		return -1;
	LPCTSTR p = lpszTag + nStart;
	LPCTSTR b = lpszSub;
	if(!lpszSub) return -1;
	LONG_PTR flag = 0;
	LONG_PTR szSublen = std::_tcslen(lpszSub);
	while(*p){
		if (*p == *b)
		{
			//LPTSTR p1 = p,*b1 = b;
			flag = 1;
			for(LONG_PTR i = 0 ; i < szSublen ; i++){
				if (*(p+i) != *(b+i))
				{
					flag = 0;
					p+=i;
					break;
				}				
			}
			if(flag == 1)
				return (p - lpszTag)/sizeof(TCHAR);
		}
		p++;
	}
	return -1;
}
LONG_PTR  CLString::reverseFind(TCHAR ch) 
{
	LPCTSTR p = _tcsrchr(pHead,ch);
	return p ? (p - pHead)/sizeof(TCHAR) : -1 ;
}
LONG_PTR  CLString::findFirstOneOf(LPCTSTR lpszCharSet) 
{
	if (!lpszCharSet)
		return -1;
	LPCTSTR p = pHead;
	LONG_PTR len = std::_tcslen(lpszCharSet);
	while (*p)
	{
		for (LONG_PTR i =0 ; i < len; i++)
		{
			if (*p == *(lpszCharSet + i))
			{
				return (p - pHead)/sizeof(TCHAR);
			}
		}
		p++;
	}
	return -1;
}
void CLString::findFlagAndSort(std::vector<TCHAR*>& sortRet, const TCHAR* lpFlagString, const TCHAR** tagStringList, size_t tagStringCounts)
{
	throw runtime_error("findFlagAndSort is not used!");
	sortRet.clear();
	if (!(tagStringList && tagStringCounts && lpFlagString))
		return;
	sortRet.resize(tagStringCounts);
}
BOOL CLString::createDirectory(LPCTSTR lpszPath)
{
	if(!lpszPath /*|| (nSi = std::_tcslen(lpszPath)) > MAX_PATH*/ )
		return FALSE;
	if(_FindFirstFileExists(lpszPath,FALSE))
		return TRUE;
	TCHAR path[MAX_PATH] = {0};
	BOOL bSuccess = TRUE;
	//BOOL bValid = FALSE;
	auto pc = lpszPath; size_t n = 0,si =0;
	TCHAR ch;
	while (ch = pc[n]) {
		if(ch == _T('/') || ch == _T('\\')){
			//bValid = TRUE;
			if (!_FindFirstFileExists(path, FALSE) && path[si - 1] != _T(':')) {
				bSuccess = (bSuccess && ::CreateDirectory(path, NULL));
				if (!bSuccess) {
					CLString str;
					auto lt = GetLastError();
					str.getLastErrorMessageBoxExceptSucceed(lt);
					return FALSE;
				}
			}
			path[si++] = _T('\\');
			while (ch = pc[n+1]) {
				if (ch == _T('/') || ch == _T('\\'))
					n++;
				else
					break;
			}
		}
		else {
			path[si++] = ch;
		}
		n++;
	}
	if (si > 0 && path[si - 1] != _T('\\')) {
		if (!_FindFirstFileExists(path, FALSE)) {
			bSuccess = (bSuccess && ::CreateDirectory(path, NULL));
			if (!bSuccess) {
				CLString str;
				auto lt = GetLastError();
				str.getLastErrorMessageBoxExceptSucceed(lt);
				return FALSE;
			}
		}
	}
	return bSuccess;
}
BOOL CLString::createDirectoryByFileName(LPCTSTR lpszFileName)
{
	if (!lpszFileName)
		return FALSE;
	auto pe = lpszFileName; size_t n = 0;
	while (pe[n])n++;
	if (n == 0)return FALSE;
	if (!((pe[n - 1] == _T('/') || pe[n - 1] == _T('\\')) || pe[n - 1] == _T(':'))) {//是文件名
		TCHAR path[MAX_PATH*10];
		TCHAR ch = pe[n - 1];
		while (n > 0 && (!(ch == _T('/') || ch == _T('\\') || ch == _T(':')))) { ch = pe[--n - 1]; }
		while (n > 0 && ((ch == _T('/') || ch == _T('\\') || ch == _T(':')))) { ch = pe[--n - 1]; }
		if (n == 0)return FALSE;
		if (n - 1 > MAX_PATH * 10)
			throw std::runtime_error("CLString::createDirectoryByFileName,fileName buf overflow!");
		_tcsncpy_s(path, lpszFileName, n); 
		return createDirectory(path);
	}
	else {//是文件夹
		return createDirectory(lpszFileName);
	}
}
BOOL CLString::createDirectory(void)
{
	return createDirectory(string());
}

const std::vector<LPCTSTR>& CLString::getVT()
{
	if (!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	return *(makeDEx()->m_vtInnerStringVector_);
}
const std::vector<LPCTSTR>& CLString::makeVtDescendingOrder()
{
	if (!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	LONG_PTR nsi = (makeDEx()->m_vtInnerStringVector_)->size();
	LPCTSTR p;
	for (LONG_PTR i = 0;i< nsi;i++)
	{
		for(LONG_PTR j = i+1;j < nsi;j++)
		{
			if(lstrcmp((makeDEx()->m_vtInnerStringVector_)->at(i),(makeDEx()->m_vtInnerStringVector_)->at(j)) < 0)
			{
				p = (makeDEx()->m_vtInnerStringVector_)->at(i);
				(makeDEx()->m_vtInnerStringVector_)->at(i) = (makeDEx()->m_vtInnerStringVector_)->at(j);
				(makeDEx()->m_vtInnerStringVector_)->at(j) = p;
			}
		}
	}
	return *(makeDEx()->m_vtInnerStringVector_);
}
const std::vector<LPCTSTR>& CLString::makeVtDescendingOrderI()
{
	if (!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	LONG_PTR nsi = (makeDEx()->m_vtInnerStringVector_)->size();
	LPCTSTR p;
	for (LONG_PTR i = 0;i< nsi;i++)
	{
		for(LONG_PTR j = i+1;j < nsi;j++)
		{
			if(lstrcmpi((makeDEx()->m_vtInnerStringVector_)->at(i),(makeDEx()->m_vtInnerStringVector_)->at(j)) < 0)
			{
				p = (makeDEx()->m_vtInnerStringVector_)->at(i);
				(makeDEx()->m_vtInnerStringVector_)->at(i) = (makeDEx()->m_vtInnerStringVector_)->at(j);
				(makeDEx()->m_vtInnerStringVector_)->at(j) = p;
			}
		}
	}
	return *(makeDEx()->m_vtInnerStringVector_);
}
const std::vector<LPCTSTR>& CLString::makeVtAscendingOrder()
{
	if (!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	LONG_PTR nsi = (makeDEx()->m_vtInnerStringVector_)->size();
	LPCTSTR p;
	for (LONG_PTR i = 0;i< nsi;i++)
	{
		for(LONG_PTR j = i+1;j < nsi;j++)
		{
			if(lstrcmp((makeDEx()->m_vtInnerStringVector_)->at(i),(makeDEx()->m_vtInnerStringVector_)->at(j)) > 0)
			{
				p = (makeDEx()->m_vtInnerStringVector_)->at(i);
				(makeDEx()->m_vtInnerStringVector_)->at(i) = (makeDEx()->m_vtInnerStringVector_)->at(j);
				(makeDEx()->m_vtInnerStringVector_)->at(j) = p;
			}
		}
	}
	return *(makeDEx()->m_vtInnerStringVector_);
}
const std::vector<LPCTSTR>& CLString::makeVtAscendingOrderI()
{
	if (!(makeDEx()->m_vtInnerStringVector_))
		(makeDEx()->m_vtInnerStringVector_) = new std::vector<LPCTSTR>;
	LONG_PTR nsi = (makeDEx()->m_vtInnerStringVector_)->size();
	LPCTSTR p;
	for (LONG_PTR i = 0;i< nsi;i++)
	{
		for(LONG_PTR j = i+1;j < nsi;j++)
		{
			if(lstrcmpi((makeDEx()->m_vtInnerStringVector_)->at(i),(makeDEx()->m_vtInnerStringVector_)->at(j)) > 0)
			{
				p = (makeDEx()->m_vtInnerStringVector_)->at(i);
				(makeDEx()->m_vtInnerStringVector_)->at(i) = (makeDEx()->m_vtInnerStringVector_)->at(j);
				(makeDEx()->m_vtInnerStringVector_)->at(j) = p;
			}
		}
	}
	return *(makeDEx()->m_vtInnerStringVector_);
}

#ifdef UNICODE
#define max_char 65536
#define max_char_1 65535
#define move_dis 857
#else
#define max_char 256
#define max_char_1 255
#define move_dis 19
#endif
int* _encrypteStringS(int * s,int* ss ,size_t n ,size_t nTimes /*= 1*/)
{
	if(nTimes >1){
		int at;int*p;
		for(size_t i = 0;i<nTimes;i++){
			p = _encrypteStringS(s,ss,n,1);
			if(i%2 == 0){
				for (size_t j=0;j < n/2;j++){
					at=p[j];
					p[j]=p[n-j-1];
					p[n-j-1]=at;
				}
			}
			s = ss;
			ss = p;
		}
		return ss;
	}
	if(nTimes <1)
		return ss;
	size_t t;
	//_tprintf_s(_T("--------------\n"));
	for(size_t i = 0;i<n;i++){
		if(i%2 == 0){
			t = (n+1)/2-i/2 -1;
			s[ t ] = (int)ss[i]+move_dis;
			s[ t ] = s[ t ] > max_char_1 ? s[ t ] - max_char_1 : s[ t ];
		}
		else{
			t = (n+1)/2 + i/2;
			s[ t ]  = (int)ss[i]-move_dis;
			s[ t ] = s[ t ] < 1 ? max_char_1 + s[ t ] : s[ t ];
		}	
		//_tprintf_s(_T("%d,"),ss[i]);
	}
	//_tprintf_s(_T("\n"));
// 	for(int i = 0;i<n;i++){
// 			_tprintf_s(_T("%d,"),s[i]);
// 		}
	//_tprintf_s(_T("\n--------------\n"));
	return s;
}
CLStringR CLString::encrypteString(CLStringR outputString,LPCTSTR inputString,size_t nDenpth /*= 1*/)
{
	outputString.empty();
	if(!inputString)
		return outputString;
	size_t nCharCounts =  std::_tcslen(inputString);
	//_tprintf_s(_T("%s\n"),inputString.string());
	int *s = new int[ nCharCounts ];
	int *ss = new int[ nCharCounts ];
	encrypteString(outputString,inputString,s,ss,nCharCounts,nDenpth);
	delete[] s;
	delete[] ss;
	//_tprintf_s(_T("%s\n"),outputString.string());
	return outputString;
}
CLStringR CLString::encrypteString(LPCTSTR inputString,size_t nDenpth /*= 1*/)
{
	return encrypteString(*this,inputString,nDenpth);
}
CLStringR CLString::encrypteString(CLStringR outputString,LPCTSTR inputString,int* s,int* ss,size_t nCharCounts,size_t nDenpth /*= 1*/)
{
	outputString.empty();
	if(!inputString)
		return outputString;
	for(size_t i=0;i<nCharCounts;s[i]=(int)inputString[i],s[i] = (s[i]<0?s[i]+max_char:s[i]),i++);
	nDenpth = nDenpth < 1 ? 1:nDenpth;
	int *p = _encrypteStringS(ss,s,nCharCounts,nDenpth+1);
	for(size_t i = 0;i<nCharCounts;i++){
		outputString << (TCHAR)(p[i]);
	}
	return outputString;
}
CLStringR CLString::encrypteString(LPCTSTR inputString,int* s,int* ss,size_t nCharCounts,size_t nDepth /*= 1*/)
{
	return encrypteString(*this,inputString,s,ss,nCharCounts,nDepth);
}
int* _unEncrypteStringS(int *s, int* ss ,size_t n ,size_t nTimes /*= 1*/)
{
	if(nTimes >1){
		int at;int*p;
		for(size_t i = nTimes-1;1;){
			if(i%2 == 0){
				for (size_t j=0;j< n/2;j++){
					at=ss[j];
					ss[j] = ss[n-j-1];
					ss[n-j-1]=at;
				}
			}
			p = _unEncrypteStringS(s,ss,n,1);			
			s = ss;
			ss = p;
			if(i==0)break;
			i--;
		}
		return ss;
	}
	if(nTimes <1)
		return ss;
	size_t t;
	//_tprintf_s(_T("--------------\n"));
	for(size_t i = 0;i<n;i++){
		if(i%2 == 0){
			t = (n+1)/2-i/2 -1;
			s[ i ] = (int)ss[t]-move_dis;
			s[ i ] = s[ i ] < 1 ? max_char_1 + s[ i ] : s[ i ];		
		}
		else{
			t = (n+1)/2 + i/2;
			s[ i ]  = (int)ss[t]+move_dis;
			s[ i ] = s[ i ] > max_char_1 ? s[ i ] - max_char_1 : s[ i ];
		}
		//_tprintf_s(_T("%d,"),ss[i]);
	}
	//_tprintf_s(_T("\n"));
// 	for(int i = 0;i<n;i++){		
// 		_tprintf_s(_T("%d,"),s[i]);
// 	}
	//_tprintf_s(_T("\n--------------\n"));
	return s;
}

bool operator<( CLStringRC str1, LPCTSTR str2 )
{
	return str2 && ((str1.string() == 0) ||  _tcscmp(str1.string(),str2) < 0);
}

bool operator<( LPCTSTR str1, CLStringRC str2 )
{
	return str2 > str1;
}

bool operator<( CLStringRC str1, CLStringRC str2 )
{
	return str1 < (str2.string());
}

bool operator<=( CLStringRC str1, LPCTSTR str2 )
{
	return (str1.string() == str2) || (str2 && ((str1.string() == 0) ||  _tcscmp(str1.string(),str2) <= 0));
}

bool operator<=( LPCTSTR str1, CLStringRC str2 )
{
	return str2 >= str1;
}

bool operator<=( CLStringRC str1, CLStringRC str2 )
{
	return str1 <= (str2.string());
}

bool operator>( CLStringRC str1, LPCTSTR str2 )
{
	return str1.string() && ((str2 == 0) ||  _tcscmp(str1.string(),str2) > 0);
}

bool operator>( LPCTSTR str1, CLStringRC str2 )
{
	return str2 < str1;
}

bool operator>( CLStringRC str1, CLStringRC str2 )
{
	return str1 > str2.string();
}

bool operator>=( CLStringRC str1, LPCTSTR str2 )
{
	return (str1.string() == str2) || (str1.string() && ((str2 == 0) ||  _tcscmp(str1.string(),str2) >= 0));
}

bool operator>=( LPCTSTR str1, CLStringRC str2 )
{
	return str2 <= str1;
}

bool operator>=( CLStringRC str1, CLStringRC str2 )
{
	return str1 >= str2.string();
}

bool operator==( CLStringRC str1, LPCTSTR str2 )
{
	return (str1.string() == str2) || (_tcscmp(str1.string(),str2) == 0) ;
}

bool operator==( LPCTSTR str1, CLStringRC str2 )
{
	return str2 == str1;
}

bool operator==( CLStringRC str1, CLStringRC str2 )
{
	return str2 == str1.string();
}

bool operator!=( CLStringRC str1, LPCTSTR str2 )
{
	return !(str1 == str2);
}

bool operator!=( LPCTSTR str1, CLStringRC str2 )
{
	return !(str2 == str1);
}

bool operator!=( CLStringRC str1, CLStringRC str2 )
{
	return !(str1 == str2.string());
}

CLStringR CLString::unEncrypteString(CLStringR outputString,LPCTSTR inputString,size_t nDenpth)
{
	outputString.empty();
	if(!inputString)
		return outputString;
	size_t nCharCounts = std::_tcslen(inputString);
	int *s = new int[nCharCounts];
	int *ss = new int[nCharCounts];
	unEncrypteString(outputString,inputString,s,ss,nCharCounts,nDenpth);
	delete[] s;
	delete[] ss;
	return outputString;
}
CLStringR CLString::unEncrypteString(LPCTSTR inputString,size_t nDenpth )
{
	return unEncrypteString(*this,inputString,nDenpth);
}
CLStringR CLString::unEncrypteString(CLStringR outputString,LPCTSTR inputString,int* s,int* ss,size_t nCharCounts,size_t nDenpth  )
{
	outputString.empty();
	if(!inputString)
		return outputString;
	for(size_t i=0;i<nCharCounts;s[i]=(int)inputString[i],s[i] = (s[i]<0?s[i]+max_char:s[i]),i++);
	nDenpth = nDenpth < 1 ? 1:nDenpth;
	int *p = _unEncrypteStringS(ss,s,nCharCounts,nDenpth+1);
	for(size_t i = 0;i<nCharCounts;i++){
		outputString << (TCHAR)(p[i]);
	}
	return outputString;
}
CLStringR CLString::unEncrypteString(LPCTSTR inputString,int* s,int* ss,size_t nCharCounts,size_t nDenpth /*= 1*/)
{
	return unEncrypteString(*this,inputString,s,ss,nCharCounts,nDenpth);
}

BOOL CLString::findStringInPair(LPCTSTR lpOrg,LPCTSTR lpBeginFlag,LPCTSTR lpEndFlag, OUT CLString::PFSIP_INF inf,size_t orgNeedCounts)
{
	LPCTSTR pc = lpOrg;
	size_t ci = 0,si,ei;
	size_t orgMaxCheckCounts = (orgNeedCounts > 0 ? orgNeedCounts : (size_t)(-1));
	byte ty = 0;BOOL rt = FALSE;
	if(inf){inf->pE2 = inf->pE1 = inf->pV2 = 0;inf->nV=inf->nE =0;}
//n1:
	while( pc && (*pc) && ci < orgMaxCheckCounts ){
		if(ty == 0){
			if( lpBeginFlag == 0){
				ty =1; 
				if(inf){inf->pV1=pc; inf->pS2 = inf->pS1 = 0;;inf->nS=0;}
			}
			else{
				si = 0;
				while( lpBeginFlag[si]  && (ci+si) < orgMaxCheckCounts){
					if( pc[si] != lpBeginFlag[si]){break;}
					si++;
				}
				if(lpBeginFlag[si] == 0){
					ty =1;
					if(si == 0){   
						if(inf){ 
							if(*pc ) 
								inf->pV1 = pc; 
							inf->pS2 = inf->pS1 = 0;
							inf->nS=0;
						}
					}
					else{
						if(inf){ 
							inf->pS1 = pc;
							inf->pS2=pc+si-1;
							inf->nS = si;							
							if(pc[si] != 0)
								inf->pV1 = pc+si;
							else 
							{
								inf->pV1 = 0;
								if(lpEndFlag == 0 || *lpEndFlag == 0)
									rt=TRUE;
							}
						}
						pc+=si;
						ci+=si;
					}
				}else{
					if(si == 0){ci++;pc++;}
					else{ pc+=si;ci+=si;}
				}
			}
		}
		else {
			if(lpEndFlag == 0){
				while( *pc !=0  && ci < orgMaxCheckCounts){ 
					if(inf){
						inf->pV2 = pc;
						inf->nV = inf->pV2 - inf->pV1+1;
					}
					ci++;pc++;
				}
				rt = TRUE;
			}
			else{
				ei = 0;
				while( lpEndFlag[ei] && (ci+ei) < orgMaxCheckCounts){
					if( pc[ei] != lpEndFlag[ei] ){break;}
					ei++;
				}
				if(lpEndFlag[ei] == 0){ //is match
					rt = TRUE;
					if(ei == 0){
						if(inf){ //store data	
							inf->pV2 = pc;
							inf->nV = inf->pV2 - inf->pV1 +1;
						}
						pc++;ci++;
					}
					else{
						if(inf){							
							inf->pE1 = pc;
							inf->pE2 = pc+ei-1;
							inf->nE = ei;
							if(inf->pE1 && inf->pS2){
								if(inf->pE1 > inf->pS2+1){
									inf->pV2 = pc -1;
									inf->nV = inf->pV2 - inf->pV1+1;
								}else{ inf->pV1 = 0;}
							}
							else if(inf->pS2 == 0 && inf->pE1){
								if(inf->pE1 > lpOrg){
									inf->pV2 = pc -1;
									inf->nV = inf->pV2 - inf->pV1+1;
								}else{ inf->pV1 = 0;}
							}else inf->pV1 = 0;
						}
						//pc+=ei;ci+=ei;
						break;
					}					
				}	
				else{
 					if(ei == 0){
						pc++;ci++;
					}
					else{
						pc+=ei;ci+=ei;
					}
				}				
			}//end else
		}// end else
	}
	if(!rt && inf){inf->pS1 = inf->pS2 = inf->pV1 = 0;inf->nS=0;}
	return rt;
}

BOOL CLString::findStringInPair(LPCTSTR lpBeginFlag,LPCTSTR lpEndFlag,OUT PFSIP_INF pInfOut ,size_t orgMaxCheckCounts){
	return CLString::findStringInPair( this->string(), lpBeginFlag, lpEndFlag, pInfOut, orgMaxCheckCounts);
}

CLString& CLString::setn( LPCTSTR pStr,LONG_PTR nSiChar )
{
	assert( pStr != NULL );
	store(nSiChar = (nSiChar < 0 ? 0 : nSiChar) );
	for(LONG_PTR i=0; (pHead[i] = pStr[i]) != 0 && i < nSiChar; i++);
	pHead[nSiChar]=0;
	return *this;
}

//全局函数：取得windows消息码所对应的消息定义字符串，Release模式下返回一行表述函数已弃用的提示字符串，而不是NULL指针。
LPCTSTR CLString::getWindowsMsgStringFromMsgID(UINT messageId){

#define ITERN_WNDDOWSMSGID2STRING( szName , nID ) case (nID):return _T(#szName);break
	switch(messageId){
ITERN_WNDDOWSMSGID2STRING( WM_NULL                         , 0x0000
);ITERN_WNDDOWSMSGID2STRING( WM_CREATE                       , 0x0001
);ITERN_WNDDOWSMSGID2STRING( WM_DESTROY                      , 0x0002
);ITERN_WNDDOWSMSGID2STRING( WM_MOVE                         , 0x0003
);ITERN_WNDDOWSMSGID2STRING( WM_SIZE                         , 0x0005
);ITERN_WNDDOWSMSGID2STRING( WM_ACTIVATE                     , 0x0006
);ITERN_WNDDOWSMSGID2STRING( WM_SETFOCUS                     , 0x0007
);ITERN_WNDDOWSMSGID2STRING( WM_KILLFOCUS                    , 0x0008
);ITERN_WNDDOWSMSGID2STRING( WM_ENABLE                       , 0x000A
);ITERN_WNDDOWSMSGID2STRING( WM_SETREDRAW                    , 0x000B
);ITERN_WNDDOWSMSGID2STRING( WM_SETTEXT                      , 0x000C
);ITERN_WNDDOWSMSGID2STRING( WM_GETTEXT                      , 0x000D
);ITERN_WNDDOWSMSGID2STRING( WM_GETTEXTLENGTH                , 0x000E
);ITERN_WNDDOWSMSGID2STRING( WM_PAINT                        , 0x000F
);ITERN_WNDDOWSMSGID2STRING( WM_CLOSE                        , 0x0010);
#ifndef _WIN32_WCE
ITERN_WNDDOWSMSGID2STRING( WM_QUERYENDSESSION              , 0x0011
);ITERN_WNDDOWSMSGID2STRING( WM_QUERYOPEN                    , 0x0013
);ITERN_WNDDOWSMSGID2STRING( WM_ENDSESSION                   , 0x0016);
#endif
ITERN_WNDDOWSMSGID2STRING( WM_QUIT                         , 0x0012
);ITERN_WNDDOWSMSGID2STRING( WM_ERASEBKGND                   , 0x0014
);ITERN_WNDDOWSMSGID2STRING( WM_SYSCOLORCHANGE               , 0x0015
);ITERN_WNDDOWSMSGID2STRING( WM_SHOWWINDOW                   , 0x0018
);ITERN_WNDDOWSMSGID2STRING( WM_WININICHANGE                 , 0x001A
);ITERN_WNDDOWSMSGID2STRING( WM_DEVMODECHANGE                , 0x001B
);ITERN_WNDDOWSMSGID2STRING( WM_ACTIVATEAPP                  , 0x001C
);ITERN_WNDDOWSMSGID2STRING( WM_FONTCHANGE                   , 0x001D
);ITERN_WNDDOWSMSGID2STRING( WM_TIMECHANGE                   , 0x001E
);ITERN_WNDDOWSMSGID2STRING( WM_CANCELMODE                   , 0x001F
);ITERN_WNDDOWSMSGID2STRING( WM_SETCURSOR                    , 0x0020
);ITERN_WNDDOWSMSGID2STRING( WM_MOUSEACTIVATE                , 0x0021
);ITERN_WNDDOWSMSGID2STRING( WM_CHILDACTIVATE                , 0x0022
);ITERN_WNDDOWSMSGID2STRING( WM_QUEUESYNC                    , 0x0023
);ITERN_WNDDOWSMSGID2STRING( WM_GETMINMAXINFO                , 0x0024
);ITERN_WNDDOWSMSGID2STRING( WM_PAINTICON                    , 0x0026
);ITERN_WNDDOWSMSGID2STRING( WM_ICONERASEBKGND               , 0x0027
);ITERN_WNDDOWSMSGID2STRING( WM_NEXTDLGCTL                   , 0x0028
);ITERN_WNDDOWSMSGID2STRING( WM_SPOOLERSTATUS                , 0x002A
);ITERN_WNDDOWSMSGID2STRING( WM_DRAWITEM                     , 0x002B
);ITERN_WNDDOWSMSGID2STRING( WM_MEASUREITEM                  , 0x002C
);ITERN_WNDDOWSMSGID2STRING( WM_DELETEITEM                   , 0x002D
);ITERN_WNDDOWSMSGID2STRING( WM_VKEYTOITEM                   , 0x002E
);ITERN_WNDDOWSMSGID2STRING( WM_CHARTOITEM                   , 0x002F
);ITERN_WNDDOWSMSGID2STRING( WM_SETFONT                      , 0x0030
);ITERN_WNDDOWSMSGID2STRING( WM_GETFONT                      , 0x0031
);ITERN_WNDDOWSMSGID2STRING( WM_SETHOTKEY                    , 0x0032
);ITERN_WNDDOWSMSGID2STRING( WM_GETHOTKEY                    , 0x0033
);ITERN_WNDDOWSMSGID2STRING( WM_QUERYDRAGICON                , 0x0037
);ITERN_WNDDOWSMSGID2STRING( WM_COMPAREITEM                  , 0x0039);
#if(WINVER >=  0x0500)
#ifndef _WIN32_WCE
ITERN_WNDDOWSMSGID2STRING( WM_GETOBJECT                    , 0x003D);
#endif
#endif /* WINVER >=  0x0500 */
ITERN_WNDDOWSMSGID2STRING( WM_COMPACTING                   , 0x0041
);ITERN_WNDDOWSMSGID2STRING( WM_COMMNOTIFY                   , 0x0044
);ITERN_WNDDOWSMSGID2STRING( WM_WINDOWPOSCHANGING            , 0x0046
);ITERN_WNDDOWSMSGID2STRING( WM_WINDOWPOSCHANGED             , 0x0047
);ITERN_WNDDOWSMSGID2STRING( WM_POWER                        , 0x0048
);ITERN_WNDDOWSMSGID2STRING( WM_COPYDATA                     , 0x004A
);ITERN_WNDDOWSMSGID2STRING( WM_CANCELJOURNAL                , 0x004B);
#if(WINVER >=  0x0400)
ITERN_WNDDOWSMSGID2STRING( WM_NOTIFY                       , 0x004E
);ITERN_WNDDOWSMSGID2STRING( WM_INPUTLANGCHANGEREQUEST       , 0x0050
);ITERN_WNDDOWSMSGID2STRING( WM_INPUTLANGCHANGE              , 0x0051
);ITERN_WNDDOWSMSGID2STRING( WM_TCARD                        , 0x0052
);ITERN_WNDDOWSMSGID2STRING( WM_HELP                         , 0x0053
);ITERN_WNDDOWSMSGID2STRING( WM_USERCHANGED                  , 0x0054
);ITERN_WNDDOWSMSGID2STRING( WM_NOTIFYFORMAT                 , 0x0055
);ITERN_WNDDOWSMSGID2STRING( WM_CONTEXTMENU                  , 0x007B
);ITERN_WNDDOWSMSGID2STRING( WM_STYLECHANGING                , 0x007C
);ITERN_WNDDOWSMSGID2STRING( WM_STYLECHANGED                 , 0x007D
);ITERN_WNDDOWSMSGID2STRING( WM_DISPLAYCHANGE                , 0x007E
);ITERN_WNDDOWSMSGID2STRING( WM_GETICON                      , 0x007F
);ITERN_WNDDOWSMSGID2STRING( WM_SETICON                      , 0x0080);
#endif /* WINVER >=  0x0400 */

ITERN_WNDDOWSMSGID2STRING( WM_NCCREATE                     , 0x0081
);ITERN_WNDDOWSMSGID2STRING( WM_NCDESTROY                    , 0x0082
);ITERN_WNDDOWSMSGID2STRING( WM_NCCALCSIZE                   , 0x0083
);ITERN_WNDDOWSMSGID2STRING( WM_NCHITTEST                    , 0x0084
);ITERN_WNDDOWSMSGID2STRING( WM_NCPAINT                      , 0x0085
);ITERN_WNDDOWSMSGID2STRING( WM_NCACTIVATE                   , 0x0086
);ITERN_WNDDOWSMSGID2STRING( WM_GETDLGCODE                   , 0x0087);
#ifndef _WIN32_WCE
ITERN_WNDDOWSMSGID2STRING( WM_SYNCPAINT                    , 0x0088);
#endif
ITERN_WNDDOWSMSGID2STRING( WM_NCMOUSEMOVE                  , 0x00A0
);ITERN_WNDDOWSMSGID2STRING( WM_NCLBUTTONDOWN                , 0x00A1
);ITERN_WNDDOWSMSGID2STRING( WM_NCLBUTTONUP                  , 0x00A2
);ITERN_WNDDOWSMSGID2STRING( WM_NCLBUTTONDBLCLK              , 0x00A3
);ITERN_WNDDOWSMSGID2STRING( WM_NCRBUTTONDOWN                , 0x00A4
);ITERN_WNDDOWSMSGID2STRING( WM_NCRBUTTONUP                  , 0x00A5
);ITERN_WNDDOWSMSGID2STRING( WM_NCRBUTTONDBLCLK              , 0x00A6
);ITERN_WNDDOWSMSGID2STRING( WM_NCMBUTTONDOWN                , 0x00A7
);ITERN_WNDDOWSMSGID2STRING( WM_NCMBUTTONUP                  , 0x00A8
);ITERN_WNDDOWSMSGID2STRING( WM_NCMBUTTONDBLCLK              , 0x00A9);
#if(_WIN32_WINNT >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_NCXBUTTONDOWN                , 0x00AB
);ITERN_WNDDOWSMSGID2STRING( WM_NCXBUTTONUP                  , 0x00AC
);ITERN_WNDDOWSMSGID2STRING( WM_NCXBUTTONDBLCLK              , 0x00AD);
#endif /* _WIN32_WINNT >=  0x0500 */


#if(_WIN32_WINNT >=  0x0501)
 ITERN_WNDDOWSMSGID2STRING( WM_INPUT_DEVICE_CHANGE          , 0x00FE);
#endif /* _WIN32_WINNT >=  0x0501 */

#if(_WIN32_WINNT >=  0x0501)
ITERN_WNDDOWSMSGID2STRING( WM_INPUT                        , 0x00FF);
#endif /* _WIN32_WINNT >=  0x0501 */

ITERN_WNDDOWSMSGID2STRING( WM_KEYFIRST / WM_KEYDOWN          , 0x0100
);ITERN_WNDDOWSMSGID2STRING( WM_KEYUP                        , 0x0101
);ITERN_WNDDOWSMSGID2STRING( WM_CHAR                         , 0x0102
);ITERN_WNDDOWSMSGID2STRING( WM_DEADCHAR                     , 0x0103
);ITERN_WNDDOWSMSGID2STRING( WM_SYSKEYDOWN                   , 0x0104
);ITERN_WNDDOWSMSGID2STRING( WM_SYSKEYUP                     , 0x0105
);ITERN_WNDDOWSMSGID2STRING( WM_SYSCHAR                      , 0x0106
);ITERN_WNDDOWSMSGID2STRING( WM_SYSDEADCHAR                  , 0x0107);
#if(_WIN32_WINNT >=  0x0501)
ITERN_WNDDOWSMSGID2STRING( WM_UNICHAR / WM_KEYLAST           , 0x0109);
#else
ITERN_WNDDOWSMSGID2STRING( WM_KEYLAST                      , 0x0108
);
#endif /* _WIN32_WINNT >=  0x0501 */

#if(WINVER >=  0x0400)
ITERN_WNDDOWSMSGID2STRING( WM_IME_STARTCOMPOSITION         , 0x010D
);ITERN_WNDDOWSMSGID2STRING( WM_IME_ENDCOMPOSITION           , 0x010E
);ITERN_WNDDOWSMSGID2STRING( WM_IME_COMPOSITION / WM_IME_KEYLAST , 0x010F
);
#endif /* WINVER >=  0x0400 */

ITERN_WNDDOWSMSGID2STRING( WM_INITDIALOG                   , 0x0110
);ITERN_WNDDOWSMSGID2STRING( WM_COMMAND                      , 0x0111
);ITERN_WNDDOWSMSGID2STRING( WM_SYSCOMMAND                   , 0x0112
);ITERN_WNDDOWSMSGID2STRING( WM_TIMER                        , 0x0113
);ITERN_WNDDOWSMSGID2STRING( WM_HSCROLL                      , 0x0114
);ITERN_WNDDOWSMSGID2STRING( WM_VSCROLL                      , 0x0115
);ITERN_WNDDOWSMSGID2STRING( WM_INITMENU                     , 0x0116
);ITERN_WNDDOWSMSGID2STRING( WM_INITMENUPOPUP                , 0x0117
);
#if(WINVER >=  0x0601)
ITERN_WNDDOWSMSGID2STRING( WM_GESTURE                      , 0x0119
);ITERN_WNDDOWSMSGID2STRING( WM_GESTURENOTIFY                , 0x011A
);
#endif /* WINVER >=  0x0601 */
ITERN_WNDDOWSMSGID2STRING( WM_MENUSELECT                   , 0x011F
);ITERN_WNDDOWSMSGID2STRING( WM_MENUCHAR                     , 0x0120
);ITERN_WNDDOWSMSGID2STRING( WM_ENTERIDLE                    , 0x0121
);
#if(WINVER >=  0x0500)
#ifndef _WIN32_WCE
ITERN_WNDDOWSMSGID2STRING( WM_MENURBUTTONUP                , 0x0122
);ITERN_WNDDOWSMSGID2STRING( WM_MENUDRAG                     , 0x0123
);ITERN_WNDDOWSMSGID2STRING( WM_MENUGETOBJECT                , 0x0124
);ITERN_WNDDOWSMSGID2STRING( WM_UNINITMENUPOPUP              , 0x0125
);ITERN_WNDDOWSMSGID2STRING( WM_MENUCOMMAND                  , 0x0126);

#ifndef _WIN32_WCE
#if(_WIN32_WINNT >=  0x0500)
ITERN_WNDDOWSMSGID2STRING( WM_CHANGEUISTATE                , 0x0127
);ITERN_WNDDOWSMSGID2STRING( WM_UPDATEUISTATE                , 0x0128
);ITERN_WNDDOWSMSGID2STRING( WM_QUERYUISTATE                 , 0x0129
);
#endif /* _WIN32_WINNT >=  0x0500 */
#endif

#endif
#endif /* WINVER >=  0x0500 */

ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORMSGBOX               , 0x0132
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLOREDIT                 , 0x0133
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORLISTBOX              , 0x0134
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORBTN                  , 0x0135
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORDLG                  , 0x0136
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORSCROLLBAR            , 0x0137
);ITERN_WNDDOWSMSGID2STRING( WM_CTLCOLORSTATIC               , 0x0138
);ITERN_WNDDOWSMSGID2STRING( WM_MOUSEFIRST / WM_MOUSEMOVE    , 0x0200
);ITERN_WNDDOWSMSGID2STRING( WM_LBUTTONDOWN                  , 0x0201
);ITERN_WNDDOWSMSGID2STRING( WM_LBUTTONUP                    , 0x0202
);ITERN_WNDDOWSMSGID2STRING( WM_LBUTTONDBLCLK                , 0x0203
);ITERN_WNDDOWSMSGID2STRING( WM_RBUTTONDOWN                  , 0x0204
);ITERN_WNDDOWSMSGID2STRING( WM_RBUTTONUP                    , 0x0205
);ITERN_WNDDOWSMSGID2STRING( WM_RBUTTONDBLCLK                , 0x0206
);ITERN_WNDDOWSMSGID2STRING( WM_MBUTTONDOWN                  , 0x0207
);ITERN_WNDDOWSMSGID2STRING( WM_MBUTTONUP                    , 0x0208
);ITERN_WNDDOWSMSGID2STRING( WM_MBUTTONDBLCLK                , 0x0209);
#if (_WIN32_WINNT >=  0x0400) || (_WIN32_WINDOWS >  0x0400)
ITERN_WNDDOWSMSGID2STRING( WM_MOUSEWHEEL                   , 0x020A);
#endif
#if (_WIN32_WINNT >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_XBUTTONDOWN                  , 0x020B
);ITERN_WNDDOWSMSGID2STRING( WM_XBUTTONUP                    , 0x020C
);ITERN_WNDDOWSMSGID2STRING( WM_XBUTTONDBLCLK                , 0x020D);
#endif
#if (_WIN32_WINNT >=  0x0600)
 ITERN_WNDDOWSMSGID2STRING( WM_MOUSEHWHEEL / WM_MOUSELAST    , 0x020E);
#endif

#if (_WIN32_WINNT >=  0x0600) 
#elif (_WIN32_WINNT >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_MOUSELAST                    , 0x020D);
#elif (_WIN32_WINNT >=  0x0400) || (_WIN32_WINDOWS >  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_MOUSELAST                    , 0x020A);
#else
 ITERN_WNDDOWSMSGID2STRING( WM_MOUSELAST                    , 0x0209);
#endif /* (_WIN32_WINNT >=  0x0600) */

 ITERN_WNDDOWSMSGID2STRING( WM_PARENTNOTIFY                 , 0x0210
);ITERN_WNDDOWSMSGID2STRING( WM_ENTERMENULOOP                , 0x0211
);ITERN_WNDDOWSMSGID2STRING( WM_EXITMENULOOP                 , 0x0212
);
#if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_NEXTMENU                     , 0x0213
);ITERN_WNDDOWSMSGID2STRING( WM_SIZING                       , 0x0214
);ITERN_WNDDOWSMSGID2STRING( WM_CAPTURECHANGED               , 0x0215
);ITERN_WNDDOWSMSGID2STRING( WM_MOVING                       , 0x0216
);
#endif /* WINVER >=  0x0400 */

 #if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_POWERBROADCAST               , 0x0218
);
#endif /* WINVER >=  0x0400 */

 #if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_DEVICECHANGE                 , 0x0219
);
#endif /* WINVER >=  0x0400 */

 ITERN_WNDDOWSMSGID2STRING( WM_MDICREATE                    , 0x0220
);ITERN_WNDDOWSMSGID2STRING( WM_MDIDESTROY                   , 0x0221
);ITERN_WNDDOWSMSGID2STRING( WM_MDIACTIVATE                  , 0x0222
);ITERN_WNDDOWSMSGID2STRING( WM_MDIRESTORE                   , 0x0223
);ITERN_WNDDOWSMSGID2STRING( WM_MDINEXT                      , 0x0224
);ITERN_WNDDOWSMSGID2STRING( WM_MDIMAXIMIZE                  , 0x0225
);ITERN_WNDDOWSMSGID2STRING( WM_MDITILE                      , 0x0226
);ITERN_WNDDOWSMSGID2STRING( WM_MDICASCADE                   , 0x0227
);ITERN_WNDDOWSMSGID2STRING( WM_MDIICONARRANGE               , 0x0228
);ITERN_WNDDOWSMSGID2STRING( WM_MDIGETACTIVE                 , 0x0229
);ITERN_WNDDOWSMSGID2STRING( WM_MDISETMENU                   , 0x0230
);ITERN_WNDDOWSMSGID2STRING( WM_ENTERSIZEMOVE                , 0x0231
);ITERN_WNDDOWSMSGID2STRING( WM_EXITSIZEMOVE                 , 0x0232
);ITERN_WNDDOWSMSGID2STRING( WM_DROPFILES                    , 0x0233
);ITERN_WNDDOWSMSGID2STRING( WM_MDIREFRESHMENU               , 0x0234
);
#if(WINVER >=  0x0601)
 ITERN_WNDDOWSMSGID2STRING( WM_TOUCH                        , 0x0240
);
#endif /* WINVER >=  0x0601 */

#if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_IME_SETCONTEXT               , 0x0281
);ITERN_WNDDOWSMSGID2STRING( WM_IME_NOTIFY                   , 0x0282
);ITERN_WNDDOWSMSGID2STRING( WM_IME_CONTROL                  , 0x0283
);ITERN_WNDDOWSMSGID2STRING( WM_IME_COMPOSITIONFULL          , 0x0284
);ITERN_WNDDOWSMSGID2STRING( WM_IME_SELECT                   , 0x0285
);ITERN_WNDDOWSMSGID2STRING( WM_IME_CHAR                     , 0x0286
);
#endif /* WINVER >=  0x0400 */
#if(WINVER >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_IME_REQUEST                  , 0x0288
);
#endif /* WINVER >=  0x0500 */
#if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_IME_KEYDOWN                  , 0x0290
);ITERN_WNDDOWSMSGID2STRING( WM_IME_KEYUP                    , 0x0291
);
#endif /* WINVER >=  0x0400 */

#if((_WIN32_WINNT >=  0x0400) || (WINVER >=  0x0500))
 ITERN_WNDDOWSMSGID2STRING( WM_MOUSEHOVER                   , 0x02A1
);ITERN_WNDDOWSMSGID2STRING( WM_MOUSELEAVE                   , 0x02A3);
#endif
#if(WINVER >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_NCMOUSEHOVER                 , 0x02A0
);ITERN_WNDDOWSMSGID2STRING( WM_NCMOUSELEAVE                 , 0x02A2
);
#endif /* WINVER >=  0x0500 */

#if(_WIN32_WINNT >=  0x0501)
 ITERN_WNDDOWSMSGID2STRING( WM_WTSSESSION_CHANGE            , 0x02B1

);ITERN_WNDDOWSMSGID2STRING( WM_TABLET_FIRST                 , 0x02c0
);ITERN_WNDDOWSMSGID2STRING( WM_TABLET_LAST                  , 0x02df
);
#endif /* _WIN32_WINNT >=  0x0501 */

 ITERN_WNDDOWSMSGID2STRING( WM_CUT                          , 0x0300
);ITERN_WNDDOWSMSGID2STRING( WM_COPY                         , 0x0301
);ITERN_WNDDOWSMSGID2STRING( WM_PASTE                        , 0x0302
);ITERN_WNDDOWSMSGID2STRING( WM_CLEAR                        , 0x0303
);ITERN_WNDDOWSMSGID2STRING( WM_UNDO                         , 0x0304
);ITERN_WNDDOWSMSGID2STRING( WM_RENDERFORMAT                 , 0x0305
);ITERN_WNDDOWSMSGID2STRING( WM_RENDERALLFORMATS             , 0x0306
);ITERN_WNDDOWSMSGID2STRING( WM_DESTROYCLIPBOARD             , 0x0307
);ITERN_WNDDOWSMSGID2STRING( WM_DRAWCLIPBOARD                , 0x0308
);ITERN_WNDDOWSMSGID2STRING( WM_PAINTCLIPBOARD               , 0x0309
);ITERN_WNDDOWSMSGID2STRING( WM_VSCROLLCLIPBOARD             , 0x030A
);ITERN_WNDDOWSMSGID2STRING( WM_SIZECLIPBOARD                , 0x030B
);ITERN_WNDDOWSMSGID2STRING( WM_ASKCBFORMATNAME              , 0x030C
);ITERN_WNDDOWSMSGID2STRING( WM_CHANGECBCHAIN                , 0x030D
);ITERN_WNDDOWSMSGID2STRING( WM_HSCROLLCLIPBOARD             , 0x030E
);ITERN_WNDDOWSMSGID2STRING( WM_QUERYNEWPALETTE              , 0x030F
);ITERN_WNDDOWSMSGID2STRING( WM_PALETTEISCHANGING            , 0x0310
);ITERN_WNDDOWSMSGID2STRING( WM_PALETTECHANGED               , 0x0311
);ITERN_WNDDOWSMSGID2STRING( WM_HOTKEY                       , 0x0312 
);
#if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_PRINT                        , 0x0317
);ITERN_WNDDOWSMSGID2STRING( WM_PRINTCLIENT                  , 0x0318
);
#endif /* WINVER >=  0x0400 */

#if(_WIN32_WINNT >=  0x0500)
 ITERN_WNDDOWSMSGID2STRING( WM_APPCOMMAND                   , 0x0319
);
#endif /* _WIN32_WINNT >=  0x0500 */

#if(_WIN32_WINNT >=  0x0501)
 ITERN_WNDDOWSMSGID2STRING( WM_THEMECHANGED                 , 0x031A
);
#endif /* _WIN32_WINNT >=  0x0501 */


#if(_WIN32_WINNT >=  0x0501)
 ITERN_WNDDOWSMSGID2STRING( WM_CLIPBOARDUPDATE              , 0x031D
);
#endif /* _WIN32_WINNT >=  0x0501 */

#if(_WIN32_WINNT >=  0x0600)
 ITERN_WNDDOWSMSGID2STRING( WM_DWMCOMPOSITIONCHANGED        , 0x031E
);ITERN_WNDDOWSMSGID2STRING( WM_DWMNCRENDERINGCHANGED        , 0x031F
);ITERN_WNDDOWSMSGID2STRING( WM_DWMCOLORIZATIONCOLORCHANGED  , 0x0320
);ITERN_WNDDOWSMSGID2STRING( WM_DWMWINDOWMAXIMIZEDCHANGE     , 0x0321
);
#endif /* _WIN32_WINNT >=  0x0600 */

#if(_WIN32_WINNT >=  0x0601)
 ITERN_WNDDOWSMSGID2STRING( WM_DWMSENDICONICTHUMBNAIL           , 0x0323
);ITERN_WNDDOWSMSGID2STRING( WM_DWMSENDICONICLIVEPREVIEWBITMAP   , 0x0326
);
#endif /* _WIN32_WINNT >=  0x0601 */


 #if(WINVER >=  0x0600)
 ITERN_WNDDOWSMSGID2STRING( WM_GETTITLEBARINFOEX            , 0x033F
);
#endif /* WINVER >=  0x0600 */

 #if(WINVER >=  0x0400)

 ITERN_WNDDOWSMSGID2STRING( WM_HANDHELDFIRST                , 0x0358
);ITERN_WNDDOWSMSGID2STRING( WM_HANDHELDLAST                 , 0x035F

);ITERN_WNDDOWSMSGID2STRING( WM_AFXFIRST                     , 0x0360
);ITERN_WNDDOWSMSGID2STRING( WM_AFXLAST                      , 0x037F
);
#endif /* WINVER >=  0x0400 */

 ITERN_WNDDOWSMSGID2STRING( WM_PENWINFIRST                  , 0x0380
);ITERN_WNDDOWSMSGID2STRING( WM_PENWINLAST                   , 0x038F);

 #if(WINVER >=  0x0400)
 ITERN_WNDDOWSMSGID2STRING( WM_APP                          , 0x8000);
#endif /* WINVER >=  0x0400 */

/*
 * NOTE: All Message Numbers below , 0x0400 are RESERVED.
 *
 * Private Window Messages Start Here:
 */
ITERN_WNDDOWSMSGID2STRING( WM_USER                         , 0x0400);

	default:
		return _T("自定义消息");
	};
}

#define CLSTRING_MAKE_EXCEPTION_CPP(logicName) \
	void CLString::throw_##logicName(){\
		throw std::logicName(this->getASCII());\
	};

CLSTRING_MAKE_EXCEPTION_CPP(exception);//异常
CLSTRING_MAKE_EXCEPTION_CPP(logic_error);//逻辑错误
CLSTRING_MAKE_EXCEPTION_CPP(domain_error); //域错误
CLSTRING_MAKE_EXCEPTION_CPP(invalid_argument); //非法参数
CLSTRING_MAKE_EXCEPTION_CPP(length_error); //通常是创建对象是给出的尺寸太大
CLSTRING_MAKE_EXCEPTION_CPP(out_of_range); //访问超界

CLSTRING_MAKE_EXCEPTION_CPP(runtime_error);//运行时错误
CLSTRING_MAKE_EXCEPTION_CPP(range_error); //边界错误
CLSTRING_MAKE_EXCEPTION_CPP(overflow_error);//上溢
CLSTRING_MAKE_EXCEPTION_CPP(underflow_error);//下溢

CLString& CLString::operator*( LPCTSTR lpSplit )
{
	
	if((split(lpSplit).size())>0){
		(makeDEx()->m_curSplitOutIndex_) = 1;
	}else{
		(makeDEx()->m_curSplitOutIndex_) = 0;
	}
	return *this;
}

CLString& CLString::operator>>(char& ch )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ 
		ch = 0;
	}else if((makeDEx()->m_curSplitOutIndex_) == 0){
		ch = getASCII()[0];(makeDEx()->m_curSplitOutIndex_)=-1;
	}else { 
		LPCSTR lpStr;
		if(m_encoding == EnCode_ASCII)
			ch = vtAt((makeDEx()->m_curSplitOutIndex_)-1)[0];
		else if( lpStr = unicodeToAsciiInner((LPCWSTR)vtAt((makeDEx()->m_curSplitOutIndex_)-1))){
			ch = lpStr[0];
		}else{ 
			ch = 0;
		}
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>(unsigned char& number)
{
	if ((makeDEx()->m_curSplitOutIndex_) < 0) { number = 0; }
	else if ((makeDEx()->m_curSplitOutIndex_) == 0) { number = stob(); (makeDEx()->m_curSplitOutIndex_) = -1; }
	else {
		number = stob(vtAt((makeDEx()->m_curSplitOutIndex_) - 1));
		if (++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_) = -1;
	}
	return *this;
}

CLString& CLString::operator>>(wchar_t& ch )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ 
		ch = 0;
	}else if((makeDEx()->m_curSplitOutIndex_) == 0){
		ch = getUnicode()[0];(makeDEx()->m_curSplitOutIndex_)=-1;
	}else { 
		LPCWSTR lpStr;
		if(m_encoding == EnCode_ULE)
			ch = vtAt((makeDEx()->m_curSplitOutIndex_)-1)[0];
		else if( lpStr = asciiToUnicodeInner((LPCSTR)vtAt((makeDEx()->m_curSplitOutIndex_)-1))){
			ch = lpStr[0];
		}else{ 
			ch = 0;
		}
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( CLString& mString )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ 
		mString.empty();
	}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ 
		mString.set(this->string());(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	else { 
		mString = vtAt((makeDEx()->m_curSplitOutIndex_)-1);
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( CLString* pString )
{
	//assert(pString!=NULL);
	if((makeDEx()->m_curSplitOutIndex_) < 0){ 
		if(pString)pString->empty();
	}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ 
		if(pString)pString->set(this->string());(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	else { 
		if(pString)*pString = vtAt((makeDEx()->m_curSplitOutIndex_)-1);
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>(long long& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoll();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stoll(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( unsigned long long& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoull();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stoull(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( long& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stol();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = _ttol(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( unsigned long& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoul();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stoul(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( int& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoi();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = _ttoi(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( unsigned int& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoui();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stoui(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( short& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoi();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = _ttoi(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( unsigned short& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stoui();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stoui(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( float& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stof();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stof(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( double& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stof();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stof(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>( long double& number )
{
	if((makeDEx()->m_curSplitOutIndex_) < 0){ number = 0;}
	else if((makeDEx()->m_curSplitOutIndex_) == 0){ number = stof();(makeDEx()->m_curSplitOutIndex_)=-1;}
	else { 
		number = stof(vtAt((makeDEx()->m_curSplitOutIndex_)-1));
		if(++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_)=-1;
	}
	return *this;
}

CLString& CLString::operator>>(bool& number)
{
	if ((makeDEx()->m_curSplitOutIndex_) < 0) { number = false; }
	else if ((makeDEx()->m_curSplitOutIndex_) == 0) { number = stoi() == 0 ? false:true; (makeDEx()->m_curSplitOutIndex_) = -1; }
	else {
		int i = _ttoi(vtAt((makeDEx()->m_curSplitOutIndex_) - 1));
		number = i == 0 ? false : true;
		if (++(makeDEx()->m_curSplitOutIndex_) > (long)vtSize())
			(makeDEx()->m_curSplitOutIndex_) = -1;
	}
	return *this;
}

LPSTR CLString::storeA( LONG_PTR iStoreMaxCharLen/*=MAX_PATH*/ )
{
	if(iStoreMaxCharLen < 0)iStoreMaxCharLen = 0;
	if(m_encoding == EnCode_ASCII){ //当前状态是和所需类型相同 ascii
		return reinterpret_cast<LPSTR>(store(iStoreMaxCharLen));
	}else{
		empty();
		LONG_PTR _tagLen = iStoreMaxCharLen + 1;
		if((makeDEx()->m_pointerBufSizeInByte_) < (_tagLen)*(LONG_PTR)sizeof(CHAR)){
			if((makeDEx()->m_pointer_).pMultiByte)
				delete[] (makeDEx()->m_pointer_).pMultiByte;
			(makeDEx()->m_pointer_).pMultiByte = new CHAR[(makeDEx()->m_pointerBufSizeInByte_) = (_tagLen)*sizeof(CHAR)];
			ZeroMemory((makeDEx()->m_pointer_).pMultiByte,(makeDEx()->m_pointerBufSizeInByte_));
		}
		(makeDEx()->m_PtExSaveType_) = EnCode_ASCII;
		return (makeDEx()->m_pointer_).pMultiByte;
	}
}

LPWSTR CLString::storeW( LONG_PTR iStoreMaxCharLen/*=MAX_PATH*/ )
{
	if(iStoreMaxCharLen < 0)iStoreMaxCharLen = 0;
	if(m_encoding != EnCode_ASCII){ //当前状态是和所需类型不相同 ascii
		return reinterpret_cast<LPWSTR>(store(iStoreMaxCharLen));
	}else{
		empty();
		LONG_PTR _tagLen = iStoreMaxCharLen + 1;
		if((makeDEx()->m_pointerBufSizeInByte_) < (_tagLen)*(LONG_PTR)sizeof(WCHAR)){
			if((makeDEx()->m_pointer_).pWideChar)
				delete[] (makeDEx()->m_pointer_).pWideChar;
			(makeDEx()->m_pointer_).pWideChar = new WCHAR[(makeDEx()->m_pointerBufSizeInByte_) = (_tagLen)*sizeof(WCHAR)];
			ZeroMemory((makeDEx()->m_pointer_).pWideChar,(makeDEx()->m_pointerBufSizeInByte_));
		}
		(makeDEx()->m_PtExSaveType_) = isBigEndian() ? EnCode_UBE : EnCode_ULE;
		return (makeDEx()->m_pointer_).pWideChar;
	}
}

CLString& CLString::saveExStore()
{
	if(pDataEx == 0)
		return *this;;
	if(pDataEx->m_PtExSaveType_ == EnCode_UNKNOWN)
		return *this;;		
	if(pDataEx->m_pointer_.pMultiByte){ 
		if(pDataEx->m_PtExSaveType_ == EnCode_ASCII){		
			set((LPCSTR)pDataEx->m_pointer_.pMultiByte);
			*(pDataEx->m_pointer_.pMultiByte) = 0;
		}else if(pDataEx->m_PtExSaveType_ == (isBigEndian() ? EnCode_UBE:EnCode_ULE)){ 
			set((LPCWSTR)pDataEx->m_pointer_.pWideChar);
			*(pDataEx->m_pointer_.pWideChar) = 0;
		}
	}
	pDataEx->m_PtExSaveType_ = EnCode_UNKNOWN;
	return *this;
}

LPCSTR CLString::vtAtA( size_t i )
{
	if(m_encoding == EnCode_ASCII)
		return (LPCSTR)vtAt(i);
	else{
		return unicodeToAsciiInner((LPCWSTR)vtAt(i));
	}
}

LPCWSTR CLString::vtAtW( size_t i )
{
	if(m_encoding != EnCode_ASCII)
		return (LPCWSTR)vtAt(i);
	else{
		return asciiToUnicodeInner((LPCSTR)vtAt(i));
	}
}

CLString& CLString::copyExData( CLStringRC str )
{
	if(&str == this ){
		MessageBox(NULL, _T("无法拷贝对象自身！"), _T("CLString对象拷贝错误"), MB_ICONERROR);
		throw std::logic_error("无法拷贝对象自身！");
		return *this;
	}
	if(str.pDataEx == NULL)
		return *this;
	if(pDataEx == NULL) 
		pDataEx = makeDEx();
	pDataEx->m_PtExSaveType_ = str.pDataEx->m_PtExSaveType_;
	if(pDataEx->m_pointerBufSizeInByte_ < str.pDataEx->m_pointerBufSizeInByte_){
		if(pDataEx->m_pointerBufSizeInByte_ > 0 && pDataEx->m_pointer_.pMultiByte){
			delete[] pDataEx->m_pointer_.pMultiByte;
			pDataEx->m_pointer_.pMultiByte = new CHAR[ (pDataEx->m_pointerBufSizeInByte_ = str.pDataEx->m_pointerBufSizeInByte_)/sizeof(CHAR)];
			memcpy_s(pDataEx->m_pointer_.pMultiByte,pDataEx->m_pointerBufSizeInByte_,str.pDataEx->m_pointer_.pMultiByte,str.pDataEx->m_pointerBufSizeInByte_);
		}
	}else{
		if(pDataEx->m_pointerBufSizeInByte_ > 0 && pDataEx->m_pointer_.pMultiByte){
			memcpy_s(pDataEx->m_pointer_.pMultiByte,pDataEx->m_pointerBufSizeInByte_,str.pDataEx->m_pointer_.pMultiByte,str.pDataEx->m_pointerBufSizeInByte_);
		}
	}
	if(str.pDataEx->m_sysTimeStart_){
		if(pDataEx->m_sysTimeStart_ == NULL)pDataEx->m_sysTimeStart_ = new SYSTEMTIME;
		*pDataEx->m_sysTimeStart_ = *str.pDataEx->m_sysTimeStart_;
	}
	if(str.pDataEx->m_sysTimeEnd_){
		if(pDataEx->m_sysTimeEnd_ == NULL)pDataEx->m_sysTimeEnd_ = new SYSTEMTIME;
		*pDataEx->m_sysTimeEnd_ = *str.pDataEx->m_sysTimeEnd_;
	}
	if(str.pDataEx->m_largeIntegerStart_){
		if(pDataEx->m_largeIntegerStart_ == NULL)pDataEx->m_largeIntegerStart_ = new LARGE_INTEGER;
		*pDataEx->m_largeIntegerStart_ = *str.pDataEx->m_largeIntegerStart_;
	}
	if(str.pDataEx->m_largeIntegerEnd_){
		if(pDataEx->m_largeIntegerEnd_ == NULL)pDataEx->m_largeIntegerEnd_ = new LARGE_INTEGER;
		*pDataEx->m_largeIntegerEnd_ = *str.pDataEx->m_largeIntegerEnd_;
	}
	pDataEx->m_isEndOnce_ = str.pDataEx->m_isEndOnce_;
	pDataEx->m_curSplitOutIndex_ = str.pDataEx->m_curSplitOutIndex_;
	if(str.pDataEx->m_vtInnerStringVector_){
		if(pDataEx->m_vtInnerStringVector_ == NULL)pDataEx->m_vtInnerStringVector_ = new std::vector<LPCTSTR>;
		//if(pDataEx->m_vtStringStoreLst_ == NULL)pDataEx->m_vtStringStoreLst_ = new std::vector<LPTSTR>;
		//if(pDataEx->m_vtStringStoreBufSizeLst_ == NULL)pDataEx->m_vtStringStoreBufSizeLst_ = new std::vector<LONG_PTR>;
		pDataEx->m_vtInnerStringVector_->reserve(str.pDataEx->m_vtInnerStringVector_->size());
		size_t si;
		pDataEx->m_vtInnerStringVector_->clear();
		for (LONG_PTR i=0;i < (LONG_PTR)str.pDataEx->m_vtInnerStringVector_->size();i++)
		{
			si = std::_tcslen(str.pDataEx->m_vtInnerStringVector_->at(i)) + 1;
			LPTSTR lp = _newbuffer((LONG_PTR)si,i);
			_tcscpy_s(lp,si,str.pDataEx->m_vtInnerStringVector_->at(i));
			pDataEx->m_vtInnerStringVector_->push_back(lp);
		}		
	}
	return *this;
}


CLString& CLString::copy( CLStringRC str )
{
	return set(str).copyExData(str);
}


void CLString::_ClStringExData::init()
{
	m_pointer_.pWideChar = NULL;
	m_PtExSaveType_ = EnCode_UNKNOWN;
	m_pointerBufSizeInByte_ = 0;
	m_hFile_ = NULL;
	m_fileType_ = EnCode_UNKNOWN;
	hInternet_ = hConnect_ = hRequest_ = NULL;	
	m_vtInnerStringVector_ = NULL;
	m_vtStringStoreLst_ = NULL;
	m_vtStringStoreBufSizeLst_ = NULL;
	m_sysTimeStart_ = m_sysTimeEnd_ = NULL;
	m_largeIntegerStart_ = m_largeIntegerEnd_ = NULL;
	m_isEndOnce_ = FALSE;
	m_curSplitOutIndex_ = -1;
}
void CLString::_ClStringExData::closeInternet(){ //关网络
	if (hInternet_)
	{
		InternetCloseHandle(hInternet_);
		hInternet_ = NULL;
	}
	if (hConnect_)
	{
		InternetCloseHandle(hConnect_);
		hConnect_ = NULL;
	}
	if (hRequest_)
	{
		InternetCloseHandle(hRequest_);
		hRequest_ = NULL;
	}
}
BOOL CLString::_ClStringExData::closeFile(){ //关文件	
	if(m_hFile_ == INVALID_HANDLE_VALUE) {
		m_hFile_ = NULL;
		return TRUE;
	}
	if (m_hFile_) {
		CloseHandle(m_hFile_);
		m_hFile_ = NULL;
		m_fileType_ = EnCode_UNKNOWN;
	}
	return TRUE;
}
void CLString::_ClStringExData::closeMemery(){ //释放内存
	//释放内存
	if (m_pointer_.pMultiByte)
	{
		delete[] m_pointer_.pMultiByte;
		m_pointer_.pMultiByte=NULL;
		m_PtExSaveType_ = EnCode_UNKNOWN;
	}
	m_pointerBufSizeInByte_ = 0;
	m_curSplitOutIndex_ = -1;
	//清理std::vector的内存
	if(m_vtInnerStringVector_){
		m_vtInnerStringVector_->clear();
		delete m_vtInnerStringVector_;
		m_vtInnerStringVector_ = NULL;
	}
	if(m_vtStringStoreBufSizeLst_){
		m_vtStringStoreBufSizeLst_->clear();
		delete m_vtStringStoreBufSizeLst_;
		m_vtStringStoreBufSizeLst_ = NULL;
	}
	if(m_vtStringStoreLst_){
		clearStringVector((std::vector<LPCTSTR>&)*m_vtStringStoreLst_);
		delete m_vtStringStoreLst_;
		m_vtStringStoreLst_ = NULL;
	}
	if(m_sysTimeStart_){
		delete m_sysTimeStart_;
		m_sysTimeStart_ = NULL;
	}
	if(m_sysTimeEnd_){
		delete m_sysTimeEnd_;
		m_sysTimeEnd_ = NULL;
	}
	if(m_largeIntegerStart_){
		delete m_largeIntegerStart_;
		m_largeIntegerStart_ = NULL;
	}
	if(m_largeIntegerEnd_){
		delete m_largeIntegerEnd_;
		m_largeIntegerEnd_ = NULL;
	}
}
void CLString::_ClStringExData::closeAndClear()
{
	closeInternet();
	closeFile();
	closeMemery();	
}

