#include <shellapi.h>  // ��ק�ļ�ʱ�õ���HDROP�����ڴ��ļ���Ҫд��windowsx.h֮ǰ��HANDLE_WM_DROPFILES���ܱ�ʶ��
#include <windowsx.h>
#include<commdlg.h>

void InitOFN(HWND hwnd,OPENFILENAME *Pofn,TCHAR OpenSaveFileName[]);
BOOL AddMusicFileToList(HWND hwnd);

BOOL AddDirMusic(HWND hwnd);
BOOL SearchFileAddtoList(HWND hwnd,TCHAR path[],TCHAR format[]);

void Main_OnDropFiles(HWND hwnd,HDROP hdrop);