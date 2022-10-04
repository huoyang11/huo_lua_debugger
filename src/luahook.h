#ifndef __LUAHOOK_H__
#define __LUAHOOK_H__

#include <vector>
#include <string>
#include <stdint.h>
#include <functional>

struct lua_State;
struct lua_Debug;

namespace huo_lua
{
	typedef struct variable
	{
		std::string var_name;
		std::string var_value;
	}variable;

	typedef struct lua_frame_info
	{
		struct 
		{
			uint16_t opcode;
			int a;
			int b;
			int c;
			int ax;
			int bx;
			int sb;
			int sc;
			int sbx;
			int isk;
		}code_arg;

		int line;
		std::string op_name;
		std::string funname;
		std::string filepath;
		std::vector<variable> locals;
		std::vector<variable> upvalues;
	}lua_frame_info;

	extern std::function<void(const std::vector<lua_frame_info>& frames)> Hook_call;

	void lua_Hook_call(lua_State* L, lua_Debug* ar);

	void set_lua_Hook_call(std::function<void(const std::vector<lua_frame_info>& frames)> fun);
}

#endif