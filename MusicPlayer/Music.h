#include<mmsystem.h>
#pragma comment(lib,"winmm")

#define SONG_INIT 0
#define SONG_PLAYING 1
#define SONG_PAUSE 2
#define SONG_STOP 3

//��Ǹ�����Ϣ�Ľṹ��
typedef struct _MP3_ID3v1_STRUCT
{
	char Header[4];  /*��ǩͷ������"TAG"������Ϊû�б�ǩ*/ 
	char Title[31];  /*����*/ 
	char Artist[31]; /*����*/ 
	char Album[31];  /*ר��*/ 
	char Year[5];    /*��Ʒ���*/ 
	char Comment[29];/*��ע*/ 
	char reserve;    /*����*/ 
	char track;;     /*����*/ 
	char Genre;      /*����*/ 
}MP3_ID3v1_STRUCT,* pMP3_ID3v1_STRUCT;

//��ȡ������Ϣ�ĺ���
BOOL GetMp3Info(TCHAR szFilePath[],pMP3_ID3v1_STRUCT Mp3Info);

//������
class song
{
	public:
		TCHAR szName[MAX_PATH];//������
		TCHAR szFilePathName[2*MAX_PATH];//ȫ·��(����������)
		long iLength; //��������
		int state;    //��ȡֵΪSONG_INIT,SONG_PLAYING,SONG_PAUSE,SONG_STOP
		MP3_ID3v1_STRUCT SongInfo;//������Ϣ
	private:
		TCHAR szShortFilePath[MAX_PATH];//ȫ·���Ķ�·����ʽ

	public:
		song();
		song(TCHAR szName1[],TCHAR szFilePathName1[]);
		song(TCHAR szName1[],TCHAR szFilePathName1[],int iLength1);

		void Play();
		void Play(int iVol);
		void Pause();
		void Stop();

		void Init();
		void KillSound();//���þ���
		void ResumeSound();//�ָ�����
		void SetVolume(int iVolume);//��������
		void JumpTo(LONG lPos);//����ָ��λ��
		long GetSongLen();//��ø�������
		long GetCurPlayingPos();//��õ�ǰ����λ��
};