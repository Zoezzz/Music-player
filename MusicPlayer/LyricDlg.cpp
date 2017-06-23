#include "stdafx.h"
#include "LyricDlg.h"

BOOL CALLBACK SongLyric_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			//MP3_ID3v1_STRUCT *PSongInfo=(MP3_ID3v1_STRUCT *)lParam;
			SetWindowPos(hWnd,0,920,95,330,420,SWP_NOZORDER);
			return true;
		}
		break;

		case WM_COMMAND:
		{

		}
		break;

		case WM_CLOSE:
		{	
			//EndDialog(hWnd,0);
			 DestroyWindow(hWnd);  
			 return TRUE;

		}
			break;
	}
	return FALSE;
}
