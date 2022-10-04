#ifndef __BREAK_POINT_H__
#define __BREAK_POINT_H__

#include <string>

namespace huo_lua
{
	struct lua_debugger_context;

	typedef struct break_point
	{
		int line;
		std::string filepath;
	}break_point;

	int break_point_handle(lua_debugger_context *ctx);
}

#endif