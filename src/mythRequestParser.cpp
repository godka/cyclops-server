#include "mythRequestParser.hh"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
mythRequestParser::mythRequestParser(const char* str)
{
	Success = false;
	parseCore(str);
}

void mythRequestParser::parseCore(const char* str)
{
	std::string tmp = URLDecode(str);
	std::vector<std::string> findGet;
	split(tmp, " ", findGet);
	if (findGet.size() > 1){
		_header = findGet[0];
		int len = findGet[1].size();
		const char* t = findGet[1].c_str();
		if (t[0] != '/'){
			Success = false;
			return;
		}
		char* tmp = (char*) &t[1];
		char key[4096] = { 0 };
		int keyindex = 0;
		char value[4096] = { 0 };
		int valueindex = 0;
		int status = 0;
		int instring = 0;
		for (int i = 0; i < strlen(tmp); i++){
			switch (tmp[i])
			{
			case '=':
				if (instring == 0){
					status = 1;
				}
				else{
					switch (status)
					{
					case 0:
						key[keyindex] = tmp[i];
						keyindex++;
						break;
					case 1:
						value[valueindex] = tmp[i];
						valueindex++;
						break;
					default:
						break;
					}
				}
				break;
			case '&':
				if (instring == 0){
					status = 0;
					_mapcore[key] = value;
					memset(key, 0, 4096);
					memset(value, 0, 4096);
					keyindex = 0;
					valueindex = 0;
				}
				else{
					switch (status)
					{
					case 0:
						key[keyindex] = tmp[i];
						keyindex++;
						break;
					case 1:
						value[valueindex] = tmp[i];
						valueindex++;
						break;
					default:
						break;
					}
				}
				break;
			case '"':
				if (instring == 0){
					instring = 1;
				}
				else{
					instring = 0;
				}
				switch (status)
				{
				case 0:
					key[keyindex] = tmp[i];
					keyindex++;
					break;
				case 1:
					//value[valueindex] = tmp[i];
					//valueindex++;
					break;
				default:
					break;
				}
				break;
			default:
				switch (status)
				{
				case 0:
					key[keyindex] = tmp[i];
					keyindex++;
					break;
				case 1:
					value[valueindex] = tmp[i];
					valueindex++;
					break;
				default:
					break;
				}
				break;
			}
			if (i == (strlen(tmp) - 1)){
				if (instring == 0){
					status = 0;
					_mapcore[key] = value;
					memset(key, 0, 4096);
					memset(value, 0, 4096);
					keyindex = 0;
					valueindex = 0;
				}
			}
		}
		Success = true;
	}
	else{
		Success = false;
	}
}

void mythRequestParser::split(const std::string& s, const std::string& c, std::vector<std::string>& v)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

std::string& mythRequestParser::Parse(std::string key)
{
	return _mapcore[key];
}

int mythRequestParser::ParseInt(std::string key)
{
	std::string ret = Parse(key);
	if (ret == ""){
		return -1;
	}
	else{
		return atoi(ret.c_str());
	}
	//return _mapcore[key];
}
std::string mythRequestParser::GetHeader()
{
	return _header;
}

mythRequestParser::~mythRequestParser()
{
}
