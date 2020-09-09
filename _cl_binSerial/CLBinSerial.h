#pragma once

#ifndef __CL_BINSERIAL_H__
#define __CL_BINSERIAL_H__

//#include "../../_cl_extension/_cl_extension.h"
#include "windows.h"
#include "tchar.h"
#include "assert.h"
#include "fstream"
using namespace std;
class CLBinSerial;
typedef CLBinSerial *PCLBinSerial;
typedef const CLBinSerial *PCCLBinSerial;
typedef CLBinSerial& CLBinSerialR;
typedef const CLBinSerial& CLBinSerialRC;

class CLBinSerial {
private:
	HANDLE m_hFile;
	long long m_SizeInBytes;
	long long m_readPointer;
	TCHAR m_fileName[MAX_PATH];
	BOOL m_change;
	long long m_counts;
	byte m_attachType;
	inline BOOL checkFile()
	{
		return (m_hFile) ? TRUE : FALSE;
	}
	inline long long movePointer(long long index)//移动指针到某位置，index基于0的索引以项为单位
	{
		return SetFilePointer(m_hFile, (long)index*m_SizeInBytes, 0, FILE_BEGIN);
	}
	inline long long getPinter()//取得指针，index基于0的索引以项为单位
	{
		return SetFilePointer(m_hFile, 0, 0, FILE_CURRENT) / m_SizeInBytes;
	}

public:
	//构造函数，参数指定文件所保存的数据中每一条数据的数据结构以Byte计的大小。
	CLBinSerial(long long nDataItemSizeInByte);

	virtual ~CLBinSerial();
	//关闭原连接，以读写方式连接文件，文件不存在就创建文件。
	//若文件所处的路径不存在，则函数自动先创建路径，若创建路径失败，则不会继续创建文件。
	CLBinSerialR attach(LPCTSTR fileNameToAttach);
	//关闭原连接，以只读模式连接文件，文件不存在则链接失败。
	CLBinSerialR attachToRead(LPCTSTR fileNameToAttach);
	//是否绑定了文件，参数为文件名称输出buf
	BOOL isAttach(LPTSTR attachFileNameBuf = NULL, long long bufSizeInTChar = 0);
	//是否可读
	BOOL isRead() const;
	//是否可读写
	BOOL isWrite() const;
	//关闭连接的文件。若对象没有连接则什么也不做。
	CLBinSerialR detach();
	//读写模式下，清除连接文件内容。若对象没有连接则什么也不做。
	CLBinSerialR clearFile();
	//读写模式下，增加新数据到连接文件的尾部。不会改变读取指针的位置。
	CLBinSerialR append(LPCVOID lpData);
	//设置某位置的数据。
	CLBinSerialR set(LPCVOID lpData, long long index);
	//读写模式下，增加新数据到连接文件的尾部。不会改变读取指针的位置。
	CLBinSerialR operator << (LPCVOID lpData);
	CLBinSerialR operator << (byte& data);
	//读取数据到指针buf中。移动读取指针到下一条数据位置。
	CLBinSerialR operator >> (LPVOID lpOutputDataBuf);
	//返回文件绑定的项目条数，没绑定返回0。
	long long getCounts();
	//移动读取指针到指定的所有位置，返回原位置的索引，所有超过最大数量将指针移动到末尾。
	long long moveReadPionter(long long newIndex);
	//读取数据到指针buf中。移动读取指针到下一条数据位置。
	BOOL getItem(LPVOID lpOutputDataBuf);
	//读取数据到指针buf中。不会改变读取指针的位置。
	//匹配条件为[varStart,varEnd]之前的值
	//varPosInByte表示比较数据在结构中的位置索引以byte为单位的偏移
	template<typename T>
	long long getItemCmp(LPVOID lpOutputData, T varStart, T varEnd, DWORD varPosInByte = 0)
	{
		assert(lpOutputData != NULL);
		if (!checkFile()) {
			ZeroMemory(lpOutputData, m_SizeInBytes);
			return 0;
		}
		T _var = 0; BOOL isFind = FALSE; DWORD nRead = 0;
		long long ret = 0;
		SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
		while (::ReadFile(m_hFile, lpOutputData, m_SizeInBytes, &nRead, NULL) && (nRead == m_SizeInBytes)) {
			ret++;
			_var = *((T*)((LPBYTE)lpOutputData + varPosInByte));
			if (BETWEEN(_var, varStart, varEnd)) {
				isFind = TRUE; break;
			}
		}
		movePointer(m_readPointer);
		if (isFind)
			return ret;
		else {
			ZeroMemory(lpOutputData, m_SizeInBytes);
			return 0;
		}
	}

