#ifndef __LUAFROMAT_H__
#define __LUAFROMAT_H__

#include "LuaExDef.h"
#include "LuaValue.h"
#include <vector>
#include <string>

namespace HiFun
{
	class LUAEX_API LuaFromat
	{
	public:
		static LuaValue RegFun2LuaVaue(luaL_Reg *Funs);
		static std::vector<std::string> stringSplit(const std::string& str, char delim);
	};
}

#endif // !__LUAFROMAT_H__