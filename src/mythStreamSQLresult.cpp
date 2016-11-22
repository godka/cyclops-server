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
string mythStreamSQLresult::TryPrase(const char* keywords){
	if (element){
		TiXmlElement* retchild = element->FirstChildElement(keywords);
		if (retchild){
			if (retchild->FirstChild()){
				return retchild->FirstChild()->Value();
			}
			else{
				return "";
			}
		}
		else{
			return "";
		}
	}
	else{
		return "";
	}
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
