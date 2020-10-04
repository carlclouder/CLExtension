
// 前向定义文件
//

#pragma once

#include "CLShowTool_targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include "WinUser.h"
#include "Windowsx.h"

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <stack>
#include <locale.h>
#include <ShlObj.h>//选择文件夹 用到
#include <CommDlg.h>

#include "CLShowTool_resource.h"

#include "../_cl_string/CLString.h"
#include "../_cl_time/CLTime.h"

//定义API，MFC下的画直线函数宏
#define drawLine( pDC , x1 , y1 , x2 , y2 )     \
{ assert( (pDC) != (NULL));                     \
	(pDC)->MoveTo((x1),(y1));				    \
	(pDC)->LineTo((x2),(y2));}
#define drawLineP( pDC , CPenObj , x1 , y1 , x2 , y2 )  \
{ assert( (pDC) != (NULL));                             \
	(pDC)->SelectObject(&(CPenObj));				    \
	(pDC)->MoveTo((x1),(y1));					        \
	(pDC)->LineTo((x2),(y2));}


//定义常用GDI颜色
#define CLRED      (RGB(255,50,50))        //红
#define CLREDHIGH  (RGB(255,0,0))          //亮红
#define CLREDDARK  (RGB(128,0,0))          //暗红
#define CLGOLD     (RGB(250,215,0))        //金
#define CLYELLOWHIGH   (RGB(250,250,20))   //亮黄
#define CLYELLOW   (RGB(240,248,136))      //黄
#define CLYELLOWDARK   (RGB(110,88,16))    //暗黄
#define CLORANGE   (RGB(255,165,0))        //橙色
#define CLYOUNG    (RGB(84,252,252))       //青
#define CLWHITE    (RGB(255,255,255))      //白
#define CLBLACK    (RGB(0,0,0))            //黑
#define CLGREEN    (RGB(0,230,0))          //绿
#define CLGREENSOFT (RGB(210,240,210))	   //护眼绿
#define CLBLUE     (RGB(51,153,210))       //蓝
#define CLGRAYDEEP (RGB(30,30,30))         //深灰
#define CLGRAYX3     (RGB(54,54,54))      //灰X3
#define CLGRAYX2     (RGB(79,79,79))      //灰X2
#define CLGRAYX1     (RGB(105,105,105))      //灰X1
#define CLGRAY     (RGB(127,137,137))      //灰
#define CLGRAYHIGH (RGB(196,196,196))      //浅灰 
#define CLVIOLETHIGH   (RGB(238,130,238))       //亮紫色
#define CLVIOLET   (RGB(200,30,210))       //紫色
#define CLVIOLETDARK   (RGB(148,0,211))       //暗紫色

//K线数据日线数据结构体（用于：mysql、内部）
typedef struct _str_dayDataSetsUnit1 {
	ULONG nIndex;//索引
	long long date;//日期
	float open;//开盘价	
	float high;//最高价
	float low;//最低价
	float close;//收盘价
	long long volum;//成交量,原值
	float adj_close;//前一交易日收盘价
	float zde;//涨跌额
	float zdf;//涨跌幅
	float hsl;//换手率
	double amount;//成交金额，原值
	double zsz;//总市值
	double ltsz;//流通市值
	LONG cjbs;//成交笔数
	//构造数据单元（一天）
	_str_dayDataSetsUnit1& reset() { ZeroMemory(this, sizeof(_str_dayDataSetsUnit1)); return *this; }
	_str_dayDataSetsUnit1() { reset(); }
}DAYDATASETS_UNIT1, * PDAYDATASETS_UNIT1;//K线数据日线数据结构体（用于：mysql、内部）

//返回的mysql数据/K线数据日线数据链串结构体（用于：MySQL、内部）
typedef struct _str_mySqlDayDateResultSets1 {
	char szStockName[9];//股票名称
	long nStockNumber;//股票代码
	long nDataCounts;//数据个数
	LONGLONG startDate;//其实时间
	LONGLONG endDate;//结束时间
	DWORD dFieldMask;//结果集的可用字段掩码
	std::vector<DAYDATASETS_UNIT1> vtData;//数据表列
	_str_mySqlDayDateResultSets1() { szStockName[0] = 0; nStockNumber = nDataCounts = 0; startDate = endDate = 0; dFieldMask = 0; }
	void reset() { szStockName[0] = 0; nStockNumber = nDataCounts = 0; startDate = endDate = 0; dFieldMask = 0; vtData.clear(); }
}DAYDATASETS1, * PDAYDATASETS1;//返回的mysql数据/K线数据日线数据链串结构体（用于：MySQL、内部）

