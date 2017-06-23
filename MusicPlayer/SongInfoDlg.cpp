#include "stdafx.h"
#include "Music.h"
#include "resource.h"

BOOL WINAPI SongInfo_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			MP3_ID3v1_STRUCT *PSongInfo=(MP3_ID3v1_STRUCT *)lParam;
			
			SetDlgItemText(hWnd,IDC_EDITSONGNAME,PSongInfo->Title);
			SetDlgItemText(hWnd,IDC_EDITARTIST,PSongInfo->Artist);
			SetDlgItemText(hWnd,IDC_EDITALBUM,PSongInfo->Album);
			SetDlgItemText(hWnd,IDC_EDITYEAR,PSongInfo->Year);
			SetDlgItemText(hWnd,IDC_EDITOTHERINFO,PSongInfo->Comment);
		}
		break;

		/*case WM_COMMAND:
		{
			if(IDC_EXIT==LOWORD(wParam))
			{
				EndDialog(hWnd,0);
			}
		}
		break;*/

		case WM_CLOSE:
			EndDialog(hWnd,0);
			break;
	}
	return FALSE;
}
