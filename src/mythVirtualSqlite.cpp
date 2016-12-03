﻿#include "mythVirtualSqlite.hh"
#include "tinyxml.h"
#include "mythStreamSQLresult.hh"
#include "MythSocket.hh"
mythVirtualSqlite*  mythVirtualSqlite::mVirtualSqllite = NULL;
mythVirtualSqlite* mythVirtualSqlite::GetInstance(){
	if (!mVirtualSqllite){
		return CreateNew(NULL);
	}
	else{
		return mVirtualSqllite;
	}
}

int mythVirtualSqlite::convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen)
{
	/*
    iconv_t cd;
    char   *inbuf = src;
    char *outbuf = save;
    size_t outbufsize = savelen;
    int status = 0;
    size_t  savesize = 0;
    size_t inbufsize = srclen;
    const char* inptr = inbuf;
    size_t      insize = inbufsize;
    char* outptr = outbuf;
    size_t outsize = outbufsize;
    cd = iconv_open(to, from);
    iconv(cd,NULL,NULL,NULL,NULL);
    if (inbufsize == 0) {
        status = -1;
        goto done;
    }
    while (insize > 0) {
        size_t res = iconv(cd,(const char**)&inptr,&insize,(char**)&outptr,&outsize);
        if (outptr != outbuf) {
            int saved_errno = errno;
            int outsize = outptr - outbuf;
            strncpy(save+savesize, outbuf, outsize);
            errno = saved_errno;
        }
        if (res == (size_t)(-1)) {
            if (errno == EILSEQ) {
                int one = 1;
                iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
                status = -3;
            } else if (errno == EINVAL) {
                if (inbufsize == 0) {
                    status = -4;
                    goto done;
                } else {
                    break;
                }
            } else if (errno == E2BIG) {
                status = -5;
                goto done;
            } else {
                status = -6;
                goto done;
            }
        }
    }
    status = strlen(save);
done:
    iconv_close(cd);
    return status;
	*/
	return 0;
}
mythVirtualSqlite::mythVirtualSqlite()
{
	//m_ip = "127.0.0.1";
	this->mVirtualSqllite = this;
	m_ip = mythIniFile::GetInstance()->GetStr("config","ip");
	//result = (SQLresult*)malloc(sizeof(SQLresult));
}

mythVirtualSqlite* mythVirtualSqlite::CreateNew(char* filename){
	mythVirtualSqlite* mybase = new mythVirtualSqlite();
	if(mybase->open(filename) == 0){
		return mybase;
	}else
		return NULL;
}
int mythVirtualSqlite::open(char* filename){
	//int nRes = sqlite3_open(filename, &pDB);  
	//if (nRes == SQLITE_OK){
	//	return 0;
	//}else{
	//	return 1;
	//}
	return 0;
}

int mythVirtualSqlite::setfilename(char* filename){
	//this->m_filename = filename;
	return 0;
}

void mythVirtualSqlite::close(){
	//sqlite3_close(pDB);
}

int mythVirtualSqlite::UserResult(int argc, char **argv, char **azColName){
	return 0;
}

int mythVirtualSqlite::UserResultstatic(void *NotUsed, int argc, char **argv, char **azColName){
	if(NotUsed != NULL){
		mythVirtualSqlite* tmp = (mythVirtualSqlite*)NotUsed;
		tmp->UserResult(argc,argv,azColName);
		return 0;
	}else
		return 1;
}
char* mythVirtualSqlite::parseSQL(const char* keywords){
	return NULL;
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

int mythVirtualSqlite::execSQLex(const char* str){
	std::string generatestr = replace(str, "%20", " ");
	return execSQL(generatestr.c_str());
}
int mythVirtualSqlite::execSQL(const char* string){
	return 1;
	/*
	char* ErrorMsg;
	int nRes = sqlite3_exec(pDB , string ,UserResultstatic ,this, &ErrorMsg);
	if (nRes == SQLITE_OK){
		return 0;
	}else{
		return 1;
	}*/
}
mythVirtualSqlite::~mythVirtualSqlite(void)
{
	//this->close();
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

	myth_toupper((char*)str);
	std::string header = GetHeader(str);
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
		mythLog::GetInstance()->printf("error:Cannot connect to database! remoteip: %s\n", m_ip.c_str());
	}
	delete socket;
	return retresult;
	//toupper()
}
std::string mythVirtualSqlite::GetHeader(const char* str){
	//string tmpret;
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