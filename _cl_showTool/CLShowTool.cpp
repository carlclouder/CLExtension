#include "CLShowTool.h"
#include "CLShowTool_WndProc.h"
#include "WinUser.h"

#undef ST_FALSE
#define ST_FALSE \
(this->messageBox(_T("The show tool is occur one error!")),\
throw std::runtime_error("The show tool is occur one error!"),0)

CLRect g_stSimpleLineDefaultRec = { 0,0,750,350 };
CLRect g_stKlineDefaultRec = { 0,0,750,550 };

HFONT CLFont::CreatePointFont(LONG nPointSize, LPCTSTR lpszFaceName, CLDC* pDC) {
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize;
	_tcsncpy_s(logFont.lfFaceName, (sizeof(logFont.lfFaceName) / sizeof(logFont.lfFaceName[0])), lpszFaceName, ((size_t)-1));
	HDC hDC;
	if (pDC != NULL)
	{
		hDC = pDC->m_hAttribDC;
	}
	else
		hDC = ::GetDC(NULL);

	// convert nPointSize to logical units based on pDC
	POINT pt;
	// 72 points/inch, 10 decipoints/point
	pt.y = ::MulDiv(::GetDeviceCaps(hDC, LOGPIXELSY), logFont.lfHeight, 720);
	pt.x = 0;
	::DPtoLP(hDC, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);

	if (pDC == NULL)
		ReleaseDC(NULL, hDC);

	hobj = ::CreateFontIndirect(&logFont);
	return (hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
}

CLPaintDC::CLPaintDC(CLShowTool* pWnd) {
	Attach(::BeginPaint(m_hWnd = pWnd->getSafeHwnd(), &m_ps));
}

CLClientDC::CLClientDC(CLShowTool* pWnd) {
	AttachWnd(pWnd);
}

BOOL CLClientDC::AttachWnd(CLShowTool* pWnd) {
	return Attach(::GetDC(m_hWnd = pWnd->getSafeHwnd()));
}

CLWindowDC::CLWindowDC(CLShowTool* pWnd) {
	AttachWnd(pWnd);
}

BOOL CLWindowDC::AttachWnd(CLShowTool* pWnd) {
	return Attach(::GetWindowDC(m_hWnd = pWnd->getSafeHwnd()));
}

LONG CLShowTool::showAsModel()
{
	enterPermission();
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	LONG       nCmdShow = SW_SHOW;
	MSG msg;
	HACCEL hAccelTable;

	m_isMainWnd = TRUE;

	// ��ʼ��ȫ���ַ���
	TCHAR szTitle1[MAX_LOADSTRING];
	LoadString(hInstance, IDS_CLST_TITLE, szTitle1, MAX_LOADSTRING);
	szTitle = szTitle1;
	//_tcscpy_s(szWindowClass,_T("WNDCLASS_CLShowTool"));
	ATOM at = MyRegisterClass(hInstance);
	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance(hInstance, nCmdShow))
	{
		DWORD le = GetLastError();
		CLString().getLastErrorMessageBoxExceptSucceed(le);
		leavePermission();
		return ST_FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLST_CLASSNAME));
	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	leavePermission();
	return (LONG)msg.wParam;
}

//����Ϊ���߳���������ʾ����ģ̬��ʽ��ʾ���ڣ��ڹرմ���ǰ�������᷵�ء�

inline LONG CLShowTool::show() { return showAsModel(); }

//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM CLShowTool::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &CLShowTool::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR); //����װָ��
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLST_MAIN));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_CLST_CLASSNAME);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_CLST_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, LONG)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL CLShowTool::InitInstance(HINSTANCE hInstance, LONG nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
	m_hWnd = ::CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, m_hWndParent, NULL, hInstance, NULL);
	if (!m_hWnd)
		return ST_FALSE;
#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
	GetMapManager().InsertPairToMap(getSafeHwnd(), this);
#else
	SetWindowLongPtr(m_hWnd, CLST_USE_EXTRADATA_TO_SAVE_DLGPTR, (LONG_PTR)this);
#endif
	showWindow(nCmdShow);
	updateWindow();
	return TRUE;
}


HWND CLShowTool::createWindow(HWND hParentWnd)
{
	if (m_hWnd && isWindow())
		destroyWindow();
	m_isMainWnd = FALSE;//�ǳ���Ҫ,ָ����ǰ����������ѭ������״̬����ʾ
	create(NULL, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, m_hWndParent = hParentWnd, NULL, hInst, NULL);
	if (m_hWnd == NULL || m_hWnd == INVALID_HANDLE_VALUE)
	{
		DWORD le = GetLastError();
		CLString().getLastErrorMessageBoxExceptSucceed(le);
		return NULL;
	}
#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
	GetMapManager().InsertPairToMap(getSafeHwnd(), this);
#else
	SetWindowLongPtr(m_hWnd, CLST_USE_EXTRADATA_TO_SAVE_DLGPTR, (LONG_PTR)this);
#endif
	showWindow(SW_SHOW);
	updateWindow();
	return m_hWnd;
}

inline HWND CLShowTool::createWindow() { return createWindow(m_hWndParent); }

CLShowTool::CLShowTool(HWND hParent)
{
	hRunEvent = CreateMutex(0, 0, 0);
	//printf("\ncreate %p", hRunEvent);
	m_hWndParent = hParent;
	init();
}

CLShowTool::CLShowTool(const CLShowTool& othre)
{
	hRunEvent = CreateMutex(0, 0, 0);
	//printf("\ncreate %p", hRunEvent);
	m_hWndParent = othre.m_hWndParent;
	init();
	*this = othre;
}

CLShowTool& CLShowTool::operator=(const CLShowTool& othre)
{
	attach(othre);
	return *this;
}

CLShowTool::~CLShowTool()
{
	// ����öԻ������Զ���������
	//  ���˴���ָ��öԻ���ĺ���ָ������Ϊ NULL���Ա�
	//  �˴���֪���öԻ����ѱ�ɾ����
	if (isWindow())
		destroyWindow();
	release();
	enterPermission();
	CloseHandle(hRunEvent);
	hRunEvent = 0;
	//printf("\nfree %p", hRunEvent);
}

//���������ڶ����߳�����ģʽ�µĶ���ָ�룬�Ƿ��ڶ����߳��˳������ٻ��Ǽ�����Ч��Ĭ����������Զ����ٶ���ġ�
//����ΪTRUE�󣬶����̴߳��ڹرպ������ٶ�̬�����Ķ���ָ��

void CLShowTool::setAutoDeleteInNewThread(BOOL bIsAutoDelete) {
	m_isAutoDeleteSelfInNewThread = bIsAutoDelete;
}

BOOL CLShowTool::getAutoDeleteInNewThread() const {
	return m_isAutoDeleteSelfInNewThread;
}

BOOL CLShowTool::getPipeHandle(LONGLONG _nPipeMaxBufInByte, HANDLE* _hRead, HANDLE* _hWrite)
{
c1:
	if (!hRead || !hWrite) {
		SECURITY_ATTRIBUTES sa;
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		if (!CreatePipe(&hRead, &hWrite, &sa, _nPipeMaxBufInByte)) {
			goto f1;
		}
		nPipeSize = _nPipeMaxBufInByte;
	}
	else {
		if (_nPipeMaxBufInByte > nPipeSize) {
			CloseHandle(hRead);
			CloseHandle(hWrite);
			hRead = 0;
			hWrite = 0;
			goto c1;
		}
	}
	if (_hRead)*_hRead = hRead;
	if (_hWrite)*_hWrite = hWrite;
	return TRUE;
f1:
	hRead = hWrite = 0;
	nPipeSize = 0;
	if (_hRead)*_hRead = 0;
	if (_hWrite)*_hWrite = 0;
	return ST_FALSE;
}

BOOL CLShowTool::isPipeEmpty() const
{
	if (!hRead)return TRUE;
	BYTE b = 0;//dwRead=0;
	DWORD _dwRead, _dwTotal, _dwleft;
	if (!PeekNamedPipe(hRead, &b, 1, &_dwRead, &_dwTotal, &_dwleft))return FALSE;
	if (_dwRead > 0)return FALSE;
	return TRUE;
}

BOOL CLShowTool::writeData(LPCVOID buf, LONGLONG nDataBufSizeInByte, LONGLONG nPipeMaxBufInByte)
{
	if (!getPipeHandle(nPipeMaxBufInByte))
		throw std::runtime_error("Have no write pipe handle!");
	if (nPipeSize < nDataBufSizeInByte)
		throw std::runtime_error("write pipe buffer is not enough!");
	if (!WriteFile(hWrite, buf, nDataBufSizeInByte, &dwWrite, NULL))
		throw std::runtime_error("Write data to pipe handle fail!");
	PostMessage(*this, WM_TIMER, TIMERID_PEEKDATA, 0);
	return TRUE;
}

BOOL CLShowTool::writeHeader(const SHOWGRAPHERHEARDER& hdr)
{
	return writeData(&hdr, sizeof(SHOWGRAPHERHEARDER), hdr.pipeTotalSizeInByte);
}
BOOL CLShowTool::writeBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo)
{
	if (nPipeSize < (LONGLONG)(btHdr.bfSize + sizeof(SHOWGRAPHERHEARDER)))
		return ST_FALSE;
	if (writeData(&btHdr, sizeof(BITMAPFILEHEADER), nPipeSize)) {
		if (btInfo)
			return writeData(btInfo, btHdr.bfSize - sizeof(BITMAPFILEHEADER), nPipeSize);
		else
			return TRUE;
	}
	else return ST_FALSE;
}

BOOL CLShowTool::writeSimpleBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo, LPCTSTR szTitle, LONG X, LONG Y)
{
	assert(btInfo != NULL);
	if (btHdr.bfSize == 0)return ST_FALSE;
	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, getDefSimLineCX(), getDefSimLineCY());
	hdr.type = SHOWGRAPHER_TYPE_BITMAP;
	hdr.addBitmapSize(btHdr.bfSize).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	hdr.addAxisFlag = STLT_ADDAXIS_X | STLT_ADDAXIS_Y;
	hdr.AxisTextDataAccuracy = 3;
	writeHeader(hdr);
	writeBitmap(btHdr, btInfo);
	if (isWindow())setWindowText(szTitle);
	return TRUE;
}

BOOL CLShowTool::writeLine(const STUTHEADER& lineHdr, const DOUBLE* pLine)
{
	if (nPipeSize < (LONGLONG)(lineHdr.dataCounts * sizeof(DOUBLE) + sizeof(STUTHEADER) + sizeof(SHOWGRAPHERHEARDER)))
		return ST_FALSE;
	assert((lineHdr.utType == STLT_LINE || lineHdr.utType == STLT_VERLINE || lineHdr.utType == STLT_ADDAXIS_X || lineHdr.utType == STLT_ADDAXIS_Y));
	if (writeData(&lineHdr, sizeof(STUTHEADER), nPipeSize)) {
		if (pLine)
			return writeData(pLine, lineHdr.dataCounts * sizeof(DOUBLE), nPipeSize);
		else
			return TRUE;
	}
	else return ST_FALSE;
}

BOOL CLShowTool::writeLine2(const STUTHEADER& lineHdr, const DOUBLE* pLineV, const DOUBLE* pLineX)
{
	if (pLineV == 0 || pLineX == 0 || lineHdr.dataCounts == 0)
		return ST_FALSE;
	STUTHEADER hdr = lineHdr;
	hdr.exFlag |= STEF_USE_AXIS_X_DATA;
	//double *line2 = new double[lineHdr.dataCounts];
	std::vector<double> line2(lineHdr.dataCounts);
	for (LONG i = 0; i < lineHdr.dataCounts / 2; i++)
	{
		line2[i] = pLineV[i];
	}
	for (LONG i = lineHdr.dataCounts / 2, j = 0; i < lineHdr.dataCounts; i++, j++)
	{
		line2[i] = pLineX[j];
	}
	writeLine(hdr, line2.data());
	//delete[] line2;
	return TRUE;
}

BOOL CLShowTool::writeText(const STUTHEADER& textHdr, const TCHAR* pText)
{
	if (!pText) return ST_FALSE;
	if (nPipeSize < (LONGLONG)(textHdr.dataCounts * sizeof(TCHAR) + sizeof(STUTHEADER) + sizeof(SHOWGRAPHERHEARDER)))
		return ST_FALSE;
	assert(textHdr.utType == STLT_TEXT);
	if (writeData(&textHdr, sizeof(STUTHEADER), nPipeSize)) {
		return writeData(pText, textHdr.dataCounts * sizeof(TCHAR), nPipeSize);
	}
	else return ST_FALSE;
}

BOOL CLShowTool::writeKLine(const SHOWGRAPHERHEARDER& hdr, const KLINEDATAHEADER& KlineHdr, const DAYDATASETS_UNIT1* pLine)
{
	if (writeHeader(hdr)) {
		if (nPipeSize < (LONGLONG)(KlineHdr.nDays * sizeof(DAYDATASETS_UNIT1) + sizeof(KLINEDATAHEADER) + sizeof(SHOWGRAPHERHEARDER)))
			return ST_FALSE;
		if (writeData(&KlineHdr, sizeof(KLINEDATAHEADER), nPipeSize))
			return writeData(pLine, KlineHdr.nDays * sizeof(DAYDATASETS_UNIT1), nPipeSize);
		else
			return ST_FALSE;
	}
	else return ST_FALSE;
}

BOOL CLShowTool::display(BOOL forceShow)
{
	if (!isWindow())
		return FALSE;
	if (forceShow) {
		return showWindow(SW_SHOW);
	}
	if (isWindowVisible()) {
		showWindow(SW_HIDE);
		return FALSE;
	}
	else {
		return showWindow(SW_SHOW);
	}
}

// CLShowTool ��Ϣ�������

