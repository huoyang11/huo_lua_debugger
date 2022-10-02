//--------------------------------------------------------------------
// 文件名:      LuaMetatable.h
// 内  容:      Lua元表接口代理类
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUAMETATABLE_H__
#define __LUAMETATABLE_H__

#include <string>
#include "LuaExDef.h"

struct lua_State;
struct Table;

namespace HiFun
{
	class LuaTable;
	class LuaValue;

	enum LUAEX_API LuaTmType
	{
		LuaTmType_index, LuaTmType_newindex,
		LuaTmType_gc, LuaTmType_mode, LuaTmType_len, LuaTmType_eq,
		LuaTmType_add, LuaTmType_sub, LuaTmType_mul, LuaTmType_mod, LuaTmType_pow,
		LuaTmType_div, LuaTmType_idiv,
		LuaTmType_band, LuaTmType_bor, LuaTmType_bxor, LuaTmType_shl, LuaTmType_shr,
		LuaTmType_unm, LuaTmType_bnot, LuaTmType_lt, LuaTmType_le,
		LuaTmType_concat, LuaTmType_call, LuaTmType_close
	};

	class LUAEX_API LuaMetatable
	{
	public:
		LuaMetatable(const std::string &name);
		LuaMetatable(const std::string &name,lua_State* L);
		LuaMetatable(Table *t);
		LuaMetatable(lua_State *L,Table* t);

		Table* GetMetatable() const;
		void SetMetatable(enum LuaTmType, const LuaValue& val);

		~LuaMetatable();

	private:
		void Init(const std::string &name);

	private:
		Table *m_mt;
		lua_State* m_L;
	};
}

#endif