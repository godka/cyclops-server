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
#include "mythVirtualDecoder.hh"


mythVirtualDecoder::mythVirtualDecoder(void)
	:mythListFactory(){
	flag = 0;
	m_count = 0;
	ori_count = 0;
	ret_count = 0;
	m_timeid = SDL_AddTimer(1000, TimerCallbackStatic, this);
}

void mythVirtualDecoder::start(){}
void mythVirtualDecoder::stop(){}

mythVirtualDecoder* mythVirtualDecoder::CreateNew(void){
	return new mythVirtualDecoder();
}
mythVirtualDecoder::~mythVirtualDecoder(void){
}

Uint32 mythVirtualDecoder::TimerCallbackStatic(Uint32 interval, void *param){
	mythVirtualDecoder* decoder = (mythVirtualDecoder*) param;
	return decoder->TimerCallback(interval);
}

Uint32 mythVirtualDecoder::TimerCallback(Uint32 interval){
	int tmp = m_count - ori_count;
	ret_count = tmp / interval;
	ori_count = m_count;
	//printf("%d k/s\n", ret_count);
	return interval;
}

unsigned int mythVirtualDecoder::GetTimeCount(){
	return 0;
}