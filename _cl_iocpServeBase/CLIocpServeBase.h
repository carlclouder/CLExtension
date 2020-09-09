
#ifndef _CL_IOCPMGR_HPP_
#define _CL_IOCPMGR_HPP_

#include "../_cl_common/CLCommon.h"
#include <windows.h>
#include <tchar.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <vector>
#include <iostream>
#include "WS2tcpip.h"
#pragma comment(lib, "WS2_32.lib")

//定义内存池支持-----------------------------------
//#undef CLMP_USE_LOCK_TYPE
//#define CLMP_USE_LOCK_TYPE 1
#include "../_cl_objectMemPool/CLMemPool.h" //若未使用内存池请注释掉

#ifndef newObj //后续使用到的宏，可由内存池定义覆盖
#define newObj( x )  new x
#define deleteObj( x ) delete x
#define giveUpObj( x ) delete x
#endif

#define ACCEPT_SOCKET_NUM  20 //初始连接数
#define DEF_PORT 20011
#define DEF_IPADDR _T("127.0.0.1")
//定义的事件枚举值
typedef enum _OVLP_TYPE
{
	CPKEY_NULL = 0x0,
	CPKEY_ACP = 0x1,
	CPKEY_CLOSE = 0x2,
	CPKEY_RECV = 0x4,
	CPKEY_SEND = 0x8,
} OVLP_TYPE, *POVLP_TYPE;
//绑定监听套接字目标的信息结构体
typedef class LISTENER_SOCKET
{
public:
	SOCKET m_sListen;                            // 监听的Socket
	LPFN_ACCEPTEX m_lpfnAcceptEx;                // 和监听sok绑定的控制函数
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptSockAddrs;    // 和监听sok绑定的控制函数
	TCHAR m_ip[50];							               // 该监听socket监听的目标ip地址
	USHORT m_port;										   // 该监听socket监听的目标ip地址对应的端口
	LISTENER_SOCKET(){reset();}
	~LISTENER_SOCKET(){reset();}
	void reset(){
		m_sListen = INVALID_SOCKET;
		m_lpfnAcceptEx = 0;
		m_lpfnGetAcceptSockAddrs = 0;
		_tcscpy_s(m_ip, DEF_IPADDR);
		m_port = DEF_PORT;
	}
	BOOL isListening() const {
		return m_sListen != INVALID_SOCKET && m_lpfnAcceptEx && m_lpfnGetAcceptSockAddrs;
	}
} *PLISTENER_SOCKET;
//内部iocp使用的重叠结构体扩展基类，必须实现resetBuffer()方法，用于设置缓存空间
typedef class OVLP_BASE
{
public:
	OVERLAPPED   m_Overlapped;				// 每一个重叠I/O网络操作都要有一个
	SOCKET       m_sSock;					// 这个I/O操作所使用的Socket，每个连接的都是一样的
	SOCKADDR_IN  m_addr;			        // 对端地址结构
	OVLP_TYPE	 m_olType;               // 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
	WSABUF       m_wsaBuf;					// 存储数据的缓冲区，用来给重叠操作传递参数的
	const LISTENER_SOCKET* pSokPack;	    // 监听结构体指针
	OVLP_BASE(void){		
		resetOverlapped();
		m_sSock = INVALID_SOCKET;
	}
	virtual ~OVLP_BASE(void){
		//ol->close(); //close方法应该在释放前手动控制
	}
	void resetOverlapped(){
		ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
	}
	//该函数应该被重写,完成缓存空间WSABUF m_wsaBuf的设置
	virtual void resetBuffer() = 0;
	void close() {
		if (m_sSock != INVALID_SOCKET) {
			shutdown(m_sSock, SD_BOTH);
			closesocket(m_sSock);
		}
		m_sSock = INVALID_SOCKET;
	}
	void reset() {
		resetBuffer();
		resetOverlapped();
		close();
	}
	void inheritSocket(OVLP_BASE* oltag) {
		if (oltag)
			m_sSock = oltag->m_sSock, oltag->m_sSock = INVALID_SOCKET;
	}
} *POVLP_BASE;
//每一个客户端对应的io请求
typedef class CLIENT_IOINFO
{
public:
	SOCKET                   m_Socket;              // 每一个客户端连接的Socket
	SOCKADDR_IN              m_ClientAddr;          // 这个客户端的地址
	std::vector<OVLP_BASE*>& m_arrayIoContext;      // 数组，所有客户端IO操作的参数，也就是说对于每一个客户端Socket是可以在上面同时投递多个IO请求的
	CLIENT_IOINFO():
		m_arrayIoContext(*newObj(std::vector<OVLP_BASE*>))
	{
		m_arrayIoContext.clear();
		m_Socket = INVALID_SOCKET;
		m_ClientAddr = { 0 };
	}
	~CLIENT_IOINFO() {
		if (m_Socket != INVALID_SOCKET) {
			shutdown(m_Socket, SD_BOTH);
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		m_arrayIoContext.clear();
		giveUpObj(&m_arrayIoContext);
	}
} *PCLIENTOVLP;

#define OVLP_DEFAULT_BUFFER_LEN 4096 //4kb

//实现具体的缓冲区管理方案的重叠结构类，buffer大小OVLP_DEFAULT_BUFFER_LEN个字节。
template<int _bufSize = OVLP_DEFAULT_BUFFER_LEN>
class OVLP :public OVLP_BASE
{
public:
	char m_szBuffer[_bufSize]; // 对应加入WSABUF里的缓冲区
	OVLP() {
		resetBuffer();
	}
	virtual void resetBuffer()
	{
		ZeroMemory(m_szBuffer, _bufSize);
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = _bufSize;
	}
};

//iocp模型服务管理模块类，可由子类继承并重载关键函数来实现多态
template<typename _MY_OVLP = OVLP<>>
class CLIocpServeBase : protected CLTaskSvc
{	
public:
	using OVLP = _MY_OVLP;
	using POVLP = _MY_OVLP*;

	CLIocpServeBase()
	{
		setTrdCounts();
		initLock(0);
		createIOCP();
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE)
			throw std::logic_error("iocp manager create fail!");
	}
	virtual ~CLIocpServeBase()
	{
		closeServe();
		closeIOCP();
	}
	//注册一个监听地址和端口，并启动所有队列中的监听服务，若地址已经存在则不增加监听地址。只启动已有监听列表。
	BOOL startListen(USHORT port, LPCTSTR ip = DEF_IPADDR)
	{
		addListen(port, ip);
		return preStartListen(port, ip) ? initWinSockListener() : FALSE;
	}
	//启动所有已经增加至监听队列的监听
	BOOL startListen()
	{
		return initWinSockListener();
	}
	//设置工作组线程数，0表示自动
	void setTrdCounts(size_t n = 0) {
		m_trdCounts = (n == 0 ? getCpuCoreCounts() : n);
	}
	//取得运行中的工作组线程数
	size_t getRunningTrdCounts() const {
		return CLTaskSvc::getActivateThreadsNum();
	}
	//增加一个监听地址和端口，请在startListen之前增加，关闭服务后所有监听地址及端口列表会清空，再次启动服务需重新增加
	BOOL addListen(USHORT port = DEF_PORT, LPCTSTR ip = DEF_IPADDR)
	{
		for (auto i = m_ListenLst.cbegin(); i != m_ListenLst.cend(); i++)
		{
			if (i->m_port == port && ip && _tcscmp(i->m_ip, ip) == 0)
				return FALSE;
		}
		LISTENER_SOCKET lsPack;
		lsPack.m_port = port;
		_tcscpy_s(lsPack.m_ip, ip);
		m_ListenLst.push_back(lsPack);
		return 1;
	}
	//关闭监听服务
	BOOL closeServe()
	{
		postClose();//可重载，发送iocp关闭信息到完成端

		lock();
		//2:清空等待accept的套接字m_vecAcps
		auto iter = m_vecAcps.begin();
		for (; iter != m_vecAcps.end(); )
		{
			OVLP_BASE* p = *iter;
			if (p) {
				p->close();
				deleteObj(p);
			}
			++iter;
		}
		m_vecAcps.clear();
		//3:清空已连接的套接字m_vecContInfo并清空缓存
		auto iter2 = m_vecContInfo.begin();
		for (; iter2 != m_vecContInfo.end(); )
		{
			OVLP_BASE* ol = iter2->second;
			if (ol) {
				ol->close();
				deleteObj(ol);
			}
			++iter2;
		}
		m_vecContInfo.clear();

		for (size_t i = 0; i < m_ListenLst.size(); i++)
		{
			if (m_ListenLst[i].m_sListen != INVALID_SOCKET)
				closesocket(m_ListenLst[i].m_sListen);
			m_ListenLst[i].m_sListen = INVALID_SOCKET;
		}
		m_ListenLst.clear();

		if (wsaData.wVersion > 0)
		{
			WSACleanup();
			wsaData = { 0 };
		}
		unlock();
		return TRUE;
	}
	//检查是否启动
	BOOL isListening() const
	{
		return CLTaskSvc::getActivateThreadsNum() ? TRUE : FALSE;
	}
private:
	//iocp句柄
	HANDLE m_hIOCompletionPort = 0;
	//监听套接字队列
	std::vector<LISTENER_SOCKET> m_ListenLst;
	WSADATA wsaData = { 0 };
	//等待accept的套接字，这些套接字是没有使用过的，数量为ACCEPT_SOCKET_NUM。同时会有ACCEPT_SOCKET_NUM个套接字等待accept
	std::vector<OVLP_BASE*> m_vecAcps;
	//已建立连接的信息，每个结构含有一个套接字、发送缓冲和接收缓冲，以及对端地址
	//std::vector<OVLP_BASE*> m_vecContInfo;
	std::unordered_map<SOCKET, OVLP_BASE*> m_vecContInfo;

	//工作线程组启动数量
	size_t m_trdCounts = 1;

	//重载以便在在startWorkersThreads之前调用，返回false则不执行startWorkersThreads
	virtual BOOL preStartWorkersThreads(size_t n) { return TRUE; }
	//启动工作线程组,返回启动工作线程的数量
	int startWorkersThreads(size_t n = 0)
	{
		return preStartWorkersThreads(n) ? start(n == 0 ? 2 * getCpuCoreCounts() : n) : 0;
	}
	//初始化并绑定一个监听套接字进入运行系统
	BOOL bindListener(LISTENER_SOCKET* sokPack)
	{
		// 初始化Socket
		// 这里需要特别注意，如果要使用重叠I/O的话，这里必须要使用WSASocket来初始化Socket
		// 注意里面有个WSA_FLAG_OVERLAPPED参数
		sokPack->m_sListen = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sokPack->m_sListen == INVALID_SOCKET)
		{
			int res = WSAGetLastError();
			logError(res, _T("bindListener,WSASocketW error:"));
			return FALSE;
		}

		if (!CreateIoCompletionPort((HANDLE)sokPack->m_sListen, getIOCP(), (ULONG_PTR)CPKEY_ACP, 0))
		{
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			int res = GetLastError();
			logError(res, _T("CreateIoCompletionPort error:"));
			return FALSE;
		}

		sockaddr_in ServerAddress;
		// 填充地址结构信息
		ZeroMemory(&ServerAddress, sizeof(ServerAddress));
		ServerAddress.sin_family = AF_INET;
		// 这里可以选择绑定任何一个可用的地址，或者是自己指定的一个IP地址
		if (sokPack->m_ip == 0 || (sokPack->m_ip[0] == 0))
			ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			InetPton(AF_INET, sokPack->m_ip, &ServerAddress.sin_addr);
		ServerAddress.sin_port = htons(sokPack->m_port);
		int res;
		// 绑定端口
		if (::bind(sokPack->m_sListen, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
		{
			lock(0);
			//cout << "bind(" << sokPack->m_ip << ":" << sokPack->m_port << ") error, " << (res = WSAGetLastError(), CLString().getLastErrorString(res)) << endl;
			cout << "bind(" << sokPack->m_ip << ":" << sokPack->m_port << ") error: " << (res = WSAGetLastError()) << endl;
			unlock(0);
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			return FALSE;
		}
		lock(0);
		cout << "bind " << sokPack->m_ip << ":" << sokPack->m_port << " ok!" << endl;
		unlock(0);
		// 开始监听
		if (listen(sokPack->m_sListen, SOMAXCONN) == SOCKET_ERROR)
		{
			lock(0);
			cout << "listen(" << sokPack->m_ip << ":" << sokPack->m_port << ") error: " << (res = WSAGetLastError()) << endl;
			unlock(0);
			closesocket(sokPack->m_sListen);
			sokPack->m_sListen = INVALID_SOCKET;
			return FALSE;
		}
		lock(0);
		cout << "listen " << sokPack->m_ip << ":" << sokPack->m_port << " ok!" << endl;
		unlock(0);

		if (!getFuncAddr(sokPack))
		{
			throw std::logic_error("func addr get fail!");
			return FALSE;
		}

		//创建acceptex和工作组线程一样多的初始接受端
		for (size_t i = 0; i < m_trdCounts; i++)
		{
			//用accept
			OVLP_BASE* ol = newObj(OVLP);
			ol->pSokPack = sokPack;
			if (!postAccept(ol))
			{
				ol->close();
				deleteObj(ol);
				return FALSE;
			}
			else {
				lock();
				m_vecAcps.push_back(ol);
				unlock();
			}
		}
		return TRUE;
	}
	//初始化监听过程
	BOOL initWinSockListener()
	{
		// 初始化Socket库
		if (wsaData.wVersion == 0)
			WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (getActivateThreadsNum() == 0)
			if (startWorkersThreads(m_trdCounts) == 0)
				return FALSE;
		BOOL rt = FALSE;
		for (size_t i = 0; i < m_ListenLst.size(); i++)
		{
			if (bindListener(&(m_ListenLst.at(i))) != FALSE)
				rt = TRUE;
		}
		return rt;
	}
	//获取绑定的控制函数指针
	BOOL getFuncAddr(LISTENER_SOCKET* sokPack)
	{
		GUID GuidAcceptEx = WSAID_ACCEPTEX;        // GUID，这个是识别AcceptEx函数必须的
		DWORD dwBytes = 0;
		if (SOCKET_ERROR == WSAIoctl(
			sokPack->m_sListen,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx,
			sizeof(GuidAcceptEx),
			&sokPack->m_lpfnAcceptEx,
			sizeof(sokPack->m_lpfnAcceptEx),
			&dwBytes, NULL, NULL))
		{
			int res = WSAGetLastError();
			logError(res, _T("WSAIoctl get AcceptEx function addr error:"));
			return FALSE;
		}
		//获取GetAcceptexSockAddrs函数指针
		GUID GuidGetAcceptexSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
		if (SOCKET_ERROR == WSAIoctl(
			sokPack->m_sListen,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidGetAcceptexSockAddrs,
			sizeof(GuidGetAcceptexSockAddrs),
			&sokPack->m_lpfnGetAcceptSockAddrs,
			sizeof(sokPack->m_lpfnGetAcceptSockAddrs),
			&dwBytes, NULL, NULL))
		{
			int res = WSAGetLastError();
			logError(res, _T("WSAIoctl get GetAcceptexSockAddrs function addr error:"));
			return FALSE;
		}
		return TRUE;
	}
	//针对内部iocp的相关操作{--------------------------
	HANDLE CLIocpServeBase::getIOCP() const { return m_hIOCompletionPort; }
	BOOL CLIocpServeBase::closeIOCP()
	{
		BOOL bResult = TRUE;
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE) {}
		else bResult = CloseHandle(getIOCP());
		m_hIOCompletionPort = NULL;
		return(bResult);
	}
	BOOL CLIocpServeBase::createIOCP(int nMaxConcurrency = 0)
	{
		if (getIOCP() == 0 || getIOCP() == INVALID_HANDLE_VALUE)
			m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
		assert(getIOCP() != NULL);
		return(getIOCP() != NULL);
	}
	BOOL CLIocpServeBase::postStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0, OVERLAPPED* po = NULL)
	{
		BOOL fOk = PostQueuedCompletionStatus(getIOCP(), dwNumBytes, CompKey, po);
		assert(fOk);
		return(fOk);
	}
	//}------------------------------------------------