#define TSDIS_COUNTS1 (201)
#define TSDEV_COUNTS1 (240)
#define TSSTD_COUNTS1 (TSDEV_COUNTS1)

#define OFFSET 13
#define OFFSETMID (OFFSET*5)
#define DEVICE0 (1-DEVICE1-DEVICE2)
#define DEVICE1 0.25
#define DEVICE2 0.1

#define LEFTWIDE   30
#define BOTTONWIDE 25 
#define DISSTEP    50

#define FIRSTDAYS 60

#define TIMERID_PEEKDATA       10
#define TIMERECLIPSE_PEEKDATA  1000

#define TIMERID_TS_REDRAW       11
#define TIMERECLIPSE_TS_DUMPDIS 3000
class CLDC;
class CLShowTool;
typedef struct CLPOINT {
	double x;
	double y;
	CLPOINT() { set(0, 0); }
	CLPOINT(double _x, double _y) { set(_x, _y); }
	void set(double _x, double _y) { x = _x; y = _y; }
} *PCLPOINT;
typedef struct _str_Rage {
	double LB, RT;//左范围坐标值，右范围坐标值
	double XV;//该范围的中间代表数值
	_str_Rage() : LB(0), RT(0), XV(0) { };
	_str_Rage(double _LB, double _RT) : LB(_LB), RT(_RT), XV(0) {  };
	_str_Rage(double _LB, double _RT, double _XV) : LB(_LB), RT(_RT), XV(_XV) {  };
}RAGE;
typedef struct _str_lineBuf {
	long si;
	double* Line;
	std::map<long, RAGE> LineX;
}LINEBUF, * PLINEBUF;
class CLPoint {
public:
	LONG x;
	LONG y;
	inline CLPoint() { x = y = 0; }
	inline CLPoint(LPARAM lp) {
		x = ((LONG)(short)GET_X_LPARAM(lp));
		y = ((LONG)(short)GET_Y_LPARAM(lp));
	}
};
class CLRect {
public:
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
	inline CLRect() { left = 0, right = 0, top = 0, bottom = 0; }
	inline CLRect(LONG l, LONG t, LONG r, LONG b) { left = l, right = r, top = t, bottom = b; }
	inline void Set(LONG l, LONG t, LONG r, LONG b) { left = l, right = r, top = t, bottom = b; }
	inline LONG Width() { return right - left >= 0 ? right - left : left - right; }
	inline LONG Height() { return bottom - top >= 0 ? bottom - top : top - bottom; }
};
class CLSize {
public:
	LONG cx;
	LONG cy;
	inline CLSize() { cx = cy = 0; }
	inline CLSize(LONG _cx, LONG _cy) { cx = _cx; cy = _cy; }
};
class CLFont {
public:
	HFONT hobj;
	inline CLFont() { hobj = 0; }
	~CLFont() { DeleteObject(); }
	void DeleteObject() {
		if (hobj)::DeleteObject(hobj);
		hobj = 0;
	}
	inline virtual operator HGDIOBJ() const { return GetSafeHandle(); }
	inline virtual operator HFONT() const { return GetSafeHandle(); }
	inline virtual HFONT GetSafeHandle() const { assert(hobj != NULL); return hobj; }
	HFONT CreatePointFont(LONG nPointSize, LPCTSTR lpszFaceName, CLDC* pDC = NULL);
};
class CLBitmap {
public:
	HBITMAP hobj;

	inline CLBitmap() { hobj = 0; }
	~CLBitmap() { DeleteObject(); }
	void DeleteObject() {
		if (hobj)::DeleteObject(hobj);
		hobj = 0;
	}
	inline virtual operator HGDIOBJ() const { return GetSafeHandle(); }
	inline virtual operator HBITMAP() const { return GetSafeHandle(); }
	inline virtual HBITMAP GetSafeHandle() const { assert(hobj != NULL); return hobj; }
	HBITMAP CreateCompatibleBitmap(HDC hdc, LONG cx, LONG cy) {
		DeleteObject();
		hobj = ::CreateCompatibleBitmap(hdc, cx, cy);
		return (HBITMAP)(hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
	}
};
class CLPen {
public:
	HPEN hobj;

