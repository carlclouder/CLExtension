#include "CLCurl.h"
#include "curl.lib/curl-7.76.1/lib/urldata.h"
#include "../_cl_string/CLString.h"

size_t _write_data_doNothing(void* ptr, size_t size, size_t nmemb, void* stream)
{
	return nmemb; //什么也不做
}

size_t CLCurl::getInfoContentLength() {
	double downloadFileLenth = 0;
	if (!_isInit)init();
	//直接由底层源码取值；该方式需要工程完全由源码编译构成，而不是连接外部库；
	bool isHeadBk = this->curl->set.include_header;
	bool isNoBody = this->curl->set.opt_no_body;
	auto pFbk = this->curl->set.fwrite_func;
	auto pParam = this->curl->set.out;
	curl_easy_setopt(*this, CURLOPT_HEADER, true);
	curl_easy_setopt(*this, CURLOPT_NOBODY, true);
	curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, _write_data_doNothing);//屏蔽调用默认输出函数
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, 0);
	_lastError = curl_easy_perform(*this);
	_CHECK_CODE_ERROR();
	if (getLastError() == CURLE_OK) {
		_lastError = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
		_CHECK_CODE_ERROR();
	}
	//此处可以还原
	curl_easy_setopt(*this, CURLOPT_HEADER, isHeadBk);
	curl_easy_setopt(*this, CURLOPT_NOBODY, isNoBody);
	curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, pFbk);
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, pParam);
	return (size_t)(downloadFileLenth < 0 ? 0: downloadFileLenth);
}
PCStr CLCurl::getLastErrorString(CURLcode err)
{
	switch (err)
	{
	case CURLE_OK: return "成功！"; break;
	case CURLE_UNSUPPORTED_PROTOCOL:
		return "你的URL传递给libcurl的使用协议，这libcurl的不支持；支持可能是你没有使用一个编译时的选项，它可以是一个拼写错的协议字符串，或者只是一个协议的libcurl没有代码"; break;
	case  CURLE_FAILED_INIT:
		return "非常早期的初始化代码失败。这可能是内部错误或问题，资源问题，一些基本的东西可能无法完成初始化时间"; break;
	case  CURLE_URL_MALFORMAT:
		return "该网址的格式不正确"; break;
	case  CURLE_NOT_BUILT_IN:
		return "libcurl的内置在一个编译时决定所要求的功能，协议或购股权没有被发现。"; break;
	case  CURLE_COULDNT_RESOLVE_PROXY:
		return "无法解析代理服务器；代理主机无法得到解决"; break;
	case  CURLE_COULDNT_RESOLVE_HOST:
		return "无法解析主机；给定的远程主机没有得到解决"; break;
	case  CURLE_COULDNT_CONNECT: return "connect 的主机或代理失败"; break;
	case CURLE_FTP_WEIRD_SERVER_REPLY: return "连接到一个FTP服务器后，libcurl的预期得到一定的回复返回；这个错误代码表示，它有一个奇怪的或坏的答复；指定的远程服务器可能不是一个确定的FTP服务器"; break;
	case CURLE_REMOTE_ACCESS_DENIED: return "我们被拒绝访问的资源的URL；对于FTP，发生这种情况而力图改变的远程目录"; break;
	case CURLE_FTP_ACCEPT_FAILED: return "在等待服务器的连接时，一个主动FTP会话使用，被送到控制连接或类似的错误代码"; break;
	case CURLE_FTP_WEIRD_PASS_REPLY: return "发送到服务器的FTP密码后，libcurl的预计正确的答复；此错误代码指示返回了意外的代码"; break;
	case CURLE_FTP_ACCEPT_TIMEOUT: return "在当前的FTP会话在等待服务器连接，CURLOPT_ACCEPTTIMOUT_MS（或内部默认），超时过期"; break;
	case CURLE_FTP_WEIRD_PASV_REPLY: return "libcurl的失败作为一个PASV或EPSV命令从服务器得到一个合理的结果；服务器是有缺陷的"; break;
	case CURLE_FTP_WEIRD_227_FORMAT: return "FTP服务器返回一个227行作为一个PASV命令的响应；如果libcurl的无法解析该行，此返回代码被传递回"; break;
	case CURLE_FTP_CANT_GET_HOST: return "内部故障查找主机使用新的连接"; break;
	case CURLE_FTP_COULDNT_SET_TYPE: return "收到一个错误，当试图传送模式设置为二进制或ASCII"; break;
	case CURLE_PARTIAL_FILE: return "文件传输，短于或大于预期；发生这种情况时，服务器首先报告预期的传输大小，然后提供数据不匹配前面给出的大小"; break;
	case CURLE_FTP_COULDNT_RETR_FILE: return "这是一个奇怪的回答“RETR”命令或一个零字节传输完成"; break;
	case CURLE_QUOTE_ERROR: return "当发送到远程服务器，自定义的“QUOTE”命令的一个命令返回的错误代码为或更高（对于FTP）或表示不成功的完成命令"; break;
	case CURLE_HTTP_RETURNED_ERROR: return "这是返回CURLOPT_FAILONERROR设置为TRUE和HTTP服务器返回的错误代码是 > = "; break;
	case CURLE_WRITE_ERROR: return "发生错误，写作时接收到的数据到本地文件，或者返回错误libcurl的一个写回调"; break;
	case CURLE_UPLOAD_FAILED: return "开始上载失败；对于FTP，服务器通常否认的STOR命令；通常的错误缓冲区包含了服务器的解释"; break;
	case CURLE_READ_ERROR: return "有一个问题读取本地文件或返回一个错误的读回调"; break;
	case CURLE_OUT_OF_MEMORY: return "内存分配请求失败；这是严重的不良和活动，如果发生过严重搞砸了"; break;
	case CURLE_OPERATION_TIMEDOUT: return "操作超时；根据条件达到指定的超时期间"; break;
	case CURLE_FTP_PORT_FAILED: return "FTP PORT命令返回错误；这主要是当你还没有足够的地址指定了一个良好的libcurl的使用；See CURLOPT_FTPPORT"; break;
	case CURLE_FTP_COULDNT_USE_REST: return "FTP REST命令返回错误；如果服务器是明智的，这不应该发生"; break;
	case CURLE_RANGE_ERROR: return "服务器不支持或接受范围请求"; break;
	case CURLE_HTTP_POST_ERROR: return "这是一个奇怪的错误，主要发生是由于内部的混乱"; break;
	case CURLE_SSL_CONNECT_ERROR: return "出现问题的地方，在SSL / TLS握手；你真正想要的的错误缓冲区和阅读邮件，因为它针对问题稍微；可能是证书（文件格式，路径，权限），密码，和其他人"; break;
	case CURLE_BAD_DOWNLOAD_RESUME: return "下载无法恢复，因为指定的偏移量为文件的边界"; break;
	case CURLE_FILE_COULDNT_READ_FILE: return "无法打开的文件FILE : //；最有可能的，因为该文件的路径不能识别现有文件；你是否检查文件的权限？"; break;
	case CURLE_LDAP_CANNOT_BIND: return "LDAP无法绑定；LDAP绑定操作失败"; break;
	case CURLE_LDAP_SEARCH_FAILED: return "LDAP搜索失败"; break;
	case CURLE_FUNCTION_NOT_FOUND: return "函数没有找到；一个必需的zlib的功能没有被发现"; break;
	case CURLE_ABORTED_BY_CALLBACK: return "通过回调中止；返回的回调“中止”libcurl的"; break;
	case CURLE_BAD_FUNCTION_ARGUMENT: return "内部错误；函数调用了一个错误的参数"; break;
	case CURLE_INTERFACE_FAILED: return "接口错误；指定的出接口不能使用；设置接口使用传出连接的源IP地址与CURLOPT_INTERFACE"; break;
	case CURLE_TOO_MANY_REDIRECTS: return "过多的重定向；以下重定向时，libcurl的创下的最高金额；设置您的与CURLOPT_MAXREDIRS限制"; break;
	case CURLE_UNKNOWN_OPTION: return "无法识别的选项传递给libcurl的选项；请参阅相应的文档；这是最有可能在程序中使用libcurl的问题；的的错误缓冲区可能包含准确的选项，它涉及更具体的信息"; break;
	case CURLE_TELNET_OPTION_SYNTAX: return "telnet选项字符串被非法格式化"; break;
	case CURLE_PEER_FAILED_VERIFICATION: return "远程服务器的SSL证书或SSH的MD指纹被认为是不正常的"; break;
	case CURLE_GOT_NOTHING: return "没有从服务器返回的，和得到什么的情况下，被认为是一个错误"; break;
	case CURLE_SSL_ENGINE_NOTFOUND: return "未找到指定的加密引擎"; break;
	case CURLE_SSL_ENGINE_SETFAILED: return "设置所选的SSL加密引擎，默认情况下失败"; break;
	case CURLE_SEND_ERROR: return "发送网络数据失败"; break;
	case CURLE_RECV_ERROR: return "如果接收网络数据"; break;
	case CURLE_SSL_CERTPROBLEM: return "与当地的客户端证书的问题"; break;
	case CURLE_SSL_CIPHER: return "无法使用指定的密码"; break;
		//case CURLE_SSL_CACERT: return "同侪凭证不能与已知的CA证书进行身份验证";break; 
	case CURLE_BAD_CONTENT_ENCODING: return "无法识别的传输编码"; break;
	case CURLE_LDAP_INVALID_URL: return "无效的LDAP URL"; break;
	case CURLE_FILESIZE_EXCEEDED: return "最大文件大小超过"; break;
	case CURLE_USE_SSL_FAILED: return "要求FTP SSL水平失败"; break;
	case CURLE_SEND_FAIL_REWIND: return "在做了一个发送操作卷曲，倒带重传的数据，但的倒带操作失败"; break;
	case CURLE_SSL_ENGINE_INITFAILED: return "SSL发动机启动失败"; break;
	case CURLE_LOGIN_DENIED: return "远程服务器拒绝卷曲登录（加入..）"; break;
	case CURLE_TFTP_NOTFOUND: return "TFTP服务器上找不到文件"; break;
	case CURLE_TFTP_PERM: return "TFTP服务器上的权限问题"; break;
	case CURLE_REMOTE_DISK_FULL: return "出在服务器上的磁盘空间"; break;
	case CURLE_TFTP_ILLEGAL: return "非法的TFTP操作"; break;
	case CURLE_TFTP_UNKNOWNID: return "未知TFTP传输ID"; break;
	case CURLE_REMOTE_FILE_EXISTS: return "文件已经存在，并不会被覆盖"; break;
	case CURLE_TFTP_NOSUCHUSER: return "这个错误不应该被返回正常工作的TFTP服务器"; break;
	case CURLE_CONV_FAILED: return "字符转换失败"; break;
	case CURLE_CONV_REQD: return "调用者必须注册转换回调"; break;
	case CURLE_SSL_CACERT_BADFILE: return "问题读取SSL证书（路径的访问权限？）"; break;
	case CURLE_REMOTE_FILE_NOT_FOUND: return "不存在的URL引用的资源"; break;
	case CURLE_SSH: return "未指定的错误发生在SSH会话"; break;
	case CURLE_SSL_SHUTDOWN_FAILED: return "关闭SSL连接失败"; break;
	case CURLE_AGAIN: return "Socket是没有准备好发送/接收等待，直到它准备好了，然后再试一次。"; break;
	case CURLE_SSL_CRL_BADFILE: return "无法加载CRL文件"; break;
	case CURLE_SSL_ISSUER_ERROR: return "发行人检查失败"; break;
	case CURLE_FTP_PRET_FAILED: return "FTP服务器不理解的PRET命令，所有不支持给定的参数；要小心时usingCURLOPT_CUSTOMREQUEST，自定义列表“命令将发送PRET CMD前PASV以及"; break;
	case CURLE_RTSP_CSEQ_ERROR: return "RTSP的Cseq号码不匹配"; break;
	case CURLE_RTSP_SESSION_ERROR: return "RTSP会话标识符不匹配"; break;
	case CURLE_FTP_BAD_FILE_LIST: return "无法，解析FTP文件列表（在FTP通配符下载）"; break;
	case CURLE_CHUNK_FAILED: return "块回调报告错误"; break;
	default:return "未知结果！"; break;
	}
}
CURLcode CLCurl::getLastErrorMsgBox(CURLcode err) {
	PCStr rt = getLastErrorString(err);
	CLString().format("code:  %d  \n%s", err, rt).messageBox("CURLcode info:",MB_ICONWARNING);
	return err;
}
CURLcode CLCurl::getLastErrorMsgBoxExceptSucceed(CURLcode err) {
	return err == CURLE_OK ? err : getLastErrorMsgBox(err);
}
