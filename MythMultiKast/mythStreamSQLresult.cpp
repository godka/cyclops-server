/********************************************************************
Created by MythKAst
©2013 MythKAst Some rights reserved.


You can build it with vc2010,gcc.
Anybody who gets this source code is able to modify or rebuild it anyway,
but please keep this section when you want to spread a new version.
It's strongly not recommended to change the original copyright. Adding new version
information, however, is Allowed. Thanks.
For the latest version, please be sure to check my website:
Http://code.google.com/mythkast


你可以用vc2010,gcc编译这些代码
任何得到此代码的人都可以修改或者重新编译这段代码，但是请保留这段文字。
请不要修改原始版权，但是可以添加新的版本信息。
最新版本请留意：Http://code.google.com/mythkast
B
MythKAst(asdic182@sina.com), in 2013 June.
*********************************************************************/
#include "mythStreamSQLresult.hh"


mythStreamSQLresult::mythStreamSQLresult(const char* str)
{
	tablecontent = NULL;
	element = NULL;
	this->xmlstring = str;
	init();
}

void mythStreamSQLresult::init(){
	this->doc = new TiXmlDocument();
	if (doc){
		doc->Parse(xmlstring);
		TiXmlElement* rootElement = doc->RootElement();  //<xml>
		if (rootElement){
			this->tablecontent = rootElement->FirstChildElement("TableContent");
		}
	}
	isfirst = true;
}
mythStreamSQLresult::~mythStreamSQLresult()
{
	if (doc){
		delete doc;
	}
}

bool mythStreamSQLresult::MoveNext()
{
	if (isfirst){
		if (tablecontent){
			this->element = tablecontent->FirstChildElement("line");
		}
		isfirst = false;
	}
	else{
		this->element = element->NextSiblingElement();
	}
	if (element){
		return true;
	}else
		return false;
}

int mythStreamSQLresult::close()
{
	return 0;
}

char* mythStreamSQLresult::prase(char* keywords)
{
	if (element){
		TiXmlElement* retchild = element->FirstChildElement(keywords);
		if (retchild){
			if (retchild->FirstChild()){
				return (char*) retchild->FirstChild()->Value();
			}
			else{
				return NULL;
			}
		}
		else{
			return NULL;
		}
	}
	else{
		return NULL;
	}
}
