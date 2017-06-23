#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include"AddMusic.h"
#include"Music.h"
#include "LyricDlg.h"
#include <commctrl.h>
#include <stdlib.h>//�õ�������õ�
#include <ctime>//�õ�������õ�
#include <stdio.h>
#include "Tray.h"//���ص�����

#include <fstream.h>

int iExitFlag=1;	//��ǰ�ϵͳ��ť������ʱ�Ķ�����1������С������
int iVolume=500;	//��ǰ���ŵ�������С
int ShowFlag=0;		//����ұߴ���δ��
int iPlayMode=0;	//����ģʽ,��ȡ����ĺ궨��

#define ONCE 0			//����һ��
#define REPATE 1		//����ѭ��
#define RANDOM 2		//�������
#define ONCEALL 3		//ȫ��һ��
#define REPATEALL 4		//ȫ��ѭ��

song CurPlayingSong;    //��ǰ���ŵĸ���
int PlayingIndex;       //��ǰ�������б��е����

tray myTrayClass;       //��С�������̵���Ķ���

void InitWelcomSound();
void InitPlayMode(HWND hwnd);
void Initsound(HWND hwnd);
void PlayMusicInList(HWND hwnd,int index);
void StopMusic(HWND hwnd);
void RemoveSelectedMusic(HWND hwnd);
void CALLBACK AdjustProgessPos(HWND hwnd,UINT uMsg,UINT iTimeID,DWORD dwTime);
void ConvLTime2Str(LONG LTime,TCHAR *StrTime);
int  GetRandNum(int a,int b);
void WriteSettingInfo(HWND hwnd);
void ReadSettingInfo(HWND hwnd);
/*==============================================================================
*Fun Purpose:
*	Parameter:
*	Return Val:
================================================================================*/
BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd, WM_HSCROLL,Main_OnHScroll);//ˮƽ������
		HANDLE_MSG(hWnd, WM_DROPFILES,Main_OnDropFiles);
		
		case WM_CLOSE:
		{
			if(iExitFlag==1)
			{
				HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
				HMENU hTrayMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_TRAYMENU));
				hTrayMenu=GetSubMenu(hTrayMenu,0);
				myTrayClass.ToTray(hWnd,IDI_ICON,hTrayMenu,TEXT("KZoe Musicpalyer"));
			}
			else
			{
				WriteSettingInfo(hWnd);
				EndDialog(hWnd,0);
			}
		}
		break;

		case WM_SHOWTASK:
		{
			//wParam���յ���ͼ���ID����lParam���յ���������Ϊ ��С������������ʾ;
			myTrayClass.ShowTask(hWnd,wParam,lParam);
		}
		break;

		case WM_CONTEXTMENU://�Ҽ����
		{
			if((HWND)wParam==GetDlgItem(hWnd,IDC_LIST))
			{
				HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
				HMENU hPopMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_LISTMENU));
				hPopMenu=GetSubMenu(hPopMenu,0);
				
				//���ListBoxѡ�м����������ò˵���״̬
				int iSelCount=ListBox_GetSelCount(GetDlgItem(hWnd,IDC_LIST));
				if(iSelCount==0)//û��ѡ�е���
				{
					EnableMenuItem(hPopMenu,IDC_BUTTONREMOVE,MF_GRAYED);
					EnableMenuItem(hPopMenu,IDM_VIEWINFO,MF_GRAYED);
					EnableMenuItem(hPopMenu,IDM_OPENLOCATION,MF_GRAYED);
				}
				else if(iSelCount!=1)//ѡ�ж��
				{
					EnableMenuItem(hPopMenu,IDM_VIEWINFO,MF_GRAYED);
					EnableMenuItem(hPopMenu,IDM_OPENLOCATION,MF_GRAYED);
				}
				TrackPopupMenu(hPopMenu,TPM_LEFTALIGN,LOWORD(lParam),HIWORD(lParam),0,hWnd,NULL);				
				DestroyMenu(hPopMenu);
			}
		}
		break;
	}
    return FALSE;
}
/*==============================================================================
*Fun Purpose:
*	Parameter:
*	Return Val:
================================================================================*/
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//��ȡ������Ϣ
	ReadSettingInfo(hwnd);	//��ȡ������������ģʽ���б����
	
	//���ݶ�����iExitFlag���ò˵�
	HMENU hMenu=GetMenu(hwnd);
	hMenu=GetSubMenu(hMenu,1); //nInpos
	if(iExitFlag)
	{
		CheckMenuItem(hMenu,0,MF_BYPOSITION|MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hMenu,0,MF_BYPOSITION|MF_UNCHECKED);
	}
	
	//���ݶ�����ģʽ����
	if(iPlayMode==ONCE)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("����һ��"));
	}
	else if(iPlayMode==REPATE)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("����ѭ��"));
	}
	else if(iPlayMode==RANDOM)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("�������"));
	}
	else if(iPlayMode==ONCEALL)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("ȫ��һ��"));
	}
	else if(iPlayMode==REPATEALL)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("ȫ��ѭ��"));
	}

	//�������
	SetWindowText(hwnd,TEXT("KZoe MusicPlayer"));

	//����ͼ��
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HICON hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	SendMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
	SendDlgItemMessage(hwnd,IDC_PLAYPAUSE,WM_SETICON,ICON_BIG,(LPARAM)hIcon);

	//��ǰ��������
	PlayingIndex=-1;
	
	//����
	SetWindowPos(hwnd,0,122,75,450,620,SWP_NOMOVE|SWP_NOZORDER);
	
	//��ʼ������������
	Initsound(hwnd);

	//����֧����ק�ļ�
	DragAcceptFiles(hwnd,TRUE);
	
	//���Ż�ӭ����
	InitWelcomSound();

	//Ƥ��
	SkinH_Attach();
	return TRUE;
}
/*=================================================================================
*Fun Purpose:��ӭ����
=================================================================================*/
void InitWelcomSound()
{
	TCHAR ExePath[MAX_PATH];
	TCHAR WelcomSoundPath[MAX_PATH+10];
	TCHAR WelcomSoundShortPath[MAX_PATH+10];

	GetExePath(ExePath,sizeof(ExePath));
	wsprintf(WelcomSoundPath,"%s\\login.wav",ExePath);
	GetShortPathName(WelcomSoundPath,WelcomSoundShortPath,sizeof(WelcomSoundShortPath));
	TCHAR WelcomCmd[MAX_PATH+20];
	wsprintf(WelcomCmd,"play %s",WelcomSoundShortPath);
	mciSendString(WelcomCmd,"",0,NULL);
}
/*==============================================================================
*Fun Purpose:��ʼ���������ڵĹ�����
*	Parameter:
*	Return Val:
================================================================================*/
void Initsound(HWND hwnd)
{
	SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETRANGEMIN,(WPARAM)TRUE,0);       //�趨��Сֵ
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETRANGEMAX,(WPARAM)TRUE,1000);    //�趨���ֵ
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETPOS,(WPARAM)TRUE,0);            //�趨��ǰ�Ĺ���ֵ
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER), TBM_SETPAGESIZE, 0, (LPARAM)100);     //���� PAGE UP or PAGE DOWN key�Ľ���  
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER), TBM_SETLINESIZE, 0, (LPARAM)100);     //����RIGHT ARROW or DOWN ARROW key�Ľ��� 
	SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETPOS,TRUE,iVolume);              //�趨��ǰ�Ĺ���ֵ
}
/*==============================================================================
*Fun Purpose:��Ӧˮƽ�������¼�
*	Parameter:
*	Return Val:
================================================================================*/
void Main_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(hwndCtl==GetDlgItem(hwnd,IDC_SOUND_SLIDER))//��������
	{
		iVolume=SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_GETPOS,0,0);
		CurPlayingSong.SetVolume(iVolume);
	}
	else//������
	{
		if(CurPlayingSong.state==SONG_INIT || CurPlayingSong.state==SONG_STOP)
		{//���������ֹͣ״̬
			return;
		}
		if(SB_ENDSCROLL == code || SB_THUMBPOSITION==code)
		{
			LONG lProgress = SendMessage(hwndCtl, TBM_GETPOS, 0, 0);
			CurPlayingSong.JumpTo(lProgress);
		}
	}
}
/*==============================================================================
*Fun Purpose:
*	Parameter:
*	Return Val:
================================================================================*/
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HWND hwndList=GetDlgItem(hwnd,IDC_LIST);
	
	switch(id)
    {
		/********************���ſ��ư�ť*****************************************/
        case IDC_PLAYPAUSE://����/��ͣ��ť
		{
			if(CurPlayingSong.state==SONG_PLAYING)//���ڲ���
			{
				CurPlayingSong.Pause();
				SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("����"));
			}
			else if(CurPlayingSong.state==SONG_PAUSE)//��ͣ��
			{
				CurPlayingSong.Play();
				SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("��ͣ"));
			}
			else//����δ���й����������Ѿ�ֹͣ
			{
				int index=ListBox_GetCurSel(hwndList);
				PlayMusicInList(hwnd,index);
			}
		}	
        break;

		case IDC_BUTTONSTOP://ֹͣ��ť
		{
			StopMusic(hwnd);
		}
		break;

		case IDC_BUTTONNEXT://��һ��
		{
			int Count=ListBox_GetCount(hwndList);
			if(Count-1==PlayingIndex)
			{
				PlayMusicInList(hwnd,0);
			}
			else
			{
				PlayMusicInList(hwnd,PlayingIndex+1);
			}
		}
		break;

		case IDC_BUTTONPRE://��һ��
		{
			if(0==PlayingIndex)
			{
				int Count=ListBox_GetCount(hwndList);
				PlayMusicInList(hwnd,Count-1);
			}
			else
			{
				PlayMusicInList(hwnd,PlayingIndex-1);
			}
		}
		break;

		/**************�������************************************************/
		case IDC_CHECKSOUND://���þ���
		{
			HWND hwndcheck=GetDlgItem(hwnd,IDC_CHECKSOUND);
			int CheckState=Button_GetCheck(hwndcheck);

			if(BST_CHECKED==CheckState)//���þ���
			{
				CurPlayingSong.KillSound();
			}
			else if(BST_UNCHECKED==CheckState)
			{
				CurPlayingSong.ResumeSound();
			}
		}

		break;
		/*******************������ֵ���ذ�ť**************************************/
		case IDC_BUTTONADD://��Ӱ�ť
		{
			RECT rcAdd;
			GetWindowRect(GetDlgItem(hwnd,IDC_BUTTONADD),&rcAdd);
			HMENU hPopMenu=CreatePopupMenu();
			AppendMenu(hPopMenu,MF_STRING,IDC_BUTTONADDFILE,TEXT("����ļ�"));
			AppendMenu(hPopMenu,MF_STRING,IDC_ADDDIR,TEXT("����ļ���"));
			TrackPopupMenu(hPopMenu,TPM_LEFTBUTTON,rcAdd.left,rcAdd.bottom,0,hwnd,NULL);
			DestroyMenu(hPopMenu);
		}
		break;

		case IDC_BUTTONADDFILE://����ļ�
		{
			AddMusicFileToList(hwnd);
		}
		break;

		case IDC_ADDFILE://����ļ���
		{
			AddDirMusic(hwnd);
		}
		break;

		/**********************�б���صİ�ť************************************/
		//����ListBox������¼�
		case IDC_LIST:
		{
			switch(codeNotify)
			{
				case LBN_DBLCLK://���˫��
				{
					int index=ListBox_GetCurSel(hwndList);
					PlayMusicInList(hwnd,index);

					TCHAR szLyricPath[MAX_PATH];  //���·��
					ZeroMemory(szLyricPath,sizeof(szLyricPath));
				
					TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//����ȫ·��

					strcpy(szLyricPath,PszFilePathName);
					CHAR *Ptemp=strrchr(szLyricPath,'.');
					*(Ptemp+1)='l';
					*(Ptemp+2)='r';
					*(Ptemp+3)='c';

					ifstream infile(szLyricPath,ios::in);
					if(!infile)
					{
						MessageBox(hwnd,TEXT("�Ҳ�����ʣ�"),TEXT("��Ϣ"),MB_OK|MB_ICONERROR);
						return;
					}
					TCHAR sztemp[1000];
					TCHAR sttotal[10000]={0};
					int nLength;
					while(infile.getline(sztemp,1000))
					{
						nLength=lstrlen(sztemp);
						if((nLength>0&&sztemp[nLength-1]!=']')||sztemp[nLength-4]=='.')
						{
							char *temp1=strrchr(sztemp,']');
							strcat(sttotal,temp1+1);		
						}
						else
							strcat(sttotal,sztemp);
						strcat(sttotal,"\r\n");
						SetDlgItemText(hwnd,IDC_EDIT1,sttotal);
					}
				}
				break;
			}
		}
		break;
		
		case IDC_BUTTONREMOVE://�˵�---����Ŀ�Ƴ��б�
		{
			RemoveSelectedMusic(hwnd);
		}
		break;

		case IDC_BUTTONREMOVEALL://�˵�----��ղ����б�
		{
			int ret=MessageBox(hwnd,TEXT("�Ƿ���ղ����б�"),TEXT("��Ϣ"),MB_YESNO|MB_ICONQUESTION);
			if(IDYES!=ret)return;

			int Count=ListBox_GetCount(hwndList);
			for(int i=0;i<Count;i++)
			{
				free((LPTSTR)ListBox_GetItemData(hwndList,i));
			}
			ListBox_ResetContent(hwndList);
			StopMusic(hwnd);
		}
		break;
		
		case IDM_VIEWINFO://�鿴������Ϣ
		{
			BOOL WINAPI SongInfo_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//��������

			MP3_ID3v1_STRUCT SongInfo;

			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//����ȫ·��

			if(!GetMp3Info(PszFilePathName,&SongInfo))
			{
				MessageBox(hwnd,TEXT("��ȡ��Ϣʧ�ܣ�"),TEXT("��Ϣ"),MB_OK|MB_ICONERROR);
				return;
			}

			HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
			DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_SONGINFO),hwnd,SongInfo_Proc,(LPARAM)&SongInfo);
		}
		break;

		case IDM_OPENLOCATION://�򿪸���������·��
		{
			TCHAR szDirPath[MAX_PATH];//Ŀ¼·��
			ZeroMemory(szDirPath,sizeof(szDirPath));

			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//����ȫ·��

			TCHAR *Ptemp=strrchr(PszFilePathName,'\\');
			strncpy(szDirPath,PszFilePathName,Ptemp-PszFilePathName);
			//MessageBox(hwnd,szDirPath,szDirPath,0);
			ShellExecute(hwnd,TEXT("open"),szDirPath,0,0,SW_SHOWNORMAL);
		}
		break;

		/*******************����ģʽ��ذ�ť**************************************/
		case IDC_PLAYMODE://����ģʽѡ��ť
		{
			RECT rect;
			GetWindowRect(GetDlgItem(hwnd,IDC_PLAYMODE),&rect);
			HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
			HMENU hPopMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_PLAYMODEMENU));
			hPopMenu=GetSubMenu(hPopMenu,0);
			
			TrackPopupMenu(hPopMenu,TPM_LEFTBUTTON,rect.left,
				rect.bottom-10,0,hwnd,NULL);
			DestroyMenu(hPopMenu);
		}
		break;

		case IDM_PLAYONCE://�˵�--����һ��
		{
			iPlayMode=ONCE;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("����һ��"));
		}
		break;

		case IDM_PLAYREPART://�˵�--����ѭ��
		{
			iPlayMode=REPATE;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("����ѭ��"));
		}
		break;

		case IDM_PLAYRANDOM://�˵�--���ѭ��
		{
			iPlayMode=RANDOM;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("�������"));
		}
		break;

		case IDM_PLAYONCEALL://�˵�--ȫ��һ��
		{
			iPlayMode=ONCEALL;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("ȫ��һ��"));
		}
		break;

		case IDM_PLAYREPATEALL://�˵�--ȫ��ѭ��
		{
			iPlayMode=REPATEALL;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("ȫ��ѭ��"));
		}
		break;

		/***********************************************************************/
		/*case IDC_BUTTONHELP://����
		{
			MessageBox(hwnd,TEXT(""),TEXT("����"),MB_OK);
		}
		break;*/

		case IDC_SHOW://��-----��ʾ���
		{
			TCHAR szLyricPath[MAX_PATH];  //���·��
			ZeroMemory(szLyricPath,sizeof(szLyricPath));
				
			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//����ȫ·��

			strcpy(szLyricPath,PszFilePathName);
			CHAR *Ptemp=strrchr(szLyricPath,'.');
			*(Ptemp+1)='l';
			*(Ptemp+2)='r';
			*(Ptemp+3)='c';

			if(ShowFlag==0)  //show lyricdialog
			{
				ShowFlag=1;
				SetDlgItemText(hwnd,IDC_SHOW,TEXT("<"));
				SetWindowPos(hwnd,0,122,75,805,620,SWP_NOMOVE|SWP_NOZORDER);

				ifstream infile(szLyricPath,ios::in);
				if(!infile)
				{
					MessageBox(hwnd,TEXT("�Ҳ�����ʣ�"),TEXT("��Ϣ"),MB_OK|MB_ICONERROR);
					return;
				}
				TCHAR sztemp[1000];
				TCHAR sttotal[10000]={0};
				int nLength;
				while(infile.getline(sztemp,1000))
				{
					nLength=lstrlen(sztemp);
					if((nLength>0&&sztemp[nLength-1]!=']')||sztemp[nLength-4]=='.')
					{
						char *temp1=strrchr(sztemp,']');
						strcat(sttotal,temp1+1);		
					}
					else
						strcat(sttotal,sztemp);

					strcat(sttotal,"\r\n");

					SetDlgItemText(hwnd,IDC_EDIT1,sttotal);
				}		
			}
			else  
			{
				ShowFlag=0;
				SetDlgItemText(hwnd,IDC_SHOW,TEXT("��ʾ���"));
				SetWindowPos(hwnd,0,122,75,450,620,SWP_NOMOVE|SWP_NOZORDER);

			}
		}
		break;
	
		case IDC_EXIT://�˳�
		{
			WriteSettingInfo(hwnd);
		    EndDialog(hwnd, 0);
		}
		break;

		case IDC_SETSYSEXIT://����ϵͳ��ť������ʱ�Ķ���
		{
			HMENU hMenu=GetMenu(hwnd);
			hMenu=GetSubMenu(hMenu,1);
			if(iExitFlag)//�ر�ʱ��С�����̣��˵��Ѹ�ѡ
			{
				iExitFlag=0;
				CheckMenuItem(hMenu,0,MF_BYPOSITION|MF_UNCHECKED);
			}
			else
			{
				iExitFlag=1;
				CheckMenuItem(hMenu,0,MF_BYPOSITION|MF_CHECKED);
			}
		}
		break;
		default:
		break;
    }
}
/*==============================================================================
*Fun Purpose:
================================================================================*/
void RemoveSelectedMusic(HWND hwnd)
{
	HWND hwndList=GetDlgItem(hwnd,IDC_LIST);
	int count=ListBox_GetSelCount(hwndList);
	int *pSelectIndexes=new int[count];
	ListBox_GetSelItems(hwndList,count,pSelectIndexes);
	for (int i=0;i<count;i++)
	{
		pSelectIndexes[i]-=i;  //�ڴ�֮ǰ��ɾ��i����¼�������¼������ҲҪ�����ı�
		free((LPTSTR)ListBox_GetItemData(hwndList,pSelectIndexes[i]));
		ListBox_DeleteString(hwndList,pSelectIndexes[i]);
		if(pSelectIndexes[i]<PlayingIndex)  //���ڲ��ŵĸ�����������1
		{
			PlayingIndex--;
		}
		else if(pSelectIndexes[i]==PlayingIndex)  //ɾ���������ڲ��ŵ�����
		{
			StopMusic(hwnd);
		}
	}
}
/*==============================================================================
*Fun Purpose:�����б�������Ϊindex������
================================================================================*/
void PlayMusicInList(HWND hwnd,int index)
{
	if(LB_ERR==index)return;
	
	CurPlayingSong.Stop();   //��ֹͣ��ǰ����
	
	HWND hwndList=GetDlgItem(hwnd,IDC_LIST);
	ListBox_GetText(hwndList,index,CurPlayingSong.szName);
	lstrcpy(CurPlayingSong.szFilePathName,
			(TCHAR *)ListBox_GetItemData(hwndList,index));
	CurPlayingSong.Play(iVolume);  //��iVolume������ʼ����
	
	//���ò��Ž������������Сֵ
	SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGEMIN,TRUE,0);
	SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGEMAX,TRUE,CurPlayingSong.iLength);
	
	//�趨ʱ��
	TCHAR szLen[256];
	ConvLTime2Str(CurPlayingSong.iLength,szLen);
	SetDlgItemText(hwnd,IDC_TOTALTIME,szLen);
	
	//�趨��ʱ����ÿ��һ���趨��������λ��
	SetTimer(hwnd,1,1000,AdjustProgessPos);

	//���ò�����ͣ��ť��ͼ��
	SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("��ͣ"));
	
	//��ǰ���ŵ�����
	PlayingIndex=index;
	
	//���õ�ǰ�����ı��򼰳���ı���
	SetDlgItemText(hwnd,IDC_EDITPLAYING,CurPlayingSong.szName);
	TCHAR szTitle[256];
	wsprintf(szTitle,"%s--KZoe MusicPlayer",CurPlayingSong.szName);
	SetWindowText(hwnd,szTitle);
}
/*==============================================================================
*Fun Purpose:������ֹͣ
================================================================================*/
void StopMusic(HWND hwnd)
{
	CurPlayingSong.Stop();
	CurPlayingSong.Init();
	SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("����"));
	SetDlgItemText(hwnd,IDC_EDITPLAYING,TEXT("�������"));
	SetWindowText(hwnd,TEXT("KZoe Musicplayer"));
	KillTimer(hwnd,1);
}
/*========================================================================================
//�������͵�ʱ��ת����00:00:00��ʽ
==========================================================================================*/
void ConvLTime2Str(LONG LTime,TCHAR *StrTime)
{
	LTime=LTime/1000;//����LTime�����΢�������ʳ���1000
	struct Time
	{
		int hour;
		int min;
		int sec;
	}t;
	t.hour=LTime/3600;
	t.min=(LTime-t.hour*3600)/60;
	t.sec=LTime%60;

	TCHAR h[5],m[2],s[2];
	
	if(t.hour<10)
		wsprintf(h,"0%d",t.hour);
	else
		wsprintf(h,"%d",t.hour);

	if(t.min<10)
		wsprintf(m,"0%d",t.min);
	else
		wsprintf(m,"%d",t.min);

	if(t.sec<10)
		wsprintf(s,"0%d",t.sec);
	else
		wsprintf(s,"%d",t.sec);

	wsprintf(StrTime,"%s:%s:%s",h,m,s);
}
/*==============================================================================
*Fun Purpose:ÿ��һ����ã����ý�������ǰλ��
================================================================================*/
void CALLBACK AdjustProgessPos(HWND hwnd,UINT uMsg,UINT iTimeID,DWORD dwTime)
{
	long LCurTime=CurPlayingSong.GetCurPlayingPos();
	
	//���ý�����
	HWND hwndsld=GetDlgItem(hwnd,IDC_SLIDER);
	SendMessage(hwndsld,TBM_SETPOS,TRUE,LCurTime);
	
	//����ʱ���
	TCHAR szCurTime[256];
	ConvLTime2Str(LCurTime,szCurTime);
	SetDlgItemText(hwnd,IDC_PLAYTIME,szCurTime);
	
	//����Ƿ񲥷����
	if(LCurTime==0)//������Ϻ󻬿��Զ��ص���ͷ
	{
		if(ONCE==iPlayMode)//����һ��
		{
			StopMusic(hwnd);
		}//����һ��
		else if(REPATE==iPlayMode)//����ѭ��
		{
			CurPlayingSong.Play();
		}//����ѭ��
		else if(REPATEALL==iPlayMode)//ȫ��ѭ��
		{
			int Count=ListBox_GetCount(GetDlgItem(hwnd,IDC_LIST));
			if(Count-1==PlayingIndex)
			{
				PlayMusicInList(hwnd,0);
			}
			else
			{
				PlayMusicInList(hwnd,PlayingIndex+1);
			}
		}//ȫ��ѭ��
		else if(ONCEALL==iPlayMode)//ȫ��һ��
		{
			int Count=ListBox_GetCount(GetDlgItem(hwnd,IDC_LIST));
			if(Count-1==PlayingIndex)
			{
				StopMusic(hwnd);
			}
			else
			{
				PlayMusicInList(hwnd,PlayingIndex+1);
			}
		}//ȫ��һ��
		else if(RANDOM==iPlayMode)//�������
		{
			int Count=ListBox_GetCount(GetDlgItem(hwnd,IDC_LIST));
			int RanIndex=GetRandNum(0,Count-1);
			PlayMusicInList(hwnd,RanIndex);
		}
	}//if(LCurTime==0)
}
/*=================================================================================
*Fun Purpose:�õ�a,b֮��������
=================================================================================*/
int GetRandNum(int a,int b)
{
	srand((unsigned long)time(NULL));
	int num=rand();
	num=a+num%(b-a+1);
	return num;
}
/*=================================================================================
*Fun Purpose:��������Ϣд���ļ�
=================================================================================*/
void WriteSettingInfo(HWND hwnd)
{
	TCHAR exePath[MAX_PATH];
	GetExePath(exePath,sizeof(exePath));
	
	TCHAR SetFilePath[MAX_PATH+10];
	wsprintf(SetFilePath,"%s\\setting.ini",exePath);

	FILE *fp=fopen(SetFilePath,"w");
	if(NULL==fp)
	{
		showError(hwnd);
		return;
	}
	
	fprintf(fp,"%d\n",iVolume);
	fprintf(fp,"%d\n",iPlayMode);
	fprintf(fp,"%d\n",iExitFlag);

	HWND hwndList=GetDlgItem(hwnd,IDC_LIST);
	int count=ListBox_GetCount(hwndList);
	TCHAR szMusicName[MAX_PATH];
	TCHAR *PszPath;
	for(int i=0;i<count;i++)
	{
		ListBox_GetText(hwndList,i,szMusicName);
		PszPath=(TCHAR *)ListBox_GetItemData(hwndList,i);
		fprintf(fp,"%s\n",szMusicName);
		fprintf(fp,"%s\n",PszPath);
	}
	fclose(fp);
}
/*=================================================================================
*Fun Purpose:�������ļ���ȡ��Ϣ�����Ѹ�����ӵ��б�
=================================================================================*/
void ReadSettingInfo(HWND hwnd)
{
	TCHAR exePath[MAX_PATH];
	GetExePath(exePath,sizeof(exePath));
	TCHAR SetFilePath[MAX_PATH+10];
	wsprintf(SetFilePath,"%s\\setting.ini",exePath);

	FILE *fp=fopen(SetFilePath,"r");
	if(NULL==fp)
	{
		showError(hwnd);
		return;
	}

	fscanf(fp,"%d\n",&iVolume);
	fscanf(fp,"%d\n",&iPlayMode);
	fscanf(fp,"%d\n",&iExitFlag);

	int index;
	TCHAR szMusicName[MAX_PATH];
	TCHAR szTempPath[2*MAX_PATH];
	TCHAR *PszPath=(TCHAR *)malloc(2*MAX_PATH);

	while(fgets(szMusicName,MAX_PATH,fp)!=NULL)
	{
		//������fscanf ��Ϊ�пո񡣡�
		fgets(szTempPath,2*MAX_PATH,fp);
		szMusicName[strlen(szMusicName)-1]='\0';
		szTempPath[strlen(szTempPath)-1]='\0';
		lstrcpy(PszPath,szTempPath);

		index=SendDlgItemMessage(hwnd,IDC_LIST,LB_INSERTSTRING,-1,(LPARAM)szMusicName);
		SendDlgItemMessage(hwnd,IDC_LIST,LB_SETITEMDATA,index,(LPARAM)PszPath);
		PszPath=(TCHAR *)malloc(2*MAX_PATH);
	}
	fclose(fp);
}
