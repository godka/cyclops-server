#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/shm.h>// for void *shmat(int shmid, const void *shmaddr, int shmflg);
#include <sys/ipc.h>// for int shmget(key_t key, size_t size, int shmflg);
#include <unistd.h>
#include <memory.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/mman.h>// for void *mmap(void*start,size_t length,int prot,int flags,int fd,off_toffsize);

#include <errno.h> //for errno
//#include <error.h>

#define FILE_MAP_READ 1
#define FILE_MAP_WRITE 2
#define FILE_MAP_ALL_ACCESS 3
#endif

#include <string>

class ShareMemory
{
private:
#ifdef WIN32
	HANDLE m_hFile;
	HANDLE m_hFileMap;
#else
	int m_shmid;
	shmid_ds m_ds;
#endif
	void*m_lpFileMapBuffer;
	std::string m_shareName;
	int m_digName;
	unsigned long m_dwSize;
	std::string m_mapFilePath;
	std::string m_lastError;
public:
	/*
	*����/�򿪹����ڴ�
	*����
	*keyȫ�ֱ�ʶ��linux��ֻ��������id
	*size �����С
	*if ( �Ǵ��� )
	*�򴴽�size��С�Ĺ����ڴ棬
	*else //�Ǵ��Ѵ��ڵĹ����ڴ�
	*if ( ���ļ�ӳ�� && ��linuxϵͳ )
	* ��ʹ�õ�ǰ�����ڴ��С��size�нϴ��һ��,��Ϊ�����ڴ�Ĵ�С
	*else ��Ч
	*else ��Ч
	*path ʹ���ļ�ӳ��Ĺ����ڴ棬�ļ�·����keyΪ�ļ���
	*/
	ShareMemory(const char *key, unsigned long size, const char *path);
	/*
	*����/�򿪹����ڴ�
	*����
	*keyȫ�ֱ�ʶ
	*size �����С
	*if ( �Ǵ��� )
	*�򴴽�size��С�Ĺ����ڴ棬
	*else //�Ǵ��Ѵ��ڵĹ����ڴ�
	*if ( ���ļ�ӳ�� && ��linuxϵͳ )
	* ��ʹ�õ�ǰ�����ڴ��С��size�нϴ��һ��,��Ϊ�����ڴ�Ĵ�С
	*else ��Ч
	*else ��Ч
	*path ʹ���ļ�ӳ��Ĺ����ڴ棬�ļ�·����keyΪ�ļ���
	*/
	ShareMemory(const int key, unsigned long size, const char *path);
	virtual ~ShareMemory();

private:
	friend int main(int argc, char* argv []);

	static void TestFile();

	static void TestSystem();

	static void TestMemory();

public:
	void* GetBuffer();
	unsigned long GetSize();
	void Destory();
	bool isCreated(){
		return m_isCreated;
	}

private:
	void Init();
	bool Create();
	bool Open(unsigned long dwAccess);
	void Close();

	bool OpenFileMap();
	bool CheckDir(const char *strIPCDir);
	bool CheckFile(const char *strIPCFile);
	bool m_isCreated;
};

#endif // !defined SHAREMEMORY_H