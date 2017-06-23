#include<shellapi.h>
#define WM_SHOWTASK WM_USER+1

class tray
{
	private:
		NOTIFYICONDATA nid;//最小化到托盘用到的记录图标信息
		int TaskIconID;
		HMENU hTrayMenu;
	public:
		void ToTray(HWND hwnd,int IconID,HMENU hMenu,TCHAR szTip[]);
		void ShowTask(HWND hwnd,UINT IconID,UINT iMouseMsg);
};
