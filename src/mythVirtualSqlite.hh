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
	static mythVirtualSqlite* CreateNew(char* filename);
	mythStreamSQLresult* doSQLFromStream(const char* str);
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

