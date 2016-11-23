#pragma once
#include <string>
#include "tinyxml.h"
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
	std::string TryPrase(const char* keywords);
private:
	const char* xmlstring;
	TiXmlDocument* doc;
	TiXmlElement* tablecontent;
	TiXmlElement* element;
	bool isfirst;
};