BOOL CLShowTool::OnInitDialog()
{
	if (m_isInit == TRUE)
		return TRUE;
	//OldClass::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	//SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	//SetIcon(m_hIcon, FALSE);		// ����Сͼ��	
	m_pdc.AttachWnd(this);
	m_memdc.CreateCompatibleDC(m_pdc); //����һ���ڴ�DC
	m_bkBitmap.CreateCompatibleBitmap(m_pdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); //����һ���ڴ�λͼ
	m_memdc.SelectObject(m_bkBitmap); //�����ڴ�DC���ڴ�λͼ	

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
#ifdef _DEBUG
	//m_cmdL.showContent();
	//AfxMessageBox(_T("_DEBUG Break Point!"));
#endif

	if (hRead == INVALID_HANDLE_VALUE) hRead = 0;
	if (hWrite == INVALID_HANDLE_VALUE) hWrite = 0;
	m_type = SHOWGRAPHER_TYPE_NO;
	pPt = NULL;
	pBitmapBuf = 0;
	pBitmapBufSi = 0;
	hBitmap = 0;
	m_bitUpdateTimes = 0;
	getClientRect(&m_strRect);
	ZeroMemory(&kLineHeader, sizeof(KLINEDATAHEADER));
	ZeroMemory(&m_pipeHeader, sizeof(SHOWGRAPHERHEARDER));
	lastWriteTime = lastReadTime = 0;
	m_isYAxis = m_isYAxis = FALSE;
	leftWD = LEFTWIDE;
	bottonWD = BOTTONWIDE;
	offset = OFFSET;
	m_isLD = 0;
	m_isXdata = 0;
	m_cpt.x = m_cpt.y = -1;
	m_strechRat = 1;
	//::SetTimer(*this,TIMERID_PEEKDATA,TIMERECLIPSE_PEEKDATA,NULL);
	::PostMessage(getSafeHwnd(), WM_TIMER, TIMERID_PEEKDATA, 0);
	::SetTimer(*this, TIMERID_TS_REDRAW, TIMERECLIPSE_TS_DUMPDIS, NULL);

	//��ʾ������
	showSubLstWnd();

	// 	//��һ��ģ����
	// 	CLRect rt;
	// 	getClientRect(&rt);
	// 	PostMessage(getSafeHwnd(),WM_LBUTTONDOWN,0,MAKELPARAM(rt.Width()/2,rt.Height()/2));
	// 	PostMessage(getSafeHwnd(),WM_LBUTTONUP,0,MAKELPARAM(rt.Width()/2,rt.Height()/2));
	return m_isInit = TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLShowTool::OnPaint()
{
	CLPaintDC m_dc(this); // ���ڻ��Ƶ��豸������
	if (IsIconic(*this))
	{
		::SendMessage(*this, WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(m_dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		LONG cxIcon = GetSystemMetrics(SM_CXICON);
		LONG cyIcon = GetSystemMetrics(SM_CYICON);
		CLRect rect;
		getClientRect((CLRect*)&rect);
		LONG x = (rect.Width() - cxIcon + 1) / 2;
		LONG y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		//m_dc.DrawIcon(x, y, m_hIcon);
		DrawIcon(m_dc.GetSafeHdc(), x, y, m_hIcon);
	}
	else
	{
		//m_dc.BitBlt(0,0,clientRect.Width(),clientRect.Height(),&m_memdc,0,0,SRCCOPY);//�ڴ�λͼ��ʾ����	
		if (m_type == SHOWGRAPHER_TYPE_BITMAP) {
			getClientRect(&clientRect);
			m_dc.FillSolidRect(&clientRect, m_gbkColor);//����൱�ڲ����Ĳ��裬�������ֱ�����ɫ
			if (m_bitUpdateTimes++ == 1) {
				if (double(bitmap.bmWidth) / bitmap.bmHeight > double(clientRect.Width()) / clientRect.Height())
					m_strRect = { 0,0,bitmap.bmWidth ,long(bitmap.bmWidth / double(clientRect.Width()) * clientRect.Height()) },
					m_strechRat = bitmap.bmWidth / double(clientRect.Width());
				else
					m_strRect = { 0,0,long(bitmap.bmHeight / double(clientRect.Height()) * clientRect.Width()) ,bitmap.bmHeight },
					m_strechRat = bitmap.bmHeight / double(clientRect.Height());
				//::BitBlt(m_dc.GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(), m_memdc, 0, 0, SRCCOPY);//�ڴ�λͼ��ʾ����
			}
			//else 
			::StretchBlt(m_dc.GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(), m_memdc,
				m_strRect.left, m_strRect.top, m_strRect.Width(), m_strRect.Height(), SRCCOPY);//�ڴ�λͼ��ʾ����	
		}
		else { //m_dc.BitBlt(0,0,clientRect.Width(),clientRect.Height(),&m_memdc,0,0,SRCCOPY);//�ڴ�λͼ��ʾ����	
			::BitBlt(m_dc.GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(), m_memdc, 0, 0, SRCCOPY);//�ڴ�λͼ��ʾ����	
		}
	}
}


//ָ���ÿؼ��Ƿ��������߳���

void CLShowTool::enterPermission() {
	auto rt = WaitForSingleObject(hRunEvent, 0);
	if (WAIT_OBJECT_0 == rt) {
		return;
	}
	else if (rt == WAIT_ABANDONED) {
		return;
	}
	this->messageBox(_T("The show tool is in one running process!"));
	throw std::runtime_error("The show tool is in running process!");
}

void CLShowTool::leavePermission() {
	if (!ReleaseMutex(hRunEvent)) {
		this->messageBox(_T("Leave permission fail, the pernit is not getted!"));
		throw std::runtime_error("Leave permission fail, the pernit is not getted!");
	}
	//printf("\nleave %p", hRunEvent);
}

DOUBLE CLShowTool::getWide(LONG pos)
{
	switch (pos) {
	case 1:return m_isYAxis ? leftWD : offset;
	case 2:return offset;
	case 3:return offset;
	case 4:return m_isXAxis ? bottonWD : offset;
	default:return offset;
	}
}

void CLShowTool::release()
{
	enterPermission();
	m_type = SHOWGRAPHER_TYPE_NO;
	if (pPt) { delete[] pPt; pPt = NULL; }
	if (hBitmap)::DeleteObject(hBitmap), hBitmap = 0;
	if (pBitmapBuf) { delete[] pBitmapBuf; pBitmapBuf = NULL; pBitmapBufSi = 0; }
	if (hRead) { CloseHandle(hRead); hRead = NULL; }
	if (hWrite) { CloseHandle(hWrite); hWrite = NULL; }
	nPipeSize = 0;
	for (auto i = m_mpTextLst.begin(); i != m_mpTextLst.end(); i++)delete (i->second);
	m_mpTextLst.clear();
	for (auto i = m_mplineLst.begin(); i != m_mplineLst.end(); i++)delete[](i->second.Line);
	m_mplineLst.clear();
	releaseSubWndLst();
	leavePermission();
}

void CLShowTool::updateMaxYMinY(DOUBLE* line, STUTHEADER* hdr) throw(...)
{
	assert(hdr != 0);
	BOOL isNo0 = (hdr->exFlag & STEF_NODRAWLINE_0);
	long _dataCounts = (hdr->exFlag & STEF_USE_AXIS_X_DATA) ? hdr->dataCounts / 2 : hdr->dataCounts;//ʹ��һ������
	//DOUBLE tp[2000]={0};
	//memcpy_s(tp,sizeof(tp),line,hdr->dataCounts*sizeof(DOUBLE));
	for (LONG i = 0; i < _dataCounts; i++)
	{
		if (isNo0 && line[i] == 0)
			continue;
		if (line[i] > m_maxY)
			m_maxY = (line[i]);
		if (line[i] < m_minY)
			m_minY = (line[i]);
	}
}

void CLShowTool::doExFlag(CLDC& dc, STUTHEADER& hdr, DOUBLE* line)
{
	if (hdr.exFlag & STEF_ADDTOPLINE)
	{
		STUTHEADER nh(hdr);
		nh.exFlag = (hdr.exFlag & STEF_NODRAWLINE_0) ? STEF_NODRAWLINE_0 : 0;
		nh.clr = nh.exClr;
		drawUtLine(dc, nh, line, 0);
	}

}

BOOL CLShowTool::drawGrapher(CLDC& dc)
{
	getClientRect(&clientRect);
	if (m_type != SHOWGRAPHER_TYPE_BITMAP) {
		dc.FillSolidRect(&clientRect, m_gbkColor);//����൱�ڲ����Ĳ��裬�������ֱ�����ɫ
	}
	//����Ϊshow�����ʾͼ��
	if (m_type == SHOWGRAPHER_TYPE_SHOWKLINE)
	{
		drawKLine(dc, pPt, kLineHeader.nDays);
	}
	else if (m_type == SHOWGRAPHER_TYPE_DUMPTSDISTRIB)
	{
		drawTsDistribution(dc, dis);
	}
	else if (m_type == SHOWGRAPHER_TYPE_LINES) {
		m_minY = m_minX = 1.0e38;
		m_maxY = m_maxX = -1.0e38;
		for (auto i = m_mpUtHdr.begin(); i != m_mpUtHdr.end(); ++i)
		{
			if ((i->second.utType == STLT_ADDAXIS_X) 
				|| (i->second.utType == STLT_ADDAXIS_Y)
				|| (i->second.utType == STLT_TEXT))
				continue;
			if (i->second.minX < m_minX)m_minX = i->second.minX;
			if (i->second.maxX > m_maxX)m_maxX = i->second.maxX;
			if (i->second.defMinY < m_minY)m_minY = i->second.defMinY;
			if (i->second.defMaxY > m_maxY)m_maxY = i->second.defMaxY;
			if (i->second.utType == STLT_LINE || i->second.utType == STLT_VERLINE)
			{
				updateMaxYMinY(m_mplineLst[i->first].Line, &(i->second));
			}
		}
		if (m_minX > m_maxX)
			m_maxX = m_minX = 0;
		if (m_minY > m_maxY)
			m_maxY = m_minY = 0;
		//leftWD = LEFTWIDE;
		//bottonWD = BOTTONWIDE;
		//���ȼ��������ؿ�
		if (m_isYAxis > 0) 
			drawUtAxisY(dc, m_mpUtHdr[m_isYAxis]);		
		//��μ����¿�
		if (m_isXAxis > 0)
			drawUtAxisX(dc, m_mpUtHdr[m_isXAxis]);
		//�����������
		for (auto i = m_mpUtHdr.begin(); i != m_mpUtHdr.end(); ++i)
		{
			switch (i->second.utType) {
			case STLT_LINE:
				drawUtLine(dc, i->second, m_mplineLst[i->first].Line, &(m_mplineLst[i->first].LineX)); break;
			case STLT_VERLINE:
				drawUtVerline(dc, i->second, m_mplineLst[i->first].Line, &(m_mplineLst[i->first].LineX)); break;
			case STLT_TEXT:
				drawUtText(dc, i->second, m_mpTextLst[i->first]); break;
			//���ټ���xy
			/*case STLT_ADDAXIS_X:
				drawUtAxisX(dc, i->second); break;
			case STLT_ADDAXIS_Y:
				drawUtAxisY(dc, i->second); break;*/
			default:break;
			}
		}
	}
	else if (m_type == SHOWGRAPHER_TYPE_BITMAP)
	{
		if (m_isLD > 0) {
			if (abs(m_cpt.x - m_strPt.x) + abs(m_cpt.y - m_strPt.y) < 100) {
				auto wi = m_strRect.Width();
				auto hi = m_strRect.Height();
				m_strRect.left = m_strRect.left - ((m_cpt.x - m_strPt.x) * m_strechRat);
				m_strRect.right = m_strRect.left + wi;
				m_strRect.top = m_strRect.top - ((m_cpt.y - m_strPt.y) * m_strechRat);
				m_strRect.bottom = m_strRect.top + hi;
			}
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
			m_strPt = m_cpt;
			LONG idr = (LONG)LoadCursor(NULL, IDC_HAND);
			SetClassLong(*this, -12, idr);
		}
		drawBitmap(dc);
	}
	else
	{//����Ϊ����ʱ�򣬷���
		m_err = _T("Warning �� û�п������ݣ� ...");
		dc.TextOut(0, 0, m_err.string(), m_err.strlen());
	}
	invalidate(FALSE);
	updateWindow();
	return TRUE;
}

void CLShowTool::drawUtLine(CLDC& dc, STUTHEADER& hdr, DOUBLE* line, std::map<long, RAGE>* lx)
{
	//assert(lx != NULL);
	CLPen pen(hdr.linePenType, hdr.lineWide, hdr.clr);
	long _dataCounts;
	if (hdr.exFlag & STEF_USE_AXIS_X_DATA) {
		_dataCounts = hdr.dataCounts / 2;//ʹ��һ������
		if (lx)lx->clear();
	}
	else {
		_dataCounts = hdr.dataCounts;
	}
	
	DOUBLE step = (DOUBLE)(clientRect.Width() - RIGHT_WIDE - LEFT_WIDE) * ((hdr.maxX - hdr.minX) / (m_maxX - m_minX)) / DOUBLE(_dataCounts > 1 ? _dataCounts - 1 : 1);
	if (step < 0)step = 0;
	DOUBLE maxy = clientRect.Height() - TOP_WIDE - BOTTON_WIDE;
	if (maxy < 0)maxy = 0;
	DOUBLE x = LEFT_WIDE + (hdr.minX - m_minX) / (m_maxX - m_minX) * (clientRect.Width() - RIGHT_WIDE - LEFT_WIDE) - step, y = 0;
	if (x < LEFT_WIDE)x = LEFT_WIDE - step;
	dc.SelectObject(&pen);
	DOUBLE disY = m_maxY - m_minY;
	if (disY <= 0)disY = 1;
	CLPOINT ptbak;
	x = ptbak.x = x + step;
	ptbak.y = TOP_WIDE + maxy * (1 - (line[0] - m_minY) / disY);
	if (hdr.exFlag & STEF_ADDTOPTEXT)
	{
		CLFont ft;
		ft.CreatePointFont(hdr.fontSize, hdr.fontTypeName, &dc);  //�����������
		dc.SelectObject(&ft);
		dc.SetTextColor(hdr.exClr);
		dc.SetBkMode(TRANSPARENT);
	}
	LONG acc = getFitDataAccuracy(hdr.dataAccuracy, m_minY, m_maxY);
	for (LONG i = 1; i < _dataCounts; i++)
	{
		x += step;
		y = TOP_WIDE + maxy * (1 - (line[i] - m_minY) / disY);
		if (!((hdr.exFlag & STEF_NODRAWLINE_0) && (line[i - 1] == 0 || line[i] == 0))) {
			drawLine(&dc, ptbak.x, ptbak.y, x, y);
			if ((hdr.exFlag & STEF_USE_AXIS_X_DATA) && (lx != NULL)) {
				//long cx = ((int)ptbak.x + (int)x) / 2;
				long cx = (int)x;
				auto pr = &((*lx)[cx]);
				pr->XV = line[i + _dataCounts];
				pr->LB = (ptbak.x + x) / 2 - 1;
				pr->RT = x + (x - ptbak.x) / 2;
			}
			if (hdr.exFlag & STEF_ADDTOPTEXT)
			{
				if (!((hdr.exFlag & STEF_NODRAWTEXT_0) && (line[i - 1] == 0 || line[i] == 0))) {
					m_szTmp.empty().appendf(line[i] * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
					drawText(dc, m_szTmp.string(), m_szTmp.strlen(), x, y, line[i] > 0 ? 8 : 2);
				}
			}
		}
		ptbak.x = x;
		ptbak.y = y;
	}
	doExFlag(dc, hdr, line);
}

void CLShowTool::drawUtVerline(CLDC& dc, STUTHEADER& hdr, DOUBLE* line, std::map<long, RAGE>* lx)
{

	long _dataCounts;
	if (hdr.exFlag & STEF_USE_AXIS_X_DATA) {
		_dataCounts = hdr.dataCounts / 2;//ʹ��һ������
		if (lx)lx->clear();
	}
	else {
		_dataCounts = hdr.dataCounts;
	}
	CLBrush bru(hdr.clr);
	CLBrush bruEx;
	BOOL _isUseDB = FALSE;
	if (_isUseDB = (hdr.exFlag & STEF_VERLINEUSEDBCOLOR))
		bruEx.CreateSolidBrush(hdr.exClr);
	DOUBLE step = (DOUBLE)(clientRect.Width() - RIGHT_WIDE - LEFT_WIDE) 
		* ((hdr.maxX - hdr.minX) / (m_maxX - m_minX)) 
		/// DOUBLE(_dataCounts > 1 ? _dataCounts - 1 : 1);
		/ DOUBLE(_dataCounts > 1 ? _dataCounts : 1);
	if (step < 0)step = 0;
	DOUBLE maxy = clientRect.Height() - TOP_WIDE - BOTTON_WIDE;
	if (maxy < 0)maxy = 0;
	DOUBLE disY = m_maxY - m_minY;
	if (disY <= 0)disY = 1;
	DOUBLE x = LEFT_WIDE + (hdr.minX - m_minX) / (m_maxX - m_minX) * (clientRect.Width() - RIGHT_WIDE - LEFT_WIDE) - step,
		y1 = 0, y0 = TOP_WIDE + maxy * (1 - (0 - m_minY) / disY);
	if (x < LEFT_WIDE)x = LEFT_WIDE - step;
	CLRect rt;
	if (hdr.exFlag & STEF_ADDTOPTEXT)
	{
		CLFont ft;
		ft.CreatePointFont(hdr.fontSize, hdr.fontTypeName, &dc);  //�����������
		dc.SelectObject(&ft);
		dc.SetTextColor(hdr.exClr);
		dc.SetBkMode(TRANSPARENT);
	}
	DOUBLE lef, cwd;
	if (hdr.verLineWide <= 0) {
		lef = ((step - 1.0) <= 1 ? 1 : (step - 1.0)) / 2.0;
		cwd = 2.0 * lef - 1.0;
	}
	else {
		lef = (hdr.verLineWide) / 2.0;
		cwd = 2.0 * lef - 1.0;
	}
	x += 0.5 * step; //ǰ�ư��
	LONG acc = getFitDataAccuracy(hdr.dataAccuracy, m_minY, m_maxY);
	for (LONG i = 0; i < _dataCounts; i++)
	{
		x += step;
		y1 = TOP_WIDE + maxy * (1 - (line[i] - m_minY) / disY);
		if (!((hdr.exFlag & STEF_NODRAWLINE_0) && (line[i] == 0))) {
			rt.left = (LONG)(x - lef);
			rt.right = rt.left + (LONG)cwd + 1;
			rt.top = y1;
			rt.bottom = y0;
			dc.FillRect(&rt, ((_isUseDB && (line[i] < 0)) ? &bruEx : &bru));
			if ((hdr.exFlag & STEF_USE_AXIS_X_DATA) && (lx != NULL)) {
				//long cx = ((int)ptbak.x + (int)x) / 2;
				long cx = (rt.left + rt.right) / 2;
				auto pr = &((*lx)[cx]);
				pr->XV = line[i + _dataCounts];
				pr->LB = x - lef - 1;
				pr->RT = x - lef + cwd + 1;
			}
			if (hdr.exFlag & STEF_ADDTOPTEXT)
			{
				if (!((hdr.exFlag & STEF_NODRAWTEXT_0) && (line[i] == 0))) {
					m_szTmp.empty().appendf(line[i] * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
					drawText(dc, m_szTmp.string(), m_szTmp.strlen(), x, y1, line[i] > 0 ? 8 : 2);
				}
			}
		}
	}
	doExFlag(dc, hdr, line);
}

void CLShowTool::drawUtText(CLDC& dc, STUTHEADER& hdr, PCLString str)
{
	CLFont ft;
	ft.CreatePointFont(hdr.fontSize, hdr.fontTypeName, &dc);
	dc.SelectObject(&ft);
	dc.SetTextColor(hdr.clr);
	dc.SetBkMode(TRANSPARENT);
	SIZE si = { 0 };
	GetTextExtentPoint32(dc.m_hDC, str->string(), str->strlen(), &si);
	LONG n = 0;
	if (hdr.textLine > 0)
		n = (hdr.textLine - 1) * si.cy;
	else if (hdr.textLine < 0) {
		n = clientRect.bottom - hdr.textLine * (-1) * si.cy;
	}
	drawText(dc, str->string(), str->strlen(), (m_isYAxis > 0 ? LEFT_WIDE + 5: 0), n, 1);
}

void CLShowTool::drawUtAxisX(CLDC& dc, STUTHEADER& hdr)
{
	DOUBLE
		xLeft = LEFT_WIDE,
		xRight = (DOUBLE)clientRect.right - RIGHT_WIDE,
		yBotton = (DOUBLE)clientRect.bottom - BOTTON_WIDE + 4,
		yBotton2 = yBotton + 5;
	if (xRight < xLeft)
		xRight = xLeft;
	CLPen pen(hdr.linePenType, hdr.lineWide, hdr.clr);
	dc.SelectObject(&pen);
	if (yBotton > TOP_WIDE)
	drawLine(&dc, xLeft - 4, yBotton, xRight + 4, yBotton);//��X��
	CLFont ft;
	ft.CreatePointFont(hdr.fontSize, hdr.fontTypeName, &dc);  //�����������
	dc.SelectObject(&ft);
	dc.SetTextColor(hdr.clr);
	dc.SetBkMode(TRANSPARENT);
	LONG acc = getFitDataAccuracy(hdr.dataAccuracy, m_minX, m_maxX);
	DOUBLE
		xASpan = xRight - xLeft,//x�����ڳ�
		xASpanbak = xASpan; //x��ԭ��
	SIZE szMinVSize, szMaxVSize;
	if (m_minX < -1e307 || m_minX > 1e307)
		szMinV = _T("Err���������");
	else
		szMinV.ftos(m_minX * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
	if (m_maxX > 1e307 || m_maxX < -1e307)
		szMaxV = _T("Err���������");
	else
		szMaxV.ftos(m_maxX * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
	GetTextExtentPoint32(dc.m_hDC, szMinV.string(), szMinV.strlen(), &szMinVSize);
	GetTextExtentPoint32(dc.m_hDC, szMaxV.string(), szMaxV.strlen(), &szMaxVSize);
	DOUBLE _min = xLeft, _max = xRight; //��С���������ֵ
	DOUBLE x0 = (0 - m_minX) / (m_maxX - m_minX) * xASpan + LEFT_WIDE; //����x0��x����
	DOUBLE _maxWD = BOTTONWIDE; //�ײ�������ؿ��
	if (xASpan > szMinVSize.cx) { //��Сֵ�Ŀ��С��x������ֵ
		if(yBotton > TOP_WIDE)
			drawText(dc, szMinV.string(), szMinV.strlen(), xLeft, yBotton2 + 1, 1);
		xASpan -= szMinVSize.cx;
		_min += szMinVSize.cx;
		if (yBotton > TOP_WIDE)
			drawLine(&dc, xLeft, yBotton, xLeft, yBotton2);//����Сֵ�Ŀ̶���
		_maxWD = szMinVSize.cy + 12;
	}
	if (xASpan > szMaxVSize.cx) {
		if (yBotton > TOP_WIDE)
			drawText(dc, szMaxV.string(), szMaxV.strlen(), xRight, yBotton2 + 1, 3);
		xASpan -= szMaxVSize.cx;
		_max -= szMaxVSize.cx;
		if (yBotton > TOP_WIDE)
			drawLine(&dc, xRight, yBotton, xRight, yBotton2);//�����̶�
		if (szMaxVSize.cy + 12 > _maxWD)
			_maxWD = szMaxVSize.cy + 12;
	}
	if (xASpan > 10 && _max > _min && BETWEENO(x0, _min, _max)) //����x0�̶�
	{
		m_szTmp.empty().appendf(hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString); //����x0ƫ��
		if (yBotton > TOP_WIDE) {
			drawText(dc, m_szTmp.string(), m_szTmp.strlen(), x0, yBotton2 + 1, 2);
			drawLine(&dc, x0, yBotton, x0, yBotton2);
		}
	}
	if (m_isLD > 0 && BETWEENO(m_cpt.x, xLeft, xRight) && xASpanbak > 0) //����ǰ��µ����
	{
		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(m_gbkColor);
		//dc.SetBkMode(TRANSPARENT);

		LONG basePtX;// = m_cpt.x;
		DOUBLE baseX;// = (((DOUBLE)(m_cpt.x) - xLeft) / xASpan2 * (m_maxX - m_minX) + m_minX);
		getBasePtX(basePtX, baseX, m_cpt.x, xLeft, xASpanbak);

		m_szTmp.empty().appendf(baseX * hdr.dataPrintMulti + hdr.dataPrintOffset,
			acc).append(hdr.dataPrintEndString);
		GetTextExtentPoint32(dc.m_hDC, m_szTmp.string(), m_szTmp.strlen(), &szMaxVSize);
		if (szMaxVSize.cy + 12 > _maxWD)
			_maxWD = szMaxVSize.cy + 12;
		drawText(dc, m_szTmp.string(), m_szTmp.strlen(), basePtX, yBotton2 + 1, 2);
		drawLine(&dc, basePtX, yBotton, basePtX, yBotton2);//��Y
		CLPen pen2(PS_DOT, 1, CLGRAY);
		drawLineP(&dc, pen2, basePtX, yBotton - hdr.lineWide, basePtX, TOP_WIDE);//��X
	}
	bottonWD = _maxWD;
}

void CLShowTool::drawUtAxisY(CLDC& dc, STUTHEADER& hdr)
{
	DOUBLE xLeft = TOP_WIDE,
		xRight = (DOUBLE)clientRect.bottom - BOTTON_WIDE;	
	if (xRight < xLeft)xRight = xLeft;	
	CLFont ft;
	ft.CreatePointFont(hdr.fontSize, hdr.fontTypeName, &dc);  //�����������
	dc.SelectObject(&ft);	
	DOUBLE
		xASpan = xRight - xLeft,
		xASpan2 = xASpan;
	SIZE s1, s2;
	LONG acc = getFitDataAccuracy(hdr.dataAccuracy, m_minY, m_maxY);
	//szMinV.format(30,_T("%g"),m_maxY);
	if (m_maxY > 1e307 || m_maxY < -1e307)
		szMinV = _T("Err���������");
	else
		szMinV.ftos(m_maxY * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
	if (m_minY < -1e307 || m_minY > 1e307)
		szMaxV = _T("Err���������");
	else
		szMaxV.ftos(m_minY * hdr.dataPrintMulti + hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
	GetTextExtentPoint32(dc.m_hDC, szMinV.string(), szMinV.strlen(), &s1);//ȡ���ֵ�Ĵ�С
	GetTextExtentPoint32(dc.m_hDC, szMaxV.string(), szMaxV.strlen(), &s2);//ȡ��Сֵ��С
	
	//���±����ȼ���ǰ����
	DOUBLE _maxWD = LEFTWIDE;
	if (xASpan > s1.cy) { //д���ֵ
		_maxWD = s1.cx + 12;
	}
	if (xASpan > s2.cy) { //д��Сֵ
		if (s2.cx + 12 > _maxWD)_maxWD = s2.cx + 12;
	}
	if (m_isLD > 0 && BETWEENO(m_cpt.y, xLeft, xRight) && xASpan2 > 0)
	{
		m_szTmp.empty().appendf((m_maxY - ((DOUBLE)(m_cpt.y) - xLeft) / xASpan2 * (m_maxY - m_minY)) * hdr.dataPrintMulti + hdr.dataPrintOffset,
			acc).append(hdr.dataPrintEndString);
		SIZE s3;
		GetTextExtentPoint32(dc.m_hDC, m_szTmp.string(), m_szTmp.strlen(), &s3);
		if (s3.cx + 12 > _maxWD)_maxWD = s3.cx + 12;
	}
	leftWD = _maxWD; //�ȸ��¿�
	DOUBLE _min = xLeft,
		_max = xRight,
		x0 = (m_maxY) / (m_maxY - m_minY) * xASpan + TOP_WIDE;
	const DOUBLE y = LEFT_WIDE - 4,
		y2 = y - 5;
	CLPen pen(hdr.linePenType, hdr.lineWide, hdr.clr);
	dc.SelectObject(&pen);
	drawLine(&dc, y, xLeft - 4, y, xRight + 4); //��y��
	xASpan = xRight - xLeft,
		xASpan2 = xASpan;
	dc.SetTextColor(hdr.clr);
	dc.SetBkMode(TRANSPARENT);
	if (xASpan > s1.cy) { //д���ֵ
		drawText(dc, szMinV.string(), szMinV.strlen(), y2 - 1, xLeft, 3);
		xASpan -= s1.cy;
		_min += s1.cy;
		drawLine(&dc, y, xLeft, y2, xLeft);
	}
	if (xASpan > s2.cy) { //д��Сֵ
		drawText(dc, szMaxV.string(), szMaxV.strlen(), y2 - 1, xRight, 9);
		xASpan -= s2.cy;
		_max -= s2.cy;
		drawLine(&dc, y, xRight, y2, xRight);
	}
	if (xASpan > 10 && _max > _min&& BETWEENO(x0, _min, _max))
	{
		m_szTmp.empty().appendf(hdr.dataPrintOffset, acc).append(hdr.dataPrintEndString);
		drawText(dc, m_szTmp.string(), m_szTmp.strlen(), y2 - 1, x0, 6);
		drawLine(&dc, y, x0, y2, x0);
	}
	if (m_isLD > 0 && BETWEENO(m_cpt.y, xLeft, xRight) && xASpan2 > 0)
	{
		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(RGB(0, 0, 0));
		m_szTmp.empty().appendf((m_maxY - ((DOUBLE)(m_cpt.y) - xLeft) / xASpan2 * (m_maxY - m_minY)) * hdr.dataPrintMulti + hdr.dataPrintOffset,
			acc).append(hdr.dataPrintEndString);
		drawText(dc, m_szTmp.string(), m_szTmp.strlen(), y2 - 1, m_cpt.y, 6);
		drawLine(&dc, y, m_cpt.y, y2, m_cpt.y);
		CLPen pen2(PS_DOT, 1, CLGRAY);
		drawLineP(&dc, pen2, y + hdr.lineWide, m_cpt.y, clientRect.right - RIGHT_WIDE, m_cpt.y);
	}	
}

LONG CLShowTool::getFitDataAccuracy(LONG curAcc, DOUBLE vmin, DOUBLE vmax) {
	curAcc = max(0, curAcc);
	DOUBLE v = max(abs(vmin), abs(vmax));
	LONGLONG i1 = 0; LONG acc = 0;
	for (; acc < 13;++acc) {		
		i1 = v;
		if (i1 > 0)
			break;
		v -= i1;
		v *= 10;
	}
	if (acc > 0)
		return min(acc + curAcc, 13);
	else 
		return curAcc;
}

void CLShowTool::drawText(CLDC& dc, LPCTSTR str, LONG szlen, LONG x, LONG y, LONG alignFlag)
{
	if (alignFlag < 0 || alignFlag > 9)alignFlag = 0;
	if (alignFlag == 0) { dc.TextOut(x, y, str, szlen); return; }
	switch (alignFlag) {
	case 1: SetTextAlign(dc.m_hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP); break;
	case 2: SetTextAlign(dc.m_hDC, TA_CENTER | TA_TOP | TA_NOUPDATECP); break;
	case 3: SetTextAlign(dc.m_hDC, TA_RIGHT | TA_TOP | TA_NOUPDATECP); break;
	case 4: SetTextAlign(dc.m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP); break;
	case 5: SetTextAlign(dc.m_hDC, TA_CENTER | TA_BASELINE | TA_NOUPDATECP); break;
	case 6: SetTextAlign(dc.m_hDC, TA_RIGHT | TA_BASELINE | TA_NOUPDATECP); break;
	case 7: SetTextAlign(dc.m_hDC, TA_LEFT | TA_BOTTOM | TA_NOUPDATECP); break;
	case 8: SetTextAlign(dc.m_hDC, TA_CENTER | TA_BOTTOM | TA_NOUPDATECP); break;
	case 9: SetTextAlign(dc.m_hDC, TA_RIGHT | TA_BOTTOM | TA_NOUPDATECP); break;
	default:return;
	}
	dc.TextOut(x, y, str, szlen);
}

LONG CLShowTool::drawTsDistribution(CLDC& dc, DOUBLE* dis)
{
	CLBrush yongBru(CLYOUNG);
	CLBrush redBru(CLRED);
	CLPen yellowPen(PS_SOLID, 1, CLYELLOW);
	LONG it = (TSDIS_COUNTS1 - 1);
	DOUBLE step = (clientRect.Width() - OFFSET * 2) / (TSDIS_COUNTS1 - 1);
	if (step < 2)step = 2;
	DOUBLE max = 0;
	for (LONG i = 1; i <= it; i++)
		if (max < dis[i])
			max = dis[i];
	if (max < 10)max = 10;
	DOUBLE maxy = clientRect.Height() - OFFSET * 2;
	if (maxy < 0)maxy = 0;
	DOUBLE x = OFFSET - step, y0 = clientRect.bottom - OFFSET, y1 = 0;
	CLRect rt;
	dc.SelectObject(&yellowPen);
	CLPOINT ptbak;
	ptbak.x = x + step;
	ptbak.y = OFFSET + maxy * (1 - (DOUBLE)dis[1] / (DOUBLE)max);
	for (LONG i = 1; i <= it / 2; i++)
	{
		x += step;
		y1 = OFFSET + maxy * (1 - (DOUBLE)dis[i] / (DOUBLE)max);
		rt.left = x;
		rt.right = x + step - 2 < x + 1 ? x + 1 : x + step - 2;
		rt.top = y1;
		rt.bottom = y0;
		dc.FillRect(&rt, &yongBru);
		drawLine(&dc, ptbak.x, ptbak.y, x, y1);
		ptbak.x = x;
		ptbak.y = y1;
	}
	for (LONG i = it / 2 + 1; i <= it; i++)
	{
		x += step;
		y1 = OFFSET + maxy * (1 - (DOUBLE)dis[i] / (DOUBLE)max);
		rt.left = x;
		rt.right = x + step - 2 < x + 1 ? x + 1 : x + step - 2;
		rt.top = y1;
		rt.bottom = y0;
		dc.FillRect(&rt, &redBru);
		drawLine(&dc, ptbak.x, ptbak.y, x, y1);
		ptbak.x = x;
		ptbak.y = y1;
	}
	return 1;
}

DOUBLE CLShowTool::MaxV(DAYDATASETS_UNIT1 pPt[], LONGLONG nSize, LONGLONG nSiRi,
	LONGLONG nSiStart, LONGLONG nSiEnd)
{
	DOUBLE ret = 0;
	if (nSize == 0) return 0;
	for (LONG i = nSiStart; i <= nSiEnd; i++)
	{
		if (pPt[i].high > ret)
			ret = (DOUBLE)pPt[i].high;
	}
	return ret;
}
DOUBLE CLShowTool::MinV(DAYDATASETS_UNIT1 pPt[], LONGLONG nSize, LONGLONG nSiRi,
	LONGLONG nSiStart, LONGLONG nSiEnd)
{
	if (nSize == 0) return 0;
	DOUBLE ret = pPt[nSiStart].low;
	for (LONG i = nSiStart; i <= nSiEnd; i++)
	{
		if (pPt[i].low <= ret)
			ret = (DOUBLE)pPt[i].low;
	}
	return ret;
}
DOUBLE CLShowTool::MaxN(DAYDATASETS_UNIT1 pPt[], LONGLONG nSize, LONGLONG nSiRi,
	LONGLONG nSiStart, LONGLONG nSiEnd)
{
	DOUBLE ret = 0;
	if (nSize == 0) return 1;
	for (LONG i = nSiStart; i <= nSiEnd; i++)
	{
		if (pPt[i].volum > ret)
			ret = (DOUBLE)pPt[i].volum;
	}
	return !ret ? 1 : ret;
}

BOOL CLShowTool::drawOneKBox(const CLPOINT& spt, CLPOINT& sptn, DAYDATASETS_UNIT1& rdu, DOUBLE PWD, LONG PHV, LONG PHN, DOUBLE radio_vy, DOUBLE radio_ny, DOUBLE maxV, DOUBLE minV, CLDC& dc)
{
	BOOL isUp = ((rdu.close - rdu.open) >= 0) ? true : false;
	CLBrush brushR(CLRED);//��ˢ����
	CLBrush brushG(CLGREEN);//��ˢ����
	CLPen penR(PS_SOLID, 1, CLRED);
	CLPen penG(PS_SOLID, 1, CLGREEN);
	HGDIOBJ pOleBru = dc.SelectObject((isUp ? &brushR : &brushG));
	HGDIOBJ pOldPen = dc.SelectObject((isUp ? &penR : &penG));
	//����ɽ���������
	CLRect rect(spt.x + 1, spt.y, spt.x + PWD - 1, spt.y - rdu.volum * radio_ny + 1);
	dc.Rectangle(&rect);//���ɽ���

	POINT p1, p2, p3, p4;
	DOUBLE midx = spt.x + PWD / 2;
	DOUBLE vstarty = spt.y - PHN - OFFSETMID;
	p1.x = p2.x = p3.x = p4.x = midx;
	p1.y = vstarty - (rdu.low - minV) * radio_vy;
	p4.y = vstarty - (rdu.high - minV) * radio_vy;

	if (isUp)
	{
		//ʹ��͸����ˢ
		HBRUSH hbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		//CLBrush *pBrush=CLBrush::FromHandle(hbrush);
		dc.SelectObject(hbrush);
		p2.y = vstarty - (rdu.open - minV) * radio_vy;
		p3.y = vstarty - (rdu.close - minV) * radio_vy;
	}
	else
	{
		p2.y = vstarty - (rdu.close - minV) * radio_vy;
		p3.y = vstarty - (rdu.open - minV) * radio_vy;
	}
	dc.MoveTo(p1.x, p1.y);
	dc.LineTo(p2.x, p2.y);
	dc.MoveTo(p3.x, p3.y);
	dc.LineTo(p4.x, p4.y);

	CLRect rect2(spt.x, p2.y, spt.x + PWD - 1, p3.y);
	dc.Rectangle(rect2);//��K��
	if (p2.y == p3.y) //���ߵ����
	{
		dc.MoveTo(spt.x, p2.y);
		dc.LineTo(spt.x + PWD - 1, p3.y);
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOleBru);

	sptn.x = spt.x - PWD;
	return TRUE;
}
BOOL CLShowTool::drawBitmap(CLDC& dc)
{
	if (!hBitmap) {
		hBitmap = ::CreateDIBitmap(dc,
			&m_BtmapInfoHdr,
			CBM_INIT,
			(BYTE*)pBitmapBuf + m_BtmapFileHdr.bfOffBits,
			pBitmapInfo,
			DIB_RGB_COLORS);
		//if(pBitmapBuf)delete[] pBitmapBuf;
		//pBitmapBuf = 0;		
		::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		::SelectObject(dc, hBitmap);
	}
	if (hBitmap) {
		OnPaint();
	}
	return TRUE;
}

BOOL CLShowTool::drawKLine(CLDC& dc, DAYDATASETS_UNIT1 pPt[], LONGLONG nSize)
{

	if (nSize < 2)
	{
		messageBox(_T("nSize < 2 drawKLine return ST_FALSE"));
		return ST_FALSE;
	}
	CLPOINT spt, sptn;
	if (isEirstPaint) //��ʼ����Χ
	{
		nSiEnd = nSize - 1;
		if (nSize > FIRSTDAYS)
			nSiStart = nSiEnd - FIRSTDAYS + 1;
		isEirstPaint = FALSE;
	}
	nSiRi = nSiEnd - nSiStart + 1;
	DOUBLE PWD = (clientRect.right - 2 * OFFSET) / nSiRi;//ÿ���������
	if (PWD < 3)
	{
		PWD = 3;
		nSiRi = (LONG)(clientRect.right - 2 * OFFSET) / PWD;//���Ʋ���
		if (nSiRi > nSize) nSiRi = nSize;
		nSiStart = nSiEnd - nSiRi + 1;
	}
	nSiAll = nSize - 1;

	DOUBLE PHV = (DOUBLE)(((DOUBLE)clientRect.bottom) * (1 - DEVICE1 - DEVICE2) - OFFSET);//k�߻������ؿռ�߶�
	DOUBLE PHN = (DOUBLE)(((DOUBLE)clientRect.bottom) * DEVICE1 - OFFSETMID - OFFSET);//�ɽ����߻������ؿռ�߶�
	LONG PHT = clientRect.bottom * DEVICE2 - OFFSET;
	DOUBLE maxV = MaxV(pPt, nSize, nSiRi, nSiStart, nSiEnd);
	DOUBLE minV = MinV(pPt, nSize, nSiRi, nSiStart, nSiEnd);
	DOUBLE maxN = MaxN(pPt, nSize, nSiRi, nSiStart, nSiEnd);
	DOUBLE dlta = (maxV - minV) != 0 ? (maxV - minV) : 1;
	DOUBLE radio_vy = PHV / dlta;
	DOUBLE radio_ny = PHN / maxN;
	spt.x = (DOUBLE)clientRect.right - OFFSET - PWD + 1;
	spt.y = (DOUBLE)clientRect.bottom * (1 - DEVICE2) - OFFSET;

	CLFont font;
	font.CreatePointFont(100, _T("����"), NULL);  //�����������
	HGDIOBJ pOldFont = dc.SelectObject(&font);
	TEXTMETRIC xASpan;
	dc.GetTextMetrics(&xASpan);
	COLORREF oldColor = dc.SetTextColor(RGB(255, 0, 0));//�ú�ɫ��
	dc.SetBkMode(TRANSPARENT);//�������ֱ���ɫ͸��

	//����ÿһ���K��
	BOOL isDrawDate = FALSE;
	CLSize szd = dc.GetTextExtent(_T("xxxx-xx-xx"), 10);
	CLString inf;
	for (LONG i = nSiEnd; i < nSize && i >= nSiStart; i--)
	{
		if (nSiRi <= 4 && (i == nSiEnd || i == nSiStart))
			isDrawDate = TRUE;
		else if (i == nSiEnd || i == nSiStart
			|| i == (LONG)(nSiEnd - nSiRi / 3) || i == (LONG)(nSiEnd - nSiRi * 2 / 3))
			isDrawDate = TRUE;
		if (isDrawDate)
		{
			//д����
			DOUBLE t1 = PWD < szd.cx ? PWD : PWD / 2 + szd.cx / 2;
			m_szTmp.format(10, _T("%04d-%02d-%02d"), EXTR_YEAR(pPt[i].date), EXTR_MON(pPt[i].date), EXTR_DAY(pPt[i].date));
			dc.TextOut(spt.x + t1 - 1 - szd.cx, spt.y + OFFSET + PHT / 2 - szd.cy / 2, m_szTmp.string(), m_szTmp.strlen());
			//������
			CLPen penR(PS_SOLID, 1, RGB(255, 0, 0));
			HGDIOBJ pOle = dc.SelectObject(&penR);
			dc.MoveTo(spt.x + PWD / 2, spt.y + OFFSET + PHT * 1 / 3);
			dc.LineTo(spt.x + PWD / 2, spt.y);
			dc.SelectObject(pOle);
		}
		drawOneKBox(spt, sptn, pPt[i], PWD, PHV, PHN, radio_vy, radio_ny, maxV, minV, dc);
		spt.x = sptn.x;
		isDrawDate = FALSE;
	}

	szMaxV.format(50, _T("%.2f"), maxV);
	szMidV.format(50, _T("%.2f"), (maxV + minV) / 2);
	szMinV.format(50, _T("%.2f"), minV);
	szMaxN.format(50, _T("%.0f"), maxN / 100.0);
	szMidN.format(50, _T("%.0f"), maxN / 200.0);
	szMinN = _T("0");

	//���Ʋο����Ͳο���

	CLPen pLDot(PS_DOT, 1, RGB(255, 0, 0));//���õ㻭��
	CLPen pLSolid(PS_SOLID, 1, RGB(255, 0, 0));
	HGDIOBJ pOldPen = dc.SelectObject(&pLDot);

	//��1--------------
	dc.TextOut(OFFSET / 2, OFFSET - xASpan.tmHeight / 2, szMaxV.string(), szMaxV.strlen());
	CLSize sz = dc.GetTextExtent(szMaxV.string(), szMaxV.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, OFFSET);
	dc.LineTo(clientRect.right, OFFSET);

	//��2--------------
	dc.TextOut(OFFSET / 2, OFFSET + PHV / 2 - sz.cy / 2, szMidV.string(), szMidV.strlen());
	sz = dc.GetTextExtent(szMidV.string(), szMidV.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, OFFSET + PHV / 2);
	dc.LineTo(clientRect.right, OFFSET + PHV / 2);
	//��3--------------
	dc.TextOut(OFFSET / 2, OFFSET + PHV - sz.cy / 2, szMinV.string(), szMinV.strlen());
	sz = dc.GetTextExtent(szMinV.string(), szMinV.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, OFFSET + PHV);
	dc.LineTo(clientRect.right, OFFSET + PHV);
	//��4--------------
	dc.TextOut(OFFSET / 2, clientRect.bottom * DEVICE0 + OFFSETMID - sz.cy / 2, szMaxN.string(), szMaxN.strlen());
	sz = dc.GetTextExtent(szMaxN.string(), szMaxN.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, clientRect.bottom * DEVICE0 + OFFSETMID);
	dc.LineTo(clientRect.right, clientRect.bottom * DEVICE0 + OFFSETMID);
	//��5--------------
	dc.TextOut(OFFSET / 2, clientRect.bottom * DEVICE0 + OFFSETMID + PHN / 2 - sz.cy / 2, szMidN.string(), szMidN.strlen());
	sz = dc.GetTextExtent(szMidN.string(), szMidN.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, clientRect.bottom * DEVICE0 + OFFSETMID + PHN / 2);
	dc.LineTo(clientRect.right, clientRect.bottom * DEVICE0 + OFFSETMID + PHN / 2);
	//��6--------------
	dc.TextOut(OFFSET / 2, clientRect.bottom * DEVICE0 + OFFSETMID + PHN - sz.cy / 2, szMinN.string(), szMinN.strlen());
	sz = dc.GetTextExtent(szMinN.string(), szMinN.strlen());
	dc.MoveTo(OFFSET / 2 + sz.cx, clientRect.bottom * DEVICE0 + OFFSETMID + PHN);
	dc.LineTo(clientRect.right, clientRect.bottom * DEVICE0 + OFFSETMID + PHN);

	//auto m_vt = m_cmdL.split(_T(' '));
	inf.format(100, _T("[StockID��%06d] %s"), kLineHeader.stockId, kLineHeader.szStockName);
	dc.TextOut(0, clientRect.bottom - sz.cy - 3, inf.string(), inf.strlen());
	dc.SetTextColor(oldColor);
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
	return TRUE;
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLShowTool::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ���û��ر� UI ʱ������������Ա���������ĳ��
//  �������Զ�����������Ӧ�˳�����Щ
//  ��Ϣ�������ȷ����������: �����������ʹ�ã�
//  ������ UI�������ڹرնԻ���ʱ��
//  �Ի�����Ȼ�ᱣ�������

void CLShowTool::OnClose()
{
	//OldClass::OnClose();
}

void CLShowTool::OnOK()
{
	//OldClass::OnOK();
}

void CLShowTool::OnCancel()
{
	//OldClass::OnCancel();
}

void CLShowTool::OnDestroy()
{
	release();
	m_memdc.DeleteDC();
	m_bkBitmap.DeleteObject();//������
// 	if(m_pdc){
// 		ReleaseDC(m_pdc);
// 		m_pdc=0;
// 	}
	m_isInit = FALSE;
}

HBRUSH CLShowTool::OnCtlColor(CLDC* pDC, CLShowTool* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = OldClass::OnCtlColor(pDC, pWnd, nCtlColor);
	HBRUSH hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// TODO:  �ڴ˸��� DC ���κ�����
	if (nCtlColor == CTLCOLOR_DLG)
	{
		hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CLShowTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_type == 1)
	{
		size_t nSold, nEold;
		switch (nChar)
		{
		case VK_LEFT:
			if (isEirstPaint)break;
			nSiStart -= 1;
			nSiEnd -= 1;
			if (nSiStart < 0)
				nSiStart = 0;
			if (nSiEnd < nSiRi - 1)
				nSiEnd = nSiRi - 1;
			drawGrapher(m_memdc);
			break;

		case VK_RIGHT:
			if (isEirstPaint)break;
			nSiStart += 1;
			nSiEnd += 1;
			if (nSiStart > nSiAll - nSiRi + 1) nSiStart = nSiAll - nSiRi + 1;
			if (nSiEnd > nSiAll) nSiEnd = nSiAll;
			drawGrapher(m_memdc);
			break;

		case VK_UP:
			if (isEirstPaint)break;
			nSold = nSiStart;
			nEold = nSiEnd;
			nSiStart += 1;
			if (nSiStart > nSiEnd) nSiStart = nSold;
			nSiEnd -= 1;
			if (nSiEnd < nSiStart) nSiEnd = nEold;
			if (nSiStart > nSiAll) nSiStart = nSiStart;
			if (nSiEnd < 0) nSiEnd = 0;
			drawGrapher(m_memdc);
			break;

		case VK_DOWN:
			if (isEirstPaint)break;
			nSold = nSiStart;
			nEold = nSiEnd;
			nSiStart -= 1;
			if (nSiStart > nSiEnd) nSiStart = nSold;
			nSiEnd += 1;
			if (nSiEnd < nSiStart) nSiEnd = nEold;
			if (nSiStart < 0) nSiStart = 0;
			if (nSiEnd > nSiAll) nSiEnd = nSiAll;
			if (nSiEnd - nSiStart > (clientRect.right - 2 * OFFSET) / 3)
			{
				nSiStart = nSold;
				nSiEnd = nEold;
			}
			if (nSiStart > nSiAll) nSiStart = nSiStart;
			if (nSiEnd < 0) nSiEnd = 0;
			drawGrapher(m_memdc);
			break;
		}
	}
	//OldClass::OnKeyDown(nChar, nRepCnt, nFlags);
}

// BOOL CLShowTool::PreTranslateMessage(MSG* pMsg)
// {
// 	if (pMsg->message==WM_KEYDOWN)//��ⰴť����
// 	{
// 		OnKeyDown(pMsg->wParam,	LOWORD(pMsg->lParam),HIWORD(pMsg->lParam)); //������Ϣ��Ӧ����
// 		//Sleep(0);
// 	}
// 	return OldClass::PreTranslateMessage(pMsg);
// }


void CLShowTool::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	BOOL rt = FALSE;
	LPBYTE p = 0;
	BYTE bt = 0;
	LONG i = 0;
	LONG lineType = 0;
	switch (nIDEvent)
	{
	case TIMERID_PEEKDATA:
		if (hRead == NULL) {
			m_type = SHOWGRAPHER_TYPE_NO;
			setWindowPos(NULL, 0, 0, 200, 100, SWP_NOZORDER | SWP_NOMOVE);
			break;
		}
		dwRead = dwTotal = dwleft = 0;
		rt = PeekNamedPipe(hRead, &bt, 1, &dwRead, &dwTotal, &dwleft);
		if (rt && dwTotal > 0) {
			m_isXAxis = m_isYAxis = 0;
			LONG x = 0, y = 0, cx = 0, cy = 0;
			m_isXdata = 0;
			if (dwTotal >= sizeof(SHOWGRAPHERHEARDER)) {
				if (!ReadFile(hRead, &m_pipeHeader, sizeof(SHOWGRAPHERHEARDER), &dwRead, NULL))
				{
					m_type = SHOWGRAPHER_TYPE_NO; break;
				}
				if (lastWriteTime == 0 && m_pipeHeader.type != SHOWGRAPHER_TYPE_NO) {
					x = m_pipeHeader.x; y = m_pipeHeader.y; cx = m_pipeHeader.cx; cy = m_pipeHeader.cy;
					setWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER
						| ((x == 0 && y == 0) ? SWP_NOMOVE : 0)
						| ((cx < 20 || cy < 20) ? SWP_NOSIZE : 0));
				}
				m_type = m_pipeHeader.type;
				m_gbkColor = m_pipeHeader.gbkColor;
			}
			else m_type = SHOWGRAPHER_TYPE_NO;

			switch (m_type) {
			case SHOWGRAPHER_TYPE_SHOWKLINE:
				if (!ReadFile(hRead, &kLineHeader, sizeof(KLINEDATAHEADER), &dwRead, NULL))
				{
					m_type = SHOWGRAPHER_TYPE_NO; break;
				}
				if (pPt)delete[] pPt;
				pPt = NULL;
				pPt = new DAYDATASETS_UNIT1[kLineHeader.nDays];
				if (!ReadFile(hRead, pPt, kLineHeader.nDays * sizeof(DAYDATASETS_UNIT1), &dwRead, NULL)) {
					m_type = SHOWGRAPHER_TYPE_NO;
					if (pPt)delete[] pPt;
					pPt = NULL;
					break;
				}
				m_dataExsit = TRUE;
				isEirstPaint = TRUE;
				nSiStart = nSiEnd = nSiAll = nSiRi = 0;
				setWindowText(std::_tcslen(m_pipeHeader.title) == 0 ? _T("K��ͼ") : m_pipeHeader.title);
				if (x == 0 && y == 0 && (cx <= 20 || cy <= 0))
					this->setWindowPos(NULL, 0, 0, 940, 600, SWP_NOZORDER);
				else
					this->setWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER
						| ((x == 0 && y == 0) ? SWP_NOMOVE : 0)
						| ((cx < 20 || cy < 20) ? SWP_NOSIZE : 0));
				break;
			case SHOWGRAPHER_TYPE_DUMPTSDISTRIB:
				if (lastWriteTime == 0)
					this->setWindowPos(NULL, 0, 0, (TSDIS_COUNTS1 - 1) * 2 + OFFSET * 4, 130, SWP_NOZORDER | SWP_NOMOVE);
				ZeroMemory(dis, sizeof(DOUBLE) * TSDIS_COUNTS1);
				if (!ReadFile(hRead, dis, sizeof(DOUBLE) * TSDIS_COUNTS1, &dwRead, NULL))
				{
					m_type = SHOWGRAPHER_TYPE_NO; break;
				}
				setWindowText(std::_tcslen(m_pipeHeader.title) == 0 ? _T("ͼ������������ṹ�ֲ�ͼ") : m_pipeHeader.title);
				break;
			case SHOWGRAPHER_TYPE_LINES:
				setWindowText(std::_tcslen(m_pipeHeader.title) == 0 ? _T("ͼ�����") : m_pipeHeader.title);
				m_mpUtHdr.clear();
				while (m_pipeHeader.dataWritedSizeInByte > 0) {

					STUTHEADER hdrt;
					rt = ReadFile(hRead, &(hdrt), sizeof(STUTHEADER), &dwRead, 0);

					m_pipeHeader.dataWritedSizeInByte -= (LONGLONG)dwRead;
					lineType = hdrt.utType;
					if (hdrt.exFlag & STEF_USE_AXIS_X_DATA)
						m_isXdata = 1;
					if (lineType == STLT_LINE || lineType == STLT_VERLINE)
					{
						i++; m_mpUtHdr[i] = hdrt;
						auto lie = m_mplineLst.find(i);
						if (lie != m_mplineLst.end()) {
							if (m_mpUtHdr[i].dataCounts > lie->second.si) {
								delete[] lie->second.Line;
								lie->second.Line = new DOUBLE[lie->second.si = m_mpUtHdr[i].dataCounts];
								lie->second.LineX.clear();
							}
						}
						else {
							m_mplineLst[i].Line = new DOUBLE[m_mplineLst[i].si = m_mpUtHdr[i].dataCounts];
						}
						ZeroMemory(m_mplineLst[i].Line, m_mpUtHdr[i].dataCounts * sizeof(DOUBLE));
						ReadFile(hRead, m_mplineLst[i].Line, m_mpUtHdr[i].dataCounts * sizeof(DOUBLE), &dwRead, 0);
						m_pipeHeader.dataWritedSizeInByte -= (LONGLONG)dwRead;
					}
					else if (lineType == STLT_TEXT)
					{
						i++; m_mpUtHdr[i] = hdrt;
						if (m_mpTextLst.find(i) == m_mpTextLst.end())
							m_mpTextLst[i] = new CLString(m_mpUtHdr[i].dataCounts);
						ReadFile(hRead, m_mpTextLst[i]->store(m_mpUtHdr[i].dataCounts), m_mpUtHdr[i].dataCounts * sizeof(TCHAR), &dwRead, 0);
						m_pipeHeader.dataWritedSizeInByte -= (LONGLONG)dwRead;
					}
					else if (lineType == STLT_ADDAXIS_X) {
						i++; m_mpUtHdr[i] = hdrt;
						m_isXAxis = i;
					}
					else if (lineType == STLT_ADDAXIS_Y) {
						i++; m_mpUtHdr[i] = hdrt;
						m_isYAxis = i;
					}
					else if (lineType == STLT_ADDAXIS_XY) {
						i++; m_mpUtHdr[i] = hdrt;
						m_isXAxis = i;
						m_mpUtHdr[m_isXAxis].utType = STLT_ADDAXIS_X;
						m_mpUtHdr[i = m_isYAxis = i + 1] = m_mpUtHdr[m_isXAxis];
						m_mpUtHdr[m_isYAxis].utType = STLT_ADDAXIS_Y;
					}
				}
				if (m_pipeHeader.addAxisFlag & STLT_ADDAXIS_X) {
					i++;
					m_mpUtHdr[i].reset();
					m_mpUtHdr[i].utType = STLT_ADDAXIS_X;
					m_mpUtHdr[i].dataAccuracy = m_pipeHeader.AxisTextDataAccuracy;
					m_isXAxis = i;
				}
				if (m_pipeHeader.addAxisFlag & STLT_ADDAXIS_Y) {
					i++;
					m_mpUtHdr[i].reset();
					m_mpUtHdr[i].utType = STLT_ADDAXIS_Y;
					m_mpUtHdr[i].dataAccuracy = m_pipeHeader.AxisTextDataAccuracy;
					m_isYAxis = i;
				}
				if (lastWriteTime == 0)
					drawGrapher(m_memdc);
				break;
			case SHOWGRAPHER_TYPE_BITMAP:
				setWindowText(std::_tcslen(m_pipeHeader.title) == 0 ? _T("λͼ���") : m_pipeHeader.title);
				while (m_pipeHeader.dataWritedSizeInByte > 0) {
					if (!pBitmapBuf || (long long)pBitmapBufSi < m_pipeHeader.dataWritedSizeInByte) {
						if (pBitmapBuf)delete[] pBitmapBuf;
						pBitmapBuf = new byte[pBitmapBufSi = m_pipeHeader.dataWritedSizeInByte];
					}
					if (pBitmapBuf)
						memset(pBitmapBuf, 0, pBitmapBufSi);
					rt = ReadFile(hRead, pBitmapBuf, m_pipeHeader.dataWritedSizeInByte, &dwRead, 0);
					m_pipeHeader.dataWritedSizeInByte -= (LONGLONG)dwRead;
					m_BtmapFileHdr = *((BITMAPFILEHEADER*)pBitmapBuf);
					m_BtmapInfoHdr = *((BITMAPINFOHEADER*)(((BITMAPFILEHEADER*)pBitmapBuf) + 1));
					pBitmapInfo = ((BITMAPINFO*)(((BITMAPFILEHEADER*)pBitmapBuf) + 1));
				}
				if (hBitmap)
					DeleteObject(hBitmap), hBitmap = 0;
				m_bitUpdateTimes++;
				if (lastWriteTime == 0)
					drawGrapher(m_memdc);
				break;
			default:
				p = new BYTE[dwTotal];
				ReadFile(hRead, p, dwTotal, &dwRead, NULL);
				delete[] p;
				m_type = SHOWGRAPHER_TYPE_NO;
				this->setWindowPos(NULL, 0, 0, 200, 100, SWP_NOZORDER | SWP_NOMOVE);
				//OnPaint();
				break;
			}
			drawGrapher(m_memdc);
		}
		else if (rt == FALSE) {
			CLString err;
			err.getLastErrorString(GetLastError());
		}
		break;
	case TIMERID_TS_REDRAW:
		if (m_type != SHOWGRAPHER_TYPE_NO && lastWriteTime != m_pipeHeader.writeTime) {
			lastWriteTime = m_pipeHeader.writeTime;
		}
		break;
	default:
		break;
	}
	//OldClass::OnTimer(nIDEvent);
}

void CLShowTool::OnNcDestroy()
{
	// TODO: �ڴ˴������Ϣ����������
}

void CLShowTool::OnLButtonDown(UINT nFlags, CLPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_isLD = 1;
	m_cpt = point;
	drawGrapher(m_memdc);
	//OldClass::OnLButtonDown(nFlags, point);
}

void CLShowTool::OnLButtonUp(UINT nFlags, CLPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_isLD = 0;
	m_cpt = point;
	LONG idr = (LONG)LoadCursor(NULL, IDC_ARROW);
	SetClassLong(*this, -12, idr);
	drawGrapher(m_memdc);
	//OldClass::OnLButtonUp(nFlags, point);
}

void CLShowTool::OnMouseMove(UINT nFlags, CLPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ	
	if (m_isLD > 0) {
		m_cpt = point;
		drawGrapher(m_memdc);
	}
	//OldClass::OnMouseMove(nFlags, point);
}

void CLShowTool::OnSizing(UINT fwSide, LPRECT pRect)
{
	//OldClass::OnSizing(fwSide, pRect);

	// TODO: �ڴ˴������Ϣ����������
	drawGrapher(m_memdc);
}

void CLShowTool::OnSize(UINT nType, LONG cx, LONG cy)
{
	//OldClass::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	drawGrapher(m_memdc);
}

void showFile(LPCTSTR szFile)
{
	//_tprintf(_T("FileOrPath:%s/n"),szFile);
	LPCTSTR p = szFile;
	while (*(p++))
	{
	}
	if (*p == 0)
	{
		//_tprintf(_T("no Other File/n"));
		return;
	}
	do
	{
		//_tprintf(_T("FileName:%s/n"),p);
		while (*(p++))
		{
		}
	} while (*p != 0);
	//_tprintf(_T("ShowFile end/n"));
}

BOOL CLShowTool::windowsToBmp(HWND hTagWnd, LPCTSTR outFileName)
{
	if (!::IsWindow(hTagWnd))
		return FALSE;
	if (::IsIconic(hTagWnd))
		return FALSE;
	CLString file, path = outFileName, name = outFileName;
	if (!path.deleteLastStrFromPath().filePathExists())
		if (!path.createDirectory())
			return ::MessageBox(hTagWnd, CLString(_T("·��������:\n"), (LPCTSTR)(path), 0).string(), _T("������ʾ"), MB_ICONERROR), FALSE;
	name.getLastStrByPath();
	auto pos = name.reverseFind(_T('.'));
	if (pos > 0)
		name.leftSave(pos);
	else
		return ::MessageBox(hTagWnd, CLString(_T("�ļ�������:\n"), (LPCTSTR)(outFileName), 0).string(), _T("������ʾ"), MB_ICONERROR), FALSE;
	file = path + name + _T(".bmp");
	if (file.fileExists()) {
		DeleteFile(file);
	}
	CLRect rect;
	::GetWindowRect(hTagWnd, (RECT*)&rect);
	//�����豸������
	HDC hs = ::GetWindowDC(hTagWnd);
	//�������ݵ��豸������
	HDC hp = CreateCompatibleDC(hs);
	// �������豸��������ݵ�λͼ
	HBITMAP hbitmap = CreateCompatibleBitmap(hs, rect.Width(), rect.Height());
	// ��λͼѡ���豸��������
	SelectObject(hp, hbitmap);
	// ���豸�����������ڴ��豸��������
	BitBlt(hp, 0, 0, rect.Width(), rect.Height(), hs, 0, 0, SRCCOPY);
	DeleteDC(hs);
	DeleteDC(hp);
	HDC hdc = 0;
	DWORD dwbmbitssize, dwdibsize;
	BITMAP bitmap; //λͼ���Խṹ
	BITMAPFILEHEADER bmfhdr; //λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER bi; //λͼ��Ϣͷ�ṹ
	LPBITMAPINFOHEADER lpbi; //ָ��λͼ��Ϣͷ�ṹ
	HANDLE hdib = NULL, hpal = NULL, holdpal = NULL;
	//����λͼ��Ϣͷ
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwbmbitssize = ((bitmap.bmWidth * 24 + 31) / 32) * 4 * bitmap.bmHeight;
	hdib = GlobalAlloc(GHND, dwbmbitssize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
	*lpbi = bi;
	// �����ɫ�� 
	hpal = GetStockObject(DEFAULT_PALETTE);
	if (hpal)
	{
		hdc = ::GetDC(NULL);
		holdpal = ::SelectPalette(hdc, (HPALETTE)hpal, false);
		::RealizePalette(hdc);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ
	::GetDIBits(hdc, hbitmap, 0, (UINT)bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
	// λͼ�ļ�ͷ
	bmfhdr.bfType = 0x4d42;
	dwdibsize = 54 + dwbmbitssize;
	bmfhdr.bfSize = dwdibsize;
	bmfhdr.bfReserved1 = 0;
	bmfhdr.bfReserved2 = 0;
	bmfhdr.bfOffBits = 54;
	FILE* fl = NULL;
	if (0 == _tfopen_s(&fl, file.string(), _T("wb")) && fl != 0 && lpbi != 0) {
		fwrite((void*)&bmfhdr, sizeof(BITMAPFILEHEADER), 1, fl);
		fwrite((void*)lpbi, dwdibsize, 1, fl);
		fclose(fl);
	}
	::GlobalUnlock(hdib);
	::GlobalFree(hdib);
	return TRUE;
}
BOOL CLShowTool::windowsToBmp(LPCTSTR outFileName) const { return windowsToBmp(m_hWnd, outFileName); }
BOOL CLShowTool::exportBmp(LPCTSTR outFileName) const { return windowsToBmp(m_hWnd, outFileName); }
//CLRect simpleRect(0, 0, 750, 350);
//const CLRect CLShowTool::setGlobleSimpleWindowsRect(const CLRect* newRect)
//{
//	if (newRect == NULL)
//		return simpleRect;
//	CLRect bk = simpleRect;
//	simpleRect = *newRect;
//	return bk;
//}

BOOL CLShowTool::isInCurrentThread() const
{
	return (!isWindow()) || GetWindowThreadProcessId(m_hWnd,nullptr) == GetCurrentThreadId();
}

CLShowTool* CLShowTool::getWnd(HWND hWnd)
{
#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
	return GetMapManager().GetValueFromMap(hWnd);
#else
	return (CLShowTool*)GetWindowLongPtr(hWnd, CLST_USE_EXTRADATA_TO_SAVE_DLGPTR);
#endif
}

void CLShowTool::OnMenuItem_BITMAPOUT()
{

	// 	CLShowTool st(getSafeHwnd());
	// 	st.createWindow();
	// 	return;

	CLRect rect;
	::GetWindowRect(*this, (RECT*)&rect);
	HDC hs;
	HDC hp;
	HBITMAP hbitmap;
	//�����豸������
	hs = ::GetWindowDC(getSafeHwnd());
	//�������ݵ��豸������
	hp = CreateCompatibleDC(hs);
	// �������豸��������ݵ�λͼ
	hbitmap = CreateCompatibleBitmap(hs, rect.Width(), rect.Height());
	// ��λͼѡ���豸��������
	SelectObject(hp, hbitmap);
	// ���豸�����������ڴ��豸��������
	BitBlt(hp, 0, 0, rect.Width(), rect.Height(), hs, 0, 0, SRCCOPY);
	DeleteDC(hs);
	DeleteDC(hp);

	HDC hdc = 0;
	DWORD dwbmbitssize, dwdibsize;
	BITMAP bitmap; //λͼ���Խṹ
	BITMAPFILEHEADER bmfhdr; //λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER bi; //λͼ��Ϣͷ�ṹ
	LPBITMAPINFOHEADER lpbi; //ָ��λͼ��Ϣͷ�ṹ
	HANDLE hdib, hpal, holdpal = NULL;
	//����λͼ��Ϣͷ
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwbmbitssize = ((bitmap.bmWidth * 24 + 31) / 32) * 4 * bitmap.bmHeight;
	hdib = GlobalAlloc(GHND, dwbmbitssize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
	*lpbi = bi;
	// �����ɫ�� 
	hpal = GetStockObject(DEFAULT_PALETTE);
	if (hpal)
	{
		hdc = ::GetDC(NULL);
		holdpal = ::SelectPalette(hdc, (HPALETTE)hpal, false);
		::RealizePalette(hdc);
	}

	// ��ȡ�õ�ɫ�����µ�����ֵ
	::GetDIBits(hdc, hbitmap, 0, (UINT)bitmap.bmHeight, (LPSTR)lpbi +
		sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	// λͼ�ļ�ͷ
	bmfhdr.bfType = 0x4d42;
	dwdibsize = 54 + dwbmbitssize;
	bmfhdr.bfSize = dwdibsize;
	bmfhdr.bfReserved1 = 0;
	bmfhdr.bfReserved2 = 0;
	bmfhdr.bfOffBits = 54;
	// 	CFileDialog filedlg(FALSE,_T(""),_T("*.bmp"),OFN_OVERWRITEPROMPT,_T("BMP FILES(*.bmp)|*.bmp"));
	// 	CFile fl;
	// 	CFileException e;
	// 	if(filedlg.DoModal()==IDOK)
	// 	{
	// 		if(fl.Open(filedlg.GetPathName(),CFile::modeWrite|CFile::modeCreate,&e)){
	// 			fl.Write((void*)&bmfhdr,sizeof(BITMAPFILEHEADER));
	// 			fl.Write((void*)lpbi,dwdibsize);
	// 			fl.Close();
	// 			MessageBox(CLString(_T("�ɹ�����ͼ���ļ�:\n"),(LPCTSTR)(filedlg.GetPathName()),0).string(),_T("��ȷ��ʾ"),MB_ICONINFORMATION);
	// 		}else MessageBox(CLString(_T("����ͼ���ļ���ʧ��:\n"),(LPCTSTR)(filedlg.GetPathName()),0).string(),_T("������ʾ"),MB_ICONERROR);
	// 	}


#define		MAX_FILE_FOR_SEL 20	//�������ѡ����ļ���
	_tsetlocale(LC_CTYPE, _T(""));//��wprintf ֧������
	TCHAR szPathName[MAX_PATH * MAX_FILE_FOR_SEL];
	OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };//or  {sizeof (OPENFILENAME)}
	// lStructSize
	// ָ������ṹ�Ĵ�С�����ֽ�Ϊ��λ��
	// Windows 95/98��Windows NT 4.0������ΪWindows 95/98��Windows NT 4.0��������WINVER��_WIN32_WINNT >= 0x0500����ʱ��
	//	Ϊ�����Աʹ��OPENFILENAME_SIZE_VERSION_400��
	// Windows 2000�����߰汾���������ʹ��sizeof (OPENFILENAME) ��
	ofn.hwndOwner = GetForegroundWindow();// ��OR�����ļ��Ի���ĸ�����
	ofn.lpstrFilter = _T("BMP FILES(*.bmp)\0*.bmp\0\0");
	//������ ���Ϊ NULL ��ʹ�ù�����
	//�����÷�������  ע�� /0
	CLString _str;
	GetWindowText(getSafeHwnd(), _str.store(), MAX_PATH);
	_str += _T(".bmp");
	lstrcpy(szPathName, _str);
	ofn.lpstrFile = szPathName;
	ofn.nMaxFile = sizeof(szPathName);//����û�ѡ���ļ��� ·�����ļ��� ������

// 	TCHAR szCurDir[MAX_PATH];
// 	GetCurrentDirectory(sizeof(szCurDir),szCurDir);
// 	ofn.lpstrInitialDir = szCurDir;//���öԻ�����ʾ�ĳ�ʼĿ¼
	CLString szDesktop;
	szDesktop.getSpecialFolderPath();
	ofn.lpstrInitialDir = szDesktop.string();//���öԻ�����ʾ�ĳ�ʼĿ¼
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;//�����Ҫѡ�����ļ� ��������  OFN_ALLOWMULTISELECT��־
	//_tprintf(_T("select file/n"));
	BOOL bOk;

	// 	ofn.lpstrTitle = _T("ѡ���ļ�");//ѡ���ļ��Ի������
	// 	bOk = GetOpenFileName(&ofn);//���öԻ�����ļ�
	// 	if (bOk)showFile(szPathName);

	ofn.lpstrTitle = _T("ѡ�񱣴��ļ���λ��");//ѡ���ļ��Ի������
	bOk = GetSaveFileName(&ofn);//���öԻ��򱣴��ļ�
	if (bOk) {
		_str = szPathName;
		if (_str.find(_T(".bmp")) == -1)
			_str += _T(".bmp");
		FILE* fl = NULL;
		if (0 == _tfopen_s(&fl, _str.string(), _T("wb"))) {
			fwrite((void*)&bmfhdr, sizeof(BITMAPFILEHEADER), 1, fl);
			fwrite((void*)lpbi, dwdibsize, 1, fl);
			fclose(fl);
			::MessageBox(getSafeHwnd(), CLString(_T("�ɹ�����ͼ���ļ�:\n"), (LPCTSTR)(szPathName), 0).string(), _T("��ȷ��ʾ"), MB_ICONINFORMATION);
		}
		else ::MessageBox(getSafeHwnd(), CLString(_T("����ͼ���ļ���ʧ��:\n"), (LPCTSTR)(szPathName), 0).string(), _T("������ʾ"), MB_ICONERROR);
	}

	//�������ʹ�� GetOpenFileName��ѡ���ļ��� ���Բο� http://blog.csdn.net/norsd/archive/2008/12/08/3476606.aspx

	//������������ shell32.dll api   ��������ļ��жԻ���	
// 	BROWSEINFO bInfo={0};
// 	bInfo.hwndOwner = getSafeHwnd();//������
// 	bInfo.lpszTitle=_T("����Ϊ");
// 	bInfo.ulFlags=BIF_RETURNONLYFSDIRS |BIF_USENEWUI/*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/|
// 		BIF_UAHINT/*��TIPS��ʾ*/ |BIF_NONEWFOLDERBUTTON /*�����½��ļ��а�ť*/;
// 	//���ڸ���� ulFlags �ο� http://msdn.microsoft.com/en-us/library/bb773205(v=vs.85).aspx
// 	LPITEMIDLIST lpDlist;
// 	lpDlist=SHBrowseForFolder(&bInfo);
// 	if (lpDlist!=NULL)//������ȷ����ť
// 	{
// 		SHGetPathFromIDList(lpDlist,szPathName);
// 		//_tprintf(_T("Select path %s/n"),szPathName);
// 	}
	::GlobalUnlock(hdib);
	::GlobalFree(hdib);
}

void CLShowTool::OnContextMenu(CLPoint point)
{
	HMENU hPopMenu = ::CreatePopupMenu();
	if (hPopMenu == NULL || hPopMenu == INVALID_HANDLE_VALUE) {
		DWORD le = GetLastError();
		CLString().getLastErrorMessageBoxExceptSucceed(le);
		return;
	}
	::AppendMenu(hPopMenu, MF_STRING, ID_CLST_BT_OUTBITMAPOUT, _T("�����.bmp�ļ�"));
	::AppendMenu(hPopMenu, MF_SEPARATOR, 0, 0);
	::AppendMenu(hPopMenu, MF_STRING, ID_CLST_BT_ABOUT, _T("�����ڡ��ؼ� ..."));
	::TrackPopupMenu(hPopMenu, TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, 0, this->getSafeHwnd(), NULL);
	::DestroyMenu(hPopMenu);
}

void CLShowTool::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//OldClass::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CLShowTool::OnMouseWheel(UINT nFlags, SHORT zDelta, CLPoint pt)
{
	//nFlags ���ص�ǰͬʱ��������ʱ��������Щ��
	//zDelta ����һ�������Ĵ�С,һ����120�ı���,�������������ǰ,�����������
	if (m_type == SHOWGRAPHER_TYPE_BITMAP) {
		const double step = 1.08;//���ű���
		getClientRect(&clientRect);
		if (zDelta > 0) {
			m_strechRat /= step;
		}
		else if (zDelta < 0) {
			m_strechRat *= step;
		}
		long detXb = clientRect.Width() * m_strechRat - m_strRect.Width();
		long detYb = clientRect.Height() * m_strechRat - m_strRect.Height();

		double Rx = double(pt.x) / clientRect.Width();
		double Ry = double(pt.y) / clientRect.Height();

		m_strRect.left = m_strRect.left - Rx * detXb;
		if (m_strRect.Width() > bitmap.bmWidth)
			m_strRect.left = 0;
		m_strRect.right = m_strRect.left + clientRect.Width() * m_strechRat;
		m_strRect.top = m_strRect.top - Ry * detYb;
		if (m_strRect.Height() > bitmap.bmHeight)
			m_strRect.top = 0;
		m_strRect.bottom = m_strRect.top + clientRect.Height() * m_strechRat;
		if (m_strRect.top < 0) {
			m_strRect.bottom += abs(m_strRect.top);
			m_strRect.top = 0;
		}
		if (m_strRect.left < 0) {
			m_strRect.right += abs(m_strRect.left);
			m_strRect.left = 0;
		}
		drawGrapher(m_memdc);
	}
	return TRUE;
}

BOOL CLShowTool::addSubTool(CLShowTool* pSub, ...)
{
	releaseSubWndLst();
	CLShowTool* p = pSub;
	if (!p)return FALSE;
	va_list ap;
	va_start(ap, pSub);
	for (; p; p = va_arg(ap, CLShowTool*))
		m_subWndLst.push_back(p);
	va_end(ap);
	return TRUE;
}

void CLShowTool::releaseSubWndLst()
{
	for (size_t i = 0; i < m_subWndLst.size(); i++) {
		CLShowTool* p = m_subWndLst[i];
		if (p) {
			if (p->isWindow())
				p->destroyWindow();
			if (m_isIndependRun)//�ڶ����߳�ģʽ����Ҫ�����ͷţ�����������ڴ�
				delete p;
			m_subWndLst[i] = NULL;
		}
	}
	m_subWndLst.clear();
}

void CLShowTool::getBasePtX(OUT LONG& basePtX, OUT DOUBLE& baseX, IN LONG orgX, IN DOUBLE xLeft, IN DOUBLE xASpan)
{
	if (m_isXdata == 0) {
	fails:
		basePtX = orgX;
		baseX = (((DOUBLE)(orgX)-xLeft) / xASpan * (m_maxX - m_minX) + m_minX);
	}
	else {
		int i = 0;
		for (auto ie = m_mpUtHdr.cbegin(); ie != m_mpUtHdr.cend(); i++)
		{
			if (
				(ie->second.utType == STLT_LINE || ie->second.utType == STLT_VERLINE) &&
				(ie->second.exFlag & STEF_USE_AXIS_X_DATA)
				) {
				auto p = &(m_mplineLst[ie->first]);
				for (auto je = p->LineX.cbegin(); je != p->LineX.cend(); )
				{
					if (BETWEEN((double)orgX, je->second.LB, je->second.RT)) {
						basePtX = je->first;
						baseX = je->second.XV;
						return;
					}
					++je;
				}
			}
			++ie;
		}
		goto fails;
	}
	return;
}

void CLShowTool::showSubLstWnd()
{
	for (size_t i = 0; i < m_subWndLst.size(); i++)
	{
		CLShowTool* p = m_subWndLst[i];
		if (p) {
			if (!p->isWindow()) {
				if (m_isIndependRun)
					p->m_isIndependRun = TRUE;//ָ��������Ϊ�������б������ǳ���Ҫ
				p->createWindow(getSubLstShowAsSubWnd() ? getSafeHwnd() : NULL);
			}
			else
				p->showWindow();
		}
	}
}


BOOL CLShowTool::writeSimpleLine(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle, LONG X, LONG Y, DOUBLE minx, DOUBLE maxx, COLORREF lineClr, LONG AxisTextDataAccuracy)
{
	assert(vlst != NULL);
	if (nCounts == 0)return ST_FALSE;
	auto simpleRect = getDefaultSimpleLineRect();
	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, simpleRect.Width(), simpleRect.Height());
	hdr.type = SHOWGRAPHER_TYPE_LINES;
	hdr.addOneLineSize(nCounts, 1).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	hdr.addAxisFlag = STLT_ADDAXIS_X | STLT_ADDAXIS_Y;
	hdr.AxisTextDataAccuracy = AxisTextDataAccuracy;
	writeHeader(hdr);

	STUTHEADER ldrQ;//ldrQ,p->q
	ldrQ.dataCounts = nCounts;
	ldrQ.utType = STLT_LINE;
	ldrQ.clr = lineClr;
	ldrQ.exClr = CLYELLOW;
	ldrQ.minX = minx;
	ldrQ.maxX = maxx;
	writeLine(ldrQ, vlst);
	if (isWindow())setWindowText(szTitle);//�����첽�������ݸò��߱�����writeData֮��
	return TRUE;
}

BOOL CLShowTool::writeSimpleLine2D(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle, LONG X, LONG Y, DOUBLE minx, DOUBLE maxx, COLORREF lineClr, LONG AxisTextDataAccuracy)
{
	assert(vlst != NULL);
	if (nCounts == 0)return ST_FALSE;

	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, getDefSimLineCX(), getDefSimLineCY());
	hdr.type = SHOWGRAPHER_TYPE_LINES;
	hdr.addOneLineSize(nCounts, 1).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	hdr.addAxisFlag = STLT_ADDAXIS_X | STLT_ADDAXIS_Y;
	hdr.AxisTextDataAccuracy = AxisTextDataAccuracy;
	writeHeader(hdr);

	STUTHEADER ldrQ;//ldrQ,p->q
	ldrQ.dataCounts = nCounts;
	ldrQ.utType = STLT_LINE;
	ldrQ.clr = lineClr;
	ldrQ.exClr = CLYELLOW;
	ldrQ.minX = minx;
	ldrQ.maxX = maxx;
	ldrQ.exFlag |= STEF_USE_AXIS_X_DATA;
	writeLine(ldrQ, vlst);
	if (isWindow())setWindowText(szTitle);//�����첽�������ݸò��߱�����writeData֮��
	return TRUE;
}
BOOL CLShowTool::writeSimpleLine2D(VD_RC vdv, VD_RC vdt, LPCTSTR szTitle /*= _T("Simple Line")*/, LONG X /*= 1*/, LONG Y /*= 1*/, DOUBLE minx /*= 0*/, DOUBLE maxx /*= 100*/, COLORREF lineClr /*= CLYELLOW*/, LONG AxisTextDataAccuracy /*= 3*/)
{
	assert(vdv.size() != 0 && vdv.size() <= vdt.size());
	size_t nCounts = vdv.size();
	std::vector<DOUBLE> line(nCounts * 2, 0);
	DOUBLE* vlst = line.data();
	for (size_t i = 0; i < nCounts; i++)
		vlst[i] = vdv[i];
	for (size_t i = nCounts; i < 2 * nCounts; i++)
		vlst[i] = vdt[i - nCounts];
	BOOL rt = writeSimpleLine2D(vlst, nCounts * 2, szTitle, X, Y, minx, maxx, lineClr, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}
BOOL CLShowTool::writeSimpleLine2D(const std::map<VT, VT>& vd, LPCTSTR szTitle /*= _T("Simple Line")*/, LONG X /*= 1*/, LONG Y /*= 1*/, DOUBLE minx /*= 0*/, DOUBLE maxx /*= 100*/, COLORREF lineClr /*= CLYELLOW*/, LONG AxisTextDataAccuracy /*= 3*/)
{
	assert(vd.size() != 0);
	size_t nCounts = vd.size();
	std::vector<DOUBLE> line(nCounts * 2, 0);
	DOUBLE* vlst = line.data();
	size_t ic = 0;
	for (auto i = vd.cbegin(); i != vd.cend(); )
	{
		vlst[ic] = i->second;
		vlst[ic + nCounts] = i->first;
		++i;
		++ic;
	}
	BOOL rt = writeSimpleLine2D(vlst, nCounts * 2, szTitle, X, Y, minx, maxx, lineClr, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}

BOOL CLShowTool::writeSimpleLine(VD_RC vd, LPCTSTR szTitle, LONG X, LONG Y, DOUBLE minx, DOUBLE maxx, COLORREF lineClr, LONG AxisTextDataAccuracy)
{
	assert(vd.size() != 0);
	BOOL rt = writeSimpleLine(&vd[0], vd.size(), szTitle, X, Y, minx, maxx, lineClr, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}

BOOL CLShowTool::writeSimpleVerLine(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle, LONG X, LONG Y, DOUBLE minx, DOUBLE maxx, 
	BOOL isAddTopText, COLORREF lineClr, COLORREF lineClrEx, LONG AxisTextDataAccuracy)
{
	assert(vlst != NULL);
	if (nCounts == 0)return ST_FALSE;

	auto simpleRect = getDefaultSimpleLineRect();
	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, simpleRect.Width(), simpleRect.Height());
	hdr.type = SHOWGRAPHER_TYPE_LINES;
	hdr.addOneLineSize(nCounts, 1).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	hdr.addAxisFlag = STLT_ADDAXIS_X | STLT_ADDAXIS_Y;
	hdr.AxisTextDataAccuracy = AxisTextDataAccuracy;
	writeHeader(hdr);

	STUTHEADER ldrQ;//ldrQ,p->q
	ldrQ.dataCounts = nCounts;
	ldrQ.utType = STLT_VERLINE;
	ldrQ.exFlag = (isAddTopText ? STEF_ADDTOPTEXT : 0) | STEF_VERLINEUSEDBCOLOR;
	ldrQ.clr = lineClr;
	ldrQ.exClr = lineClrEx;
	ldrQ.minX = minx;
	ldrQ.maxX = maxx;
	writeLine(ldrQ, vlst);
	if (isWindow())setWindowText(szTitle);
	return TRUE;
}

BOOL CLShowTool::writeSimpleVerLine(VD_RC vd, LPCTSTR szTitle /*= _T("Simple VerLine")*/, LONG X /*= 1*/, LONG Y /*= 1*/, DOUBLE minx /*= 0*/, DOUBLE maxx /*= 100*/,
	BOOL isAddTopText /*= FALSE*/, COLORREF lineClr /*= CLORANGE*/, COLORREF lineClrEx /*= CLYOUNG*/, LONG AxisTextDataAccuracy /*= 3*/)
{
	assert(vd.size() != 0);
	size_t nCounts = vd.size();
	std::vector<DOUBLE> line(nCounts * 2, 0);
	DOUBLE* vlst = line.data();
	for (size_t i = 0; i < nCounts; i++)
		vlst[i] = vd[i];
	BOOL rt = writeSimpleVerLine(vlst, nCounts, szTitle, X, Y, minx, maxx, isAddTopText, lineClr, lineClrEx, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}

BOOL CLShowTool::writeSimpleVerLine2D(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle, LONG X, LONG Y, DOUBLE minx, DOUBLE maxx, 
	BOOL isAddTopText /*= FALSE*/, COLORREF lineClr /*= CLORANGE*/, COLORREF lineClrEx /*= CLYOUNG*/, LONG AxisTextDataAccuracy)
{
	assert(vlst != NULL);
	if (nCounts == 0)return ST_FALSE;

	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, getDefSimLineCX(), getDefSimLineCY());
	hdr.type = SHOWGRAPHER_TYPE_LINES;
	hdr.addOneLineSize(nCounts, 1).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	hdr.addAxisFlag = STLT_ADDAXIS_X | STLT_ADDAXIS_Y;
	hdr.AxisTextDataAccuracy = AxisTextDataAccuracy;
	writeHeader(hdr);

	STUTHEADER ldrQ;//ldrQ,p->q
	ldrQ.dataCounts = nCounts;
	ldrQ.utType = STLT_VERLINE;
	ldrQ.exFlag = (isAddTopText ? STEF_ADDTOPTEXT : 0) | STEF_VERLINEUSEDBCOLOR;
	ldrQ.clr = lineClr;
	ldrQ.exClr = lineClrEx;
	ldrQ.minX = minx;
	ldrQ.maxX = maxx;
	ldrQ.exFlag |= STEF_USE_AXIS_X_DATA;
	writeLine(ldrQ, vlst);
	if (isWindow())setWindowText(szTitle);//�����첽�������ݸò��߱�����writeData֮��
	return TRUE;
}
BOOL CLShowTool::writeSimpleVerLine2D(VD_RC vdv, VD_RC vdt, LPCTSTR szTitle /*= _T("Simple Line")*/, LONG X /*= 1*/, LONG Y /*= 1*/, 
	DOUBLE minx /*= 0*/, DOUBLE maxx /*= 100*/, BOOL isAddTopText /*= FALSE*/, COLORREF lineClr /*= CLORANGE*/, COLORREF lineClrEx /*= CLYOUNG*/,
	LONG AxisTextDataAccuracy /*= 3*/)
{
	assert(vdv.size() != 0 && vdv.size() <= vdt.size());
	size_t nCounts = vdv.size();
	std::vector<DOUBLE> line(nCounts * 2, 0);
	DOUBLE* vlst = line.data();
	for (size_t i = 0; i < nCounts; i++)
		vlst[i] = vdv[i];
	for (size_t i = nCounts; i < 2 * nCounts; i++)
		vlst[i] = vdt[i - nCounts];
	BOOL rt = writeSimpleVerLine2D(vlst, nCounts * 2, szTitle, X, Y, minx, maxx, isAddTopText, lineClr, lineClrEx, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}
BOOL CLShowTool::writeSimpleVerLine2D(const std::map<VT, VT>& vd, LPCTSTR szTitle /*= _T("Simple Line")*/, LONG X /*= 1*/, LONG Y /*= 1*/,
	DOUBLE minx /*= 0*/, DOUBLE maxx /*= 100*/, BOOL isAddTopText /*= FALSE*/, COLORREF lineClr /*= CLORANGE*/,
	COLORREF lineClrEx /*= CLYOUNG*/, LONG AxisTextDataAccuracy /*= 3*/)
{
	assert(vd.size() != 0);
	size_t nCounts = vd.size();
	std::vector<DOUBLE> line(nCounts * 2, 0);
	DOUBLE* vlst = line.data();
	size_t ic = 0;
	for (auto i = vd.cbegin(); i != vd.cend(); )
	{
		vlst[ic] = i->second;
		vlst[ic + nCounts] = i->first;
		++i;
		++ic;
	}
	BOOL rt = writeSimpleVerLine2D(vlst, nCounts * 2, szTitle, X, Y, minx, maxx, isAddTopText, lineClr, lineClrEx, AxisTextDataAccuracy);
	if (isWindow())setWindowText(szTitle);
	return rt;
}


BOOL CLShowTool::writeSimpleText(LPCTSTR vlst, LPCTSTR szTitle, LONG X, LONG Y, COLORREF textClr)
{
	size_t nCounts = _tcslen(vlst);
	auto simpleRect = getDefaultSimpleLineRect();
	SHOWGRAPHERHEARDER hdr;
	hdr.setWndPos(X, Y, simpleRect.Width(), simpleRect.Height());
	hdr.type = SHOWGRAPHER_TYPE_LINES;
	hdr.addOneTextSize(nCounts).writeTime = CLTime::getLocalTime_ll();
	hdr.setTitle(szTitle);
	writeHeader(hdr);
	STUTHEADER ldrSLineT;//ldrSLineT,m_szTmp
	ldrSLineT.utType = STLT_TEXT;
	ldrSLineT.dataCounts = nCounts;
	ldrSLineT.clr = textClr;
	writeText(ldrSLineT, vlst);
	if (isWindow())setWindowText(szTitle);//�����첽�������ݸò��߱�����writeData֮��
	return TRUE;
}

CLRect CLShowTool::getDefaultSimpleLineRect()
{
	return g_stSimpleLineDefaultRec;
}

CLRect CLShowTool::setDefaultSimpleLineRect(const CLRect& newRect)
{
	auto bk = getDefaultSimpleLineRect();
	g_stSimpleLineDefaultRec = newRect;
	return bk;
}

CLRect CLShowTool::getDefaultKLineRect()
{
	return g_stKlineDefaultRec;
}

CLRect CLShowTool::setDefaultKLineRect(const CLRect& newRect)
{
	auto bk = getDefaultKLineRect();
	g_stKlineDefaultRec = newRect;
	return bk;
}

LONG CLShowTool::getDefSimLineCX()
{
	return g_stSimpleLineDefaultRec.Width();
}

LONG CLShowTool::getDefSimLineCY()
{
	return g_stSimpleLineDefaultRec.Height();
}

LONG CLShowTool::getDefKLineCX()
{
	return g_stKlineDefaultRec.Width();
}

LONG CLShowTool::getDefKLineCY()
{
	return g_stKlineDefaultRec.Height();
}

BOOL CLShowTool::writeSimpleKLine(const DAYDATASETS1& rtData, LPCTSTR szTitle, LONG X, LONG Y)
{
	size_t _maxlen = rtData.vtData.size();
	if (_maxlen == 0)return ST_FALSE;
	DAYDATASETS_UNIT1* pt = new DAYDATASETS_UNIT1[_maxlen];
	for (LONG i = 0; i < rtData.nDataCounts; i++)pt[i] = rtData.vtData.at(i);
	SHOWGRAPHERHEARDER header;
	header.type = SHOWGRAPHER_TYPE_SHOWKLINE;
	header.addKLineSize(_maxlen).writeTime = CLTime::getLocalTime_ll();
	header.setWndPos(X, Y, getDefKLineCX(), getDefKLineCY());
	header.setTitle(szTitle);
	KLINEDATAHEADER klineheader(rtData.nStockNumber, _maxlen, rtData.szStockName);
	BOOL ret = writeKLine(header, klineheader, (const DAYDATASETS_UNIT1*)pt);
	delete[] pt;
	if (isWindow())setWindowText(szTitle);
	return ret;
}

CLShowTool* CLShowTool::copyToShowInNewThread(LPDWORD _out_threadId)
{
	enterPermission();
	CLShowTool* pNewObj = new CLShowTool;
	if (!pNewObj->attach(*this)) {
		if (_out_threadId)*_out_threadId = 0;
		delete pNewObj;
		leavePermission();
		return NULL;
	}
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	DWORD nId = 0;
	HANDLE hTrd = ::CreateThread(&sa, 0, &CLShowTool::IndependentThreadProc, (LPVOID)pNewObj, 0, &nId);
	//Sleep(1);
	if (_out_threadId)
		*_out_threadId = nId;
	if (hTrd != NULL && hTrd != INVALID_HANDLE_VALUE) {
		::CloseHandle(hTrd);
		leavePermission();
		while (pNewObj->m_hWnd == 0)
			Sleep(0);
		return pNewObj;
	}
	else {
		DWORD le = GetLastError();
		CLString().getLastErrorMessageBoxExceptSucceed(le);
		delete pNewObj;
		leavePermission();
		return NULL;
	}
}

CLShowTool* CLShowTool::createAndShowInNewThread(LPDWORD _out_threadId, CLShowTool* _pNewObj, BOOL isWaitBeforeCreateWnd)
{
	CLShowTool* pNewObj = _pNewObj ? _pNewObj : new CLShowTool;
	if (pNewObj->isWindow()) {
		::MessageBox(NULL, _T("���������в���ʾ���޷������ڶ����߳��д�����"), _T("CLShowTool showInNewThread error"), MB_ICONERROR | MB_OK);
		if (!_pNewObj)
			delete pNewObj;
		return NULL;
	}
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
	DWORD nId = 0;
	HANDLE hTrd = ::CreateThread(&sa, 0, &CLShowTool::IndependentThreadProc, (LPVOID)pNewObj, 0, &nId);
	Sleep(0);
	if (_out_threadId)*_out_threadId = nId;
	if (hTrd != NULL && hTrd != INVALID_HANDLE_VALUE) {
		if (isWaitBeforeCreateWnd) {
			//while (!pNewObj->isWindow())
			while (pNewObj->m_hWnd == 0)
				Sleep(0);
		}
		::CloseHandle(hTrd);
		return pNewObj;
	}
	else {
		DWORD le = GetLastError();
		CLString().getLastErrorMessageBoxExceptSucceed(le);
		if (!_pNewObj)
			delete pNewObj;
		return NULL;
	}
}


//�ú����������������Ӵ��ڷ�ʽ��ʾ��������NULLΪ��������ʾ��isSub = false ��ʾ������NULLΪ������


//�ö���������һ����ȫ���������߳��У������¶���ָ�롣
//�����ɶ����̸߳����ͷţ���Ӧ��delete����ָ�룬����destroyWindows�������Ĵ����ڹر��Ժ��߳̽��˳��������ͷŶ��󣬶���ָ�뽫���ٿ��á�
//ע�⣺��ҪĿ��������߳̽���ʱ���Զ��ͷţ�������setAutoDeleteInNewThread����ΪTRUE��
//���ض���ָ�롣��������ʧ�ܷ���NULL

CLShowTool* CLShowTool::showInNewThread(LPDWORD _out_threadId) {
	return createAndShowInNewThread(_out_threadId, this);
}

void CLShowTool::setSubLstShowAsSubWnd(BOOL isSub) { m_isSubLstShowAsSubWnd = isSub; }

BOOL CLShowTool::getSubLstShowAsSubWnd() const { return m_isSubLstShowAsSubWnd; }

CLShowTool::operator HWND() const { return getSafeHwnd(); }

HWND CLShowTool::getSafeHwnd() const { return m_hWnd; }

BOOL CLShowTool::getClientRect(CLRect* lpRect) const { return ::GetClientRect(m_hWnd, (LPRECT)lpRect); }

BOOL CLShowTool::updateWindow() const { return ::UpdateWindow(m_hWnd); }

BOOL CLShowTool::invalidate(BOOL bErase) const {
	return ::InvalidateRect(m_hWnd, NULL, bErase) ; }

BOOL CLShowTool::invalidateRect(CLRect* lpRect, BOOL bErase) const {
	return::InvalidateRect(m_hWnd, (LPCRECT)lpRect, bErase); }

BOOL CLShowTool::setWindowPos(HWND hWndInsertAfter, LONG X, LONG Y, LONG cx, LONG cy, UINT uFlags) const {
	return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, (uFlags |= SWP_ASYNCWINDOWPOS));
}

BOOL CLShowTool::setWindowText(LPCTSTR lpString) const { 
	return isInCurrentThread() ? ::SetWindowText(m_hWnd, lpString) : postMessage(WM_SETTEXT,0,(LPARAM)lpString); }

LONG CLShowTool::showWindow(LONG nCmdShow) const { 
	return isInCurrentThread() ? ::ShowWindow(m_hWnd, nCmdShow) : postMessage(WM_SHOWWINDOW,nCmdShow); }

BOOL CLShowTool::destroyWindow() const { 
	return isInCurrentThread() ? ::DestroyWindow(m_hWnd) : postMessage(WM_DESTROY); }

BOOL CLShowTool::isWindow() const { return ::IsWindow(m_hWnd); }

BOOL CLShowTool::isWindowVisible() const { return ::IsWindowVisible(m_hWnd); }

BOOL CLShowTool::postMessage(UINT msg, WPARAM wp, LPARAM lp) const { return ::PostMessage(m_hWnd, msg, wp, lp); }

BOOL CLShowTool::attach(const CLShowTool& obj)
{
	//��Ҫ��ǿ����պ�����
	enterPermission();
	release();
	setSubLstShowAsSubWnd(obj.getSubLstShowAsSubWnd());
	m_hWndParent = obj.m_hWndParent;
	//�����Լ��������ܵ������ҿ�������
	if (obj.hRead && !obj.isPipeEmpty()) {//�����йܵ���������
		if (!hRead || !hWrite) {
			SECURITY_ATTRIBUTES sa;
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = NULL;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			if (!CreatePipe(&hRead, &hWrite, &sa, obj.nPipeSize)) {
				goto f1;
			}
			nPipeSize = obj.nPipeSize;
		}
		//������ǰ�ܵ��е�����
		if (!PeekNamedPipe(obj.hRead, 0, 0, 0, &dwTotal, &dwleft))return FALSE;
		DWORD cn = dwTotal;
		BYTE* buf = new BYTE[cn];
		if (!PeekNamedPipe(obj.hRead, buf, cn, &dwRead, &dwTotal, &dwleft)) { delete[] buf; return FALSE; }
		if (!WriteFile(hWrite, buf, cn, &dwWrite, NULL)) { delete[] buf; return FALSE; }
		delete[] buf;
	}
	//������������������
	for (size_t i = 0; i < obj.m_subWndLst.size(); i++)
	{
		CLShowTool* p = new CLShowTool;
		p->attach(*(obj.m_subWndLst[i]));
		m_subWndLst.push_back(p);
	}
	leavePermission();
	return TRUE;
f1:
	hRead = hWrite = 0;
	nPipeSize = 0;
	leavePermission();
	return FALSE;
}

void CLShowTool::init()
{
	szWindowClass = _T("WNDCLASS_CLShowTool");
	hInst = GetModuleHandle(NULL);
	m_hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(IDI_CLST_MAIN));
	m_hWnd = NULL;
	m_type = SHOWGRAPHER_TYPE_NO;
	hRead = hWrite = 0;
	nPipeSize = 0;
	pPt = 0;
	m_isInit = FALSE;
	m_isMainWnd = FALSE;
	szTitle = _T("CL Show Tool");
	m_isIndependRun = FALSE;
	pBitmapBuf = 0;
	pBitmapBufSi = 0;
	m_BtmapFileHdr = { 0 };
	setSubLstShowAsSubWnd(TRUE);
	setAutoDeleteInNewThread(TRUE);
}

inline LONG CLShowTool::messageBox(LPCTSTR lpString) const { return CLString(lpString).messageBox(_T("CLShowTool Alert"), MB_OK); }

inline HWND CLShowTool::create(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, LONG X, LONG Y, LONG nWidth, LONG nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	return m_hWnd = ::CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
