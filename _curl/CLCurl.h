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
#include "curl-7.76.1/include/curl/curl.h"

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
	static size_t perfToBufferCallBack(void* buffer, size_t size, size_t nmemb, void* buferStorePack)
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
	static bool& getGbInit() {
		static bool _isGlbInit = false;
		return _isGlbInit;
	}
public:
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
	ref operator =(CLCurl&& other) noexcept{
		std::swap(this->curl, other.curl);
		std::swap(this->_isInit, other._isInit);
		std::swap(this->_lastError, other._lastError);
		other.clean();
		return *this;
	}
	static void globalClean() {
		curl_global_cleanup();
	}
	static bool globalInit(long flags = CURL_GLOBAL_ALL) {
		if (!getGbInit()) {
			if (curl_global_init(flags) == CURLE_OK)
				getGbInit() = true;
		}
		return getGbInit();
	}	
	operator CURL* () const { return base::curl; }
	ref operator = (CURL* curl) { 
		if (this->curl)curl_easy_cleanup(*this);
		_isInit = (this->curl = curl) ? true : false;
		_lastError = CURLE_OK; 
		return *this; 
	}
	ref operator = (PCStr url) { return setUrl(url); }
	ref init() {
		globalInit(CURL_GLOBAL_ALL);	
		*this = curl_easy_init(); // ³õÊ¼»¯CURL¾ä±ú
		return setUserAgent(DefaultUserAgent);
	}
	CURLcode getLastError() const { return _lastError; }
	CURLcode getRLcode() const { return getLastError(); }
	template<class ...Args>
	ref setopt(CURLoption option, Args ...args) {
		//if (!_isInit)throw std::runtime_error("CLCurl is not init!");
		if (!_isInit)init();
		_lastError = (CURLcode)curl_easy_setopt(*this, option, std::forward<Args>(args)...);
		return *this;
	}
	ref setUrl(PCStr url) { return setopt(CURLOPT_URL, url); }
	ref setRefer(PCStr referUrl) { return setopt(CURLOPT_REFERER, referUrl); }	
	ref setUserAgent(PCStr userAgent = DefaultUserAgent) {
		return setopt(CURLOPT_USERAGENT, userAgent); }
	ref setSSLverifyPeer(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYPEER, yes); }
	ref setSSLverifyHost(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYHOST, yes); }
	ref setTimeout(int sec) { return setopt(CURLOPT_TIMEOUT, sec); }
	ref setTimeoutMs(int minisec) { return setopt(CURLOPT_TIMEOUT_MS, minisec); }
	ref setHeader(void* pheader = nullptr) { return setopt(CURLOPT_HEADER, pheader); }
	ref setEncoding(PCStr encode = "gzip") { return setopt(CURLOPT_ENCODING, encode); }
	typedef size_t(*PWFCallBack)(void*, size_t, size_t, void*);
	ref setWriteFunction(PWFCallBack pFunc) { return setopt(CURLOPT_WRITEFUNCTION, pFunc); }
	ref setWriteData(void* param) { return setopt(CURLOPT_WRITEDATA, param); }
	ref perform() {
		if (!_isInit)init();
		_lastError = curl_easy_perform(*this); 
		return *this;
	}
	ref perform(ResultBuffer& storeBuf) {
		_buferStorePack _data{ 0, storeBuf };
		return setWriteFunction(perfToBufferCallBack)
			.setWriteData((void*)&_data).perform();
	}
	ref clean() {
		if(this->curl)curl_easy_cleanup(*this), this->curl = nullptr;
		_isInit = false, _lastError = CURLE_OK;
		return *this;
	}	
};

#endif