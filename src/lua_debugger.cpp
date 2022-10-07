#include "lua_debugger.h"
#include "luahook.h"
#include "network.h"
#include "luacommon.h"

#include "LuaManage.h"
#include "LuaScript.h"
#include "LuaError.h"
#include "LuaTable.h"

#include "nlohmann/json.hpp"

#define NULL_BK {-1,-1,0,""}

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
		lua_sethook(LuaScript::m_L, lua_Hook_call, LUA_MASKCOUNT | LUA_MASKRET, 1);

		auto context = instance;
		set_lua_Hook_call([context](const std::vector<lua_frame_info>& frames) {
			context->current_frames = &frames;
			context->wait_stop();
			context->wait_break();
		});
		context->net_start = 0;

		instance->network_thread = new std::thread([context]() {
			huo_lua::network* net_websocket = new huo_lua::network("/*", 9001);
			net_websocket->set_open_function([context](uWS::WebSocket<true, true, PerSocketData>* ws) {
				context->ws = ws;
			});
			net_websocket->set_message_function([context](std::string_view message) {
				nlohmann::json data = nlohmann::json::parse(message);
				std::string ctrl = data["ctrl"];
				if (ctrl == "n") {
					context->next();
					context->continue_lua();
				}
				if (ctrl == "s") {
					context->step();
					context->continue_lua();
				}
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

	void lua_debugger_context::set_lua_status(enum debug_status status)
	{
		this->status = status;
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
			if ((last_frame->filepath == it.filepath && last_frame->line == it.line && it.opcode == -1) || 
				(last_frame->filepath == it.filepath && last_frame->line == it.line && it.opcode == last_frame->code_arg.opcode))
			{
				if (it.opcode == -1) it.opcode = last_frame->code_arg.opcode;
				this->stop_lua();
				return 0;
			}
		}

		if ((last_frame->filepath == next_bk.filepath && last_frame->line == next_bk.line && next_bk.opcode == -1) ||
			(last_frame->filepath == next_bk.filepath && last_frame->line == next_bk.line && next_bk.opcode == last_frame->code_arg.opcode))
		{
			if (next_bk.break_type == break_step)
			{
				StkId base = last_frame->ci->func + 1;
				StkId ra = base + last_frame->code_arg.a;

				if (next_bk.opcode == OP_CALL)
				{
					lua_Debug ar = { 0 };
					TValue* func = s2v(ra);
					if (!ttisLclosure(func)) return this->next();
					Closure* cl = ttisclosure(func) ? clvalue(func) : NULL;
					funcinfo(&ar, cl);
					int line = luaG_getfuncline(getproto(func), 0);
					this->next_bk = { line ,-1,break_none , ar.short_src };

					return 0;
				}
				if (next_bk.opcode == OP_TAILCALL)
				{
					return 0;
				}

				return 0;
			}

			if (next_bk.opcode == -1) next_bk.opcode = last_frame->code_arg.opcode;
			this->stop_lua();
		}

		return 0;
	}

	int lua_debugger_context::wait_stop()
	{
		if (this->status == debug_status::debug_stop) this->stop_lua();
		return 0;
	}

	int lua_debugger_context::start_lua(const std::string& lua_path)
	{
		GetLuaScript()->LuaDoFile(lua_path);
		return 0;
	}

	int lua_debugger_context::next()
	{
		const lua_frame_info* last_frame = &this->current_frames->front();
		int current_line = last_frame->line;
		CallInfo* current_ci = last_frame->ci;
		const Proto* current_p = ci_func(current_ci)->p;

		Instruction* code_end = current_p->code + current_p->sizecode;
		const Instruction* it = current_ci->u.l.savedpc;
		for (; it != code_end + 1;it++)
		{
			int line = luaG_getfuncline(current_p, pcRel(it, current_p));
			if (current_line != line)
			{
				this->next_bk = { line , -1 ,0, last_frame->filepath };
				return 0;
			}
		}

		return find_previous_break(1);
	}

	int lua_debugger_context::step()
	{
		const lua_frame_info* last_frame = &this->current_frames->front();
		int current_line = last_frame->line;
		CallInfo* current_ci = last_frame->ci;
		const Proto* current_p = ci_func(current_ci)->p;

		Instruction* code_end = current_p->code + current_p->sizecode;
		const Instruction* it = current_ci->u.l.savedpc;
		for (; it != code_end + 1; it++)
		{
			int line = luaG_getfuncline(current_p, pcRel(it, current_p));
			if (current_line == line)
			{
				OpCode opcode = GET_OPCODE(*it);
				if (opcode == OP_CALL || opcode == OP_TAILCALL)
				{
					this->next_bk = { line , (int16_t)opcode ,1,last_frame->filepath };
					return 0;
				}

				continue;
			}

			if (current_line != line)
			{
				this->next_bk = { line , -1 ,0,last_frame->filepath };
				return 0;
			}
		}

		return find_previous_break(1);
	}

	int lua_debugger_context::find_previous_break(int index)
	{
		if (this->current_frames->size() - 1 < index || index < 0)
		{
			this->next_bk = NULL_BK;
			return -1;
		}

		const lua_frame_info* frame = &this->current_frames->at(index);
		int current_line = frame->line;
		CallInfo* current_ci = frame->ci;
		const Proto* current_p = ci_func(current_ci)->p;

		Instruction* code_end = current_p->code + (current_p->sizecode - 1);
		const Instruction* it = current_ci->u.l.savedpc + 1;
		for (; it != code_end; it++)
		{
			int line = luaG_getfuncline(current_p, pcRel(it, current_p));
			if (current_line != line)
			{
				this->next_bk = { line , -1 , 0,frame->filepath };
				return 0;
			}
		}

		return find_previous_break(index - 1);
	}
}