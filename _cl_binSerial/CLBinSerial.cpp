#include "CLBinSerial.h"
#include "../_cl_string/CLString.h"

CLBinSerial::CLBinSerial(long long nDataItemSizeInByte)
{
	if(nDataItemSizeInByte == 0)
		m_SizeInBytes = 1;
	else 
		m_SizeInBytes = nDataItemSizeInByte;
	m_hFile = NULL;
	m_readPointer = 0;
	m_change = FALSE;
	m_counts = 0;
	m_attachType = 0;
	m_fileName[0]=0;
}

CLBinSerial::~CLBinSerial()
{	
	detach();
}
CLBinSerialR CLBinSerial::attach(LPCTSTR fileNameToAttach)
{
	if(isAttach())detach();
	if(!fileNameToAttach)
		return *this;	
	TCHAR _path[MAX_PATH] = {0};
	size_t i = (size_t)_tcsrchr(fileNameToAttach,_T('\\'));
	size_t i2 = (size_t)_tcsrchr(fileNameToAttach,_T('/'));
	if(i2 > i) i = i2;
	size_t n = (i-(size_t)fileNameToAttach);
	memcpy_s(_path,n,fileNameToAttach,n);
	if(!CLString::createDirectory(_path))
		return *this;

	HANDLE hFile = ::CreateFile(fileNameToAttach, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE){
#ifdef _DEBUG
		DWORD le = GetLastError();
		//CLString().getLastErrorStringR(le).messageBox(_T("Msg From CLBinSerial::attach()"),MB_ICONERROR);
#endif
		//m_fileName[0] = 0;
		return *this;
	}	
	_tcscpy_s(m_fileName,MAX_PATH-1,fileNameToAttach);
	m_hFile = hFile;
	m_attachType = 2;
	m_change = TRUE;
	return *this;
}

CLBinSerialR CLBinSerial::attachToRead(LPCTSTR fileNameToAttach)
{
	if(isAttach())detach();
	if(!fileNameToAttach)
		return *this;		
	HANDLE hFile = ::CreateFile(fileNameToAttach,GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile || hFile == INVALID_HANDLE_VALUE){
#ifdef _DEBUG
		DWORD le = GetLastError();
		//CLString().getLastErrorStringR(le).messageBox(_T("Msg From CLBinSerial::attachToRead()"),MB_ICONERROR);
#endif
		//m_fileName[0] = 0;
		return *this;
	}	
	_tcscpy_s(m_fileName,MAX_PATH-1,fileNameToAttach);
	m_hFile = hFile;
	m_attachType = 1;
	m_change = TRUE;
	return *this;
}

BOOL CLBinSerial::isAttach(LPTSTR attachFileNameBuf,long long bufSizeInTChar)
{
	if(checkFile()){
		if(attachFileNameBuf && bufSizeInTChar > 0)	
			_tcscpy_s(attachFileNameBuf,bufSizeInTChar,m_fileName);
		return TRUE;
	}
	else {
		if(attachFileNameBuf && bufSizeInTChar > 0)	
			attachFileNameBuf[0]=0;
		return FALSE;
	}
}

BOOL CLBinSerial::isRead() const
{
	if(m_attachType >= 1)
		return TRUE;
	else return FALSE;
}

BOOL CLBinSerial::isWrite() const
{
	if(m_attachType == 2)
		return TRUE;
	else return FALSE;
}

CLBinSerialR CLBinSerial::detach()
{
	if(checkFile()){
		CloseHandle(m_hFile);
		m_hFile = NULL;	
		m_readPointer = 0;
		m_change = FALSE;
		m_counts = 0;
		m_attachType = 0;
		m_fileName[0] = 0;
	}	
	return *this;
}

CLBinSerialR CLBinSerial::clearFile()
{
	if(checkFile() && m_attachType == 2){
		m_readPointer = movePointer(0);
		SetEndOfFile(m_hFile);
		m_change = TRUE;
	}
	return *this;
}

CLBinSerialR CLBinSerial::append(LPCVOID lpData)
{
	if(checkFile() && m_attachType == 2){
		SetFilePointer(m_hFile,0,0,FILE_END);
		DWORD nWrite = 0;
		if(!::WriteFile(m_hFile,lpData,m_SizeInBytes,&nWrite,NULL)){
#ifdef _DEBUG
			DWORD le = GetLastError();
			//CLString().getLastErrorStringR(le).messageBox(_T("Msg From CLBinSerial::append()"),MB_ICONERROR);
#endif
		}
		else m_change = TRUE;
		movePointer(m_readPointer);
	}
	return *this;
}

CLBinSerialR CLBinSerial::set(LPCVOID lpData,long long index)
{
	if (checkFile() && m_attachType == 2 && index>=0)
	{
		if(index > getCounts())
		{
			append(lpData);
			return *this;
		}
		movePointer(index);
		DWORD nWrite = 0;
		if(!::WriteFile(m_hFile,lpData,m_SizeInBytes,&nWrite,NULL)){
#ifdef _DEBUG
			DWORD le = GetLastError();
			//CLString().getLastErrorStringR(le).messageBox(_T("Msg From CLBinSerial::set()"),MB_ICONERROR);
#endif
		}
		else m_change = TRUE;
		movePointer(m_readPointer);
	}
	return *this;
}

CLBinSerialR CLBinSerial::operator<<(LPCVOID lpData)
{
	append(lpData);
	return *this;
}

CLBinSerialR CLBinSerial::operator>>(LPVOID lpOutputData)
{
	getItem(lpOutputData);
	return *this;
}

long long CLBinSerial::getCounts()
{
	if(m_change){
		m_change = FALSE;
		if(checkFile())
			m_counts = GetFileSize(m_hFile,NULL)/m_SizeInBytes;
		else
			m_counts = 0;			
	}
	return m_counts;		
}

long long CLBinSerial::moveReadPionter(long long newIndex)
{
	if(checkFile()){
		long long bkp = m_readPointer;
		long long _maz = getCounts();
		if(newIndex > _maz)
			m_readPointer = movePointer(_maz);
		else m_readPointer = movePointer(newIndex);
		return bkp;
	}
	else return 0;
}

BOOL CLBinSerial::getItem(LPVOID lpOutputData)
{
	if(checkFile()){
		DWORD nRead = 0;
		movePointer(m_readPointer);
		BOOL rt = ::ReadFile(m_hFile,lpOutputData,m_SizeInBytes,&nRead,NULL);
		if(nRead != m_SizeInBytes){
			rt = FALSE;
			ZeroMemory(lpOutputData,m_SizeInBytes);
		}	
		m_readPointer = getPinter();
		return rt;
	}
	else return FALSE;
}


BOOL CLBinSerial::getItemAt(LPVOID lpOutputData,long long index)
{
	assert(lpOutputData!=NULL);
	if(checkFile()){
		if(index<0){ZeroMemory(lpOutputData,m_SizeInBytes);return FALSE;}
		movePointer(index);
		DWORD nRead=0;
		BOOL rt = ReadFile(m_hFile,lpOutputData,m_SizeInBytes,&nRead,NULL);
		movePointer(m_readPointer);
		if(nRead != m_SizeInBytes){
			rt = FALSE;
			ZeroMemory(lpOutputData,m_SizeInBytes);
		}		
		return rt;
	}
	else return FALSE;
}

