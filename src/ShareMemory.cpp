// ShareMemory.cpp: implementation of the ShareMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "ShareMemory.hh"
#include <cstdio>
#include <cstdlib>
#if !(defined WIN32) && !(defined _WIN64)
#include <sys/time.h>
#endif
#include <memory.h>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ShareMemory::ShareMemory(const char *key, unsigned long size, const char *path)
{
	Init();
	if (NULL != key) m_shareName = key;
	if (0 < size) m_dwSize = size;
	if (NULL != path) m_mapFilePath = path;
	//FILE_MAP_ALL_ACCESS = FILE_MAP_WRITE|FILE_MAP_READ
	if (!Open(FILE_MAP_ALL_ACCESS))
	{
		Create();
		Open(FILE_MAP_ALL_ACCESS);
		m_isCreated = true;
	}
	else{
		m_isCreated = false;
	}

}

ShareMemory::ShareMemory(const int key, unsigned long size, const char *path)
{
	Init();
	m_digName = key;
	if (0 < m_digName)
	{
		char strnum[256];
		sprintf(strnum, "%u", m_digName);
		m_shareName = strnum;
	}
	if (0 < size) m_dwSize = size;
	if (NULL != path) m_mapFilePath = path;
	if (!Open(FILE_MAP_ALL_ACCESS))
	{
#if (defined WIN32) || (defined _WIN64)
		if ("" != m_mapFilePath) return;
#endif
		if (!Create()){
			puts("Create Sharememory Error!");
			return;
		}
		Open(FILE_MAP_ALL_ACCESS);
		m_isCreated = true;
	}
	else{
		m_isCreated = false;
	}
}

ShareMemory::~ShareMemory()
{
	Close();
}

void ShareMemory::Init()
{
#if (defined WIN32) || (defined WIN64)
	m_hFile = NULL;
	m_hFileMap = NULL;
#else
	m_shmid = -1;
#endif
	m_lpFileMapBuffer = NULL;
	m_dwSize = 8;
	m_shareName = "_DEFAULT_SMB_";
	m_mapFilePath = "";
	m_digName = -1;
}

bool ShareMemory::Create()
{
#if (defined WIN32) || (defined _WIN64)
	m_hFile = (HANDLE) 0xFFFFFFFF;// system
	if ("" != m_mapFilePath)
	{
		string strFile = m_mapFilePath + "/" + m_shareName;
		// file
		m_hFile = CreateFile(
			strFile.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,//OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (INVALID_HANDLE_VALUE == m_hFile)
		{
			m_lastError = "open file error:" + strFile;
			return false;
		}
	}
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE,
		0, m_dwSize, m_shareName.c_str());
	if (NULL == m_hFileMap)
	{
		m_lastError = "create file mapping error";
		return false;
	}
#else
	if (-1 == m_digName)
	{
		if ("" == m_shareName)
		{
			m_lastError = "key is null";
			return false;
		}
		m_digName = atoi(m_shareName.c_str());
		if (0 >= m_digName)
		{
			m_lastError = "key havo to bigger than 0";
			return false;
		}
	}
	m_shmid = shmget(m_digName, m_dwSize, IPC_CREAT | IPC_EXCL);
	if (-1 == m_shmid)
	{
		if (EEXIST == errno)
		{
			m_shmid = shmget(m_digName, m_dwSize, 0666);
		}
		if (-1 == m_shmid)
		{
			m_lastError = "open error:";
			if (EINVAL == errno)
			{
				m_lastError += "size < SHMMIN or size > SHM-MAX";
				m_lastError += ", or segment is exist, but size is small than the new size";
			}
			if (EIDRM == errno) m_lastError += "segment is delete";
			if (ENOSPC == errno) m_lastError += "requested size would cause the system to exceed the system-wide limit on shared memory (SHMALL)";
			if (ENOENT == errno) m_lastError += "No segment exists";
			if (EACCES == errno) m_lastError += "not have permission to access";
			if (ENOMEM == errno) m_lastError += "No memory could be allocated for segment overhead";
			if (ENFILE == errno) m_lastError += "open too many file";
			puts(m_lastError.c_str());
			return false;
		}
	}
#endif
	return true;
}

