#include<shellapi.h>
#define WM_SHOWTASK WM_USER+1

class tray
{
	private:
		NOTIFYICONDATA nid;//��С���������õ��ļ�¼ͼ����Ϣ
		int TaskIconID;
		HMENU hTrayMenu;
	public:
		void ToTray(HWND hwnd,int IconID,HMENU hMenu,TCHAR szTip[]);
		void ShowTask(HWND hwnd,UINT IconID,UINT iMouseMsg);
};
