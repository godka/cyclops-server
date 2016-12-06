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
	std::string tmp = str;
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
		std::string tmpstr = &t[1];
		std::vector<std::string> spstr;
		split(tmpstr, "&", spstr);
		for(auto& t : spstr){
			std::vector<std::string> sp;
			split(t, "=", sp);
			if (sp.size() > 1){
				_mapcore[sp[0]] = sp[1];
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
	auto ret = Parse(key);
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
