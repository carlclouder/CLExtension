#include "CLCurl.h"
#include "curl.lib/curl-7.76.1/lib/urldata.h"
#include "../_cl_string/CLString.h"
struct _writeFunc {
    static size_t doNothing(void* ptr, size_t size, size_t nmemb, void* stream) {
        return size * nmemb; //什么也不做
    }
};
size_t CLCurl::getContentLength() {   
	double downloadFileLenth = 0;
	if (!isInit())init();
	//直接由底层源码取值；该方式需要工程完全由源码编译构成，而不是连接外部库；
	bool isHeadBk = this->curl->set.include_header;
	bool isNoBody = this->curl->set.opt_no_body;
	auto pFbk = this->curl->set.fwrite_func;
	auto pParam = this->curl->set.out;
    auto pec = this->curl->set.str[STRING_ENCODING];
    this->curl->set.str[STRING_ENCODING] = 0;
	curl_easy_setopt(*this, CURLOPT_HEADER, true);
	curl_easy_setopt(*this, CURLOPT_NOBODY, true);
    curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, _writeFunc::doNothing);//屏蔽调用默认输出函数
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, 0);
    curl_easy_setopt(*this, CURLOPT_CUSTOMREQUEST, "GET");
    CCURL_SAVE_CODE_CHECK(curl_easy_perform(*this));
	if (getLastError() == CURLE_OK) {
        CCURL_SAVE_CODE_CHECK(curl_easy_getinfo(*this, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth));
	}
	//此处可以还原
	curl_easy_setopt(*this, CURLOPT_HEADER, isHeadBk);
	curl_easy_setopt(*this, CURLOPT_NOBODY, isNoBody);
	curl_easy_setopt(*this, CURLOPT_WRITEFUNCTION, pFbk);
	curl_easy_setopt(*this, CURLOPT_WRITEDATA, pParam);
    this->curl->set.str[STRING_ENCODING] = pec;
	return (size_t)(downloadFileLenth < 0 ? 0: downloadFileLenth);
}

