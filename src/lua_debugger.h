#ifndef __LUA_DEBUGGER_H__
#define __LUA_DEBUGGER_H__

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "network.h"
#include "luahook.h"

#define break_none 0
#define break_step 1

namespace huo_lua
{
	enum debug_status
	{
		debug_run,
		debug_stop,
	};

	typedef struct break_point
	{
		int line;
		int16_t opcode;
		int16_t break_type;
		std::string filepath;
	}break_point;

	struct lua_debugger_context
	{
		void set_lua_status(enum debug_status status);
		//暂停lua代码
		int stop_lua();
		//回复暂停的lua代码
		int continue_lua();
		//添加代码断点
		int add_break_point(const break_point& bp);
		//检查是否需要停止lua代码(断点命中,调试暂停)
		int wait_break();
		//检测是否需要停止lua代码
		int wait_stop();
		//启动lua代码
		int start_lua(const std::string &lua_path);
		//单步步进
		int next();
		//单步步入
		int step();

		int find_previous_break(int index);

		int net_start;											//网络启动状态
		debug_status status;									//调试状态
		std::mutex mtx;
		std::condition_variable cond;
		std::thread *network_thread;
		uWS::WebSocket<true, true, PerSocketData>* ws;			//websocket link
	
		break_point next_bk;									//代码下一次的中断位置(单步步进、单步步入)
		std::vector<break_point> bkps;							//代码断点集合
		const std::vector<lua_frame_info> *current_frames;		//当前lua运行的堆栈信息
	};

	struct lua_debugger_context* get_context();
}

#endif