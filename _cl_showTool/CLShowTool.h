#pragma once
#ifndef __CL_CLSHOWTOOL_H__
#define __CL_CLSHOWTOOL_H__
// ShowGrapherDlg.h : ͷ�ļ�
//

#pragma once

#include "CLShowTool_PreDefine.h"
#include "../_cl_common/CLCommon.h"


#define CLST_USE_EXTRADATA_TO_SAVE_DLGPTR 0 //ָ�뱣��id,��ǰ����extradataӳ��ģʽ��ע�͵���һ�������mapӳ��

// CLShowTool �Ի���
class CLShowTool 
{
protected:
#define MAX_LOADSTRING 100

	// ȫ�ֱ���:
	HWND m_hWnd;
	BOOL m_isInit;
	HWND m_hWndParent;
	HINSTANCE hInst;								// ��ǰʵ��
	CLString szTitle;					            // �������ı�
	LPCTSTR szWindowClass;                   		// ����������
	BOOL m_isMainWnd;								//�Ƿ����߳�������
	BOOL m_isIndependRun;							//�Ƿ����Զ����߳�������
	BOOL m_isAutoDeleteSelfInNewThread;				//�Զ����߳�������ʱ���Ƿ����˳�ʱ�����ٶ�������������ⲿ����
	std::vector<CLShowTool*> m_subWndLst;//�Ӵ�����
	BOOL m_isSubLstShowAsSubWnd;

	// �Ի�������
	enum { IDD = IDD_CLST_MAIN };

	// �˴���ģ���а����ĺ�����ǰ������:
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE, LONG);
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//�ڶ����߳��д�����ʾ���ڣ��߳̽���ʱ������ٴ���ָ�����
	static DWORD WINAPI IndependentThreadProc(LPVOID _pCopyWnd);
	//��鲢�����ܵ�
	BOOL getPipeHandle(LONGLONG nPipeMaxBufInByte, HANDLE* hRead = 0, HANDLE* hWrite = 0);
	BOOL isPipeEmpty() const;
	BOOL writeData(LPCVOID buf, LONGLONG nDataBufSizeInByte, LONGLONG nPipeMaxBufInByte);
