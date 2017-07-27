#pragma once
#include "MythConfig.hh"
class mythGlobal
{
public:
	static mythGlobal* GetInstance(){
		if (!m_global){
			return new mythGlobal();
		}
		else{
			return m_global;
		}
	}
	char* global_filename;
	static mythGlobal* m_global;
	~mythGlobal();
protected:
	mythGlobal();

};

