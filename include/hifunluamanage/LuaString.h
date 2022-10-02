//--------------------------------------------------------------------
// �ļ���:      LuaString.h
// ��  ��:      lua String�ӿڴ�����
// ˵  ��:        
// ��������:    2022/07/19
// ������:      ���
// ��Ȩ����:    �˷�����Ƽ�
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