	//读取数据到指针buf中。不会改变读取指针的位置。
	//index指定读取的第几条的索引，索引以0为基础。
	BOOL getItemAt(LPVOID lpOutputDataBuf, long long index);
};
#define TIME_STAMP_POS  0
#define TIME_STAMP_SIZE 8


class CLMemBinSerial{
public:
	CLMemBinSerial() { init(); }
	CLMemBinSerial(LPCVOID _lpMen, size_t _memBufSize) { setMem(_lpMen, _memBufSize); }
	CLMemBinSerial(LPCVOID _lpMen) { setMem(_lpMen, 0); }
	CLMemBinSerial& setMem(LPCVOID _lpMen, size_t _memBufSize) {		
		assert(_lpMen != NULL);
		init();
		memcpy_s(&lpMen, sizeof(LPBYTE), &_lpMen, sizeof(LPCVOID));
		lpMenC = lpMen;
		m_memBufSize = _memBufSize;
		return *this;
	}
	virtual ~CLMemBinSerial() { init(); }
	inline size_t getOperateCounts() const { return m_operateCounts; }
	inline LPVOID getMemBuf() const { return lpMen; }
	//将指针重置到buf起始位置
	inline CLMemBinSerial& repeat() { return  getMemBuf() == 0 ? *this : setMem(getMemBuf(), getMemBufSize()); }
	template <typename ClassTag> CLMemBinSerial& append(const ClassTag* lpTag, size_t nCounts = 1) {
		if (getMemBufSize() == 0)
			throw std::logic_error("Mem Bin Serial error: The buf writed size is 0!");
		if (lpTag == 0)return *this;
		if (nCounts == 0)nCounts = 1;
		if (getMemBufSize() <= getOperateCounts())
			throw std::logic_error("Mem Bin Serial error: The buf writed size is not long enough!");
		size_t nSi = nCounts * sizeof(ClassTag);
		memcpy_s(lpMenC, getMemBufSize() - getOperateCounts(), lpTag, nSi);
		m_operateCounts += nSi;
		lpMenC += nSi;
		return *this;
	}
	template <typename ClassTag> inline CLMemBinSerial& append(const ClassTag& tag, size_t nCounts = 1) { return append(&tag, nCounts); }
	template <typename ClassTag> inline CLMemBinSerial& operator << (const ClassTag& tag) { return append(&tag, 1); }
	template <typename ClassTag> inline CLMemBinSerial& operator << (const ClassTag* lpTag) { return append(lpTag, 1); }
	inline CLMemBinSerial& operator << (const CHAR* lpString) { return append(lpString); }
	inline CLMemBinSerial& operator << (const WCHAR* lpString) { return append(lpString); }
	inline CLMemBinSerial& append(const CHAR* lpString) { return append(lpString, lpString != 0 ? (strlen(lpString) + 1) : 0); }
	inline CLMemBinSerial& append(const WCHAR* lpString) { return append(lpString, lpString != 0 ? (wcslen(lpString) + 1) : 0); }
	template <typename ClassTag> CLMemBinSerial& putout(ClassTag* lpTag, size_t nCounts = 1) {
		if (lpTag == 0)return *this;
		if (nCounts == 0)nCounts = 1;
		size_t nSi = nCounts * sizeof(ClassTag);
		memcpy_s(lpTag, nSi, lpMenC, nSi);
		m_operateCounts += nSi;
		lpMenC += nSi;
		return *this;
	}
	template <typename ClassTag> inline CLMemBinSerial& putout(ClassTag& tag, size_t nCounts = 1) { return putout(&tag, nCounts); }
	template <typename ClassTag> inline CLMemBinSerial& operator >>(ClassTag& tag) { return putout(&tag, 1); }
	template <typename ClassTag> inline CLMemBinSerial& operator >>(ClassTag* lpTag) { return putout(lpTag, 1); }
	//请确保写入空间足够大，函数不做相关越界检查
	inline CLMemBinSerial& operator >> (CHAR* lpString) { return putout(lpString); }
	inline CLMemBinSerial& operator >> (WCHAR* lpString) { return putout(lpString); }
	inline CLMemBinSerial& putout(CHAR* lpString) { return putout(lpString, (strlen((const CHAR*)lpMenC) + 1)); }
	inline CLMemBinSerial& putout(WCHAR* lpString) { return putout(lpString, (wcslen((const WCHAR*)lpMenC) + 1)); }
protected:
	inline void init() {
		lpMenC = lpMen = 0;
		m_memBufSize = 0;
		m_operateCounts = 0;
	}
	inline size_t getMemBufSize() const { return m_memBufSize; }
private:
	LPBYTE lpMen, lpMenC;
	size_t m_memBufSize;
	size_t m_operateCounts;
};