public:		
	//д����Ҫ��װ��ͷ
	BOOL writeHeader(const SHOWGRAPHERHEARDER& hdr);
	BOOL writeBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo);
	BOOL writeSimpleBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo, LPCTSTR szTitle, LONG X, LONG Y);
	BOOL writeLine(const STUTHEADER& lineHdr,const DOUBLE* pLine);
	//д��2D�������ݣ����Զ�����STEF_USE_AXIS_X_DATA��ǩ��lineHdr.dataCounts������������ݸ���
	BOOL writeLine2(const STUTHEADER& lineHdr,const DOUBLE* pLineV, const DOUBLE* pLineX);
	template<typename T> 
	BOOL writeLine2(const STUTHEADER& lineHdr, const DOUBLE* pLineV, const std::vector<T>& pLineX,size_t startIndex = 0) {
		if (pLineV == 0 || lineHdr.dataCounts == 0 || startIndex + lineHdr.dataCounts/2 > pLineX.size())
			return throw std::logic_error("writeLine2 param is invalide!"),FALSE;
		STUTHEADER hdr = lineHdr;
		hdr.exFlag |= STEF_USE_AXIS_X_DATA;
		//double *line2 = new double[hdr.dataCounts];
		vector<DOUBLE> line2(hdr.dataCounts);
		for (long i = 0; i < hdr.dataCounts / 2; i++)
		{
			line2[i] = pLineV[i];
		}
		for (long i = hdr.dataCounts / 2, j = startIndex; i < hdr.dataCounts; i++, j++)
		{
			line2[i] = (DOUBLE)pLineX[j];
		}
		BOOL rt = writeLine(hdr, line2.data());
		//delete[] line2;
		return rt;
	}
	template<typename T1, typename T2>
	BOOL writeLine2(const STUTHEADER& lineHdr, const std::vector<T1>& pLineV, const std::vector<T2>& pLineX, size_t vStartIndex = 0, size_t xStartIndex = 0) {
		if (lineHdr.dataCounts == 0 || vStartIndex + lineHdr.dataCounts / 2 > pLineV.size() || xStartIndex + lineHdr.dataCounts / 2 > pLineX.size())
			return throw std::logic_error("writeLine2 param is invalide!"), FALSE;
		STUTHEADER hdr = lineHdr;
		hdr.exFlag |= STEF_USE_AXIS_X_DATA;
		//double *line2 = new double[hdr.dataCounts];
		vector<DOUBLE> line2(hdr.dataCounts);
		for (long i = 0,j = vStartIndex; i < hdr.dataCounts / 2; i++)
		{
			line2[i] = (DOUBLE)pLineV[i];
		}
		for (long i = hdr.dataCounts / 2, j = xStartIndex; i < hdr.dataCounts; i++, j++)
		{
			line2[i] = (DOUBLE)pLineX[j];
		}
		BOOL rt = writeLine(hdr, line2.data());
		//delete[] line2;
		return rt;
	}
	template<typename T1>
	BOOL writeLine2(const STUTHEADER& lineHdr, const std::vector<T1>& pLineV, const DOUBLE* pLineX, size_t startIndex = 0) {
		if (lineHdr.dataCounts == 0 || startIndex + lineHdr.dataCounts / 2 > pLineV.size())
			return throw std::logic_error("writeLine2 param is invalide!"), FALSE;
		STUTHEADER hdr = lineHdr;
		hdr.exFlag |= STEF_USE_AXIS_X_DATA;
		//double *line2 = new double[hdr.dataCounts];
		vector<double> line2(hdr.dataCounts);
		for (size_t i = 0,j= startIndex; i < hdr.dataCounts / 2; i++,j++)
		{
			line2[i] = (double)pLineV[j];
		}
		for (size_t i = hdr.dataCounts / 2, j = 0; i < hdr.dataCounts; i++, j++)
		{
			line2[i] = pLineX[j];
		}
		BOOL rt = writeLine(hdr, line2.data());
		//delete[] line2;
		return rt;
	}
	BOOL writeText(const STUTHEADER& textHdr,const TCHAR* pText);
	BOOL writeKLine(const SHOWGRAPHERHEARDER& hdr,const KLINEDATAHEADER& KlineHdr,const DAYDATASETS_UNIT1* pLine);
	BOOL display(BOOL forceShow = FALSE);

