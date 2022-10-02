//--------------------------------------------------------------------
// 文件名:      LuaTable.h
// 内  容:      lua table接口代理类
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUATABLE_H__
#define __LUATABLE_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

#include "LuaValue.h"
#include "LuaMetatable.h"
#include "LuaExDef.h"
#include "LuaManageDef.h"

union StackValue;
typedef StackValue* StkId;

namespace HiFun
{
	class LuaValue;
	class LUAEX_API LuaTable
	{
	public:
		LuaTable(lua_State* L);
		LuaTable();
		LuaTable(struct Table* t);
		LuaTable(LuaValue value);
		LuaTable(unsigned int nasize, unsigned int nhsize);
		LuaTable(lua_State* L,struct Table* t);
		LuaTable(lua_State* L,LuaValue value);
		LuaTable(lua_State* L,unsigned int nasize, unsigned int nhsize);
		LuaTable(const LuaTable& value);

		LuaTable& operator=(LuaValue value);
		LuaTable& operator=(const LuaTable& value);
		LuaTable& operator=(struct Table* t);

		~LuaTable();

		void Insert(const char *key, const LuaValue &value);
		void Insert(const std::string &key, const LuaValue& value);
		void Insert(const lua_Integer key, const LuaValue& value);

		void Delete(const char* key);
		void Delete(const std::string &key);
		void Delete(const lua_Integer key);

		LuaValue GetValue(const char* key);
		LuaValue GetValue(const std::string key);
		LuaValue GetValue(const lua_Integer key);

		bool HasKey(const char* key);
		bool HasKey(const std::string key);
		bool HasKey(const lua_Integer key);

		void GetValue(const char* key, LuaValue &Val);
		void GetValue(const std::string key, LuaValue& Val);
		void GetValue(const lua_Integer key, LuaValue& Val);

		LuaValue GetValue(lua_State *L, const char* key);
		LuaValue GetValue(lua_State *L, const std::string key);
		LuaValue GetValue(lua_State* L, const lua_Integer key);

		const TValue* GetTValue(const char* key);
		const TValue* GetTValue(const std::string key);
		const TValue* GetTValue(const lua_Integer key);

		uint8_t* Serialize(int32_t& size);
		Table* Deserialize(uint8_t* buf);

		std::string DumpJsonStr();

		bool IsEmpty();
		uint64_t length();

		void clean();

		struct Table* GetRawData() const;

		//================================
		void SetMetatable(const LuaMetatable &mt);
		void SetMetatable(const LuaTable &mt);
		struct Table* GetMetatable();

		std::vector<LuaValue> Dofunction(const std::string &funname, int nres ,std::vector<LuaValue> args, bool exception = false);

		void Dofunction(const std::string& funname, bool exception = false);

		void Dofunction(const std::string& funname, LuaValue arg, bool exception = false);

		void Dofunction(const std::string& funname, LuaValue arg1, LuaValue arg2, bool exception = false);

	public:
		static bool IsEmpty(Table* t);
		static uint64_t length(Table* t);
		static void clean(Table* t);
		static void Insert(lua_State *L,Table* t,const char* key, const LuaValue& value);
		static void Insert(lua_State *L,Table* t,const std::string& key, const LuaValue& value);
		static void Insert(lua_State *L,Table* t,const lua_Integer key, const LuaValue& value);
		static void GetValue(Table* t,const char* key, LuaValue& Val);
		static void GetValue(Table* t,const std::string key, LuaValue& Val);
		static void GetValue(Table* t,const lua_Integer key, LuaValue& Val);
		static LuaValue GetTableIndex(Table* t, const std::string name, bool exception = false);

		static uint8_t *Serialize(lua_State* L, Table* t, int32_t&size);
		static Table* Deserialize(lua_State* L, uint8_t *buf);

		static std::string DumpJsonStr(Table* t);
		static bool JsonStr2LuaTable(const std::string& str, LuaTable& tab);

	private:
		void Init();

	public:
		static struct Table* g_nametable;
		static uint64_t nameindex;

	private:
		std::string m_nameobj;
		struct Table* m_table;
	};
}

#endif