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
	*创建/打开共享内存
	*参数
	*key全局标识，linux下只接收数字id
	*size 共享大小
	*if ( 是创建 )
	*则创建size大小的共享内存，
	*else //是打开已存在的共享内存
	*if ( 是文件映射 && 是linux系统 )
	* 则使用当前共享内存大小与size中较大的一个,作为共享内存的大小
	*else 无效
	*else 无效
	*path 使用文件映射的共享内存，文件路径，key为文件名
	*/
	ShareMemory(const char *key, unsigned long size, const char *path);
	/*
	*创建/打开共享内存
	*参数
	*key全局标识
	*size 共享大小
	*if ( 是创建 )
	*则创建size大小的共享内存，
	*else //是打开已存在的共享内存
	*if ( 是文件映射 && 是linux系统 )
	* 则使用当前共享内存大小与size中较大的一个,作为共享内存的大小
	*else 无效
	*else 无效
	*path 使用文件映射的共享内存，文件路径，key为文件名
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