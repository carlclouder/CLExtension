#pragma once
#ifndef __CL_CURL_H__
#define __CL_CURL_H__

#ifndef BUILDING_LIBCURL
#define BUILDING_LIBCURL
#endif
#ifndef HTTP_ONLY
#define HTTP_ONLY
#endif

#include "../_cl_common/CLCommon.h"
#include "curl.lib/curl-7.76.1/include/curl/curl.h"

// ����Ҫ������ʾ������Ϊ0
#define OPEN_CHECK_CODE_ERROR_MSGBOX 1 
#if OPEN_CHECK_CODE_ERROR_MSGBOX > 0
#define _CHECK_CODE_ERROR() getLastErrorMsgBoxExceptSucceed()
#else
#define _CHECK_CODE_ERROR() 
#endif


struct CURLBASE { CURL* curl = nullptr; };
class CLCurl : CURLBASE {
public:
	typedef CURLBASE base;
	typedef CLCurl& ref;
	typedef const CLCurl& refc;
	typedef std::vector<char> ResultBuffer;
protected:
	bool _isInit = false;
	CURLcode _lastError = CURLE_OK;
	struct _buferStorePack {
		size_t time;
		ResultBuffer& buf;
	};
	static size_t _perfToBufferCallBack(void* buffer, size_t size, size_t nmemb, void* buferStorePack)
	{
		size_t& time = ((_buferStorePack*)buferStorePack)->time;
		ResultBuffer& buf = ((_buferStorePack*)buferStorePack)->buf;
		if (time++ == 0)
			buf.clear();
		auto pos = buf.size();
		buf.resize(pos + nmemb);
		memcpy_s(buf.data() + pos, nmemb, buffer, nmemb);
		return nmemb;
	}
	static bool& _getGbInit() {
		static bool _isGlbInit = false;
		return _isGlbInit;
	}
public:
	//Ĭ��ʹ�õ�������ͻ��ˡ�
	static constexpr PCStr DefaultUserAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.131 Safari/537.36";
	CLCurl() {}
	CLCurl(CURL* curl) {
		*this = curl;
	}
	CLCurl(PCStr url) {
		*this = url;
	}
	CLCurl(const ref other) = delete;
	CLCurl(CLCurl&& other) noexcept {
		*this = std::move(other);
	}
	ref operator =(CLCurl&& other) noexcept {
		std::swap(this->curl, other.curl);
		std::swap(this->_isInit, other._isInit);
		std::swap(this->_lastError, other._lastError);
		other.cleanup();
		return *this;
	}
	//curlȫ������
	static void globalCleanup() {
		curl_global_cleanup();
	}
	//curlȫ�ֳ�ʼ����
	static bool globalInit(long flags = CURL_GLOBAL_ALL) {
		if (!_getGbInit()) {
			if (curl_global_init(flags) == CURLE_OK)
				_getGbInit() = true;
		}
		return _getGbInit();
	}
	operator CURL* () const { return base::curl; }
	ref operator = (CURL* curl) {
		if (this->curl)curl_easy_cleanup(*this);
		_isInit = (this->curl = curl) ? true : false;
		_lastError = CURLE_OK;
		return *this;
	}
	ref operator = (PCStr url) { return setUrl(url); }
	//curl�����ʼ�����ú�����һ��Ҫ��ʾ���ã�������������Ҫ��ʱ���Զ�ִ�У�
	ref init() {
		globalInit(CURL_GLOBAL_ALL);
		*this = curl_easy_init(); // ��ʼ��CURL���
		return setUserAgent(DefaultUserAgent);
	}
	//ȡ����һ��Ϊ���صĽ����CURLcode��
	CURLcode getLastError() const { return _lastError; }
	//ȡ����һ��Ϊ���صĽ����CURLcode��
	CURLcode getCode() const { return getLastError(); }
	//curl�����û�ѡ�������
	template<class ...Args>
	ref setopt(CURLoption option, Args ...args) {
		//if (!_isInit)throw std::runtime_error("CLCurl is not init!");
		if (!_isInit)init();
		_lastError = (CURLcode)curl_easy_setopt(*this, option, std::forward<Args>(args)...);
		_CHECK_CODE_ERROR();
		return *this;
	}
	//curl����Ŀ��url�ַ�����
	ref setUrl(PCStr url) { return setopt(CURLOPT_URL, url); }
	//curl���������˷��͵���Դ��ַ��
	ref setRefer(PCStr referUrl) { return setopt(CURLOPT_REFERER, referUrl); }
	//curl����������ͻ��ˡ�
	ref setUserAgent(PCStr userAgent = DefaultUserAgent) {
		return setopt(CURLOPT_USERAGENT, userAgent);
	}
	//curl����SSL�Զ���֤��
	ref setSSLverifyPeer(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYPEER, yes); }
	//curl����SSL������֤��
	ref setSSLverifyHost(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYHOST, yes); }
	//curl���ó�ʱ������
	ref setTimeout(int sec) { return setopt(CURLOPT_TIMEOUT, sec); }
	//curl���ó�ʱ��������
	ref setTimeoutMs(int minisec) { return setopt(CURLOPT_TIMEOUT_MS, minisec); }
	//curl�����Ƿ�������Ϣͷ���ݡ�
	ref setHeader(bool uesHeader = false) { return setopt(CURLOPT_HEADER, uesHeader); }
	//curl���ý��뷽ʽ��
	ref setEncoding(PCStr encode = "gzip") { return setopt(CURLOPT_ENCODING, encode); }
	//curl�����ض���ץȡ��
	ref setFollowLocation(bool follow = false) { return setopt(CURLOPT_FOLLOWLOCATION, follow); }
	typedef size_t(*PWFCallBack)(void*, size_t, size_t, void*);
	//curl����д��ص�������
	ref setWriteFunction(PWFCallBack pFunc) { return setopt(CURLOPT_WRITEFUNCTION, pFunc); }
	//curl����д��ص������������ݡ�
	ref setWriteData(void* param) { return setopt(CURLOPT_WRITEDATA, param); }
	//curl����ִ�С�
	ref perform() {
		if (!_isInit)init();
		_lastError = curl_easy_perform(*this);
		_CHECK_CODE_ERROR();
		return *this;
	}
	//curl����ִ�У���������������ݼ�storeBuf��
	ref perform(ResultBuffer& storeBuf) {
		_buferStorePack _data{ 0, storeBuf };
		return setWriteFunction(_perfToBufferCallBack)
			.setWriteData((void*)&_data).perform();
	}
	//curl���������ͷš�
	ref cleanup() {
		if (this->curl)
			curl_easy_cleanup(*this), this->curl = nullptr;
		_isInit = false, _lastError = CURLE_OK;
		return *this;
	}
	//curlȡ��Ŀ��URLָ������ݵ��ֽڴ�С������������ǰӦ�ó�ֵ�����ѡ��������Ա�֤����ִ�гɹ���
	size_t getInfoContentLength();
	//curlȡ�ý����CURLcode��Ӧ�Ľ�����Ϣ�ַ�����
	static PCStr getLastErrorString(CURLcode err);
	//curlȡ�ý����CURLcode��Ӧ�Ľ�����Ϣ�ַ�����
	PCStr getLastErrorString() const { return getLastErrorString(getLastError()); }
	//curlȡ�ý����CURLcode��Ӧ�Ľ�����Ϣ�ַ�����
	PCStr getCodeString() const { return getLastErrorString(); }
	//curlȡ�ý����CURLcode��Ӧ�Ľ�����Ϣ�ַ���������Ϣ����ʾ��
	static CURLcode getLastErrorMsgBox(CURLcode err);
	//curlȡ�ý����CURLcode��Ӧ�Ľ�����Ϣ�ַ���������Ϣ����ʾ��
	CURLcode getLastErrorMsgBox() const { return getLastErrorMsgBox(getLastError()); }
	//curlȡ�ý����CURLcode��CURLE_OK�⣬��Ӧ�Ľ�����Ϣ�ַ���������Ϣ����ʾ��
	static CURLcode getLastErrorMsgBoxExceptSucceed(CURLcode err);
	//curlȡ�ý����CURLcode��CURLE_OK�⣬��Ӧ�Ľ�����Ϣ�ַ���������Ϣ����ʾ��
	CURLcode getLastErrorMsgBoxExceptSucceed() const { return getLastErrorMsgBoxExceptSucceed(getLastError()); }
};

#endif