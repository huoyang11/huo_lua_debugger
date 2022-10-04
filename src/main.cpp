#include "lua_debugger.h"

using namespace huo_lua;

int main() 
{
	struct lua_debugger_context *context = get_context();
	context->add_break_point({15,"test.lua" });
	context->start_lua("test.lua");

	return 0;
}