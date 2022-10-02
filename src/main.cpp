#include "App.h"
#include "network.h"
#include "luacommon.h"
#include "luahook.h"

#include "LuaManage.h"
#include "LuaScript.h"
#include "LuaError.h"
#include "LuaTable.h"

using namespace huo_lua;
using namespace HiFun;

static void lua_run_error(LuaError* err)
{
    std::abort();
}

int main() 
{
	//huo_lua::network* net_websocket = new huo_lua::network("/*",9001);

	//net_websocket->run();

    GetLuaManageInstance()->InitLua("", "","");
    GetLuaManageInstance()->SetLuaErrorFunction(lua_run_error);
    GetLuaManageInstance()->InitLibs();
    lua_sethook(LuaScript::m_L, lua_Hook_call, LUA_MASKCOUNT, 1);
    GetLuaScript()->LuaDoFile("test.lua");

	return 0;
}