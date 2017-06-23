#include "stdafx.h"
#include "Tray.h"

/*=================================================================================
*Fun Purpose://��С��ʱ���ص�����
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
	nid.uCallbackMessage=WM_SHOWTASK;  //�ص���Ϣ
	lstrcpy(nid.szTip,szTip);

	ShowWindow(hwnd,SW_HIDE);
	Shell_NotifyIcon(NIM_ADD,&nid);    //�������ͼ��
}
/*=================================================================================
*Fun Purpose://��������ʾ����
=================================================================================*/
void tray::ShowTask(HWND hwnd,UINT IconID,UINT iMouseMsg)
{
	if(IconID!=TaskIconID)return;
	switch(iMouseMsg)
	{
		case WM_LBUTTONDBLCLK://˫��
		{
			ShowWindow(hwnd,SW_SHOWNORMAL);
			SetForegroundWindow(hwnd);
			Shell_NotifyIcon(NIM_DELETE,&nid);
		}
		break;
		
		case WM_RBUTTONDOWN://�һ�
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