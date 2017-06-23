#include"stdafx.h"
#include"resource.h"
#include<winuser.h>
#include "AddMusic.h"
#include "Music.h"

#include<Shlobj.h>//添加目录是使用

/*==============================================================================
*Fun Purpose:初始化OPENFILENAME结构体
*	Parameter:
*	Return Val:
================================================================================*/
void InitOFN(HWND hwnd,OPENFILENAME *Pofn,TCHAR OpenSaveFileName[])
{
	ZeroMemory(Pofn,sizeof(OPENFILENAME));
	
	Pofn->lStructSize=sizeof(OPENFILENAME);
	Pofn->lpstrFile=OpenSaveFileName;
	Pofn->lpstrFile[0]=TEXT('\0');
	Pofn->nMaxFile=sizeof(OpenSaveFileName);
	Pofn->hwndOwner=hwnd;
	Pofn->lpstrFileTitle=NULL;
}
/*==============================================================================
*Fun Purpose:添加歌曲文件
*	Parameter:
*	Return Val:
================================================================================*/
BOOL AddMusicFileToList(HWND hwnd)
{
	HWND hwndlist=GetDlgItem(hwnd,IDC_LIST);
	
	OPENFILENAME ofn;
	TCHAR szOpenFileName[50*MAX_PATH];
	//初始化ofn
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.lpstrFile=szOpenFileName;
	ofn.lpstrFile[0]=TEXT('\0');
	ofn.nMaxFile=sizeof(szOpenFileName);
	ofn.hwndOwner=hwnd;
	ofn.lpstrFileTitle=NULL;
	ofn.lpstrFilter=TEXT("all\0*.*\0常见音频(mp3,wma,wav)\0*.mp3;*.wma;*.wav\0");
	ofn.nFilterIndex=2;
	ofn.lpstrTitle="添加歌曲";
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	
	if(GetOpenFileName(&ofn))
	{
		int index;
		TCHAR szMusicName[MAX_PATH];
		LPTSTR PszFilePathName;//歌曲全路径(包含歌曲名)

		int len=strlen(szOpenFileName);
		int i=len,j;
		do
		{
			if(szOpenFileName[i+1]=='\0')
			{
				break;
			}
			else
			{
				PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
				for(j=0;;j++)
				{
					i++;
					szMusicName[j]=szOpenFileName[i];
					if(szOpenFileName[i]=='\0')
					{	//读取多个文件时，返回的OpenSaveFileName要是盘符根目录就是“F:\”这种形式
						//其他的就是“F:\Music”这种形式了
						if(szOpenFileName[len-1]=='\\')
						{
							wsprintf(PszFilePathName,"%s%s",szOpenFileName,szMusicName);
						}
						else
						{
							wsprintf(PszFilePathName,"%s\\%s",szOpenFileName,szMusicName);
						}
						index=SendMessage(hwndlist,LB_INSERTSTRING,-1,(LPARAM)szMusicName);
						SendMessage(hwndlist,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);						
						break;
					}//if(szOpenFileName[i]=='\0')
				}//for(j=0;;j++)
			}//else
		}while(1);
		
		if(i==len)//只选中一个文件
		{
			TCHAR *pszMusicName=strrchr(szOpenFileName,'\\');
			index=SendMessage(hwndlist,LB_INSERTSTRING,-1,(LPARAM)(pszMusicName+1));
		
			PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
			lstrcpy(PszFilePathName,szOpenFileName);
			SendMessage(hwndlist,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);
		}//if(i==len)
	}//if(GetOpenFileName(&ofn))
	return TRUE;
}
/*==============================================================================
*Fun Purpose:
*	Parameter:
*	Return Val:
================================================================================*/
BOOL AddDirMusic(HWND hwnd)
{
	BOOL flag=TRUE;
	
	BROWSEINFO NewBro;
	char   Buffer[MAX_PATH];
	LPITEMIDLIST   lpitem;
	RtlZeroMemory(&Buffer,sizeof(Buffer));
	RtlZeroMemory(&NewBro,sizeof(BROWSEINFO));   
	NewBro.hwndOwner=hwnd;   
	NewBro.ulFlags=BIF_RETURNONLYFSDIRS;       
	lpitem=SHBrowseForFolder(&NewBro);   
	if(lpitem!=NULL)     
	{ 
		SHGetPathFromIDList(lpitem,Buffer);//得到目录的路劲放在Buffer中 
		flag=SearchFileAddtoList(hwnd,Buffer,"*.mp3");  
		flag=SearchFileAddtoList(hwnd,Buffer,"*.wma");
		flag=SearchFileAddtoList(hwnd,Buffer,"*.wav");
	}
	return flag;
}
/*==============================================================================
*Fun Purpose://添加目录时用到、选出指定路径的音乐文件
*	Parameter:
*	Return Val:
================================================================================*/
BOOL SearchFileAddtoList(HWND hwnd,TCHAR path[],TCHAR format[])
{
	TCHAR szMusicName[MAX_PATH];
	LPTSTR PszFilePathName;//歌曲全路径(包含歌曲名)

	WIN32_FIND_DATA FindFileData; 
	char SearchFormat[MAX_PATH+15];
	wsprintf(SearchFormat,"%s\\%s",path,format);
	HANDLE hFind=FindFirstFile(SearchFormat, &FindFileData);     //注意目录的形式后面要有*.*
	
	if (hFind==INVALID_HANDLE_VALUE) 
	{
		return FALSE;
	} 
	else 
	{
		PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
		strcpy(szMusicName,FindFileData.cFileName);
		wsprintf(PszFilePathName,"%s\\%s",path,szMusicName);
		int index;
		index=SendDlgItemMessage(hwnd,IDC_LIST,LB_INSERTSTRING,-1,(LPARAM)szMusicName);
		SendDlgItemMessage(hwnd,IDC_LIST,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);
	}
	while(1)
	{
		FindNextFile(hFind,&FindFileData);
		if(hFind == INVALID_HANDLE_VALUE) 
		{
			MessageBox(hwnd,TEXT("Invalid File Handle"),TEXT("Error"),0);
			return FALSE;
		} 
		else 
		{
			if(!strcmp(szMusicName,FindFileData.cFileName))
			{
				break;
			}
			else
			{
				PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
				strcpy(szMusicName,FindFileData.cFileName);
				wsprintf(PszFilePathName,"%s\\%s",path,szMusicName);
				int index;
				index=SendDlgItemMessage(hwnd,IDC_LIST,LB_INSERTSTRING,-1,(LPARAM)szMusicName);
				SendDlgItemMessage(hwnd,IDC_LIST,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);
			}
		}//else
	}//while(1)
	FindClose(hFind);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/*==============================================================================
*Fun Purpose:响应拖动文件
*	Parameter:
*	Return Val:
================================================================================*/
void Main_OnDropFiles(HWND hwnd, HDROP hDropInfo)
{
	/* ----    DragQueryFile原型为：     
    
   UINT    DragQueryFile    (HDROP    hDrop,    UINT    iFile,    LPTSTR    lpszFile,    UINTcch)   
   ----    其中hDrop是指向拖动文件结构的句柄；     
   ----    iFile    是以0开始的被托动文件的序号，因为一次可能拖动多个文件。当此参数     
		   设为0xFFFFFFFF，则函数返回拖动文件的数量；     
   ----    lpszFile    是指向文件名的缓冲区；     
   ----    cch    是文件名缓冲区的大小，即文件名的字符数。     
	*/

	void PlayMusicInList(HWND hwnd,int index);//函数声明，定义在MainDlg.cpp中的函数

	LPTSTR PszFilePathName;//歌曲全路径(包含歌曲名)

	int index=-1;
	HWND hwndlist=GetDlgItem(hwnd,IDC_LIST);
	//得到文件个数      
    UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);   
    for (UINT nIndex=0 ; nIndex< nNumOfFiles; nIndex++)
	{   
		PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
        //得到文件名    
        DragQueryFile(hDropInfo, nIndex, (LPTSTR)PszFilePathName, _MAX_PATH);
		//将歌曲加入列表
		TCHAR *pszMusicName=strrchr(PszFilePathName,'\\');
		index=SendMessage(hwndlist,LB_INSERTSTRING,-1,(LPARAM)(pszMusicName+1));
		SendMessage(hwndlist,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);
	}
	PlayMusicInList(hwnd,index);
   	DragFinish(hDropInfo); 
}