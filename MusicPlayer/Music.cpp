#include"stdafx.h"
#include"Music.h"
#include <stdlib.h>
#include<stdio.h>
/*==============================================================================
*Fun Purpose:构造函数
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
*Fun Purpose:初始化
================================================================================*/
void song::Init()
{
	lstrcpy(szName,"");
	lstrcpy(szFilePathName,"");
	iLength=-1;
	state=SONG_INIT;
}
/*==============================================================================
*Fun Purpose:播放音乐
================================================================================*/
void song::Play()
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"play %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_PLAYING;
	iLength=GetSongLen();//歌曲长度
}
/*==============================================================================
*Fun Purpose:指定音量播放音乐
================================================================================*/
void song::Play(int iVol)
{
	GetShortPathName(szFilePathName,szShortFilePath,sizeof(szShortFilePath));
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"play %s",szShortFilePath);
	mciSendString(cmd,"",0,NULL);
	state=SONG_PLAYING;
	iLength=GetSongLen();//歌曲长度
	SetVolume(iVol);//设置音量
}
/*==============================================================================
*Fun Purpose:暂停音乐
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
*Fun Purpose:停止音乐
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
*Fun Purpose:静音
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
*Fun Purpose:恢复声音
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
*Fun Purpose:声音滚动条发生变化时，设置声音
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
*Fun Purpose:跳到到一定位置
================================================================================*/
void song::JumpTo(LONG lPos)
{
	TCHAR cmd[256];
	wsprintf(cmd,"play %s from %ld",szShortFilePath,lPos);
	mciSendString(cmd,NULL,0,0);
	mciSendString(cmd,NULL,0,0);//为什么要两次？？？
	if(state==SONG_PAUSE)//如果之前歌曲是暂停的
	{
		Pause();
	}
}
/*==============================================================================
*Fun Purpose:获取歌曲长度
================================================================================*/
long song::GetSongLen()
{
	//得到歌曲的总时间
	TCHAR PlaySongLength[50];
	TCHAR cmd[MAX_PATH+10];
	wsprintf(cmd,"status %s length",szShortFilePath);
	mciSendString(cmd,PlaySongLength,sizeof(PlaySongLength),0);//获得歌曲的长度
	//得到的是歌曲的微秒数
	iLength=atol(PlaySongLength);
	return iLength;
}
/*==============================================================================
*Fun Purpose:获取当前播放位置
================================================================================*/
long song::GetCurPlayingPos()
{
	TCHAR pos[256];
	//得到当前播放的位置（时间）
	TCHAR cmd[MAX_PATH+20];
	wsprintf(cmd,"status %s position",szShortFilePath);
	mciSendString(cmd,pos,sizeof(pos),NULL);
	
	return atol(pos);
}
/*=================================================================================
*Fun Purpose:得到音乐的附加信息(定义在类外的函数)
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
	
	//下面的信息就不读取了（音轨、曲目类型）
	return TRUE;
}