//二进制文件操作
class CLFileBinSerial:public fstream {
protected:
	unsigned char* pBuf = NULL;
	size_t bufSize = 0;
	CLFileBinSerial(const CLFileBinSerial&) = delete;
	CLFileBinSerial& operator= (const CLFileBinSerial&) = delete;
public:
	CLFileBinSerial(){}
	CLFileBinSerial(LPCTSTR file,int mode)
		:fstream(file,mode | ios::binary){
		seekg(ios::beg);
	}
	CLFileBinSerial(LPCTSTR file,bool isWrite = false) 
		:fstream(file, (isWrite ? (ios::app|ios::out|ios::binary):(ios::in | ios::binary))) {
		seekg(ios::beg);
	}
	bool open(LPCTSTR file, bool isWrite = false) {
		if (is_open())
			close();
		fstream::open(file, (isWrite ? (ios::app | ios::out | ios::binary) : (ios::in | ios::binary)));
		if (is_open()) {
			seekg(ios::beg);
			return true;
		}
		else return false;
	}
	virtual ~CLFileBinSerial() { 
		if (is_open())
			close();
		if (pBuf)
			delete[] pBuf, pBuf = NULL, bufSize = 0;
	}
	//顺序将对象数据写入到文件流中（函数会对输入的数据大小做记录，保证读取时数据的完整性，这将损失部分空间用于记录对象自身的大小）
	template<class T>
	CLFileBinSerial& operator << (const T& v) {
		size_t st = sizeof(T);
		if (st < 0xFF) {
			unsigned char si = (unsigned char)st;
			write((const char*)& si, 1);
			write((const char*)& v, si);
		}
		else {
			const unsigned char si = 0xFF;
			write((const char*)& si, 1);
			write((const char*)& st, sizeof(size_t));
			write((const char*)& v, st);
		}
		return *this;
	}
	//顺序将文件数据流输出到对象内存中（函数会对记录的数据大小做读取对齐保护，保证读取数据完整性，这将损失部分空间用于记录对象自身的大小）
	template<class T>
	CLFileBinSerial& operator >> (T& v) {
		ZeroMemory(&v, sizeof(T));
		unsigned char si;
		read((char*)& si, 1);	
		if (si == 0x5) {
			float ff;
			read((char*)&ff, sizeof(float));
			//v = ff;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &ff, stt);
		}else if (si == 0x6) {
			double db;
			read((char*)& db, sizeof(double));
			//v = db;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &db, stt);
		}else if(si == 0x7) {
			long double db2;
			read((char*)& db2, sizeof(long double));
			//v = db2;
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, &db2, stt);
		}else if (si == 0xFF) {
			size_t st;
			read((char*)& st, sizeof(size_t));
			if (pBuf == NULL) {
				pBuf = new unsigned char[bufSize = st];
			}
			else if (bufSize < st) {
				delete[] pBuf;
				pBuf = new unsigned char[bufSize = st];
			}
			read((char*)pBuf, st);
			size_t stt = min(sizeof(T), st);
			memcpy_s(&v, stt, pBuf, stt);
		}
		else {
			//v = 0;
			char buf[256];
			read(buf, si);
			size_t stt = min(sizeof(T), si);
			memcpy_s(&v, stt, buf, stt);
		}
		return *this;
	}

	CLFileBinSerial& operator << (const float& v) {
		unsigned char si = 0x5;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(float));
		return *this;
	}
	CLFileBinSerial& operator << (const double& v) {
		unsigned char si = 0x6;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(double));
		return *this;
	}
	CLFileBinSerial& operator << (const long double& v) {
		unsigned char si = 0x7;
		write((const char*)& si, 1);
		write((const char*)& v, sizeof(long double));
		return *this;
	}
	CLFileBinSerial& operator << (const char* lpString) {
		size_t si = strlen(lpString) + 1;
		write((const char*)& si, sizeof(size_t));
		write(lpString, sizeof(char) * (si));
		return *this;
	}
	CLFileBinSerial& operator << (const wchar_t* lpString) {
		size_t si = wcslen(lpString) + 1;
		write((const char*)& si, sizeof(size_t));
		write((const char*)lpString, sizeof(wchar_t) * (si));
		return *this;
	}
	CLFileBinSerial& operator >> (char*& lpString) {
		size_t si = 0;
		read((char*)& si, sizeof(size_t));
		read(lpString, sizeof(char) * (si));
		return *this;
	}
	CLFileBinSerial& operator >> (wchar_t*& lpString) {
		size_t si = 0;
		read((char*)& si, sizeof(size_t));
		read((char*)lpString, sizeof(wchar_t) * (si));
		return *this;
	}
};

#endif