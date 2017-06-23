// MusicPlayerV3.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <shellapi.h>
#include<commctrl.h>
#include"resource.h"
#include"MainDlg.h"
#pragma comment(lib,"comctl32.lib")
//�������жϽ����Ƿ�����ʱ�õ�
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
	//�ж�֮ǰ�Ƿ����������
	
	if(JudgeIfRuning())
	{
		MessageBox(NULL,TEXT("�����������"),TEXT("��Ϣ"),0);
	}
	else
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Main_Proc);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
/*=================================================================================
*Fun Purpose://��Ϊ����ı����Ǳ仯�ģ�����Ԥ�ϵģ����ԾͲ��ҽ���
=================================================================================*/
BOOL JudgeIfRuning()
{
	//�õ���ִ���ļ�������
	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL,szExeFileName,sizeof(szExeFileName));
	TCHAR *Psolidus=strrchr(szExeFileName,'\\');

	int flagNum=0;//��Ǵ����ƵĽ�����Ŀ��
				//��Ϊ�¿��ĳ����������У��������֮ǰ�г������У����̴�ʱ������
	//���ҽ����б�
	HANDLE hSnapProcess;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	
	hSnapProcess=(HANDLE)CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//���̿���
	if(hSnapProcess == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,TEXT("��������ʧ��!!"),TEXT("����"),MB_OK);
		CloseHandle(hSnapProcess);
		return FALSE;
	}
	
	if(!Process32First(hSnapProcess,&pe32))//���ҵ�һ������
	{
		MessageBox(NULL,TEXT("Process32Firstʧ��!!"),TEXT("����"),MB_OK);
		CloseHandle(hSnapProcess);
		return FALSE;
	}
	else
	{
		int i=0;
		do //GetModuleFileNameEx(hProcess,NULL,szProcessName,MAX_PATH);  //szProcessName����·��
		{
			if(lstrcmp(pe32.szExeFile,Psolidus+1)==0)//����������ִ���ļ�����
			{
				if(flagNum==1)//֮ǰ�Ѿ��ҵ���һ��.�����ҵ��ڶ�������
				{
					return TRUE;
				}
				else
				{
					flagNum++;
				}
			}
		} while (Process32Next(hSnapProcess,&pe32));//ѭ������������
	}
	CloseHandle(hSnapProcess);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*==============================================================================
*Fun Purpose:��ʾδ֪�Ĵ�����Ϣ
*	Parameter:
*	Return Val:
================================================================================*/
void showError(HWND hwnd)
{
	TCHAR *lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| //�Զ�������Ϣ������

				  FORMAT_MESSAGE_FROM_SYSTEM, //��ϵͳ��ȡ��Ϣ

				  NULL,GetLastError(), //��ȡ������Ϣ��ʶ

				  MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),//ʹ��ϵͳȱʡ����

				  (LPTSTR)&lpMsgBuf, //��Ϣ������

				  0,

				  NULL);

	MessageBox(hwnd,lpMsgBuf,"",MB_ICONERROR);
}
/*==============================================================================
*Fun Purpose:�õ�EXE�ļ���·�����������ļ�����
*	Parameter:
		exePath-----�õ���·�����ڴ���
		iLen-------exePath�ĳ���
*	Return Val:
================================================================================*/
void GetExePath(TCHAR exePath[],int iLen)
{
	GetModuleFileName(NULL,exePath,iLen); //�õ�ȫ·��
	TCHAR *CutPos=strrchr(exePath,'\\');  //ָ��õ�exepath��\\��ַ����ͨ��ָ���������˲�����
	*CutPos='\0';                         //ʵ����ȥ����xxx.exe
}
/*==============================================================================
*Fun Purpose:�鿴�������˵��еİ�����ť�õ���
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