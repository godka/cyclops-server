#include "mythVirtualSqlite.hh"
#include "tinyxml.h"
#include "mythStreamSQLresult.hh"
#include "MythSocket.hh"
#include <algorithm>

mythVirtualSqlite*  mythVirtualSqlite::mVirtualSqllite = NULL;
mythVirtualSqlite* mythVirtualSqlite::GetInstance(){
	if (!mVirtualSqllite){
		return CreateNew(NULL);
	}
	else{
		return mVirtualSqllite;
	}
}

mythVirtualSqlite::mythVirtualSqlite()
{
	this->mVirtualSqllite = this;
	m_ip = mythIniFile::GetInstance()->GetStr("config","ip","127.0.0.1");
}

mythVirtualSqlite* mythVirtualSqlite::CreateNew(char* filename){
	mythVirtualSqlite* mybase = new mythVirtualSqlite();
	return mybase;
}

std::string mythVirtualSqlite::replace(std::string str, const char *string_to_replace, const char *new_string)
{
	//查找第一个匹配的字符串
	int index = str.find(string_to_replace);
	// 如果有匹配的字符串
	while(index != std::string::npos)
	{
	// 替换
		str.replace(index, strlen(string_to_replace), new_string);
		// 查找下一个匹配的字符串
		index = str.find(string_to_replace, index + strlen(new_string));
	}
	return str;
}

mythVirtualSqlite::~mythVirtualSqlite(void)
{
}

int mythVirtualSqlite::SetSQLIP(std::string ip){
	m_ip = ip;
	std::cout << "SQL IP set success:" << m_ip << std::endl;
	return 0;
}
mythStreamSQLresult* mythVirtualSqlite::doSQLFromStream(const char* str)
{
	mythStreamSQLresult* retresult = NULL;
	std::string ret = "GET /scripts/dbnet.dll?param=";
	ret += "<XML><function>";
	std::string connectstr = str;
	std::transform(connectstr.begin(), connectstr.end(), connectstr.begin(), toupper);
	//myth_toupper((char*)str);
	std::string header = GetHeader(connectstr.c_str());
	ret += "SQL_" + header;
	ret += "</function>";
	ret += "<Content>";
	ret += replace(str, " ", "%20");
	ret += "</Content></XML>  HTTP/1.0 \r\n\r\n";

	MythSocket* socket = MythSocket::CreateNew(m_ip.c_str(), 5830);
	if (socket->socket_SendStr(ret.c_str()) == 0){
		char tmp[65535] = { 0 };
		int socketret = socket->socket_ReceiveData(tmp, 65535);
		if (socketret > 0){
			retresult = mythStreamSQLresult::CreateNew(tmp);
		}
		socket->socket_CloseSocket();
	}
	else{
		mythLog::GetInstance()->printf("database error:Cannot connect to database! remoteip: %s\n", m_ip.c_str());
	}
	delete socket;
	return retresult;
}
std::string mythVirtualSqlite::GetHeader(const char* str){
	char tmpstr[256] = { 0 };
	for (int i = 0;; i++){
		if (str[i] == ' ')
			break;
		else
			tmpstr[i] = str[i];
	}
	//tmpret = tmpstr;
	return tmpstr;
}
void mythVirtualSqlite::myth_toupper(char* data)
{
	for (int i = 0;; i++){
		if (data[i] == 0)break;
		if (data[i] <= 'z'&& data[i] >= 'a'){
			data[i] -= 32;
		}
	}
}
