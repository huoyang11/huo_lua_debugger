//--------------------------------------------------------------------
// 文件名:      LuaError.h
// 内  容:      Lua错误信息,提供获取lua执行出错的信息
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUAERROR_H__
#define __LUAERROR_H__

#include <string>
#include <vector>
#include "LuaExDef.h"

struct lua_State;

namespace HiFun
{
	class LuaManage;
	int LuaErrorCall(lua_State* L);

	struct LuaErrorData
	{
		std::string errorlog;
		std::string traceback;
	};

	class LUAEX_API LuaError
	{
	public:
		LuaError(lua_State *L);
		~LuaError();

		//get error log
		std::string GetErrorLog();

		std::string GetTraceback();

		static std::string Traceback(lua_State* L);

	public:
		LuaErrorData* m_data;

	private:
		lua_State* m_L;
	};
}

#endif // !__LUAERROR_H__