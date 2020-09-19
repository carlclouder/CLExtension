#pragma once
#ifndef __CL_CLSHOWTOOL_H__
#define __CL_CLSHOWTOOL_H__
// ShowGrapherDlg.h : 头文件
//

#pragma once

#include "CLShowTool_PreDefine.h"
#include "../_cl_common/CLCommon.h"


#define CLST_USE_EXTRADATA_TO_SAVE_DLGPTR 0 //指针保存id,当前采用extradata映射模式，注释掉这一行则采用map映射

// CLShowTool 对话框
class CLShowTool 
{
protected:
#define MAX_LOADSTRING 100

	// 全局变量:
	HWND m_hWnd;
	BOOL m_isInit;
	HWND m_hWndParent;
	HINSTANCE hInst;								// 当前实例
	CLString szTitle;					            // 标题栏文本
	LPCTSTR szWindowClass;                   		// 主窗口类名
	BOOL m_isMainWnd;								//是否是线程主窗口
	BOOL m_isIndependRun;							//是否是以独立线程在运行
	BOOL m_isAutoDeleteSelfInNewThread;				//以独立线程在运行时候是否在退出时候销毁对象自身而不由外部控制
	std::vector<CLShowTool*> m_subWndLst;//子窗口连
	BOOL m_isSubLstShowAsSubWnd;

	// 对话框数据
	enum { IDD = IDD_CLST_MAIN };

	// 此代码模块中包含的函数的前向声明:
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE, LONG);
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK	DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//在独立线程中创建显示窗口，线程结束时候会销毁传入指针对象
	static DWORD WINAPI IndependentThreadProc(LPVOID _pCopyWnd);
	//检查并创建管道
	BOOL getPipeHandle(LONGLONG nPipeMaxBufInByte, HANDLE* hRead = 0, HANDLE* hWrite = 0);
	BOOL isPipeEmpty() const;
	BOOL writeData(LPCVOID buf, LONGLONG nDataBufSizeInByte, LONGLONG nPipeMaxBufInByte);
public:		
	//写入需要组装的头
	BOOL writeHeader(const SHOWGRAPHERHEARDER& hdr);
	BOOL writeBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo);
	BOOL writeSimpleBitmap(const BITMAPFILEHEADER& btHdr, const BITMAPINFO* btInfo, LPCTSTR szTitle, LONG X, LONG Y);
	BOOL writeLine(const STUTHEADER& lineHdr,const DOUBLE* pLine);
	//写入2D线条数据，会自动增加STEF_USE_AXIS_X_DATA标签，lineHdr.dataCounts必须包含总数据个数
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

//直接显示简单图形
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
	//快速的显示数据分布情况
	//pData 数据列, nCounts 数据个数, sectionCounts分布区段数
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
	//快速的显示数据分布情况
	//vd 数据vector, sectionCounts分布区段数
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
	LONG m_isXAxis,m_isYAxis;//记录x y的记录位置编号
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

	//k线数据
	BOOL isEirstPaint;
	LONGLONG nSiStart,nSiEnd,nSiAll,nSiRi;//开始和结束索引,从0开始	
	KLINEDATAHEADER kLineHeader;
	DAYDATASETS_UNIT1* pPt;
	BITMAPFILEHEADER m_BtmapFileHdr;
	BITMAPINFOHEADER m_BtmapInfoHdr; //位图信息头结构
	BITMAPINFO* pBitmapInfo;
	byte* pBitmapBuf;
	size_t pBitmapBufSi;
	HBITMAP hBitmap;
	BITMAP bitmap;

	//ts分布数据
	DOUBLE dis[TSDIS_COUNTS1];

	//检查变量
	HANDLE hRunEvent;//指定该控件是否在运行线程中
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

	// 实现
protected:
	HICON m_hIcon;	
	BOOL m_dataExsit;//数据准备标识，在初始化时候定义为真

	CLRect clientRect;
	CLClientDC m_pdc;
	CLDC m_memdc;
 	CLBitmap m_bkBitmap;

