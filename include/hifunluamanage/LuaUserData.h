//--------------------------------------------------------------------
// �ļ���:      LuaUserData.h
// ��  ��:      lua UserData�ӿڴ�����
// ˵  ��:        
// ��������:    2022/07/19
// ������:      ���
// ��Ȩ����:    �˷�����Ƽ�
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