PCStr CLCurl::getLastErrorStringCN(CURLcode err, bool useCN)
{
    if (useCN == false)
        return curl_easy_strerror(err);

#ifndef CURL_DISABLE_VERBOSE_STRINGS
    switch (err) {
    case CURLE_OK:
        return "无错误！";

    case CURLE_UNSUPPORTED_PROTOCOL:
        return "不支持的协议。";

    case CURLE_FAILED_INIT:
        return "初始化失败。";

    case CURLE_URL_MALFORMAT:
        return "URL使用错误/非法格式或缺少URL。";

    case CURLE_NOT_BUILT_IN:
        return "请求的功能、协议或选项未内置，这是由于生成时决定的。";

    case CURLE_COULDNT_RESOLVE_PROXY:
        return "无法解析代理名称。";

    case CURLE_COULDNT_RESOLVE_HOST:
        return "无法解析主机名。";

    case CURLE_COULDNT_CONNECT:
        return "无法连接到服务器。";

    case CURLE_WEIRD_SERVER_REPLY:
        return "服务器回复";

    case CURLE_REMOTE_ACCESS_DENIED:
        return "拒绝访问远程资源";

    case CURLE_FTP_ACCEPT_FAILED:
        return "FTP:服务器连接数据端口失败";

    case CURLE_FTP_ACCEPT_TIMEOUT:
        return "FTP:接受服务器连接超时";

    case CURLE_FTP_PRET_FAILED:
        return "FTP:服务器不接受PRET命令。";

    case CURLE_FTP_WEIRD_PASS_REPLY:
        return "FTP:未知的通过回复";

    case CURLE_FTP_WEIRD_PASV_REPLY:
        return "FTP:未知的PASV回复";

    case CURLE_FTP_WEIRD_227_FORMAT:
        return "FTP:未知227响应格式";

    case CURLE_FTP_CANT_GET_HOST:
        return "FTP:在PASV响应中找不到主机";

    case CURLE_HTTP2:
        return "HTTP2帧层出错";

    case CURLE_FTP_COULDNT_SET_TYPE:
        return "FTP:无法设置文件类型";

    case CURLE_PARTIAL_FILE:
        return "已传输部分文件";

    case CURLE_FTP_COULDNT_RETR_FILE:
        return "FTP:无法检索（RETR失败）指定文件";

    case CURLE_QUOTE_ERROR:
        return "Quote命令返回错误";

    case CURLE_HTTP_RETURNED_ERROR:
        return "HTTP响应码表示错误";

    case CURLE_WRITE_ERROR:
        return "将接收到的数据写入磁盘/应用程序失败";

    case CURLE_UPLOAD_FAILED:
        return "上传失败（开始/起飞前）";

    case CURLE_READ_ERROR:
        return "无法从文件/应用程序中打开/读取本地数据";

    case CURLE_OUT_OF_MEMORY:
        return "内存不足”";

    case CURLE_OPERATION_TIMEDOUT:
        return "达到超时";

    case CURLE_FTP_PORT_FAILED:
        return "FTP:命令端口失败";

    case CURLE_FTP_COULDNT_USE_REST:
        return "FTP:命令REST失败";

    case CURLE_RANGE_ERROR:
        return "服务器没有传递请求的范围";

    case CURLE_HTTP_POST_ERROR:
        return "设置岗位内部问题";

    case CURLE_SSL_CONNECT_ERROR:
        return "SSL连接错误";

    case CURLE_BAD_DOWNLOAD_RESUME:
        return "无法继续下载";

    case CURLE_FILE_COULDNT_READ_FILE:
        return "无法读取文件：//文件";

    case CURLE_LDAP_CANNOT_BIND:
        return "LDAP:无法绑定";

    case CURLE_LDAP_SEARCH_FAILED:
        return "LDAP:搜索失败";

    case CURLE_FUNCTION_NOT_FOUND:
        return "库中未找到所需函数";

    case CURLE_ABORTED_BY_CALLBACK:
        return "操作被应用程序回调中止";

    case CURLE_BAD_FUNCTION_ARGUMENT:
        return "libcurl函数的参数错误";

    case CURLE_INTERFACE_FAILED:
        return "绑定本地连接端失败";

    case CURLE_TOO_MANY_REDIRECTS:
        return "达到最大重定向次数";

    case CURLE_UNKNOWN_OPTION:
        return "向libcurl传递了未知选项";

    case CURLE_TELNET_OPTION_SYNTAX:
        return "telnet选项格式错误";

    case CURLE_GOT_NOTHING:
        return "服务器没有返回任何内容（没有头，没有数据）";

    case CURLE_SSL_ENGINE_NOTFOUND:
        return "未找到SSL加密引擎";

    case CURLE_SSL_ENGINE_SETFAILED:
        return "无法将SSL加密引擎设置为默认值";

    case CURLE_SSL_ENGINE_INITFAILED:
        return "初始化SSL加密引擎失败";

    case CURLE_SEND_ERROR:
        return "向对等方发送数据失败";

    case CURLE_RECV_ERROR:
        return "接收对等数据失败";

    case CURLE_SSL_CERTPROBLEM:
        return "本地SSL证书有问题";

    case CURLE_SSL_CIPHER:
        return "无法使用指定的SSL密码";

    case CURLE_PEER_FAILED_VERIFICATION:
        return "SSL对等证书或SSH远程密钥不正常";

    case CURLE_SSL_CACERT_BADFILE:
        return "PSSL CA证书（路径）有问题？访问权限？）";

    case CURLE_BAD_CONTENT_ENCODING:
        return "无法识别或错误的HTTP内容或传输编码";

    case CURLE_LDAP_INVALID_URL:
        return "无效的LDAP URL";

    case CURLE_FILESIZE_EXCEEDED:
        return "超过最大文件大小";

    case CURLE_USE_SSL_FAILED:
        return "请求的SSL级别失败";

    case CURLE_SSL_SHUTDOWN_FAILED:
        return "关闭SSL连接失败";

    case CURLE_SSL_CRL_BADFILE:
        return "加载CRL文件失败（路径？访问权限？、格式？）";

    case CURLE_SSL_ISSUER_ERROR:
        return "颁发者对对等证书检查失败";

    case CURLE_SEND_FAIL_REWIND:
        return "数据流倒带失败，发送失败";

    case CURLE_LOGIN_DENIED:
        return "拒绝登录";

    case CURLE_TFTP_NOTFOUND:
        return "TFTP:找不到文件";

    case CURLE_TFTP_PERM:
        return "TFTP:访问冲突";

    case CURLE_REMOTE_DISK_FULL:
        return "D磁盘已满或超出分配";

    case CURLE_TFTP_ILLEGAL:
        return "TFTP:非法操作";

    case CURLE_TFTP_UNKNOWNID:
        return "TFTP:未知传输ID";

    case CURLE_REMOTE_FILE_EXISTS:
        return "远程文件已经存在";

    case CURLE_TFTP_NOSUCHUSER:
        return "TFTP:无此用户";

    case CURLE_CONV_FAILED:
        return "转换失败";

    case CURLE_CONV_REQD:
        return "调用者必须注册CURLOPT_CONV_ 回调选项";

    case CURLE_REMOTE_FILE_NOT_FOUND:
        return "未找到远程文件";

    case CURLE_SSH:
        return "SSH层出错";

    case CURLE_AGAIN:
        return "套接字未准备好发送/接收";

    case CURLE_RTSP_CSEQ_ERROR:
        return "RTSP CSeq不匹配或CSeq无效";

    case CURLE_RTSP_SESSION_ERROR:
        return "RTSP会话错误";

    case CURLE_FTP_BAD_FILE_LIST:
        return "无法解析FTP文件列表";

    case CURLE_CHUNK_FAILED:
        return "区块回调失败";

    case CURLE_NO_CONNECTION_AVAILABLE:
        return "达到最大连接限制";

    case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
        return "SSL公钥与固定公钥不匹配";

    case CURLE_SSL_INVALIDCERTSTATUS:
        return "SSL服务器证书状态验证失败";

    case CURLE_HTTP2_STREAM:
        return "HTTP/2帧层流错误";

    case CURLE_RECURSIVE_API_CALL:
        return "从回调中调用API函数";

    case CURLE_AUTH_ERROR:
        return "认证函数返回错误";

    case CURLE_HTTP3:
        return "HTTP/3错误";

    case CURLE_QUIC_CONNECT_ERROR:
        return "QUIC连接错误";

    case CURLE_PROXY:
        return "代理握手错误";

        /* error codes not used by current libcurl */
    case CURLE_OBSOLETE20:
    case CURLE_OBSOLETE24:
    case CURLE_OBSOLETE29:
    case CURLE_OBSOLETE32:
    case CURLE_OBSOLETE40:
    case CURLE_OBSOLETE44:
    case CURLE_OBSOLETE46:
    case CURLE_OBSOLETE50:
    case CURLE_OBSOLETE51:
    case CURLE_OBSOLETE57:
    case CURL_LAST:
        break;
    }
    /*
     * By using a switch, gcc -Wall will complain about enum values
     * which do not appear, helping keep this function up-to-date.
     * By using gcc -Wall -Werror, you can't forget.
     *
     * A table would not have the same benefit.  Most compilers will
     * generate code very similar to a table in any case, so there
     * is little performance gain from a table.  And something is broken
     * for the user's application, anyways, so does it matter how fast
     * it _doesn't_ work?
     *
     * The line number for the error will be near this comment, which
     * is why it is here, and not at the start of the switch.
     */
    return "未知错误";
#else
    if (!err)
        return "无错误";
    else
        return "错误";
#endif

    //old define abandon ------------------------
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
    case CURLE_FTP_PRET_FAILED: return "FTP服务器不理解的PRET命令，所有不支持给定的参数；要小心时using CURLOPT_CUSTOMREQUEST，自定义列表“命令将发送PRET CMD前PASV以及"; break;
    case CURLE_RTSP_CSEQ_ERROR: return "RTSP的Cseq号码不匹配"; break;
    case CURLE_RTSP_SESSION_ERROR: return "RTSP会话标识符不匹配"; break;
    case CURLE_FTP_BAD_FILE_LIST: return "无法，解析FTP文件列表（在FTP通配符下载）"; break;
    case CURLE_CHUNK_FAILED: return "块回调报告错误"; break;
    default:return "未知结果！"; break;
    }
}

