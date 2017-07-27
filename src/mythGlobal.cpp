#include "mythGlobal.hh"
mythGlobal* mythGlobal::m_global = NULL;

mythGlobal::mythGlobal()
{
	m_global = this;
	global_filename = NULL;
}


mythGlobal::~mythGlobal()
{
}
