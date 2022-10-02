//--------------------------------------------------------------------
// 文件名:      LuaUserData.h
// 内  容:      lua UserData接口代理类
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUAUSERDATA_H__
#define __LUAUSERDATA_H__

#include <vector>
#include "LuaExDef.h"
#include "LuaManageDef.h"

struct Udata;
struct lua_State;
struct luaL_Reg;

namespace HiFun
{
	class LuaValue;

	class LUAEX_API LuaUserData
	{
	public:
		LuaUserData(size_t size);
		LuaUserData(lua_State *L, size_t size);
		LuaUserData(lua_State* L, size_t size, int nuvalue);
		LuaUserData(Udata* udata);

		void SetMetatable(const LuaValue& mt);
		void SetMetatable(struct Table *mt);

		struct Table* GetMetatable();
		struct Udata* GetRawData();

		void* GetUserMem();

		//void SetCtrlObjFunctions(const std::vector<luaL_Reg> &funs);

		void SetCtrlObjFunctions(luaL_Reg *funs);

		~LuaUserData();

	private:
		void Init();

	private:
		struct Udata* m_Udata;
		struct lua_State* m_L;
	};
}

#endif