bool ShareMemory::Open(unsigned long dwAccess)
{
#if (defined WIN32) || (defined _WIN64)
	if (NULL == m_hFileMap) m_hFileMap = OpenFileMapping(dwAccess, TRUE, m_shareName.c_str());
	if (NULL == m_hFileMap)
	{
		return false;
	}
	m_lpFileMapBuffer = MapViewOfFile(m_hFileMap, dwAccess, 0, 0, 0);
	if (NULL == m_lpFileMapBuffer)
	{
		Close();
		return false;
	}
	MEMORY_BASIC_INFORMATION buffer;
	int nRet = VirtualQuery(m_lpFileMapBuffer, &buffer, sizeof(buffer));
	if (0 == nRet)
	{
		Close();
		return false;
	}
	m_dwSize = buffer.RegionSize;
#else
	if ("" != m_mapFilePath) return OpenFileMap();
	if (-1 == m_digName)
	{
		if ("" == m_shareName) return false;
		m_digName = atoi(m_shareName.c_str());
		if (0 >= m_digName) return false;
	}
	m_shmid = shmget(m_digName, 0, 0666);
	if (-1 == m_shmid) return false;
	m_lpFileMapBuffer = shmat(m_shmid, 0, 0);
	shmctl(m_shmid, IPC_STAT, &m_ds);
	m_dwSize = m_ds.shm_segsz;
#endif

	return true;
}

bool ShareMemory::CheckDir(const char *strDir)
{
#if (defined WIN32) || (defined _WIN64)
	return false;
#else
	if (-1 == access(strDir, 0))
	{
		umask(0);
		if (0 != mkdir(strDir, 0777)) return false;
	}
	umask(0);
	chmod(strDir, 0777);
	return true;
#endif
}

bool ShareMemory::CheckFile(const char *strFile)
{
#if (defined WIN32) || (defined _WIN64)
	return false;
#else
	umask(0);
	chmod(strFile, 0777);
	return true;
#endif
}

