#include "CLShowTool.h"

#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
CLShowTool::CLShowToolMapManager& CLShowTool::GetMapManager(){
	static CLShowToolMapManager* _g_st_clshowTool_map_h_p = new CLShowToolMapManager;//全局图形映射管理类,该对象在堆上分配，并且保持一旦创建就不在析构
	return *_g_st_clshowTool_map_h_p;
}
#endif
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK CLShowTool::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//m_map.ErasePairFromMap(0);
#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
#define pThis (GetMapManager().GetValueFromMap(hWnd))
#else
#define pThis ((CLShowTool*)GetWindowLongPtr(hWnd,CLST_USE_EXTRADATA_TO_SAVE_DLGPTR))
#endif
	CLShowTool* p;
	int wmId, wmEvent;
	INT_PTR irt;
	MSG msg;

#define _UseLogOutWM 0
#if(_UseLogOutWM > 0)
 	LPCTSTR filepath = _T("D:\\Documents\\Desktop\\msgList.txt");
 	HANDLE handle = CreateFile(filepath, FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
 	DWORD size = 0;
 	if (handle != INVALID_HANDLE_VALUE)
 	{
 		size = GetFileSize(handle, NULL);
 		CloseHandle(handle);		
 	}	
 	if (size < 1024*1024*2)
 	{
 		LPCTSTR lpThisMsgString = CLString::getWindowsMsgStringFromMsgID(message);
 		CLString stt(150);
 		stt.format(150,_T("%lld : %s "),CLTime().getLocalTime_ll(),lpThisMsgString).writeLineToFile(filepath);
 		stt.closeFile();
 	}
#endif


	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)//对象是对话框模式下点击ok或cancel按钮
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		// 分析菜单选择:
		switch (wmId)
		{			
// 		case IDM_ABOUT:  //不需要的菜单栏驱动
// 			::DialogBox(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, &CLShowTool::About);
// 			break;
// 		case IDM_EXIT:
// 			::DestroyWindow(hWnd);
// 			break;
		case ID_CLST_BT_OUTBITMAPOUT:
			pThis->OnMenuItem_BITMAPOUT();
			break;
		case ID_CLST_BT_ABOUT:  //打开关于菜单
			irt = ::DialogBox(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CLST_ABOUTBOX), hWnd, &CLShowTool::About);
			if(irt == -1){
				::messageBoxT(hWnd, _T("CLShowTool，1.0 版\nCopyright (C) 2017 By CaiLuo"), _T("CLShowTool，1.0 版"), MB_OK, 10000);
				//DWORD le = GetLastError();
				//CLString().getLastErrorMessageBoxExceptSucceed(le);
			}
			break;
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	/*If an application processes this message, it should return TRUE to continue creation of the window. 
	If the application returns FALSE, the CreateWindow or CreateWindowEx function will return a NULL handle.*/
	case WM_NCCREATE:
		return TRUE;	
	/*WM_CREATE:If an application processes this message, it should return zero to continue creation of the window. 
	If the application returns –1, the window is destroyed and the CreateWindowEx or CreateWindow function returns a NULL handle.*/
	case WM_CREATE:
		return NULL;
	case WM_PAINT:		
		if(p = pThis) (p->OnInitDialog(), p->OnPaint());//just do once
		break;
	case WM_TIMER:
		if(p = pThis) p->OnTimer((UINT_PTR)wParam);
		break;
	case WM_KEYUP:
		pThis->OnKeyUp((UINT)wParam,(UINT)LOWORD(lParam),(UINT)HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		pThis->OnKeyDown((UINT)wParam,(UINT)LOWORD(lParam),(UINT)HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		pThis->OnLButtonDown(static_cast<UINT>(wParam), CLPoint(lParam));
		break;
	case WM_LBUTTONUP:
		pThis->OnLButtonUp(static_cast<UINT>(wParam), CLPoint(lParam));
		break;
	case WM_SIZE:
		pThis->OnSize(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_SIZING:
		pThis->OnSizing(static_cast<UINT>(wParam), reinterpret_cast<LPRECT>(lParam));
		break;
	case WM_MOUSEMOVE:
		pThis->OnMouseMove(static_cast<UINT>(wParam), CLPoint(lParam));
		break;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		pThis->OnMouseWheel(LOWORD(wParam), (short)HIWORD(wParam),CLPoint(lParam));
		break;
	case WM_CTLCOLORDLG:
		break;
	case WM_CONTEXTMENU:
		pThis->OnContextMenu(CLPoint(lParam));
		break;
	case WM_INITDIALOG:  
		return pThis->OnInitDialog();
	case WM_DESTROY: 
		pThis->OnDestroy();
		if(!PeekMessage(&msg, hWnd, WM_NCDESTROY, WM_NCDESTROY,PM_NOREMOVE))
			PostMessage(hWnd, WM_NCDESTROY, 0, 0);
		break;
	case WM_CLOSE:
		pThis->OnClose();
		::DestroyWindow(hWnd);
		break;
	case WM_NCDESTROY:
		p = pThis;
		if(p->m_isMainWnd)
			PostQuitMessage(TRUE);//是线程主窗口才能发送该消息，必须在所有处理之后发送		
		p->OnNcDestroy();		
		p->m_hWnd = NULL;
#ifndef CLST_USE_EXTRADATA_TO_SAVE_DLGPTR
		CLShowTool::GetMapManager().ErasePairFromMap(hWnd);
#endif
		//break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return FALSE;
}

INT_PTR CALLBACK CLShowTool::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
#ifdef _DEBUG
	LPCTSTR lpThisMsgString = CLString::getWindowsMsgStringFromMsgID(message);
#endif // _DEBUG	
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK CLShowTool::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	return WndProc(hDlg, message, wParam, lParam);
}
DWORD WINAPI CLShowTool::IndependentThreadProc(LPVOID _pCopyWnd ){
	//CLString(_T("开始运行CLShowTool独立线程!")).messageBox(MB_ICONINFORMATION);
	CLShowTool* pWnd = (CLShowTool*)_pCopyWnd;
	if(pWnd){
		pWnd->m_isIndependRun = TRUE;//指明这个对象是栈上分配的空间，且在自有线程运行，该标记会影响子连的释放过程,在此设置非常重要
		int ret = pWnd->show();
		if(pWnd->m_isAutoDeleteSelfInNewThread)//指定进程结束后是否销毁对象
			delete pWnd;
		return ret;
	}
	else 
		return 0;
}