CURLcode CLCurl::getLastErrorMsgBoxCN(CURLcode err) {
	PCStr rt = getLastErrorStringCN(err);
	CLString().format("code:  %d  \n%s", err, rt).messageBox("CURLcode info:",MB_ICONWARNING);
	return err;
}

CURLcode CLCurl::getLastErrorMsgBoxCNExceptSucceed(CURLcode err) {
	return err == CURLE_OK ? err : getLastErrorMsgBoxCN(err);
}

//
//  Make sure libcurl version > 7.32
//
// ---- common progress display ---- //
struct CustomProgress
{
    curl_off_t lastruntime; /* type depends on version, see above */
    CURL* curl;
};

// work for both download and upload
int progressCallback(void* p,
    curl_off_t dltotal,
    curl_off_t dlnow,
    curl_off_t ultotal,
    curl_off_t ulnow)
{
    struct CustomProgress* progress = (struct CustomProgress*)p;
    CURL* curl = progress->curl;
    curl_off_t curtime = 0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &curtime);

    /* under certain circumstances it may be desirable for certain functionality
     to only run every N seconds, in order to do this the transaction time can
     be used */
    if ((curtime - progress->lastruntime) >= 3000000)
    {
        progress->lastruntime = curtime;
        printf_s("TOTAL TIME: %f \n", (float)curtime);
    }

    // do something to display the progress
    std::cout << "UP: " << ulnow << " bytes of " << ultotal << " bytes, DOWN: " << dlnow << " bytes of " << dltotal << " bytes \n";
    if (ultotal)
        printf_s("UP progress: %0.2f\n", float(ulnow / ultotal));
    if (dltotal)
        printf_s("DOWN progress: %0.2f\n", float(dlnow / dltotal));

    return 0;
}

