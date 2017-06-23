#include"stdafx.h"
#include"resource.h"
#include<winuser.h>
#include "AddMusic.h"
#include "Music.h"

#include<Shlobj.h>//���Ŀ¼��ʹ��

/*==============================================================================
*Fun Purpose:��ʼ��OPENFILENAME�ṹ��
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
*Fun Purpose:��Ӹ����ļ�
*	Parameter:
*	Return Val:
================================================================================*/
BOOL AddMusicFileToList(HWND hwnd)
{
	HWND hwndlist=GetDlgItem(hwnd,IDC_LIST);
	
	OPENFILENAME ofn;
	TCHAR szOpenFileName[50*MAX_PATH];
	//��ʼ��ofn
	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.lpstrFile=szOpenFileName;
	ofn.lpstrFile[0]=TEXT('\0');
	ofn.nMaxFile=sizeof(szOpenFileName);
	ofn.hwndOwner=hwnd;
	ofn.lpstrFileTitle=NULL;
	ofn.lpstrFilter=TEXT("all\0*.*\0������Ƶ(mp3,wma,wav)\0*.mp3;*.wma;*.wav\0");
	ofn.nFilterIndex=2;
	ofn.lpstrTitle="��Ӹ���";
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	
	if(GetOpenFileName(&ofn))
	{
		int index;
		TCHAR szMusicName[MAX_PATH];
		LPTSTR PszFilePathName;//����ȫ·��(����������)

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
					{	//��ȡ����ļ�ʱ�����ص�OpenSaveFileNameҪ���̷���Ŀ¼���ǡ�F:\��������ʽ
						//�����ľ��ǡ�F:\Music��������ʽ��
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
		
		if(i==len)//ֻѡ��һ���ļ�
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
		SHGetPathFromIDList(lpitem,Buffer);//�õ�Ŀ¼��·������Buffer�� 
		flag=SearchFileAddtoList(hwnd,Buffer,"*.mp3");  
		flag=SearchFileAddtoList(hwnd,Buffer,"*.wma");
		flag=SearchFileAddtoList(hwnd,Buffer,"*.wav");
	}
	return flag;
}
/*==============================================================================
*Fun Purpose://���Ŀ¼ʱ�õ���ѡ��ָ��·���������ļ�
*	Parameter:
*	Return Val:
================================================================================*/
BOOL SearchFileAddtoList(HWND hwnd,TCHAR path[],TCHAR format[])
{
	TCHAR szMusicName[MAX_PATH];
	LPTSTR PszFilePathName;//����ȫ·��(����������)

	WIN32_FIND_DATA FindFileData; 
	char SearchFormat[MAX_PATH+15];
	wsprintf(SearchFormat,"%s\\%s",path,format);
	HANDLE hFind=FindFirstFile(SearchFormat, &FindFileData);     //ע��Ŀ¼����ʽ����Ҫ��*.*
	
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
*Fun Purpose:��Ӧ�϶��ļ�
*	Parameter:
*	Return Val:
================================================================================*/
void Main_OnDropFiles(HWND hwnd, HDROP hDropInfo)
{
	/* ----    DragQueryFileԭ��Ϊ��     
    
   UINT    DragQueryFile    (HDROP    hDrop,    UINT    iFile,    LPTSTR    lpszFile,    UINTcch)   
   ----    ����hDrop��ָ���϶��ļ��ṹ�ľ����     
   ----    iFile    ����0��ʼ�ı��ж��ļ�����ţ���Ϊһ�ο����϶�����ļ������˲���     
		   ��Ϊ0xFFFFFFFF�����������϶��ļ���������     
   ----    lpszFile    ��ָ���ļ����Ļ�������     
   ----    cch    ���ļ����������Ĵ�С�����ļ������ַ�����     
	*/

	void PlayMusicInList(HWND hwnd,int index);//����������������MainDlg.cpp�еĺ���

	LPTSTR PszFilePathName;//����ȫ·��(����������)

	int index=-1;
	HWND hwndlist=GetDlgItem(hwnd,IDC_LIST);
	//�õ��ļ�����      
    UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);   
    for (UINT nIndex=0 ; nIndex< nNumOfFiles; nIndex++)
	{   
		PszFilePathName=(LPTSTR)malloc(2*MAX_PATH);
        //�õ��ļ���    
        DragQueryFile(hDropInfo, nIndex, (LPTSTR)PszFilePathName, _MAX_PATH);
		//�����������б�
		TCHAR *pszMusicName=strrchr(PszFilePathName,'\\');
		index=SendMessage(hwndlist,LB_INSERTSTRING,-1,(LPARAM)(pszMusicName+1));
		SendMessage(hwndlist,LB_SETITEMDATA,index,(LPARAM)PszFilePathName);
	}
	PlayMusicInList(hwnd,index);
   	DragFinish(hDropInfo); 
}