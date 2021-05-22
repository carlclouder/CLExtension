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

// 不需要错误提示框，则置为0
#define OPEN_CCURL_ERROR_MSGBOX 1 
#if OPEN_CCURL_ERROR_MSGBOX > 0
#define CCURL_CHECK_CODE_ERROR() getLastErrorMsgBoxCNExceptSucceed()
#else
#define CCURL_CHECK_CODE_ERROR() 
#endif


struct CURLBASE { CURL* curl = nullptr; };
class CLCurl : CURLBASE {
public:
	typedef CURLBASE base;
	typedef CLCurl& ref;
	typedef const CLCurl& refc;
	typedef std::vector<char> ResultBuffer;
	typedef std::map<string, string> ResultHead;
protected:
	bool _isInit = false;
	CURLcode _lastError = CURLE_OK;
	ResultBuffer _buf;
	ResultHead _head;
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
		auto newsi = size * nmemb;
		buf.resize(pos + newsi);
		memcpy_s(buf.data() + pos, newsi, buffer, newsi);
		return nmemb;
	}
	static bool& _getGbInit() {
		static bool _isGlbInit = false;
		return _isGlbInit;
	}
	//保存表达式返回码，并检查提示。
#define CCURL_SAVE_CODE_CHECK( doReturnExpr ) (_lastError = (doReturnExpr),CCURL_CHECK_CODE_ERROR(),_lastError)
public:
	//默认使用的浏览器客户端。
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
	//curl全局清理。
	static void globalCleanup() {
		curl_global_cleanup();
	}
	//curl全局初始化。
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
	//curl对象初始化。
	//该函数不一定要显示调用，它会在其他需要的时刻自动执行。
	//若对象本身保存了一个有效初始化的句柄，调用该函数后辉先以保存的句柄，再重新创建。
	//因此，应该在显示调用前，用isInit()查询当前的对象初始化状态。
	ref init() {
		globalInit(CURL_GLOBAL_ALL);
		*this = curl_easy_init(); // 初始化CURL句柄
		return setUserAgent(DefaultUserAgent);
	}
	//curl对象检查当前是否已初始化。
	bool isInit() const { return _isInit; }
	//取得上一行为返回的结果码CURLcode。
	CURLcode getLastError() const { return _lastError; }
	//取得上一行为返回的结果码CURLcode。
	CURLcode getCode() const { return getLastError(); }
	//curl设置用户选项参数。
	template<class ...Args>
	ref setopt(CURLoption option, Args ...args) {
		//if (!isInit())throw std::runtime_error("CLCurl is not init!");
		if (!isInit())init();
		CCURL_SAVE_CODE_CHECK(curl_easy_setopt(*this, option, std::forward<Args>(args)...));
		return *this;
	}
	//curl设置目标url字符串。
	ref setUrl(PCStr url) { return setopt(CURLOPT_URL, url); }
	//curl设置向服务端发送的来源地址。
	ref setRefer(PCStr referUrl) { return setopt(CURLOPT_REFERER, referUrl); }
	//curl设置浏览器客户端。
	ref setUserAgent(PCStr userAgent = DefaultUserAgent) {
		return setopt(CURLOPT_USERAGENT, userAgent);
	}
	//curl设置SSL对端验证。
	ref setSSLverifyPeer(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYPEER, yes); }
	//curl设置SSL主机验证。
	ref setSSLverifyHost(bool yes = false) { return setopt(CURLOPT_SSL_VERIFYHOST, yes); }
	//curl设置超时秒数。
	ref setTimeout(int sec) { return setopt(CURLOPT_TIMEOUT, sec); }
	//curl设置超时毫秒数。
	ref setTimeoutMs(int minisec) { return setopt(CURLOPT_TIMEOUT_MS, minisec); }
	//curl设置是否请求信息头数据。
	ref setHeader(bool uesHeader = false) { return setopt(CURLOPT_HEADER, uesHeader); }
	//curl设置解码方式。
	ref setEncoding(PCStr encode = "gzip") { return setopt(CURLOPT_ENCODING, encode); }
	//curl设置重定向抓取。
	ref setFollowLocation(bool follow = false) { return setopt(CURLOPT_FOLLOWLOCATION, follow); }
	typedef size_t(*PWFCallBack)(void*, size_t, size_t, void*);
	//curl设置写入回调函数。
	ref setWriteFunction(PWFCallBack pFunc) { return setopt(CURLOPT_WRITEFUNCTION, pFunc); }
	//curl设置写入回调函数参数数据。
	ref setWriteData(void* param) { return setopt(CURLOPT_WRITEDATA, param); }
	//curl请求执行,保存结果到内部buf。
	ref perform() {
		return perform(this->_buf);
	}
	//curl请求执行，并将结果返回数据集storeBuf。
	//设置CURLOPT_NOPROGRESS        显示内部预设方式显示进度信息；
	//设置CURLOPT_XFERINFOFUNCTION  采用自定义回调处理进度过程；
	ref perform(ResultBuffer& storeBuf) {
		_buferStorePack _pack{ 0, storeBuf };
		setWriteFunction(_perfToBufferCallBack).setWriteData((void*)&_pack);
		CCURL_SAVE_CODE_CHECK(curl_easy_perform(*this));
		return *this;
	}
	const char* getData() const {
		return _buf.data();
	}
	size_t getDataSize() const {
		return _buf.size();
	}
	//curl对象清理释放。
	ref cleanup() {
		if (this->curl)
			curl_easy_cleanup(*this), this->curl = nullptr;
		_isInit = false, _lastError = CURLE_OK;
		return *this;
	}
	//curl对象重置;It does keep: live connections, the Session ID cache, the DNS cache and the cookies.
	ref reset() {
		if (!isInit())init();
		else curl_easy_reset(this->curl);
		_lastError = CURLE_OK;
		return *this;
	}
	//curl取得目标URL指向的内容的字节大小。（函数调用前应该充分的设置选项参数，以保证函数执行成功）
	size_t getContentLength();
	
	ref getHeader();
	const ResultHead& headerMap()const { return _head; }
	//curl取得结果码CURLcode对应的解释信息字符串。
	static PCStr getLastErrorStringCN(CURLcode err,bool useCN = true);
	//curl取得结果码CURLcode对应的解释信息字符串。
	PCStr getLastErrorStringCN() const { return getLastErrorStringCN(getLastError()); }
	//curl取得结果码CURLcode对应的解释信息字符串。
	PCStr getCodeStringCN() const { return getLastErrorStringCN(); }
	//curl取得结果码CURLcode对应的解释信息字符串并用消息框提示。
	static CURLcode getLastErrorMsgBoxCN(CURLcode err);
	//curl取得结果码CURLcode对应的解释信息字符串并用消息框提示。
	CURLcode getLastErrorMsgBoxCN() const { return getLastErrorMsgBoxCN(getLastError()); }
	//curl取得结果码CURLcode除CURLE_OK外，对应的解释信息字符串并用消息框提示。
	static CURLcode getLastErrorMsgBoxCNExceptSucceed(CURLcode err);
	//curl取得结果码CURLcode除CURLE_OK外，对应的解释信息字符串并用消息框提示。
	CURLcode getLastErrorMsgBoxCNExceptSucceed() const { return getLastErrorMsgBoxCNExceptSucceed(getLastError()); }
	// do upload, will overwrite existing file
	ref ftpUpload(PCStr remote_file_path,
		PCStr local_file_path,
		PCStr username,
		PCStr password,
		long timeout = 0, long tries = 3);
	// do download, will overwrite existing file
	ref ftpDownload(PCStr remote_file_path,
		PCStr local_file_path,
		PCStr username,
		PCStr password,
		long timeout = 30);
};

#endif