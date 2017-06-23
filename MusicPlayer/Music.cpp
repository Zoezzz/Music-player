#include"stdafx.h"
#include"Music.h"
#include <stdlib.h>
#include<stdio.h>
/*==============================================================================
*Fun Purpose:���캯��
================================================================================*/
song::song()
{
	lstrcpy(szName,"");
	lstrcpy(szFilePathName,"");
	iLength=-1;
	state=SONG_INIT;
}

song::song(TCHAR szName1[],TCHAR szFilePathName1[])
{
	lstrcpy(szName,szName1);
	lstrcpy(szFilePathName,szFilePathName1);
	iLength=-1;
	state=SONG_INIT;
}

song::song(TCHAR szName1[],TCHAR szFilePathName1[],int iLength1)
{
	lstrcpy(szName,szName1);
	lstrcpy(szFilePathName,szFilePathName1);
	iLength=iLength1;
	state=SONG_INIT;
}
/*==============================================================================
*Fun Purpose:��ʼ��
================================================================================*/
void song::Init()
{
	lstrcpy(szName,"");
	lstrcpy(szFilePathName,"");
	iLength=-1;
	state=SONG_INIT;
}
/*==============================================================================
*Fun Purpose:��������
================================================================================*/
void song::Play()
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"play %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_PLAYING;
	iLength=GetSongLen();//��������
}
/*==============================================================================
*Fun Purpose:ָ��������������
================================================================================*/
void song::Play(int iVol)
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"play %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_PLAYING;
	iLength=GetSongLen();//��������
	SetVolume(iVol);//��������
}
/*==============================================================================
*Fun Purpose:��ͣ����
================================================================================*/
void song::Pause()
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"pause %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_PAUSE;
}
/*==============================================================================
*Fun Purpose:ֹͣ����
================================================================================*/
void song::Stop()
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"stop %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_STOP;
}
/*==============================================================================
*Fun Purpose:����
*	Parameter:
*	Return Val:
================================================================================*/
void song::KillSound()
{
	TCHAR Command[MAX_PATH+20];
	wsprintf(Command,"setaudio %s off",szShortFilePath);
	mciSendString(Command,(LPTSTR)NULL,0,NULL);
}
/*==============================================================================
*Fun Purpose:�ָ�����
*	Parameter:
*	Return Val:
================================================================================*/
void song::ResumeSound()
{
	TCHAR Command[MAX_PATH+20];
	wsprintf(Command,"setaudio %s on",szShortFilePath);
	mciSendString(Command,(LPTSTR)NULL,0,NULL);
}
/*==============================================================================
*Fun Purpose:���������������仯ʱ����������
*	Parameter:
*	Return Val:
================================================================================*/
void song::SetVolume(int iVolume)
{
	TCHAR VolumeCmd[MAX_PATH+20]; 
	wsprintf(VolumeCmd,"setaudio %s volume to %d",szShortFilePath,iVolume);
    mciSendString(VolumeCmd,"",0,NULL);
}
/*==============================================================================
*Fun Purpose:������һ��λ��
================================================================================*/
void song::JumpTo(LONG lPos)
{
	TCHAR cmd[256];
	wsprintf(cmd,"play %s from %ld",szShortFilePath,lPos);
	mciSendString(cmd,NULL,0,0);
	mciSendString(cmd,NULL,0,0);//ΪʲôҪ���Σ�����
	if(state==SONG_PAUSE)//���֮ǰ��������ͣ��
	{
		Pause();
	}
}
/*==============================================================================
*Fun Purpose:��ȡ��������
================================================================================*/
long song::GetSongLen()
{
	//�õ���������ʱ��
	TCHAR PlaySongLength[50];
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"status %s length",szShortFilePath);
	mciSendString(cmd,PlaySongLength,sizeof(PlaySongLength),0);//��ø����ĳ���
	//�õ����Ǹ�����΢����
	iLength=atol(PlaySongLength);
	return iLength;
}
/*==============================================================================
*Fun Purpose:��ȡ��ǰ����λ��
================================================================================*/
long song::GetCurPlayingPos()
{
	TCHAR pos[256];
	//�õ���ǰ���ŵ�λ�ã�ʱ�䣩
	TCHAR cmd[MAX_PATH+20];
	wsprintf(cmd,"status %s position",szShortFilePath);
	mciSendString(cmd,pos,sizeof(pos),NULL);
	
	return atol(pos);
}
/*=================================================================================
*Fun Purpose:�õ����ֵĸ�����Ϣ(����������ĺ���)
=================================================================================*/
BOOL GetMp3Info(TCHAR szFilePath[],pMP3_ID3v1_STRUCT pMp3Info)
{
	FILE *fp=fopen(szFilePath,"rb");
	if(NULL==fp)
	{
		return FALSE;
	}

	ZeroMemory(pMp3Info,sizeof(MP3_ID3v1_STRUCT));
	
	fseek(fp,-128L,SEEK_END);
	fread(&(pMp3Info->Header),3,1,fp);
	if(strcmp(pMp3Info->Header,"TAG")!=0)
	{
		return FALSE;
	}
	fread(&(pMp3Info->Title),30,1,fp);
	fread(&(pMp3Info->Artist),30,1,fp);
	fread(&(pMp3Info->Album),30,1,fp);
	fread(&(pMp3Info->Year),4,1,fp);
	fread(&(pMp3Info->Comment),28,1,fp);
	
	//�������Ϣ�Ͳ���ȡ�ˣ����졢��Ŀ���ͣ�
	return TRUE;
}
