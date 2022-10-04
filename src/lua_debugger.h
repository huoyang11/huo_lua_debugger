#ifndef __LUA_DEBUGGER_H__
#define __LUA_DEBUGGER_H__

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "network.h"
#include "ctrl/break_point.h"
#include "luahook.h"

namespace huo_lua
{
	enum debug_status
	{
		debug_run,
		debug_stop,
	};

	struct lua_debugger_context
	{
		//暂停lua代码
		int stop_lua();
		//回复暂停的lua代码
		int continue_lua();
		//添加代码断点
		int add_break_point(const break_point& bp);
		//检查是否需要停止lua代码(断点命中,调试暂停)
		int wait_break();
		//启动lua代码
		int start_lua(const std::string &lua_path);

		int net_start;											//网络启动状态
		debug_status status;									//调试状态
		std::mutex mtx;
		std::condition_variable cond;
		std::thread *network_thread;
		uWS::WebSocket<true, true, PerSocketData>* ws;			//websocket link
	
		std::vector<break_point> bkps;							//代码断点集合
		const std::vector<lua_frame_info> *current_frames;		//当前lua运行的堆栈信息
	};

	struct lua_debugger_context* get_context();
}

#endif