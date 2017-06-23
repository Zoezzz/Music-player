// MusicPlayerV3.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <shellapi.h>
#include<commctrl.h>
#include"resource.h"
#include"MainDlg.h"
#pragma comment(lib,"comctl32.lib")
//下面在判断进程是否允许时用到
#include <tlhelp32.h>
BOOL JudgeIfRuning();
/*==============================================================================
*Fun Purpose:
================================================================================*/
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	InitCommonControls();
	//判断之前是否程序在运行
	
	if(JudgeIfRuning())
	{
		MessageBox(NULL,TEXT("软件正在运行"),TEXT("消息"),0);
	}
	else
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Main_Proc);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
/*=================================================================================
*Fun Purpose://因为程序的标题是变化的，不可预料的，所以就查找进程
=================================================================================*/
BOOL JudgeIfRuning()
{
	//得到可执行文件的名字
	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL,szExeFileName,sizeof(szExeFileName));
	TCHAR *Psolidus=strrchr(szExeFileName,'\\');

	int flagNum=0;//标记此名称的进程数目。
				//因为新开的程序正在运行，所以如果之前有程序运行，进程此时有两个
	//查找进程列表
	HANDLE hSnapProcess;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	hSnapProcess=(HANDLE)CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//进程快照
	if(hSnapProcess == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,TEXT("创建快照失败!!"),TEXT("错误"),MB_OK);
		CloseHandle(hSnapProcess);
		return FALSE;
	}
	
	if(!Process32First(hSnapProcess,&pe32))//查找第一个进程
	{
		MessageBox(NULL,TEXT("Process32First失败!!"),TEXT("错误"),MB_OK);
		CloseHandle(hSnapProcess);
		return FALSE;
	}
	else
	{
		int i=0;
		do //GetModuleFileNameEx(hProcess,NULL,szProcessName,MAX_PATH);  //szProcessName进程路径
		{
			if(lstrcmp(pe32.szExeFile,Psolidus+1)==0)//进程名（可执行文件名）
			{
				if(flagNum==1)//之前已经找到了一个.现在找到第二个进程
				{
					return TRUE;
				}
				else
				{
					flagNum++;
				}
			}
		} while (Process32Next(hSnapProcess,&pe32));//循环遍历各进程
	}
	CloseHandle(hSnapProcess);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*==============================================================================
*Fun Purpose:显示未知的错误信息
*	Parameter:
*	Return Val:
================================================================================*/
void showError(HWND hwnd)
{
	TCHAR *lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| //自动分配消息缓冲区

				  FORMAT_MESSAGE_FROM_SYSTEM, //从系统获取信息

				  NULL,GetLastError(), //获取错误信息标识

				  MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),//使用系统缺省语言

				  (LPTSTR)&lpMsgBuf, //消息缓冲区

				  0,

				  NULL);

	MessageBox(hwnd,lpMsgBuf,"",MB_ICONERROR);
}
/*==============================================================================
*Fun Purpose:得到EXE文件的路径（不包含文件名）
*	Parameter:
		exePath-----得到的路径放在此中
		iLen-------exePath的长度
*	Return Val:
================================================================================*/
void GetExePath(TCHAR exePath[],int iLen)
{
	GetModuleFileName(NULL,exePath,iLen); //得到全路径
	TCHAR *CutPos=strrchr(exePath,'\\');  //指针得到exepath的\\地址，并通过指针对其进行了操作，
	*CutPos='\0';                         //实际上去掉了xxx.exe
}
/*==============================================================================
*Fun Purpose:查看帮助（菜单中的帮助按钮用到）
*	Parameter:
*	Return Val:
================================================================================*/
void ViewHelp(HWND hwnd)
{
	TCHAR exePath[MAX_PATH];
	TCHAR HelpFilePath[MAX_PATH+10];
	GetExePath(exePath,sizeof(exePath));
	wsprintf(HelpFilePath,"%s\\help.chm",exePath);
	ShellExecute(hwnd,TEXT("open"),HelpFilePath,NULL,NULL,SW_SHOWMAXIMIZED);
}