// ---- upload related ---- //
// parse headers for Content-Length
size_t getContentLengthFunc(void* ptr, size_t size, size_t nmemb, void* stream)
{
    int r;
    long len = 0;

    r = sscanf_s((PCStr)ptr, "Content-Length: %ld\n", &len);
    if (r) /* Microsoft: we don't read the specs */
        *((long*)stream) = len;
    return size * nmemb;
}

// discard already downloaded data
size_t discardFunc(void* ptr, size_t size, size_t nmemb, void* stream)
{
    return size * nmemb;
}

// read data to upload
size_t readfunc(void* ptr, size_t size, size_t nmemb, void* stream)
{
    FILE* f = (FILE*)stream;
    size_t n;
    if (ferror(f))
        return CURL_READFUNC_ABORT;
    n = fread(ptr, size, nmemb, f) * size;
    return n;
}

int curlx_win32_stat(const char* path, struct_stat* buffer)
{
    int result = -1;
#ifdef _UNICODE
    wchar_t* path_w = curlx_convert_UTF8_to_wchar(path);
    if (path_w) {
#if defined(USE_WIN32_SMALL_FILES)
        result = _wstat(path_w, buffer);
#else
        result = _wstati64(path_w, buffer);
#endif
        free(path_w);
        if (result != -1)
            return result;
    }
#endif /* _UNICODE */

#if defined(USE_WIN32_SMALL_FILES)
    result = _stat(path, buffer);
#else
    result = _stati64(path, buffer);
#endif
    return result;
}
FILE* curlx_win32_fopen(const char* filename, const char* mode)
{
#ifdef _UNICODE
    FILE* rt = NULL;
    wchar_t* filename_w = curlx_convert_UTF8_to_wchar(filename);
    wchar_t* mode_w = curlx_convert_UTF8_to_wchar(mode);
    if (filename_w && mode_w)
         _wfopen_s(&rt,filename_w, mode_w);
    free(filename_w);
    free(mode_w);
    if (rt)
        return rt;
#endif
    FILE* rt = 0;
    fopen_s(&rt,filename, mode);
    return rt;
}

