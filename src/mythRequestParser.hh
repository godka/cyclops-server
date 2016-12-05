#pragma once
#include <string>
#include <map>
#include <vector>
class mythRequestParser
{
public:
	static mythRequestParser* CreateNew(const char* str){
		return new mythRequestParser(str);
	}
	bool Success;
	std::string& Parse(std::string key);
	int ParseInt(std::string key);
	std::string GetHeader();
	~mythRequestParser();
protected:
	mythRequestParser(const char* str);
private:
	std::map<std::string, std::string> _mapcore;
	void parseCore(const char* str);
	void split(const std::string& s, const std::string& c, std::vector<std::string>& v);
	std::string _header;
};

