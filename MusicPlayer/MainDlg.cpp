#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include"AddMusic.h"
#include"Music.h"
#include "LyricDlg.h"
#include <commctrl.h>
#include <stdlib.h>//得到随机数用到
#include <ctime>//得到随机数用到
#include <stdio.h>
#include "Tray.h"//隐藏到托盘

#include <fstream.h>

int iExitFlag=1;	//标记按系统按钮“×”时的动作，1代表最小到托盘
int iVolume=500;	//当前播放的音量大小
int ShowFlag=0;		//标记右边窗体未打开
int iPlayMode=0;	//播放模式,可取下面的宏定义

#define ONCE 0			//单曲一次
#define REPATE 1		//单曲循环
#define RANDOM 2		//随机播放
#define ONCEALL 3		//全部一次
#define REPATEALL 4		//全部循环

song CurPlayingSong;    //当前播放的歌曲
int PlayingIndex;       //当前播放在列表中的序号

tray myTrayClass;       //最小化到托盘的类的对象

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
		HANDLE_MSG(hWnd, WM_HSCROLL,Main_OnHScroll);//水平滚动条
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
			//wParam接收的是图标的ID，而lParam接收的是鼠标的行为 最小化任务栏的显示;
			myTrayClass.ShowTask(hWnd,wParam,lParam);
		}
		break;

		case WM_CONTEXTMENU://右键点击
		{
			if((HWND)wParam==GetDlgItem(hWnd,IDC_LIST))
			{
				HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
				HMENU hPopMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_LISTMENU));
				hPopMenu=GetSubMenu(hPopMenu,0);
				
				//检查ListBox选中几个，并设置菜单的状态
				int iSelCount=ListBox_GetSelCount(GetDlgItem(hWnd,IDC_LIST));
				if(iSelCount==0)//没有选中的项
				{
					EnableMenuItem(hPopMenu,IDC_BUTTONREMOVE,MF_GRAYED);
					EnableMenuItem(hPopMenu,IDM_VIEWINFO,MF_GRAYED);
					EnableMenuItem(hPopMenu,IDM_OPENLOCATION,MF_GRAYED);
				}
				else if(iSelCount!=1)//选中多个
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
	//读取配置信息
	ReadSettingInfo(hwnd);	//读取到声音、播放模式、列表歌曲
	
	//根据读出的iExitFlag设置菜单
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
	
	//根据读出的模式设置
	if(iPlayMode==ONCE)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("单曲一次"));
	}
	else if(iPlayMode==REPATE)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("单曲循环"));
	}
	else if(iPlayMode==RANDOM)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("随机播放"));
	}
	else if(iPlayMode==ONCEALL)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("全部一次"));
	}
	else if(iPlayMode==REPATEALL)
	{
		SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("全部循环"));
	}

	//界面标题
	SetWindowText(hwnd,TEXT("KZoe MusicPlayer"));

	//界面图标
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HICON hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	SendMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
	SendDlgItemMessage(hwnd,IDC_PLAYPAUSE,WM_SETICON,ICON_BIG,(LPARAM)hIcon);

	//当前播放索引
	PlayingIndex=-1;
	
	//窗体
	SetWindowPos(hwnd,0,122,75,450,620,SWP_NOMOVE|SWP_NOZORDER);
	
	//初始化声音滚动条
	Initsound(hwnd);

	//设置支持拖拽文件
	DragAcceptFiles(hwnd,TRUE);
	
	//播放欢迎声音
	InitWelcomSound();

	//皮肤
	SkinH_Attach();
	return TRUE;
}
/*=================================================================================
*Fun Purpose:欢迎声音
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
*Fun Purpose:初始化音量调节的滚动条
*	Parameter:
*	Return Val:
================================================================================*/
void Initsound(HWND hwnd)
{
	SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETRANGEMIN,(WPARAM)TRUE,0);       //设定最小值
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETRANGEMAX,(WPARAM)TRUE,1000);    //设定最大值
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETPOS,(WPARAM)TRUE,0);            //设定当前的滚动值
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER), TBM_SETPAGESIZE, 0, (LPARAM)100);     //设置 PAGE UP or PAGE DOWN key的进度  
    SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER), TBM_SETLINESIZE, 0, (LPARAM)100);     //设置RIGHT ARROW or DOWN ARROW key的进度 
	SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_SETPOS,TRUE,iVolume);              //设定当前的滚动值
}
/*==============================================================================
*Fun Purpose:响应水平滚动条事件
*	Parameter:
*	Return Val:
================================================================================*/
void Main_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(hwndCtl==GetDlgItem(hwnd,IDC_SOUND_SLIDER))//调节声音
	{
		iVolume=SendMessage(GetDlgItem(hwnd,IDC_SOUND_SLIDER),TBM_GETPOS,0,0);
		CurPlayingSong.SetVolume(iVolume);
	}
	else//进度条
	{
		if(CurPlayingSong.state==SONG_INIT || CurPlayingSong.state==SONG_STOP)
		{//如果现在是停止状态
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
		/********************播放控制按钮*****************************************/
        case IDC_PLAYPAUSE://播放/暂停按钮
		{
			if(CurPlayingSong.state==SONG_PLAYING)//正在播放
			{
				CurPlayingSong.Pause();
				SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("播放"));
			}
			else if(CurPlayingSong.state==SONG_PAUSE)//暂停中
			{
				CurPlayingSong.Play();
				SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("暂停"));
			}
			else//歌曲未进行过操作或者已经停止
			{
				int index=ListBox_GetCurSel(hwndList);
				PlayMusicInList(hwnd,index);
			}
		}	
        break;

		case IDC_BUTTONSTOP://停止按钮
		{
			StopMusic(hwnd);
		}
		break;

		case IDC_BUTTONNEXT://下一曲
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

		case IDC_BUTTONPRE://上一曲
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

		/**************声音相关************************************************/
		case IDC_CHECKSOUND://设置静音
		{
			HWND hwndcheck=GetDlgItem(hwnd,IDC_CHECKSOUND);
			int CheckState=Button_GetCheck(hwndcheck);

			if(BST_CHECKED==CheckState)//设置静音
			{
				CurPlayingSong.KillSound();
			}
			else if(BST_UNCHECKED==CheckState)
			{
				CurPlayingSong.ResumeSound();
			}
		}

		break;
		/*******************添加音乐的相关按钮**************************************/
		case IDC_BUTTONADD://添加按钮
		{
			RECT rcAdd;
			GetWindowRect(GetDlgItem(hwnd,IDC_BUTTONADD),&rcAdd);
			HMENU hPopMenu=CreatePopupMenu();
			AppendMenu(hPopMenu,MF_STRING,IDC_BUTTONADDFILE,TEXT("添加文件"));
			AppendMenu(hPopMenu,MF_STRING,IDC_ADDDIR,TEXT("添加文件夹"));
			TrackPopupMenu(hPopMenu,TPM_LEFTBUTTON,rcAdd.left,rcAdd.bottom,0,hwnd,NULL);
			DestroyMenu(hPopMenu);
		}
		break;

		case IDC_BUTTONADDFILE://添加文件
		{
			AddMusicFileToList(hwnd);
		}
		break;

		case IDC_ADDFILE://添加文件夹
		{
			AddDirMusic(hwnd);
		}
		break;

		/**********************列表相关的按钮************************************/
		//处理ListBox的鼠标事件
		case IDC_LIST:
		{
			switch(codeNotify)
			{
				case LBN_DBLCLK://鼠标双击
				{
					int index=ListBox_GetCurSel(hwndList);
					PlayMusicInList(hwnd,index);

					TCHAR szLyricPath[MAX_PATH];  //歌词路径
					ZeroMemory(szLyricPath,sizeof(szLyricPath));
				
					TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//歌曲全路径

					strcpy(szLyricPath,PszFilePathName);
					CHAR *Ptemp=strrchr(szLyricPath,'.');
					*(Ptemp+1)='l';
					*(Ptemp+2)='r';
					*(Ptemp+3)='c';

					ifstream infile(szLyricPath,ios::in);
					if(!infile)
					{
						MessageBox(hwnd,TEXT("找不到歌词！"),TEXT("消息"),MB_OK|MB_ICONERROR);
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
		
		case IDC_BUTTONREMOVE://菜单---将曲目移出列表
		{
			RemoveSelectedMusic(hwnd);
		}
		break;

		case IDC_BUTTONREMOVEALL://菜单----清空播放列表
		{
			int ret=MessageBox(hwnd,TEXT("是否清空播放列表？"),TEXT("消息"),MB_YESNO|MB_ICONQUESTION);
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
		
		case IDM_VIEWINFO://查看歌曲信息
		{
			BOOL WINAPI SongInfo_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//函数声明

			MP3_ID3v1_STRUCT SongInfo;

			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//歌曲全路径

			if(!GetMp3Info(PszFilePathName,&SongInfo))
			{
				MessageBox(hwnd,TEXT("获取信息失败！"),TEXT("消息"),MB_OK|MB_ICONERROR);
				return;
			}

			HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
			DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_SONGINFO),hwnd,SongInfo_Proc,(LPARAM)&SongInfo);
		}
		break;

		case IDM_OPENLOCATION://打开歌曲的所在路径
		{
			TCHAR szDirPath[MAX_PATH];//目录路径
			ZeroMemory(szDirPath,sizeof(szDirPath));

			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//歌曲全路径

			TCHAR *Ptemp=strrchr(PszFilePathName,'\\');
			strncpy(szDirPath,PszFilePathName,Ptemp-PszFilePathName);
			//MessageBox(hwnd,szDirPath,szDirPath,0);
			ShellExecute(hwnd,TEXT("open"),szDirPath,0,0,SW_SHOWNORMAL);
		}
		break;

		/*******************播放模式相关按钮**************************************/
		case IDC_PLAYMODE://播放模式选择按钮
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

		case IDM_PLAYONCE://菜单--单曲一次
		{
			iPlayMode=ONCE;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("单曲一次"));
		}
		break;

		case IDM_PLAYREPART://菜单--单曲循环
		{
			iPlayMode=REPATE;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("单曲循环"));
		}
		break;

		case IDM_PLAYRANDOM://菜单--随机循环
		{
			iPlayMode=RANDOM;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("随机播放"));
		}
		break;

		case IDM_PLAYONCEALL://菜单--全部一次
		{
			iPlayMode=ONCEALL;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("全部一次"));
		}
		break;

		case IDM_PLAYREPATEALL://菜单--全部循环
		{
			iPlayMode=REPATEALL;
			SetDlgItemText(hwnd,IDC_STATICPLAYMODE,TEXT("全部循环"));
		}
		break;

		/***********************************************************************/
		/*case IDC_BUTTONHELP://关于
		{
			MessageBox(hwnd,TEXT(""),TEXT("关于"),MB_OK);
		}
		break;*/

		case IDC_SHOW://【-----显示歌词
		{
			TCHAR szLyricPath[MAX_PATH];  //歌词路径
			ZeroMemory(szLyricPath,sizeof(szLyricPath));
				
			int index=ListBox_GetCurSel(hwndList);
			TCHAR *PszFilePathName=(TCHAR *)ListBox_GetItemData(hwndList,index);//歌曲全路径

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
					MessageBox(hwnd,TEXT("找不到歌词！"),TEXT("消息"),MB_OK|MB_ICONERROR);
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
				SetDlgItemText(hwnd,IDC_SHOW,TEXT("显示歌词"));
				SetWindowPos(hwnd,0,122,75,450,620,SWP_NOMOVE|SWP_NOZORDER);

			}
		}
		break;
	
		case IDC_EXIT://退出
		{
			WriteSettingInfo(hwnd);
		    EndDialog(hwnd, 0);
		}
		break;

		case IDC_SETSYSEXIT://设置系统按钮“×”时的动作
		{
			HMENU hMenu=GetMenu(hwnd);
			hMenu=GetSubMenu(hMenu,1);
			if(iExitFlag)//关闭时最小到托盘，菜单已复选
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
		pSelectIndexes[i]-=i;  //在此之前已删除i条记录，下面记录的索引也要发生改变
		free((LPTSTR)ListBox_GetItemData(hwndList,pSelectIndexes[i]));
		ListBox_DeleteString(hwndList,pSelectIndexes[i]);
		if(pSelectIndexes[i]<PlayingIndex)  //正在播放的歌曲的索引减1
		{
			PlayingIndex--;
		}
		else if(pSelectIndexes[i]==PlayingIndex)  //删除的是正在播放的音乐
		{
			StopMusic(hwnd);
		}
	}
}
/*==============================================================================
*Fun Purpose:播放列表中索引为index的音乐
================================================================================*/
void PlayMusicInList(HWND hwnd,int index)
{
	if(LB_ERR==index)return;
	
	CurPlayingSong.Stop();   //先停止当前播放
	
	HWND hwndList=GetDlgItem(hwnd,IDC_LIST);
	ListBox_GetText(hwndList,index,CurPlayingSong.szName);
	lstrcpy(CurPlayingSong.szFilePathName,
			(TCHAR *)ListBox_GetItemData(hwndList,index));
	CurPlayingSong.Play(iVolume);  //以iVolume音量开始播放
	
	//设置播放进度条的最大最小值
	SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGEMIN,TRUE,0);
	SendDlgItemMessage(hwnd,IDC_SLIDER,TBM_SETRANGEMAX,TRUE,CurPlayingSong.iLength);
	
	//设定时间
	TCHAR szLen[256];
	ConvLTime2Str(CurPlayingSong.iLength,szLen);
	SetDlgItemText(hwnd,IDC_TOTALTIME,szLen);
	
	//设定计时器，每隔一秒设定进度条的位置
	SetTimer(hwnd,1,1000,AdjustProgessPos);

	//设置播放暂停按钮的图标
	SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("暂停"));
	
	//当前播放的索引
	PlayingIndex=index;
	
	//设置当前播放文本框及程序的标题
	SetDlgItemText(hwnd,IDC_EDITPLAYING,CurPlayingSong.szName);
	TCHAR szTitle[256];
	wsprintf(szTitle,"%s--KZoe MusicPlayer",CurPlayingSong.szName);
	SetWindowText(hwnd,szTitle);
}
/*==============================================================================
*Fun Purpose:将音乐停止
================================================================================*/
void StopMusic(HWND hwnd)
{
	CurPlayingSong.Stop();
	CurPlayingSong.Init();
	SetDlgItemText(hwnd,IDC_PLAYPAUSE,TEXT("播放"));
	SetDlgItemText(hwnd,IDC_EDITPLAYING,TEXT("播放完毕"));
	SetWindowText(hwnd,TEXT("KZoe Musicplayer"));
	KillTimer(hwnd,1);
}
/*========================================================================================
//将长整型的时间转换成00:00:00形式
==========================================================================================*/
void ConvLTime2Str(LONG LTime,TCHAR *StrTime)
{
	LTime=LTime/1000;//由于LTime代表的微秒数，故除以1000
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
*Fun Purpose:每隔一秒调用，设置进度条当前位置
================================================================================*/
void CALLBACK AdjustProgessPos(HWND hwnd,UINT uMsg,UINT iTimeID,DWORD dwTime)
{
	long LCurTime=CurPlayingSong.GetCurPlayingPos();
	
	//设置进度条
	HWND hwndsld=GetDlgItem(hwnd,IDC_SLIDER);
	SendMessage(hwndsld,TBM_SETPOS,TRUE,LCurTime);
	
	//设置时间框
	TCHAR szCurTime[256];
	ConvLTime2Str(LCurTime,szCurTime);
	SetDlgItemText(hwnd,IDC_PLAYTIME,szCurTime);
	
	//检查是否播放完毕
	if(LCurTime==0)//播放完毕后滑块自动回到开头
	{
		if(ONCE==iPlayMode)//单曲一次
		{
			StopMusic(hwnd);
		}//单曲一次
		else if(REPATE==iPlayMode)//单曲循环
		{
			CurPlayingSong.Play();
		}//单曲循环
		else if(REPATEALL==iPlayMode)//全部循环
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
		}//全部循环
		else if(ONCEALL==iPlayMode)//全部一次
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
		}//全部一次
		else if(RANDOM==iPlayMode)//随机播放
		{
			int Count=ListBox_GetCount(GetDlgItem(hwnd,IDC_LIST));
			int RanIndex=GetRandNum(0,Count-1);
			PlayMusicInList(hwnd,RanIndex);
		}
	}//if(LCurTime==0)
}
/*=================================================================================
*Fun Purpose:得到a,b之间的随机数
=================================================================================*/
int GetRandNum(int a,int b)
{
	srand((unsigned long)time(NULL));
	int num=rand();
	num=a+num%(b-a+1);
	return num;
}
/*=================================================================================
*Fun Purpose:将配置信息写入文件
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
*Fun Purpose:从配置文件读取信息，并把歌曲添加到列表
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
		//不能用fscanf 因为有空格。。
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
