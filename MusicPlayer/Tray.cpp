#include "stdafx.h"
#include "Tray.h"

/*=================================================================================
*Fun Purpose://最小化时隐藏到托盘
=================================================================================*/
void tray::ToTray(HWND hwnd,int IconID,HMENU hMenu,TCHAR szTip[])
{
	TaskIconID=IconID;
	hTrayMenu=hMenu;

	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hWnd=hwnd;
	nid.uID=IconID;
	nid.hIcon=LoadIcon((HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),MAKEINTRESOURCE(IconID));
	nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	nid.uCallbackMessage=WM_SHOWTASK;  //回调信息
	lstrcpy(nid.szTip,szTip);

	ShowWindow(hwnd,SW_HIDE);
	Shell_NotifyIcon(NIM_ADD,&nid);    //添加托盘图标
}
/*=================================================================================
*Fun Purpose://将窗口显示出来
=================================================================================*/
void tray::ShowTask(HWND hwnd,UINT IconID,UINT iMouseMsg)
{
	if(IconID!=TaskIconID)return;
	switch(iMouseMsg)
	{
		case WM_LBUTTONDBLCLK://双击
		{
			ShowWindow(hwnd,SW_SHOWNORMAL);
			SetForegroundWindow(hwnd);
			Shell_NotifyIcon(NIM_DELETE,&nid);
		}
		break;
		
		case WM_RBUTTONDOWN://右击
		{
			POINT pos;
			GetCursorPos(&pos);
			SetForegroundWindow(hwnd);
			TrackPopupMenu(hTrayMenu,TPM_CENTERALIGN|TPM_RIGHTBUTTON,pos.x+65,pos.y-9,0,hwnd,NULL);
		}
		break;
		default:
		break;
	}
}