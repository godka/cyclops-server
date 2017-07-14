#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory.h>
typedef unsigned char BYTE;
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
	std::string GetReq();
	~mythRequestParser();
protected:
	mythRequestParser(const char* str);
private:
	std::map<std::string, std::string> _mapcore;
	void parseCore(const char* str);
	void split(const std::string& s, const std::string& c, std::vector<std::string>& v);
	std::string _header;
	std::string _req;
	inline BYTE toHex(const BYTE &x)
	{
		return x > 9 ? x - 10 + 'A' : x + '0';
	}

	inline BYTE fromHex(const BYTE &x)
	{
		return isdigit(x) ? x - '0' : x - 'A' + 10;
	}

	inline std::string URLEncode(const std::string &sIn)
	{
		std::string sOut;
		for (size_t ix = 0; ix < sIn.size(); ix++)
		{
			BYTE buf[4];
			memset(buf, 0, 4);
			if (isalnum((BYTE) sIn[ix]))
			{
				buf[0] = sIn[ix];
			}
			//else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
			//{
			//    buf[0] = '+';
			//}
			else
			{
				buf[0] = '%';
				buf[1] = toHex((BYTE) sIn[ix] >> 4);
				buf[2] = toHex((BYTE) sIn[ix] % 16);
			}
			sOut += (char *) buf;
		}
		return sOut;
	};

	inline std::string URLDecode(const std::string &sIn)
	{
		std::string sOut;
		for (size_t ix = 0; ix < sIn.size(); ix++)
		{
			BYTE ch = 0;
			if (sIn[ix] == '%')
			{
				ch = (fromHex(sIn[ix + 1]) << 4);
				ch |= fromHex(sIn[ix + 2]);
				ix += 2;
			}
			else if (sIn[ix] == '+')
			{
				ch = ' ';
			}
			else
			{
				ch = sIn[ix];
			}
			sOut += (char) ch;
		}
		return sOut;
	}
};