	inline CLPen() { hobj = 0; }
	CLPen(LONG iStyle, LONG cWidth, COLORREF color) {
		hobj = ::CreatePen(iStyle, cWidth, color);
		(hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
	}
	~CLPen() { DeleteObject(); }
	void DeleteObject() {
		if (hobj)::DeleteObject(hobj);
		hobj = 0;
	}
	inline virtual operator HGDIOBJ() const { return GetSafeHandle(); }
	inline virtual operator HPEN() const { return GetSafeHandle(); }
	inline virtual HPEN GetSafeHandle() const { assert(hobj != NULL); return hobj; }
	HPEN CreatePen(LONG iStyle, LONG cWidth, COLORREF color) {
		DeleteObject();
		hobj = ::CreatePen(iStyle, cWidth, color);
		return (hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
	}
};
class CLBrush {
public:
	HBRUSH hobj;

	inline CLBrush() { hobj = 0; }
	CLBrush(COLORREF color) {
		hobj = ::CreateSolidBrush(color);
		(hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
	}
	~CLBrush() { DeleteObject(); }
	void DeleteObject() {
		if (hobj)::DeleteObject(hobj);
		hobj = 0;
	}
	inline virtual operator HGDIOBJ() const { return GetSafeHandle(); }
	inline virtual operator HBRUSH() const { return GetSafeHandle(); }
	inline virtual HBRUSH GetSafeHandle() const { assert(hobj != NULL); return hobj; }
	HBRUSH CreateSolidBrush(COLORREF color) {
		DeleteObject();
		hobj = ::CreateSolidBrush(color);
		return (hobj == NULL || hobj == INVALID_HANDLE_VALUE) ? (hobj = 0) : (hobj);
	}
};
class CLDC {
public:
	HDC m_hDC;          // The output DC (must be first data member)
	HDC m_hAttribDC;    // The Attribute DC

	inline CLDC() { m_hAttribDC = m_hDC = 0; }
	~CLDC() { DeleteDC(); }
	BOOL DeleteDC() { if (m_hDC == NULL)return FALSE; else return ::DeleteDC(Detach()); }
	inline void SetAttribDC(HDC hDC) { m_hAttribDC = hDC; }
	inline void SetOutputDC(HDC hDC) { m_hDC = hDC; }
	inline void ReleaseAttribDC() { m_hAttribDC = NULL; }
	BOOL Attach(HDC hDC)
	{
		if (hDC == NULL || m_hDC == INVALID_HANDLE_VALUE)return FALSE;
		DeleteDC();
		m_hDC = hDC;
		SetAttribDC(m_hDC);
		return TRUE;
	}
	HDC Detach()
	{
		HDC hDC = m_hDC;
		ReleaseAttribDC();
		m_hDC = NULL;
		return hDC;
	}
	BOOL CreateCompatibleDC(HDC _hdc) {
		return Attach(::CreateCompatibleDC(_hdc));
	}
	inline operator HDC() const { return GetSafeHdc(); }
	inline HDC GetSafeHdc() const { assert(m_hDC != NULL); return m_hDC; };
	inline HGDIOBJ SelectObject(HGDIOBJ hObj) { return ::SelectObject(m_hDC, hObj); }
	inline HGDIOBJ SelectObject(CLFont* pobj) { return ::SelectObject(m_hDC, *pobj); }
	inline HGDIOBJ SelectObject(CLPen* pobj) { return ::SelectObject(m_hDC, *pobj); }
	inline HGDIOBJ SelectObject(CLBrush* pobj) { return ::SelectObject(m_hDC, *pobj); }
	void FillSolidRect(LONG x, LONG y, LONG cx, LONG cy, COLORREF clr) {
		COLORREF clrbk = ::SetBkColor(m_hDC, clr);
		CLRect rect(x, y, x + cx, y + cy);
		::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, (const RECT*)&rect, NULL, 0, NULL);
		if (clrbk != clr)::SetBkColor(m_hDC, clrbk);
	}
	void FillSolidRect(const CLRect* lpRect, COLORREF clr) {
		COLORREF clrbk = ::SetBkColor(m_hDC, clr);
		::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, (LPRECT)lpRect, NULL, 0, NULL);
		if (clrbk != clr)::SetBkColor(m_hDC, clrbk);
	}
	inline COLORREF SetTextColor(COLORREF clr) { return ::SetTextColor(m_hDC, clr); }
	inline LONG SetBkMode(LONG mode) { return ::SetBkMode(m_hDC, mode); }
	inline COLORREF SetBkColor(COLORREF clr) { return ::SetBkColor(m_hDC, clr); }
	inline BOOL LineTo(LONG x, LONG y) { return ::LineTo(m_hDC, x, y); }
	inline BOOL MoveTo(LONG x, LONG y) { return ::MoveToEx(m_hDC, x, y, NULL); }
	inline BOOL MoveTo(LONG x, LONG y, CLPoint* lpPoint) { return ::MoveToEx(m_hDC, x, y, (LPPOINT)lpPoint); }
	inline BOOL TextOut(LONG x, LONG y, LPCTSTR lpString, LONG c) { return ::TextOut(m_hDC, x, y, lpString, c); }
	inline LONG FillRect(const CLRect* lpRect, const CLBrush* pBrush) { return ::FillRect(m_hDC, (const RECT*)lpRect, *pBrush); }
	inline BOOL Rectangle(const CLRect* rect) { return ::Rectangle(m_hDC, rect->left, rect->top, rect->right, rect->bottom); }
	inline BOOL Rectangle(const CLRect& rect) { return ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom); }
	inline BOOL Rectangle(LONG l, LONG r, LONG t, LONG b) { return ::Rectangle(m_hDC, l, r, t, b); }
	inline BOOL GetTextMetrics(LPTEXTMETRIC lptm) { return ::GetTextMetrics(m_hDC, lptm); }
	CLSize GetTextExtent(LPCTSTR lpszString, LONG nCount) const {
		CLSize ret;
		GetTextExtentPoint(m_hDC, lpszString, nCount, (LPSIZE)(&ret));
		return ret;
	}
};
class CLPaintDC : public CLDC
{
public:
	explicit CLPaintDC(CLShowTool* pWnd);
protected:
	HWND m_hWnd;
public:
	PAINTSTRUCT m_ps;
public:
	virtual ~CLPaintDC() { ::EndPaint(m_hWnd, &m_ps);	Detach(); }
};
class CLClientDC : public CLDC
{
public:
	CLClientDC() { m_hWnd = 0; };
	explicit CLClientDC(CLShowTool* pWnd);
	BOOL AttachWnd(CLShowTool* pWnd);
protected:
	HWND m_hWnd;
public:
	virtual ~CLClientDC() { ::ReleaseDC(m_hWnd, Detach()); }
};
class CLWindowDC : public CLDC
{
public:
	CLWindowDC() { m_hWnd = 0; };
	explicit CLWindowDC(CLShowTool* pWnd);
	BOOL AttachWnd(CLShowTool* pWnd);
protected:
	HWND m_hWnd;
public:
	virtual ~CLWindowDC() { ::ReleaseDC(m_hWnd, Detach()); }
};

#define STLT_NO				  0     //空图形，什么也不画
#define STLT_ADDAXIS_X        0x1   //绘制X轴，该状态下不能紧跟写入曲线数据
#define STLT_ADDAXIS_Y        0x2	//绘制X轴，该状态下不能紧跟写入曲线数据
#define STLT_ADDAXIS_XY       (STLT_ADDAXIS_Y | STLT_ADDAXIS_X) //同时绘制X和Y轴，该状态下不能紧跟写入曲线数据
#define STLT_LINE			  4//画一条曲线
#define STLT_VERLINE		  5//画柱状图
#define STLT_TEXT             6//写一行文字

#define STEF_ADDTOPLINE            0x1//画曲线和柱状图模式下，增加一条顶点曲线
#define STEF_ADDTOPTEXT		       0x2//画曲线和柱状图模式下，增加逐个定点的Y值文字
#define STEF_NODRAWLINE_0          0x4//画曲线和柱状图模式下，若该点为0值则不绘制此点
#define STEF_NODRAWTEXT_0          0x8//画曲线和柱状图模式下，若该点为0值则不绘制此点的Y值文字
#define STEF_VERLINEUSEDBCOLOR     0x10//画柱状图模式下，若该点为负值，则该点的柱状图颜色采用副颜色
#define STEF_USE_AXIS_X_DATA       0x20//画柱状图、曲线图，K线图模式下，X轴将采用详细的数据定义，也就是点数据为2D数据，必须包含（x，y）,但这里的x只是X轴刻度显示数据而不是实际坐标，显示结果只与数据写入顺序有关和该值无关
#define MAX_FONTNAME_SIZE           20
#define MAX_PRINTENDSTRING_SIZE     50
typedef struct _str_showTool_UnitHeader {
	long utType;//该条曲线的类型，默认为STLT_NO
	long dataCounts;//该曲线的数据个数，默认为0
	double minX;//指定该线条定义域最小值，默认为0
	double maxX;//指定该线条定义域最大值，默认为0
	double defMinY;//默认状态图像y值最大显示值，若实际值更大按实际值，默认为0
	double defMaxY;//默认状态图像y值最小显示值，若实际值更小按实际值，默认为0
	LONG lineWide; //指定线图形的线宽，默认为1
	LONG linePenType;//指定画线的笔的样式，默认为PS_SOLID
	LONG verLineWide;//指定柱状图形的宽度，0表示自动填充宽度，默认为0
	COLORREF clr;//主颜色RGB值，控制图形颜色，和文字颜色，默认为红色
	LONG exFlag; //附加选项，可以是多值的组合，默认为0
	COLORREF exClr;//副颜色RGB值，控制第二色，负值柱状图色，顶线色等，默认为主控色
	TCHAR fontTypeName[MAX_FONTNAME_SIZE]; //指定该线条内文字使用样式，默认为系统指定
	LONG fontSize;//指定该线条内文字点状大小，实际大小为该值的1/10大小的像素尺寸，默认为100
	LONG textLine;//文字图形出现的位置，1表示第一行，-1表示在倒数第一行，行高由当前指定字体样式和字高决定，默认为1
	LONG dataAccuracy;//图形的数据在做文字输出时候的小数点保留精度，未达到就原样输出，超过则四舍五入，该值若小于0则取0，默认为9位有效精度。
	double dataPrintMulti;//数据打印倍率乘数
	double dataPrintOffset;//数据打印偏离量
	TCHAR dataPrintEndString[MAX_PRINTENDSTRING_SIZE];//数据打印末尾文字，可以写入单位等信息
	_str_showTool_UnitHeader() {
		reset();
	}
	_str_showTool_UnitHeader(const _str_showTool_UnitHeader& tag) {
		memcpy_s(this, sizeof(_str_showTool_UnitHeader), &tag, sizeof(_str_showTool_UnitHeader));
	}
	_str_showTool_UnitHeader& operator = (const _str_showTool_UnitHeader& tag) {
		memcpy_s(this, sizeof(_str_showTool_UnitHeader), &tag, sizeof(_str_showTool_UnitHeader));
		return *this;
	}
	_str_showTool_UnitHeader& reset() {
		ZeroMemory(this, sizeof(_str_showTool_UnitHeader));
		exClr = clr = CLRED;
		lineWide = 1;
		linePenType = PS_SOLID;
		fontSize = 100;
		textLine = 1;
		verLineWide = 0;
		dataAccuracy = 9;
		dataPrintMulti = 1;
		return *this;
	}
	_str_showTool_UnitHeader& setFontType(LPCTSTR lpFontTypeName) {
		lpFontTypeName ? _tcscpy_s(fontTypeName, MAX_FONTNAME_SIZE, lpFontTypeName) : fontTypeName[0] = 0;
		return *this;
	}
	_str_showTool_UnitHeader& setDataPrintEndString(LPCTSTR lpEndString) {
		lpEndString ? _tcscpy_s(dataPrintEndString, MAX_PRINTENDSTRING_SIZE, lpEndString) : dataPrintEndString[0] = 0;
		return *this;
	}
	//设置显示设置的精度，缩放倍数，偏移量，末尾文字后缀
	_str_showTool_UnitHeader& setDataPrintOpt(LONG _dataAccuracy = 9, double _dataPrintMulti = 1, double _dataPrintOffset = 0, LPCTSTR lpEndString = 0) {
		dataAccuracy = _dataAccuracy;
		dataPrintMulti = _dataPrintMulti;
		if (dataPrintMulti == 0)throw std::logic_error("dataPrintMulti can not be 0!");
		dataPrintOffset = _dataPrintOffset;
		setDataPrintEndString(lpEndString);
		return *this;
	}
}STUTHEADER, * PSTUTHEADER;


typedef struct _str_kLineData_header {
	char szStockName[9];
	long stockId;
	long nDays;
	_str_kLineData_header& reset() { ZeroMemory(this, sizeof(_str_kLineData_header)); return *this; }
	_str_kLineData_header() { reset(); }
	_str_kLineData_header(long id, long days, const char* lpStockName) { set(id, days, lpStockName); }
	_str_kLineData_header& set(long id, long days, const char* lpStockName) { stockId = id; nDays = days; strcpy_s(szStockName, 9, lpStockName); return *this; }
}KLINEDATAHEADER, * PKLINEDATAHEADER;

#define MAX_HEADER_TITLE 500
#define SHOWGRAPHER_TYPE_NO            0//无显示
#define SHOWGRAPHER_TYPE_SHOWKLINE     1//显示k线图
#define SHOWGRAPHER_TYPE_DUMPTSDISTRIB 2//显示主力分布图
#define SHOWGRAPHER_TYPE_LINES         3//混合图形
#define SHOWGRAPHER_TYPE_BITMAP        4//画图图形
typedef struct _str_showGrapher_header {
	long type;//show type
	long long writeTime;//pipe write timeStamp
	long long readTime;//pipe read timeStamp
	long long pipeTotalSizeInByte;//the pipe total size in byte( = header + data )
	long long dataTotalSizeInByte;//data total size in byte
	long long dataWritedSizeInByte;//data writed size in byte
	LONG x, y, cx, cy;//用于初始化数据传入后改变窗口位置和大小
	LONG addAxisFlag;// STLT_ADDAXIS_X 或 STLT_ADDAXIS_Y 或他们的组合值，此时采用默认样式的坐标轴
	LONG AxisTextDataAccuracy; //在增加X 或Y轴模式下，轴线数标的小数点输出位数，默认为9
	TCHAR title[MAX_HEADER_TITLE];//标题
	COLORREF gbkColor;
	_str_showGrapher_header() { reset(); }
	_str_showGrapher_header& setGlobleBkColor(COLORREF _gbkColor) {
		gbkColor = _gbkColor;
		return *this;
	}
	//初始化对象，小数精度设为0，其他为0
	_str_showGrapher_header& reset() { ZeroMemory(this, sizeof(_str_showGrapher_header)); AxisTextDataAccuracy = 9; return *this; }
	//增加一个线条数据大小容量到对象
	_str_showGrapher_header& addOneLineSize(long long dataCounts, LONG samelineCounts = 1) {
		if (dataCounts < 0 || samelineCounts < 1) return *this;
		dataTotalSizeInByte += (sizeof(STUTHEADER) + sizeof(double) * dataCounts) * samelineCounts;
		dataWritedSizeInByte = dataTotalSizeInByte;
		pipeTotalSizeInByte = sizeof(SHOWGRAPHERHEARDER) + dataTotalSizeInByte;
		return *this;
	}
	//增加一个文字数据容量到对象
	_str_showGrapher_header& addOneTextSize(long long dataCounts, LONG samelineCounts = 1) {
		if (dataCounts <= 0 || samelineCounts < 1) return *this;
		dataTotalSizeInByte += (sizeof(STUTHEADER) + sizeof(TCHAR) * dataCounts) * samelineCounts;
		dataWritedSizeInByte = dataTotalSizeInByte;
		pipeTotalSizeInByte = sizeof(SHOWGRAPHERHEARDER) + dataTotalSizeInByte;
		return *this;
	}
	//增加一个K线数据容量到对象
	_str_showGrapher_header& addKLineSize(long long dataCounts) {
		if (dataCounts <= 0) return *this;
		dataTotalSizeInByte = (sizeof(KLINEDATAHEADER) + sizeof(DAYDATASETS_UNIT1) * dataCounts);
		dataWritedSizeInByte = dataTotalSizeInByte;
		pipeTotalSizeInByte = sizeof(SHOWGRAPHERHEARDER) + dataTotalSizeInByte;
		return *this;
	}
	//增加一个bitmap容量到对象
	_str_showGrapher_header& addBitmapSize(long long bfSize) {
		if (bfSize <= 0) return *this;
		dataTotalSizeInByte = bfSize;
		dataWritedSizeInByte = dataTotalSizeInByte;
		pipeTotalSizeInByte = sizeof(SHOWGRAPHERHEARDER) + dataTotalSizeInByte;
		return *this;
	}
	//设置对象窗口标题文字
	_str_showGrapher_header& setTitle(LPCTSTR pTitle) {
		pTitle ? _tcscpy_s(title, MAX_HEADER_TITLE, pTitle) : title[0] = 0;
		return *this;
	}
	//设置对象窗口初始位置
	_str_showGrapher_header& setWndPos(LONG _x, LONG _y, LONG _cx, LONG _cy) { x = _x, y = _y, cx = _cx, cy = _cy; return *this; }
}SHOWGRAPHERHEARDER, * PSHOWGRAPHERHEARDER;

//定义下的画直线函数宏
#define drawLine( pDC , x1 , y1 , x2 , y2 )    \
{ assert( (pDC) != (NULL));                     \
	(pDC)->MoveTo((x1),(y1));				    \
	(pDC)->LineTo((x2),(y2));}
#define drawLineP( pDC , CPenObj , x1 , y1 , x2 , y2 )   \
{ assert( (pDC) != (NULL));                             \
	(pDC)->SelectObject(&(CPenObj));				    \
	(pDC)->MoveTo((x1),(y1));					        \
	(pDC)->LineTo((x2),(y2));}


//三点线性平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth3_Linear(const T in, T out, size_t N)
{
	size_t i;
	if (N < 3)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (5.0 * in[0] + 2.0 * in[1] - in[2]) / 6.0;

		for (i = 1; i <= N - 2; i++)
		{
			out[i] = (in[i - 1] + in[i] + in[i + 1]) / 3.0;
		}

		out[N - 1] = (5.0 * in[N - 1] + 2.0 * in[N - 2] - in[N - 3]) / 6.0;
	}
}
//五点线性平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth5_Linear(const T in, T out, size_t N)
{
	size_t i;
	if (N < 5)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (3.0 * in[0] + 2.0 * in[1] + in[2] - in[4]) / 5.0;
		out[1] = (4.0 * in[0] + 3.0 * in[1] + 2 * in[2] + in[3]) / 10.0;
		for (i = 2; i <= N - 3; i++)
		{
			out[i] = (in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2]) / 5.0;
		}
		out[N - 2] = (4.0 * in[N - 1] + 3.0 * in[N - 2] + 2 * in[N - 3] + in[N - 4]) / 10.0;
		out[N - 1] = (3.0 * in[N - 1] + 2.0 * in[N - 2] + in[N - 3] - in[N - 5]) / 5.0;
	}
}
//七点线性平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth7_Linear(const T in, T out, size_t N)
{
	size_t i;
	if (N < 7)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (13.0 * in[0] + 10.0 * in[1] + 7.0 * in[2] + 4.0 * in[3] +
			in[4] - 2.0 * in[5] - 5.0 * in[6]) / 28.0;

		out[1] = (5.0 * in[0] + 4.0 * in[1] + 3 * in[2] + 2 * in[3] +
			in[4] - in[6]) / 14.0;

		out[2] = (7.0 * in[0] + 6.0 * in[1] + 5.0 * in[2] + 4.0 * in[3] +
			3.0 * in[4] + 2.0 * in[5] + in[6]) / 28.0;

		for (i = 3; i <= N - 4; i++)
		{
			out[i] = (in[i - 3] + in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2] + in[i + 3]) / 7.0;
		}

		out[N - 3] = (7.0 * in[N - 1] + 6.0 * in[N - 2] + 5.0 * in[N - 3] +
			4.0 * in[N - 4] + 3.0 * in[N - 5] + 2.0 * in[N - 6] + in[N - 7]) / 28.0;

		out[N - 2] = (5.0 * in[N - 1] + 4.0 * in[N - 2] + 3.0 * in[N - 3] +
			2.0 * in[N - 4] + in[N - 5] - in[N - 7]) / 14.0;

		out[N - 1] = (13.0 * in[N - 1] + 10.0 * in[N - 2] + 7.0 * in[N - 3] +
			4 * in[N - 4] + in[N - 5] - 2 * in[N - 6] - 5 * in[N - 7]) / 28.0;
	}
}
//五点二次平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth5_Quadratic(const T in, T out, size_t N)
{
	size_t i;
	if (N < 5)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (31.0 * in[0] + 9.0 * in[1] - 3.0 * in[2] - 5.0 * in[3] + 3.0 * in[4]) / 35.0;
		out[1] = (9.0 * in[0] + 13.0 * in[1] + 12 * in[2] + 6.0 * in[3] - 5.0 * in[4]) / 35.0;
		for (i = 2; i <= N - 3; i++)
		{
			out[i] = (-3.0 * (in[i - 2] + in[i + 2]) +
				12.0 * (in[i - 1] + in[i + 1]) + 17 * in[i]) / 35.0;
		}
		out[N - 2] = (9.0 * in[N - 1] + 13.0 * in[N - 2] + 12.0 * in[N - 3] + 6.0 * in[N - 4] - 5.0 * in[N - 5]) / 35.0;
		out[N - 1] = (31.0 * in[N - 1] + 9.0 * in[N - 2] - 3.0 * in[N - 3] - 5.0 * in[N - 4] + 3.0 * in[N - 5]) / 35.0;
	}
}
//七点二次平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth7_Quadratic(const T in, T out, size_t N)
{
	size_t i;
	if (N < 7)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (32.0 * in[0] + 15.0 * in[1] + 3.0 * in[2] - 4.0 * in[3] -
			6.0 * in[4] - 3.0 * in[5] + 5.0 * in[6]) / 42.0;

		out[1] = (5.0 * in[0] + 4.0 * in[1] + 3.0 * in[2] + 2.0 * in[3] +
			in[4] - in[6]) / 14.0;

		out[2] = (1.0 * in[0] + 3.0 * in[1] + 4.0 * in[2] + 4.0 * in[3] +
			3.0 * in[4] + 1.0 * in[5] - 2.0 * in[6]) / 14.0;
		for (i = 3; i <= N - 4; i++)
		{
			out[i] = (-2.0 * (in[i - 3] + in[i + 3]) +
				3.0 * (in[i - 2] + in[i + 2]) +
				6.0 * (in[i - 1] + in[i + 1]) + 7.0 * in[i]) / 21.0;
		}
		out[N - 3] = (1.0 * in[N - 1] + 3.0 * in[N - 2] + 4.0 * in[N - 3] +
			4.0 * in[N - 4] + 3.0 * in[N - 5] + 1.0 * in[N - 6] - 2.0 * in[N - 7]) / 14.0;

		out[N - 2] = (5.0 * in[N - 1] + 4.0 * in[N - 2] + 3.0 * in[N - 3] +
			2.0 * in[N - 4] + in[N - 5] - in[N - 7]) / 14.0;

		out[N - 1] = (32.0 * in[N - 1] + 15.0 * in[N - 2] + 3.0 * in[N - 3] -
			4.0 * in[N - 4] - 6.0 * in[N - 5] - 3.0 * in[N - 6] + 5.0 * in[N - 7]) / 42.0;
	}
}
//五点三次平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth5_Cubic(const T in, T out, size_t N)
{

	size_t i;
	if (N < 5)
	{
		for (i = 0; i <= N - 1; i++)
			out[i] = in[i];
	}

	else
	{
		out[0] = (69.0 * in[0] + 4.0 * in[1] - 6.0 * in[2] + 4.0 * in[3] - in[4]) / 70.0;
		out[1] = (2.0 * in[0] + 27.0 * in[1] + 12.0 * in[2] - 8.0 * in[3] + 2.0 * in[4]) / 35.0;
		for (i = 2; i <= N - 3; i++)
		{
			out[i] = (-3.0 * (in[i - 2] + in[i + 2]) + 12.0 * (in[i - 1] + in[i + 1]) + 17.0 * in[i]) / 35.0;
		}
		out[N - 2] = (2.0 * in[N - 5] - 8.0 * in[N - 4] + 12.0 * in[N - 3] + 27.0 * in[N - 2] + 2.0 * in[N - 1]) / 35.0;
		out[N - 1] = (-in[N - 5] + 4.0 * in[N - 4] - 6.0 * in[N - 3] + 4.0 * in[N - 2] + 69.0 * in[N - 1]) / 70.0;
	}
	return;
}
//七点三次平滑（注意：函数模板可采用double*、float*、map<LONG,double>、vector<double>等拥有重载[]操作符的数据类型）
template <class T> void smooth7_Cubic(const T in, T out, size_t N)
{
	size_t i;
	if (N < 7)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (39.0 * in[0] + 8.0 * in[1] - 4.0 * in[2] - 4.0 * in[3] +
			1.0 * in[4] + 4.0 * in[5] - 2.0 * in[6]) / 42.0;
		out[1] = (8.0 * in[0] + 19.0 * in[1] + 16.0 * in[2] + 6.0 * in[3] -
			4.0 * in[4] - 7.0 * in[5] + 4.0 * in[6]) / 42.0;
		out[2] = (-4.0 * in[0] + 16.0 * in[1] + 19.0 * in[2] + 12.0 * in[3] +
			2.0 * in[4] - 4.0 * in[5] + 1.0 * in[6]) / 42.0;
		for (i = 3; i <= N - 4; i++)
		{
			out[i] = (-2.0 * (in[i - 3] + in[i + 3]) +
				3.0 * (in[i - 2] + in[i + 2]) +
				6.0 * (in[i - 1] + in[i + 1]) + 7.0 * in[i]) / 21.0;
		}
		out[N - 3] = (-4.0 * in[N - 1] + 16.0 * in[N - 2] + 19.0 * in[N - 3] +
			12.0 * in[N - 4] + 2.0 * in[N - 5] - 4.0 * in[N - 6] + 1.0 * in[N - 7]) / 42.0;
		out[N - 2] = (8.0 * in[N - 1] + 19.0 * in[N - 2] + 16.0 * in[N - 3] +
			6.0 * in[N - 4] - 4.0 * in[N - 5] - 7.0 * in[N - 6] + 4.0 * in[N - 7]) / 42.0;
		out[N - 1] = (39.0 * in[N - 1] + 8.0 * in[N - 2] - 4.0 * in[N - 3] -
			4.0 * in[N - 4] + 1.0 * in[N - 5] + 4.0 * in[N - 6] - 2.0 * in[N - 7]) / 42.0;
	}
}

