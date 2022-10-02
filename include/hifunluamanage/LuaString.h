//--------------------------------------------------------------------
// 文件名:      LuaString.h
// 内  容:      lua String接口代理类
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUASTRING_H__
#define __LUASTRING_H__

#include <string>
#include "LuaExDef.h"

struct TString;
struct lua_State;

namespace HiFun
{
	class LUAEX_API LuaString
	{
	public:
		LuaString(const char *str = "");
		LuaString(const std::string &str);
		LuaString(lua_State *L,const char* str = "");
		LuaString(lua_State* L,const std::string& str);

		struct TString* GetRawData();

		const char* GetString();

		size_t GetLength();

		LuaString& operator=(const char* str);
		LuaString& operator=(const std::string& str);

		bool operator==(const LuaString& o);

		~LuaString();

	private:
		struct TString *m_str;
	};
}

#endif // !__LUASTRING_H__