private:
	//线程主工作体
	virtual DWORD run(PCLTaskSvcTrdParam var)
	{
		while (true)
		{
			DWORD NumberOfBytes = 0;
			ULONG_PTR CompletionKey = 0;
			OVERLAPPED* ol_ = NULL;
			BOOL rt = GetQueuedCompletionStatus(getIOCP(), &NumberOfBytes, &CompletionKey, &ol_, WSA_INFINITE);
			if (CompletionKey == CPKEY_CLOSE) //收到退出消息直接退出
				break;
			OVLP* ol = ol_ ? ((OVLP*)(((char*)ol_) - sizeof(char*))) : NULL; //这里需要处理虚表头指针的位置，即地址迁移出一个虚表指针空间才能得到真实的地址。
			if (ol) {
				if (FALSE != rt)
				{
					int res = preDoGetStatus((OVLP*)ol, NumberOfBytes, var);
					if (res < 0)break;
					else if (res == 0)continue;
					else if (doGetStatus((OVLP*)ol, NumberOfBytes, var) == FALSE) //返回false直接退出工作线程
						break;
				}
				else
				{
					int res = WSAGetLastError();
					int pres = preDoGetQueuedCompletionStatusError(res, (OVLP*)ol, NumberOfBytes, var);
					if (pres < 0)break;
					else if (pres == 0)continue;
					else if (doGetQueuedCompletionStatusError(res, (OVLP*)ol, NumberOfBytes, var) == FALSE) //返回false直接退出工作线程
						break;
				}
			}
			else {
				throw std::runtime_error("Work thread get a unsupported completion status!");
			}
		}
		return 1;
	}
	//重载该函数，以处理从完成端口取得消息成功事务，注意：若该函数 返回值 =  FALSE 则工作线程将进入安全退出过程
	BOOL doGetStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		BOOL rt = TRUE;		
		if (NumberOfBytes == 0 && (ol->m_olType == CPKEY_RECV || ol->m_olType == CPKEY_SEND))
		{
			if (ol->m_olType == CPKEY_RECV)
				rt = doRecvWithNullData(ol, var);
			else if (ol->m_olType == CPKEY_SEND)
				rt = doSendWithNullData(ol, var);
		}
		else
		{
			switch (ol->m_olType)
			{
			case CPKEY_ACP:
			{
				rt = doAccept(ol, NumberOfBytes,  var);
			}
			break;
			case CPKEY_RECV:
			{
				rt = doRecv(ol, NumberOfBytes,  var);
			}
			break;
			case CPKEY_SEND:
			{
				rt = doSend(ol, NumberOfBytes,  var);
			}
			break;
			default:
				rt = doDefaultStatus(ol, NumberOfBytes,  var);
				break;
			}
		}
		return rt;
	}
	//子类应实现的重载方法，安全退出线程组的信息投送过程,投递recv请求
	void postClose()
	{
		//1：清空IOCP线程队列，退出线程,有多少个线程发送多少个PostQueuedCompletionStatus信息
		for (size_t i = 0; i < getThreadsNum(); i++)
		{
			if (FALSE == postStatus(CPKEY_CLOSE))
			{
				logError(WSAGetLastError(), _T("postClose,PostQueuedCompletionStatus error:"));
			}
		}
	}
	void addOvlpToContInfo(OVLP_BASE* ol) {
		if (!ol)return;
		lock();
		auto it = m_vecContInfo.find(ol->m_sSock);
		if (it != m_vecContInfo.end()) {
			if (ol != it->second) {
				MessageBoxA(NULL, "same OVLP obj is in vector!", "CLIocpServeBase Alert", MB_ICONERROR);
				throw std::runtime_error("same OVLP obj is in vector!");
			}
		}
		else m_vecContInfo[ol->m_sSock] = ol;
		unlock();
	}
	//处理accept请求,NumberOfBytes=0表示没有收到第一帧数据，>0表示收到第一帧数据
	BOOL doAccept(OVLP_BASE* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		if (!preDoAccept((OVLP*)ol, NumberOfBytes,  var))
			return FALSE;
		//分支用于获取远端地址。
		//如果接收CPKEY_ACP同时收到第一帧数据，则第一帧数据内包含远端地址。
		//如果没有收到第一帧数据，则通过getpeername获取远端地址
		BOOL bAccept = FALSE;
		SOCKADDR_IN* ClientAddr = NULL;
		SOCKADDR_IN* LocalAddr = NULL;
		int remoteLen = sizeof(SOCKADDR_IN);
		if (NumberOfBytes > 0)
		{
			//接受的数据分成3部分，第1部分是客户端发来的数据，第2部分是本地地址，第3部分是远端地址。
			if (ol->pSokPack->m_lpfnGetAcceptSockAddrs)
			{				
				int localLen = sizeof(SOCKADDR_IN);
				ol->pSokPack->m_lpfnGetAcceptSockAddrs(
					ol->m_wsaBuf.buf,
					ol->m_wsaBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,
					sizeof(SOCKADDR_IN) + 16,
					sizeof(SOCKADDR_IN) + 16,
					(LPSOCKADDR*)&LocalAddr,
					&localLen,
					(LPSOCKADDR*)&ClientAddr,
					&remoteLen);
				bAccept = doGetAcceptSockAddrs(LocalAddr, ClientAddr, (OVLP*)ol, NumberOfBytes,  var);
			}
			else
				throw std::runtime_error("m_lpfnGetAcceptSockAddrs is not initialize!");
		}
		else 
		{
			//未收到第一帧数据
			int funcRet = ::getpeername(ol->m_sSock, (sockaddr*)ClientAddr, &remoteLen);
			bAccept = doGetPeerName(funcRet, ClientAddr, (OVLP*)ol, var);
		}
		if(bAccept)
			bAccept = ncDoAccept(LocalAddr, ClientAddr, (OVLP*)ol, NumberOfBytes, var);
		if (bAccept) { //继续路由到下一步接受信息
			lock();
			OVLP_BASE* pol = newObj(OVLP);
			if (*(char**)pol == 0)
				throw std::runtime_error("OVLP_BASE obj is crash!");
			pol->inheritSocket(ol);
			if (ClientAddr)pol->m_addr = *ClientAddr;
			else pol->m_addr = { 0 };
			pol->pSokPack = ol->pSokPack;
			//服务端只收取recv，同时监听recv和send可用设计位偏移，用或运算实现
			if (CreateIoCompletionPort((HANDLE)pol->m_sSock, getIOCP(), CPKEY_RECV | CPKEY_SEND, 0) == getIOCP())
			{
				addOvlpToContInfo(pol);
				if (!postRecv(pol)){
					deleteLink(pol);
				}
			}
			else
			{
				//绑定失败，关闭句柄
				pol->close();
				deleteObj(pol);
			}
			unlock();
		}
		else //否则应该关闭链接
			ol->close();
		postAccept(ol);		
		return TRUE;
	}	
