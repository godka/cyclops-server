#pragma once
#include <string>
#include "tinyxml.h"
using namespace std;
class mythStreamSQLresult
{
public:
	static mythStreamSQLresult* CreateNew(const char* str){
		return new mythStreamSQLresult(str);
	}
	char* prase(char* keywords);
	bool MoveNext();
	int close();
	mythStreamSQLresult(const char* str);
	~mythStreamSQLresult();
	void init();
	string TryPrase(const char* keywords);
private:
	const char* xmlstring;
	TiXmlDocument* doc;
	TiXmlElement* tablecontent;
	TiXmlElement* element;
	bool isfirst;
};