// do upload, will overwrite existing file
CLCurl& CLCurl::ftpUpload(PCStr remote_file_path,
    PCStr local_file_path,
    PCStr username,
    PCStr password,
    long timeout, long tries)
{
    // init curl handle
    if (!isInit())init();
    CURL* curlhandle = *this;
    // get user_key pair
    char user_key[1024] = { 0 };
    sprintf_s(user_key, "%s:%s", username, password);

    FILE* file;
    long uploaded_len = 0;
    _lastError = CURLE_GOT_NOTHING;
    file = curlx_win32_fopen(local_file_path, "rb");
    if (file == NULL)
    {
        perror(NULL);
        return *this;
    }
    curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);
    if (remote_file_path) //如果为null则不设置采用内部预设。
        curl_easy_setopt(curlhandle, CURLOPT_URL, remote_file_path);
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, user_key);
    if (timeout)
        curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getContentLengthFunc);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &uploaded_len);
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, discardFunc);
    curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
    curl_easy_setopt(curlhandle, CURLOPT_READDATA, file);
    curl_easy_setopt(curlhandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */
    curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

    // set upload progress
    curl_easy_setopt(curlhandle, CURLOPT_XFERINFOFUNCTION, progressCallback);
    struct CustomProgress prog;
    curl_easy_setopt(curlhandle, CURLOPT_XFERINFODATA, &prog);
    curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 0);

    //    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L); // if set 1, debug mode will print some low level msg

        // upload: 断点续传
    for (int c = 0; (getLastError() != CURLE_OK) && (c < tries); c++)
    {
        /* are we resuming? */
        if (c)
        { /* yes */
            /* determine the length of the file already written */
            /*
            * With NOBODY and NOHEADER, libcurl will issue a SIZE
            * command, but the only way to retrieve the result is
            * to parse the returned Content-Length header. Thus,
            * getContentLengthfunc(). We need discardfunc() above
            * because HEADER will dump the headers to stdout
            * without it.
            */
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);
            _lastError = curl_easy_perform(curlhandle);
            if (_lastError != CURLE_OK)
                continue;
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);
            fseek(file, uploaded_len, SEEK_SET);
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);
        }
        else
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);

        _lastError = curl_easy_perform(curlhandle);
    }
    fclose(file);

    if (_lastError != CURLE_OK)
    {
        fprintf(stderr, "%s\n", curl_easy_strerror(getCode()));
        CCURL_CHECK_CODE_ERROR();
    }

    // exit curl handle
    //curl_easy_cleanup(curlhandle);
    //curl_global_cleanup();

    return *this;
}

