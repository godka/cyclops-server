#pragma once
#include "MythConfig.hh"
#include "mythStreamSQLresult.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
class mythVirtualSqlite
{
public:
	static mythVirtualSqlite* GetInstance();
	static mythVirtualSqlite* mVirtualSqllite;
	int setfilename(char* filename);
	static mythVirtualSqlite* CreateNew(char* filename);
	int execSQLex(const char* string);
	int execSQL(const char* string);
	//mythSQLresult* doSQL(const char* str);
	//mythSQLresult* doSQL(string str);
	std::string doSQLex(const char* str);
	mythStreamSQLresult* doSQLFromStream(const char* str);
	void close();
	//void freeResult(mythSQLresult* result);
	int open(char* filename);
	virtual int UserResult(int argc, char **argv, char **azColName);
	static int UserResultstatic(void *NotUsed, int argc, char **argv, char **azColName);
	int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen);
	char* parseSQL(const char* keywords);
	~mythVirtualSqlite(void);
	int SetSQLIP(std::string ip);
protected:
	mythVirtualSqlite();
	std::string CreateXMLline(std::string element, std::string tag);
	std::string replace(std::string str, const char *string_to_replace, const char *new_string);
	void myth_toupper(char* data);
	std::string GetHeader(const char* str);
private:
	//sqlite3* pDB;
	char* m_filename;
	std::string m_ip;
	//mythSQLresult* result;
};
