#include<mmsystem.h>
#pragma comment(lib,"winmm")

#define SONG_INIT 0
#define SONG_PLAYING 1
#define SONG_PAUSE 2
#define SONG_STOP 3

//标记歌曲信息的结构体
typedef struct _MP3_ID3v1_STRUCT
{
	char Header[4];  /*标签头必须是"TAG"否则认为没有标签*/ 
	char Title[31];  /*标题*/ 
	char Artist[31]; /*作者*/ 
	char Album[31];  /*专集*/ 
	char Year[5];    /*出品年代*/ 
	char Comment[29];/*备注*/ 
	char reserve;    /*保留*/ 
	char track;;     /*音轨*/ 
	char Genre;      /*类型*/ 
}MP3_ID3v1_STRUCT,* pMP3_ID3v1_STRUCT;

//获取歌曲信息的函数
BOOL GetMp3Info(TCHAR szFilePath[],pMP3_ID3v1_STRUCT Mp3Info);

//歌曲类
class song
{
	public:
		TCHAR szName[MAX_PATH];//歌曲名
		TCHAR szFilePathName[2*MAX_PATH];//全路径(包含歌曲名)
		long iLength; //歌曲长度
		int state;    //可取值为SONG_INIT,SONG_PLAYING,SONG_PAUSE,SONG_STOP
		MP3_ID3v1_STRUCT SongInfo;//歌曲信息
	private:
		TCHAR szShortFilePath[MAX_PATH];//全路径的短路径形式

	public:
		song();
		song(TCHAR szName1[],TCHAR szFilePathName1[]);
		song(TCHAR szName1[],TCHAR szFilePathName1[],int iLength1);

		void Play();
		void Play(int iVol);
		void Pause();
		void Stop();

		void Init();
		void KillSound();//设置静音
		void ResumeSound();//恢复声音
		void SetVolume(int iVolume);//设置声音
		void JumpTo(LONG lPos);//跳到指定位置
		long GetSongLen();//获得歌曲长度
		long GetCurPlayingPos();//获得当前播放位置
};