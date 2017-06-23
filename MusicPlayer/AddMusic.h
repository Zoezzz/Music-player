#include <shellapi.h>  // 拖拽文件时用到的HDROP定义在此文件（要写在windowsx.h之前，HANDLE_WM_DROPFILES才能被识别）
#include <windowsx.h>
#include<commdlg.h>

void InitOFN(HWND hwnd,OPENFILENAME *Pofn,TCHAR OpenSaveFileName[]);
BOOL AddMusicFileToList(HWND hwnd);

BOOL AddDirMusic(HWND hwnd);
BOOL SearchFileAddtoList(HWND hwnd,TCHAR path[],TCHAR format[]);

void Main_OnDropFiles(HWND hwnd,HDROP hdrop);