//ֱ����ʾ��ͼ��
	BOOL writeSimpleLine(VD_RC vd,LPCTSTR szTitle = _T("Simple Line"),LONG X = 1,LONG Y = 1,DOUBLE minx = 0,DOUBLE maxx = 100,
		COLORREF lineClr = CLYELLOW,LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleLine(const DOUBLE* vlst,size_t nCounts, LPCTSTR szTitle = _T("Simple Line"),LONG X = 1,LONG Y = 1,DOUBLE minx = 0,DOUBLE maxx = 100,
		COLORREF lineClr = CLYELLOW,LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleLine2D(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle = _T("Simple Line"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		COLORREF lineClr = CLYELLOW, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleLine2D(VD_RC vdv, VD_RC vdt,  LPCTSTR szTitle = _T("Simple Line"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		COLORREF lineClr = CLYELLOW, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleLine2D(const std::map<VT,VT>& vd, LPCTSTR szTitle = _T("Simple Line"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		COLORREF lineClr = CLYELLOW, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleVerLine(const DOUBLE* vlst,size_t nCounts, LPCTSTR szTitle = _T("Simple VerLine"),LONG X = 1,LONG Y = 1,DOUBLE minx = 0,DOUBLE maxx = 100,
		BOOL isAddTopText = FALSE,COLORREF lineClr = CLORANGE,COLORREF lineClrEx = CLYOUNG,LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleVerLine(VD_RC vd,LPCTSTR szTitle = _T("Simple VerLine"),LONG X = 1,LONG Y = 1,DOUBLE minx = 0,DOUBLE maxx = 100,
		BOOL isAddTopText = FALSE,COLORREF lineClr = CLORANGE,COLORREF lineClrEx = CLYOUNG,LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleVerLine2D(const DOUBLE* vlst, size_t nCounts, LPCTSTR szTitle = _T("Simple VerLine"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		BOOL isAddTopText = FALSE, COLORREF lineClr = CLORANGE, COLORREF lineClrEx = CLYOUNG, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleVerLine2D(VD_RC vdv, VD_RC vdt, LPCTSTR szTitle = _T("Simple VerLine"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		BOOL isAddTopText = FALSE, COLORREF lineClr = CLORANGE, COLORREF lineClrEx = CLYOUNG, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleVerLine2D(const std::map<VT, VT>& vd, LPCTSTR szTitle = _T("Simple VerLine"), LONG X = 1, LONG Y = 1, DOUBLE minx = 0, DOUBLE maxx = 100,
		BOOL isAddTopText = FALSE, COLORREF lineClr = CLORANGE, COLORREF lineClrEx = CLYOUNG, LONG AxisTextDataAccuracy = 3);
	BOOL writeSimpleKLine(const DAYDATASETS1& rtData,LPCTSTR szTitle = _T("Simple Kline"),LONG X = 1,LONG Y = 1);
	BOOL writeSimpleText(LPCTSTR vlst,LPCTSTR szTitle = _T("Simple Text"),LONG X = 1,LONG Y = 1,COLORREF textClr = CLGOLD);
	//���ٵ���ʾ���ݷֲ����
	//pData ������, nCounts ���ݸ���, sectionCounts�ֲ�������
	template<class _Ty> 
	CLShowTool& writeSimpleDataDistribution(const _Ty* pData, size_t nCounts, size_t sectionCounts,
		LPCTSTR szTitle = _T("Simple Data Distribution"), LONG X = 1, LONG Y = 1, 
		BOOL isAddTopText = FALSE, COLORREF lineClr = CLORANGE, COLORREF lineClrEx = CLYELLOW, LONG AxisTextDataAccuracy = 3) {
		if (!pData || nCounts == 0)
			throw runtime_error("writeSimpleDataDistribution: org data is not exist!");
		std::map<DOUBLE, DOUBLE> data;
		DOUBLE vmax = pData[0], vmin = pData[0], v;
		for (size_t k = 0; k < nCounts; k++) {
			v = pData[k];
			if (v > vmax)vmax = v;
			if (v < vmin)vmin = v;
		}
		DOUBLE sec = DOUBLE(vmax - vmin) / max(1, sectionCounts);
		for (size_t k = 0;k < nCounts;++k)
		{
			v = pData[k];
			if (v >= vmax) v -= sec;
			auto x = (DOUBLE(size_t(DOUBLE(v - vmin) / sec)) + 0.5) * sec + vmin;
			data[x] += 1.0;
		}
		writeSimpleVerLine2D(data, szTitle, X, Y, vmin, vmax, isAddTopText, lineClr, lineClrEx, AxisTextDataAccuracy);
		return *this;
	}
	//���ٵ���ʾ���ݷֲ����
	//vd ����vector, sectionCounts�ֲ�������
	template<class _Ty> 
	CLShowTool& writeSimpleDataDistribution(const vector<_Ty>& vd,size_t sectionCounts,
		LPCTSTR szTitle = _T("Simple Data Distribution"), LONG X = 1, LONG Y = 1, 
		BOOL isAddTopText = FALSE, COLORREF lineClr = CLORANGE, COLORREF lineClrEx = CLYELLOW, LONG AxisTextDataAccuracy = 3) {
		return writeSimpleDataDistribution(vd.data(),vd.size(), sectionCounts,
			szTitle, X, Y, isAddTopText, lineClr, lineClrEx, AxisTextDataAccuracy);
	}
	
protected:
	LONG m_type;
	CLString szMaxV,szMidV,szMinV,szMaxN,szMidN,szMinN,m_szTmp,m_err;
	SHOWGRAPHERHEARDER m_pipeHeader;
	DWORD dwRead,dwWrite,dwTotal,dwleft;
	LONGLONG lastWriteTime,lastReadTime;
	DOUBLE m_minX,m_maxX,m_maxY,m_minY;
	LONG m_isXAxis,m_isYAxis;//��¼x y�ļ�¼λ�ñ��
	DOUBLE leftWD,bottonWD,offset;
	LONG m_isLD;
	CLPoint m_cpt;
	DOUBLE m_strechRat;
	CLRect m_strRect;
	CLPoint m_strPt,m_strPtN;
	size_t m_bitUpdateTimes;
	LONG m_isXdata;
	COLORREF m_gbkColor = RGB(0, 0, 0);

	std::map<LONG,LINEBUF> m_mplineLst;
	std::map<LONG,PCLString> m_mpTextLst;
	std::map<LONG,STUTHEADER> m_mpUtHdr;
	HANDLE hRead,hWrite;
	LONGLONG nPipeSize;

	//k������
	BOOL isEirstPaint;
	LONGLONG nSiStart,nSiEnd,nSiAll,nSiRi;//��ʼ�ͽ�������,��0��ʼ	
	KLINEDATAHEADER kLineHeader;
	DAYDATASETS_UNIT1* pPt;
	BITMAPFILEHEADER m_BtmapFileHdr;
	BITMAPINFOHEADER m_BtmapInfoHdr; //λͼ��Ϣͷ�ṹ
	BITMAPINFO* pBitmapInfo;
	byte* pBitmapBuf;
	size_t pBitmapBufSi;
	HBITMAP hBitmap;
	BITMAP bitmap;

	//ts�ֲ�����
	DOUBLE dis[TSDIS_COUNTS1];

	//������
	HANDLE hRunEvent;//ָ���ÿؼ��Ƿ��������߳���
	void enterPermission();
	void leavePermission();
protected:
#define LEFT_WIDE   getWide(1)
#define TOP_WIDE    getWide(2)
#define RIGHT_WIDE  getWide(3)
#define BOTTON_WIDE getWide(4)
	DOUBLE getWide(LONG pos);
	void release();
	void releaseSubWndLst();
	void getBasePtX(OUT LONG & basePtX, OUT DOUBLE & baseX, IN LONG orgX, IN DOUBLE xLeft, IN DOUBLE xASpan);
	void updateMaxYMinY(DOUBLE* line,STUTHEADER* i) throw(...);
 	void doExFlag(CLDC& dc,STUTHEADER& hdr,DOUBLE* line);
	void drawText(CLDC& dc,LPCTSTR str,LONG szlen,LONG x,LONG y,LONG alignFlag = 0);
	BOOL drawGrapher(CLDC& dc);

	void drawUtLine(CLDC& dc,STUTHEADER& hdr,DOUBLE* line, std::map<long, RAGE> *lx );
	void drawUtVerline(CLDC& dc,STUTHEADER& hdr,DOUBLE* line, std::map<long, RAGE> *lx );
	void drawUtText(CLDC& dc,STUTHEADER& hdr,PCLString str);
	void drawUtAxisX(CLDC& dc,STUTHEADER& hdr);
	void drawUtAxisY(CLDC& dc,STUTHEADER& hdr);
	static LONG getFitDataAccuracy(LONG curAcc, DOUBLE vmin, DOUBLE vmax);

	BOOL drawKLine(CLDC& dc,DAYDATASETS_UNIT1 pPt[],LONGLONG nSize);
	DOUBLE MaxV(DAYDATASETS_UNIT1 pPt[],LONGLONG nSize,LONGLONG nSiRi,
		LONGLONG nSiStart,LONGLONG nSiEnd);
	DOUBLE MinV(DAYDATASETS_UNIT1 pPt[],LONGLONG nSize,LONGLONG nSiRi,
		LONGLONG nSiStart,LONGLONG nSiEnd);
	DOUBLE MaxN(DAYDATASETS_UNIT1 pPt[],LONGLONG nSize,LONGLONG nSiRi,
		LONGLONG nSiStart,LONGLONG nSiEnd);
	BOOL drawOneKBox(const CLPOINT & spt, CLPOINT & sptn, DAYDATASETS_UNIT1 & rdu, 
		DOUBLE PWD, LONG PHV, LONG PHN, DOUBLE radio_vy, DOUBLE radio_ny, DOUBLE maxV, 
		DOUBLE minV, CLDC & dc);

	BOOL drawBitmap(CLDC& dc);

	LONG drawTsDistribution(CLDC& dc,DOUBLE* dis);

	// ʵ��
protected:
	HICON m_hIcon;	
	BOOL m_dataExsit;//����׼����ʶ���ڳ�ʼ��ʱ����Ϊ��

	CLRect clientRect;
	CLClientDC m_pdc;
	CLDC m_memdc;
 	CLBitmap m_bkBitmap;

// ��Ϣӳ�亯��
#define _cl_msg virtual
 	virtual BOOL OnInitDialog();
 	_cl_msg void OnPaint();
	_cl_msg HCURSOR OnQueryDragIcon();
	_cl_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	_cl_msg void OnDestroy();
	_cl_msg HBRUSH OnCtlColor(CLDC* pDC, CLShowTool* pWnd, UINT nCtlColor);
	_cl_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	_cl_msg void OnTimer(UINT_PTR nIDEvent);
	_cl_msg void OnNcDestroy();	
	_cl_msg void OnLButtonDown(UINT nFlags, CLPoint point);
	_cl_msg void OnLButtonUp(UINT nFlags, CLPoint point);
	_cl_msg void OnMouseMove(UINT nFlags, CLPoint point);
	_cl_msg void OnSizing(UINT fwSide, LPRECT pRect);
	_cl_msg void OnSize(UINT nType, LONG cx, LONG cy);
	_cl_msg void OnMenuItem_BITMAPOUT();//ID_MENUITEM1��Ϣ����
	_cl_msg void OnContextMenu(CLPoint point);//�Ҽ��˵���Ӧӳ�亯��
	_cl_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	_cl_msg BOOL OnMouseWheel(UINT nFlags, SHORT zDelta, CLPoint pt);
	
	void showSubLstWnd();
	BOOL attach(const CLShowTool& obj);
	void init();
	LONG messageBox(LPCTSTR lpString) const;
	HWND create(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		LONG X, LONG Y, LONG nWidth, LONG nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

 public: 
	 CLShowTool(HWND hParent = NULL);	// ��׼���캯��
	 CLShowTool(const CLShowTool& othre);	
	 CLShowTool& operator =(const CLShowTool& othre);
	 virtual ~CLShowTool();
	 //���������ڶ����߳�����ģʽ�µĶ���ָ�룬�Ƿ��ڶ����߳��˳������ٻ��Ǽ�����Ч��Ĭ����������Զ����ٶ���ġ�
	 //����ΪTRUE�󣬶����̴߳��ڹرպ������ٶ�̬�����Ķ���ָ��
	 void setAutoDeleteInNewThread(BOOL bIsAutoDelete = TRUE);
	 BOOL getAutoDeleteInNewThread()const;
	 //����Ϊ���߳���������ʾ����ģ̬��ʽ��ʾ���ڣ��ڹرմ���ǰ�������᷵�ء�
	 LONG showAsModel();
	 //����Ϊ���߳���������ʾ����ģ̬��ʽ��ʾ���ڣ��ڹرմ���ǰ�������᷵�ء�
	 LONG show();;
	 //�Է�ģ̬��ʽ�������ڣ���������������߳���������֮�У�����Ϊ��ģ̬�Ի���������
	 HWND createWindow(HWND hParentWnd);
	 HWND createWindow();
	 //�ѹ��߼��������ĩβ����Ϊһ��NULLָ�����������Զ༶Ƕ�����ӣ����ǲ��������ѭ��Ƕ�ף��⽫��������Ԥ�ϵĴ���
	 BOOL addSubTool(CLShowTool* pSub,...);
	 //��һ����ȫ���������߳�����ʾ�����һ���������������������������ؿ������¶���ָ�롣������ݹ鹹�������������ڹ�������ʱҪ��ֹѭ��Ƕ�׹��췽ʽ��
	 CLShowTool* copyToShowInNewThread(LPDWORD _out_threadId = NULL);
	 //����һ��ȫ�¶���������һ����ȫ���������߳��У������¶���ָ�롣
	 //��pNewObjָ����һ�����ж����������д�������isWaitBeforeCreateWndָ�������������ɹ��Ƿ��ڵȴ����ڴ�����Ϻ�ŷ��أ�Ĭ��״̬���ȴ�������������
	 //�����Ǿ�̬�ģ����ص�ָ��ɶԶ����������д�����������Ӧ��delete����ָ�루�����ɶ����̸߳����ͷţ���
	 //����destroyWindows�������Ĵ����ڹر��Ժ��߳̽��˳��������ͷŶ��󣬷���ָ�뽫���ٿ��á�
	 //ע�⣺��ҪĿ����������߳̽���ʱ���Զ��ͷţ�������setAutoDeleteInNewThread����ΪTRUE��
	 //���ض���ָ�롣��������ʧ�ܷ���NULL
	 static CLShowTool* createAndShowInNewThread(LPDWORD _out_threadId = NULL,CLShowTool* pNewObj = NULL,BOOL isWaitBeforeCreateWnd = FALSE);
	 //�ö���������һ����ȫ���������߳��У������¶���ָ�롣
	 //�����ɶ����̸߳����ͷţ���Ӧ��delete����ָ�룬����destroyWindows�������Ĵ����ڹر��Ժ��߳̽��˳��������ͷŶ��󣬶���ָ�뽫���ٿ��á�
	 //ע�⣺��ҪĿ��������߳̽���ʱ���Զ��ͷţ�������setAutoDeleteInNewThread����ΪTRUE��
	 //���ض���ָ�롣��������ʧ�ܷ���NULL
	 CLShowTool* showInNewThread(LPDWORD _out_threadId = NULL);
	 //�ú����������������Ӵ��ڷ�ʽ��ʾ��������NULLΪ��������ʾ��isSub = false ��ʾ������NULLΪ������
	 void setSubLstShowAsSubWnd(BOOL isSub = TRUE);
	 BOOL getSubLstShowAsSubWnd()const;

	 operator HWND() const;
	 HWND getSafeHwnd() const;
	 BOOL getClientRect(CLRect* lpRect)const;
	 BOOL updateWindow()const;
	 BOOL invalidate(BOOL bErase)const;
	 BOOL invalidateRect(CLRect* lpRect, BOOL bErase)const;
	 BOOL setWindowPos(HWND hWndInsertAfter, LONG X, LONG Y, LONG cx, LONG cy, UINT uFlags)const;
	 BOOL setWindowText(LPCTSTR lpString)const;
	 LONG showWindow(LONG nCmdShow = SW_SHOW)const;
	 BOOL destroyWindow()const;
	 BOOL isWindow() const;
	 BOOL isWindowVisible()const;
	 BOOL postMessage(UINT msg = WM_NULL, WPARAM wp = 0, LPARAM lp = 0)const;
	 static BOOL windowsToBmp(HWND hTagWnd, LPCTSTR outFileName);
	 BOOL windowsToBmp(LPCTSTR outFileName)const;
	 BOOL exportBmp(LPCTSTR outFileName)const;
	 //�޸�simple windows�ĳߴ��С����ṹ��������ֵ��
	 static CLRect getDefaultSimpleLineRect();
	 static CLRect setDefaultSimpleLineRect(const CLRect& newRect);
	 static CLRect getDefaultKLineRect();
	 static CLRect setDefaultKLineRect(const CLRect& newRect);
	 static LONG getDefSimLineCX();
	 static LONG getDefSimLineCY();
	 static LONG getDefKLineCX();
	 static LONG getDefKLineCY();
	 BOOL isInCurrentThread() const;
	 //�������һ��CLShowTool����������������ͨ�����ھ������CLShowTool����ָ��,
	 //�����󴰿ڲ����ڵ�ǰ�����߳���Ĭ��״̬������Ϊ�Զ��ͷŵģ�����Ҫ��ʾ���ͷ�ָ�룻
	 static CLShowTool* getWnd(HWND hWnd);
protected:

#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
//ȫ�ֵĶ�����������ָ��ӳ�������---------------------------------------------
typedef  CLMapTemplate<HWND,CLShowTool*> CLShowToolMapManager;//����ͼ��ӳ�������
	 static CLShowToolMapManager& GetMapManager();//ȫ��Ψһ��ӳ�����ȡ�÷���
#endif
};

#endif