CLCurl& CLCurl::getHeader()
{
    if (!isInit())init();
    //直接由底层源码取值；该方式需要工程完全由源码编译构成，而不是连接外部库；
    bool isHeadBk = this->curl->set.include_header;
    bool isNoBody = this->curl->set.opt_no_body;
    auto pec = this->curl->set.str[STRING_ENCODING];
    this->curl->set.str[STRING_ENCODING] = 0;
    curl_easy_setopt(*this, CURLOPT_HEADER, true);
    curl_easy_setopt(*this, CURLOPT_NOBODY, true);
    curl_easy_setopt(*this, CURLOPT_CUSTOMREQUEST, "GET");
    perform();
   // CCURL_SAVE_CODE_CHECK(curl_easy_perform(*this));
    _head.clear();
    if (getLastError() == CURLE_OK) {
        CLString str = (LPCSTR)getData(),cut;
        str.split("\n");
        if (str.vtSize() >= 1) {
            cut = str.vtAtA(0);
            if (cut.trim().size() > 0)
                _head.insert(std::pair<string, string>(
                    cut.findMidString("", " "),
                    cut.string() + cut.find(" ") + 1
                    ));
        }
        for (size_t si = str.getVT().size(), i = 1; i < si; i++) {
            if (cut.set(str.vtAtA(i)).trim().size() == 0) 
                continue;            
            cut.split(":");
            auto& pvt = cut.getVT();
            _head.insert(std::pair<string, string>(
                pvt.size() >= 1 ? pvt.at(0) : "",
                pvt.size() >= 2 ? pvt.at(1) : ""
                ));
        }
    }
    //此处可以还原
    curl_easy_setopt(*this, CURLOPT_HEADER, isHeadBk);
    curl_easy_setopt(*this, CURLOPT_NOBODY, isNoBody);
    this->curl->set.str[STRING_ENCODING] = pec;
    return *this;
}

// ---- download related ---- //
// write data to upload
size_t writeFunc(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::cout << "--- write func ---" << std::endl;
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}


// do download, will overwrite existing file
CLCurl& CLCurl::ftpDownload(PCStr remote_file_path,
    PCStr local_file_path,
    PCStr username,
    PCStr password,
    long timeout)
{
    // init curl handle
   // curl_global_init(CURL_GLOBAL_ALL);
   // CURL* curlhandle = curl_easy_init();
    if (!isInit())init();
    CURL* curlhandle = *this;
    // get user_key pair
    char user_key[1024] = { 0 };
    sprintf_s(user_key, "%s:%s", username, password);

    FILE* file;
    curl_off_t local_file_len = -1;
    long filesize = 0;
    _lastError = CURLE_GOT_NOTHING;
    //struct stat file_info;
    struct _stat64 file_info;
    int use_resume = 0; // resume flag

    // get local file info, if success ,set resume mode
    if (curlx_win32_stat(local_file_path, &file_info) == 0)
    {
        local_file_len = file_info.st_size;
        use_resume = 1;
    }

    // read file in append mode: 断点续传
    file = curlx_win32_fopen(local_file_path, "ab+");
    if (file == NULL)
    {
        perror(NULL);
        return *this;
    }
    if(remote_file_path) //如果为null则不设置采用内部预设。
        curl_easy_setopt(curlhandle, CURLOPT_URL, remote_file_path);
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, user_key); // set user:password
    // set connection timeout
    curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);
    // set header process, get content length callback
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getContentLengthFunc);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);

    // 断点续传 set download resume, if use resume, set current local file length
    curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume ? local_file_len : 0);
    //    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, file);

    // set download progress
    curl_easy_setopt(curlhandle, CURLOPT_XFERINFOFUNCTION, progressCallback);
    struct CustomProgress prog;
    curl_easy_setopt(curlhandle, CURLOPT_XFERINFODATA, &prog);
    curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 0);

    //    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1); // if set 1, debug mode will print some low level msg
    CCURL_SAVE_CODE_CHECK(curl_easy_perform(curlhandle));
    fclose(file);

    if (getCode() != CURLE_OK)
    {
        fprintf(stderr, "%s\n", curl_easy_strerror(getCode()));
        CCURL_CHECK_CODE_ERROR();
    }

    // exit curl handle
    //curl_easy_cleanup(curlhandle);
    //curl_global_cleanup();

    return *this;
}



