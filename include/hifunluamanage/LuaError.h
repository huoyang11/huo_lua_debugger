//--------------------------------------------------------------------
// �ļ���:      LuaError.h
// ��  ��:      Lua������Ϣ,�ṩ��ȡluaִ�г������Ϣ
// ˵  ��:        
// ��������:    2022/07/19
// ������:      ���
// ��Ȩ����:    �˷�����Ƽ�
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