bool ShareMemory::OpenFileMap()
{
#if (defined WIN32) || (defined _WIN64)
	return false;
#else
	CheckDir(m_mapFilePath.c_str());
	string strFile = m_mapFilePath + "/" + m_shareName;
	CheckFile(strFile.c_str());
	int fd;
	fd = open(strFile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (-1 == fd)
	{
		m_lastError = "open file error:" + strFile;
		return false;
	}
	struct stat sb;
	fstat(fd, &sb);
	if (sb.st_size < (long) m_dwSize)
	{
		lseek(fd, m_dwSize, SEEK_SET);
		write(fd, "", 1);
		lseek(fd, 0, SEEK_SET);
		fstat(fd, &sb);
		m_dwSize = sb.st_size;
	}

	m_lpFileMapBuffer = mmap(0,
		m_dwSize,
		PROT_WRITE | PROT_READ, //ÔÊÐíÐ´Èë
		MAP_SHARED,//Ð´ÈëÄÚÈÝ±»Á¢¼´Ð´Èëµ½ÎÄ¼þ
		fd,
		0);
	close(fd);
	if (NULL == m_lpFileMapBuffer)
	{
		m_lastError = "create file map error";
		return false;
	}
	return true;
#endif
}

void ShareMemory::Close()
{
#if (defined WIN32) || (defined _WIN64)
	if (NULL != m_lpFileMapBuffer) UnmapViewOfFile(m_lpFileMapBuffer);
	if (NULL != m_hFileMap) CloseHandle(m_hFileMap);
	if (NULL != m_hFile && INVALID_HANDLE_VALUE != m_hFile) CloseHandle(m_hFile);
#else
	if ("" != m_mapFilePath && NULL != m_lpFileMapBuffer)
	{
		munmap(m_lpFileMapBuffer, m_dwSize);
	}
	else if (NULL != m_lpFileMapBuffer) shmdt(m_lpFileMapBuffer);
#endif
	Init();
}


void ShareMemory::Destory()
{
#if (defined WIN32) || (defined _WIN64)
	Close();
#else
	if (-1 != m_shmid)
	{
		Close();
		shmctl(m_shmid, IPC_RMID, 0);
	}
	else if ("" != m_mapFilePath)
	{
		string strFile = m_mapFilePath + "/" + m_shareName;
		Close();
		remove(strFile.c_str());
	}
#endif
}

void* ShareMemory::GetBuffer()
{
	return m_lpFileMapBuffer;
}

unsigned long ShareMemory::GetSize()
{
	return NULL == m_lpFileMapBuffer ? 0 : m_dwSize;
}

void ShareMemory::TestSystem()
{
#if (defined WIN32) || (defined _WIN64)
	ShareMemory a(NULL, 409600, NULL);
	char *p = (char*) a.GetBuffer();
	char sss[409600];
	SYSTEMTIME t1, t2;

	memset(p, 1, 409600);
	memset(sss, 100, 409600);
	while (true)
	{
		int i = 0;
		GetSystemTime(&t1);
		int j = 0;
		for (j = 0; j < 10000; j++)
		{
			memcpy(p, sss, 409600);//Ð´
			//memcpy( sss, p, 409600 );//¶Á
		}
		GetSystemTime(&t2);
		int h = (t2.wHour - t1.wHour) * 3600;
		int m = (t2.wMinute - t1.wMinute) * 60;
		int s = t2.wSecond - t1.wSecond;
		s = h + m + s;
		int ns = t2.wMilliseconds - t1.wMilliseconds;
		s = s * 1000 + ns;
		float ft = s / 1000.0;
		printf("%fn", ft);
	}
#else
	ShareMemory a("1234", 40960, NULL);
	char *p = (char*) a.GetBuffer();
	char sss[409600];

	struct timeval tEnd, tStart;
	struct timezone tz;
	memset(p, 'd', 40960);
	memset(sss, 'd', 40960);
	while (true)
	{
		int j = 0;
		gettimeofday(&tStart, &tz);
		for (j = 0; j < 100000; j++)
		{
			//memcpy( p, sss, 40960 );//Ð´
			memcpy(sss, p, 40960);//¶Á
		}
		gettimeofday(&tEnd, &tz);
		printf("time:%ldn", (tEnd.tv_sec - tStart.tv_sec) * 1000 + (tEnd.tv_usec - tStart.tv_usec) / 1000);
	}

#endif
}

void ShareMemory::TestFile()
{
#if (defined WIN32) || (defined _WIN64)
	ShareMemory a("12", 409600, "E:\Lib\HuoYu\HYLib\part_test\smtest");
	char sss[409600];
	SYSTEMTIME t1, t2;
	char *p = (char*) a.GetBuffer();

	memset(p, 1, 409600);
	memset(sss, 100, 409600);
	while (true)
	{
		int i = 0;
		GetSystemTime(&t1);
		int j = 0;
		for (j = 0; j < 10000; j++)
		{
			memcpy(p, sss, 409600);//Ð´
			//memcpy( sss, p, 409600 );//¶Á
		}
		GetSystemTime(&t2);
		int h = (t2.wHour - t1.wHour) * 3600;
		int m = (t2.wMinute - t1.wMinute) * 60;
		int s = t2.wSecond - t1.wSecond;
		s = h + m + s;
		int ns = t2.wMilliseconds - t1.wMilliseconds;
		s = s * 1000 + ns;
		float ft = s / 1000.0;
		printf("%fn", ft);
	}
#else
	printf("TestFilen");
	ShareMemory a("999", 40960, "./");
	char sss[409600];
	char *p = (char*) a.GetBuffer();
	struct timeval tEnd, tStart;
	struct timezone tz;
	memset(p, 'a', 40960);
	memset(sss, 'a', 40960);
	while (true)
	{
		int j = 0;
		gettimeofday(&tStart, &tz);
		for (j = 0; j < 100000; j++)
		{
			memcpy(p, sss, 40960);//Ð´
			//memcpy( sss, p, 40960 );//¶Á
		}
		gettimeofday(&tEnd, &tz);
		printf("time:%ldn", (tEnd.tv_sec - tStart.tv_sec) * 1000 + (tEnd.tv_usec - tStart.tv_usec) / 1000);
	}
#endif
}

void ShareMemory::TestMemory()
{
#if (defined WIN32) || (defined _WIN64)
	ShareMemory a(NULL, 409600, NULL);
	char *p = new char[409600];
	char sss[409600];
	SYSTEMTIME t1, t2;
	memset(p, 1, 409600);
	memset(sss, 100, 409600);
	while (true)
	{
		int i = 0;
		GetSystemTime(&t1);
		int j = 0;
		for (j = 0; j < 10000; j++)
		{
			memcpy(p, sss, 409600);//Ð´
			//memcpy( sss, p, 409600 );//¶Á
		}
		GetSystemTime(&t2);
		int h = (t2.wHour - t1.wHour) * 3600;
		int m = (t2.wMinute - t1.wMinute) * 60;
		int s = t2.wSecond - t1.wSecond;
		s = h + m + s;
		int ns = t2.wMilliseconds - t1.wMilliseconds;
		s = s * 1000 + ns;
		float ft = s / 1000.0;
		printf("%fn", ft);
	}
#else
	printf("TestMemoryn");
	char *p = new char[40960];
	char sss[409600];
	struct timeval tEnd, tStart;
	struct timezone tz;
	memset(p, 's', 40960);
	memset(sss, 's', 40960);

	while (true)
	{
		int j = 0;
		gettimeofday(&tStart, &tz);
		for (j = 0; j < 100000; j++)
		{
			memcpy(p, sss, 40960);//Ð´
			//memcpy( sss, p, 40960 );//¶Á
		}
		gettimeofday(&tEnd, &tz);
		printf("time:%ldn", (tEnd.tv_sec - tStart.tv_sec) * 1000 + (tEnd.tv_usec - tStart.tv_usec) / 1000);
	}
#endif
}