// 消息映射函数
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
	_cl_msg void OnMenuItem_BITMAPOUT();//ID_MENUITEM1消息函数
	_cl_msg void OnContextMenu(CLPoint point);//右键菜单响应映射函数
	_cl_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	_cl_msg BOOL OnMouseWheel(UINT nFlags, SHORT zDelta, CLPoint pt);
	
	void showSubLstWnd();
	BOOL attach(const CLShowTool& obj);
	void init();
	LONG messageBox(LPCTSTR lpString) const;
	HWND create(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		LONG X, LONG Y, LONG nWidth, LONG nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

 public: 
	 CLShowTool(HWND hParent = NULL);	// 标准构造函数
	 CLShowTool(const CLShowTool& othre);	
	 CLShowTool& operator =(const CLShowTool& othre);
	 virtual ~CLShowTool();
	 //函数设置在独立线程运行模式下的对象指针，是否在独立线程退出后销毁或是继续有效，默认情况下是自动销毁对象的。
	 //设置为TRUE后，独立线程窗口关闭后不再销毁动态创建的对象指针
	 void setAutoDeleteInNewThread(BOOL bIsAutoDelete = TRUE);
	 BOOL getAutoDeleteInNewThread()const;
	 //可作为该线程主窗口显示，以模态形式显示窗口，在关闭窗口前函数不会返回。
	 LONG showAsModel();
	 //可作为该线程主窗口显示，以模态形式显示窗口，在关闭窗口前函数不会返回。
	 LONG show();;
	 //以非模态方式启动窗口，需包含在其他的线程驱动过程之中，可作为非模态对话框来调用
	 HWND createWindow(HWND hParentWnd);
	 HWND createWindow();
	 //把工具加入表链，末尾必须为一个NULL指针否则出错。可以多级嵌套增加，但是不允许出现循环嵌套，这将引发不可预料的错误。
	 BOOL addSubTool(CLShowTool* pSub,...);
	 //在一个完全独立的新线程中显示对象的一个完整拷贝（包括子连），返回拷贝的新对象指针。函数会递归构造子连，所以在构造子连时要防止循环嵌套构造方式。
	 CLShowTool* copyToShowInNewThread(LPDWORD _out_threadId = NULL);
	 //创建一个全新对象运行在一个完全独立的新线程中，返回新对象指针。
	 //若pNewObj指定了一个已有对象，则不再自行创建对象。isWaitBeforeCreateWnd指定函数若创建成功是否在等待窗口创建完毕后才返回（默认状态不等待，即非阻塞）
	 //函数是静态的，返回的指针可对对象进行数据写入操作，但不应该delete对象指针（对象由独立线程负责释放），
	 //而是destroyWindows。产生的窗口在关闭以后，线程将退出并销毁释放对象，返回指针将不再可用。
	 //注意：若要目标对象不再在线程结束时候自动释放，需设置setAutoDeleteInNewThread函数为TRUE。
	 //返回对象指针。创建运行失败返回NULL
	 static CLShowTool* createAndShowInNewThread(LPDWORD _out_threadId = NULL,CLShowTool* pNewObj = NULL,BOOL isWaitBeforeCreateWnd = FALSE);
	 //让对象运行在一个完全独立的新线程中，返回新对象指针。
	 //对象由独立线程负责释放，不应该delete对象指针，而是destroyWindows。产生的窗口在关闭以后，线程将退出并销毁释放对象，对象指针将不再可用。
	 //注意：若要目标对象不在线程结束时候自动释放，需设置setAutoDeleteInNewThread函数为TRUE。
	 //返回对象指针。创建运行失败返回NULL
	 CLShowTool* showInNewThread(LPDWORD _out_threadId = NULL);
	 //该函数设置子连是以子窗口方式显示，还是以NULL为父窗口显示。isSub = false 表示子连以NULL为父窗口
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
	 //修改simple windows的尺寸大小定义结构，返回老值；
	 static CLRect getDefaultSimpleLineRect();
	 static CLRect setDefaultSimpleLineRect(const CLRect& newRect);
	 static CLRect getDefaultKLineRect();
	 static CLRect setDefaultKLineRect(const CLRect& newRect);
	 static LONG getDefSimLineCX();
	 static LONG getDefSimLineCY();
	 static LONG getDefKLineCX();
	 static LONG getDefKLineCY();
	 BOOL isInCurrentThread() const;
	 //若句柄是一个CLShowTool对象的所属句柄，则通过窗口句柄返回CLShowTool对象指针,
	 //若对象窗口不属于当前调用线程且默认状态下设置为自动释放的，则不需要显示的释放指针；
	 static CLShowTool* getWnd(HWND hWnd);
protected:

#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
//全局的对象句柄及对象指针映射管理函数---------------------------------------------
typedef  CLMapTemplate<HWND,CLShowTool*> CLShowToolMapManager;//定义图形映射对象类
	 static CLShowToolMapManager& GetMapManager();//全局唯一的映射对象取得方法
#endif
};

#endif

