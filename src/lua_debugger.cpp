#include "lua_debugger.h"
#include "luahook.h"
#include "network.h"

#include "LuaManage.h"
#include "LuaScript.h"
#include "LuaError.h"
#include "LuaTable.h"

using namespace HiFun;

namespace huo_lua
{
	static struct lua_debugger_context* instance = NULL;

	static void lua_run_error(LuaError* err)
	{
		std::abort();
	}

	static struct lua_debugger_context* init_debugger_context()
	{
		instance = new lua_debugger_context;

		GetLuaManageInstance()->InitLua("", "","");
		GetLuaManageInstance()->SetLuaErrorFunction(lua_run_error);
		GetLuaManageInstance()->InitLibs();
		lua_sethook(LuaScript::m_L, lua_Hook_call, LUA_MASKCOUNT, 1);

		auto context = instance;
		set_lua_Hook_call([context](const std::vector<lua_frame_info>& frames) {
			context->current_frames = &frames;
			context->wait_break();
		});
		context->net_start = 0;

		instance->network_thread = new std::thread([context]() {
			huo_lua::network* net_websocket = new huo_lua::network("/*", 9001);
			net_websocket->set_open_function([context](uWS::WebSocket<true, true, PerSocketData>* ws) {
				context->ws = ws;
			});
			net_websocket->set_message_function([context](std::string_view message) {
				context->continue_lua();
				return MESSAGE_NULL;
			});
			net_websocket->set_close_function([context]() {
				context->ws = NULL;
			});

			std::unique_lock<std::mutex> locker(context->mtx);
			context->net_start = 1;
			locker.unlock();
			context->cond.notify_one();

			net_websocket->run();
		});

		std::unique_lock<std::mutex> locker(context->mtx);
		while (context->net_start != 1) context->cond.wait(locker);
		locker.unlock();

		return instance;
	}

	struct lua_debugger_context* get_context()
	{
		if (instance) return instance;
		return init_debugger_context();
	}

	int lua_debugger_context::stop_lua()
	{
		std::unique_lock<std::mutex> locker(this->mtx);
		this->status = debug_status::debug_stop;
		while (this->status != debug_status::debug_run) this->cond.wait(locker);
		locker.unlock();
		return 0;
	}

	int lua_debugger_context::continue_lua()
	{
		std::unique_lock<std::mutex> locker(this->mtx);
		this->status = debug_status::debug_run;
		locker.unlock();
		this->cond.notify_one();
		return 0;
	}
	
	int lua_debugger_context::add_break_point(const break_point& bp)
	{
		this->bkps.push_back(bp);
		return 0;
	}

	int lua_debugger_context::wait_break()
	{
		const lua_frame_info* last_frame = &this->current_frames->front();
		for (auto& it : this->bkps)
		{
			if (last_frame->filepath == it.filepath && last_frame->line == it.line)
			{
				this->stop_lua();
				break;
			}
		}
		return 0;
	}

	int lua_debugger_context::start_lua(const std::string& lua_path)
	{
		GetLuaScript()->LuaDoFile(lua_path);
		return 0;
	}
}