protected:
	//线程启动时最先运行的内部初始化过程函数，必须返回TRUE，才会执行后续的run，否则直接进入exist。可以初始化一些线程独享变量放入var保存供后续过程使用。
	virtual BOOL init(PCLTaskSvcTrdParam var) { return TRUE; }
	//线程退出时的最后运行函数，线程返回值由该函数决定。他的作用实在线程释放前，清理var中保存的对象，var是线程独享的。
	virtual DWORD exist(DWORD runReturnValue, PCLTaskSvcTrdParam var) { return runReturnValue; }
	//从已连接socket列表中移除socket及释放空间,他应该释放的是后续不再需要的链接（立即断开并清除）
	BOOL deleteLink(OVLP_BASE* ol)
	{
		if (!ol)
			return false;		
		lock();		
		if (ol->m_sSock != INVALID_SOCKET) {
			shutdown(ol->m_sSock, SD_BOTH);
			closesocket(ol->m_sSock);
		}
		auto it = m_vecContInfo.find(ol->m_sSock);
		if (it != m_vecContInfo.end()) {			
			m_vecContInfo.erase(it);
		}		
		deleteObj(ol);
		unlock();		
		return true;
	}
	//重载该函数，处理从完成端口取得消息处理前的预处理事务，注意：返回值= -1安全退出,=0直接进入下一轮消息获取跳过doGetStatus，=1进入doGetStatus处理事务
	virtual int preDoGetStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return 1; }
	//重载该函数，处理从完成端口取得消息处理前的预处理事务，注意：返回值= -1安全退出,=0直接进入下一轮消息获取跳过doGetQueuedCompletionStatusError，=1进入doGetQueuedCompletionStatusError处理事务
	virtual int preDoGetQueuedCompletionStatusError(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return 1; }
	//重载该函数，以处理从完成端口取得消息失败事务，注意：若该函数 返回值 =  FALSE 则工作线程将进去安全退出过程
	virtual BOOL doGetQueuedCompletionStatusError(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		switch (errorID)
		{
		case ERROR_NETNAME_DELETED:
		{
			doGetQueuedCompletionStatusError_NetNameDeleted(errorID, ol, NumberOfBytes,  var);
		}
		break;
		default:
			return doGetQueuedCompletionStatusError_default(errorID, ol, NumberOfBytes,  var);
		}
		return TRUE;
	}
	//重载该函数，处理从完成端口取得Accept消息处理前的预处理事务。返回 TRUE 继续执行accept
	virtual BOOL preDoAccept(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) { return TRUE; }
	//重载该函数，处理从完成端口取得Accept消息处理后的处理事务。他可以处理向客户端立即答复已接受链接的信息。
	//如果返回FALSE，则该Accept是一条即时信息，做完本函数操作后，将关闭本次链接，不再接受后续Recv操作；（比如处理GET请求等即时信息）
	//函数内不应该显示调用任何释放ol的操作，应该通过返回FALSE由框架执行后续操作。
	virtual BOOL ncDoAccept(SOCKADDR_IN* localAddr, SOCKADDR_IN* clientAddr, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {return TRUE;}
	//投递accept请求,内部如果有保存socket则不释放，切断链接后供AcceptEx重用
	BOOL postAccept(OVLP_BASE* ol)
	{
		const LISTENER_SOCKET* pso = ol->pSokPack;
		ol->resetBuffer();
		ol->resetOverlapped();
		ol->close();//有就应该关闭
		ol->m_sSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (ol->m_sSock == INVALID_SOCKET)
		{
			int res = WSAGetLastError();
			logError(res, _T("postAccept,WSASocket error:"));
			throw std::runtime_error("post accept socket create false!");
			return FALSE;
		}
		//这里建立的socket用来和对端建立连接，终会加入m_vecContInfo列表
		//调用acceptex将accept socket绑定至完成端口，并开始进行事件监听
		//这里需要传递Overlapped，new一个OVLP
		//AcceptEx是m_listen的监听事件，m_listen已经绑定了完成端口；虽然ol->m_sSock已经创建，
		//但未使用，现在不必为ol->m_sSock绑定完成端口。在AcceptEx事件发生后，再为ol->m_sSock绑定IOCP
		DWORD byteReceived = 0;
		ol->m_olType = CPKEY_ACP;
		if (FALSE == pso->m_lpfnAcceptEx(
			pso->m_sListen,
			ol->m_sSock,
			ol->m_wsaBuf.buf,
			ol->m_wsaBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&byteReceived,
			&ol->m_Overlapped))
		{
			DWORD res = WSAGetLastError();
			if (ERROR_IO_PENDING != res)
			{
				logError(res, _T("postAccept,AcceptEx error:"));
				ol->close();
				throw std::runtime_error("post accept socket false!");
				return FALSE;
			}
		}
		return TRUE;
	}
	//投递recv请求
	BOOL postRecv(OVLP_BASE* ol)
	{
		DWORD BytesRecvd = 0;
		DWORD dwFlags = 0;
		ol->resetOverlapped();
		ol->resetBuffer();
		ol->m_olType = CPKEY_RECV;
		int recvnum = WSARecv(ol->m_sSock, &ol->m_wsaBuf, 1, &BytesRecvd, &dwFlags, &ol->m_Overlapped, NULL);
		if (recvnum != 0)
		{
			int res = WSAGetLastError();
			if (WSA_IO_PENDING != res)
			{
				logError(res, _T("postRecv,WSARecv error:"));
				return FALSE;
			}
		}
		return TRUE;
	}
	//投递send请求
	BOOL postSend(OVLP_BASE* ol, DWORD lenToSendBytes)
	{
		DWORD BytesSend = 0;
		DWORD dwFlags = 0;
		ol->resetOverlapped();
		ol->m_olType = CPKEY_SEND;
		ol->m_wsaBuf.len = lenToSendBytes;
		int recvnum = WSASend(ol->m_sSock, &ol->m_wsaBuf, 1, &BytesSend, dwFlags, &ol->m_Overlapped, NULL);
		if (recvnum != 0)
		{
			int res = WSAGetLastError();
			if (WSA_IO_PENDING != res)
			{
				logError(res, _T("postSend,WSASend error:"));
				ol->resetBuffer();
				return FALSE;
			}
		}
		return TRUE;
	}
	//处理外部客户机断开意外断开事件
	virtual void doGetQueuedCompletionStatusError_NetNameDeleted(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		if (ol)
		{
			if (ol->m_olType == CPKEY_ACP) {//连接阶段断开则重发一个等待的接受消息
				postAccept(ol);
			}
			else {
				//ol->close();
				//deleteObj(ol);
				deleteLink(ol);
			}
		}
	}
	//处理GetStatus错误时候除Error = NetNameDeleted外的其他错误事件，内部用switch处理即可，注意：若该函数 返回值 =  FALSE 则工作线程将进去安全退出过程
	virtual BOOL doGetQueuedCompletionStatusError_default(int errorID, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var)
	{
		logError(errorID, _T("workThread GetQueuedCompletionStatus error:"));
		switch (errorID)
		{
		case ERROR_NETNAME_DELETED:
			break;
		default:
			break;
		}
		return TRUE;
	}
	//重载以便在在startListen之前调用，返回false则不执行start
	virtual BOOL preStartListen(USHORT port, LPCTSTR ip) { return TRUE; }
	//重载以处理GetAcceptSockAddrs调用后的后处理,该函数处理接受成功后对客户端的应答（必须返回TRUE才能继续接受下一条信息,返回FALSE则不进入后续的RECV过程）
	virtual BOOL doGetAcceptSockAddrs(SOCKADDR_IN* localAddr, SOCKADDR_IN* clientAddr, OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		return TRUE;
	}
	//重载以处理getpeername调用后的后处理
	virtual BOOL doGetPeerName(int funcRet, SOCKADDR_IN* clientAddr, OVLP* ol, PCLTaskSvcTrdParam var)
	{
		if (funcRet == SOCKET_ERROR) {
			logError(WSAGetLastError(), _T("getpeername error:"));
		}
		return TRUE;
	}
	//处理recv完成请求，必须显示调用postRecv行为指定该socket的下一个任务
	virtual BOOL doRecv(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		//若使用异步接收数据必须实现此函数的行为
		MessageBox(NULL, _T("doRecv() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doRecv method need to operator!");
		return TRUE;
	}
	//处理send完成请求，必须显示调用postRecv行为指定该socket的下一个任务
	virtual BOOL doSend(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		//若使用异步发送必须实现此函数的行为
		MessageBox(NULL, _T("doSend() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doSend method need to operator!");
		return TRUE;
	}
	//处理Default请求，必须显示调用postRecv行为指定该socket的下一个任务
	virtual BOOL doDefaultStatus(OVLP* ol, DWORD NumberOfBytes, PCLTaskSvcTrdParam var) {
		postRecv(ol);
		return TRUE;
	}
	//处理NumberOfBytes = 0的请求,也就是客户端主动单方面断开链接的情况，他应该处理该socket断开后的清理工作。
	virtual BOOL doRecvWithNullData(OVLP* ol,PCLTaskSvcTrdParam var) {
		//若使用异步接收数据必须实现此函数的行为
		MessageBox(NULL, _T("doRecvWithNullData() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doRecvWithNullData method need to operator!");
		return TRUE;
	}
	virtual BOOL doSendWithNullData(OVLP* ol, PCLTaskSvcTrdParam var) {
		//若使用异步发送则必须实现此函数的行为
		MessageBox(NULL, _T("doSendWithNullData() method using asynchronous sending mode has not been implemented!"), _T("CLIocpServeBase Alert"), MB_ICONERROR);
		throw std::runtime_error("doSendWithNullData method need to operator!");
		return TRUE;
	}
	//输出socket错误信息
	virtual void logError(int errorId, LPCTSTR inf1 = 0, LPCTSTR inf2 = 0) {}
};

#endif

