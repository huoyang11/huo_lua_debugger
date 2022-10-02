//--------------------------------------------------------------------
// 文件名:      LuaVaue.h
// 内  容:      lua Tvalue接口代理类
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUAVALUE_H__
#define __LUAVALUE_H__

#include <string>
#include "LuaUserData.h"
#include "LuaExDef.h"

#include "LuaManageDef.h"

namespace HiFun
{
	class LuaString;
	class LuaTable;

	class LUAEX_API LuaValue
	{
	public:
		LuaValue();
		LuaValue(const struct TValue* val);

		LuaValue(const char *val);
		LuaValue(LuaString *val);
		LuaValue(const std::string &val);

		LuaValue(struct Table *val);
		LuaValue(LuaTable val);
		LuaValue(LuaTable *val);

		LuaValue(lua_Integer val);
		LuaValue(int val);
		LuaValue(unsigned int val);
		LuaValue(lua_Number val);

		LuaValue(bool val);

		LuaValue(lua_CFunction val);
		LuaValue(LClosure* val);
		LuaValue(CClosure* val);

		LuaValue(lua_State* val);

		LuaValue(Udata *val); //LUA_VUSERDATA
		LuaValue(LuaUserData *val); //LUA_VUSERDATA
		LuaValue(void *val);  //LUA_VLIGHTUSERDATA

		LuaValue(lua_State* L, const struct TValue* val);
		LuaValue(lua_State* L, const char* val);
		LuaValue(lua_State* L, LuaString* val);
		LuaValue(lua_State* L, const std::string& val);
		LuaValue(lua_State* L, struct Table* val);
		LuaValue(lua_State* L, LuaTable val);
		LuaValue(lua_State* L, LuaTable *val);
		LuaValue(lua_State* L, lua_Integer val);
		LuaValue(lua_State* L, int val);
		LuaValue(lua_State* L, lua_Number val);
		LuaValue(lua_State* L, bool val);
		LuaValue(lua_State* L, lua_CFunction val);
		LuaValue(lua_State* L, LClosure* val);
		LuaValue(lua_State* L, CClosure* val);
		LuaValue(lua_State* L, lua_State* val);
		LuaValue(lua_State* L, Udata* val); //LUA_VUSERDATA
		LuaValue(lua_State* L, void* val);  //LUA_VLIGHTUSERDATA

		LuaValue(const LuaValue& o);

		~LuaValue();

		//=======================================
		LuaValue& operator=(LuaString* val);
		LuaValue& operator=(const char* val);
		LuaValue& operator=(const struct TValue* val);
		LuaValue& operator=(const std::string &val);

		LuaValue& operator=(struct Table* val);
		LuaValue& operator=(LuaTable val);
		LuaValue& operator=(LuaTable *val);

		LuaValue& operator=(lua_Integer val);
		LuaValue& operator=(lua_Number val);

		LuaValue& operator=(bool val);

		LuaValue& operator=(lua_CFunction val);
		LuaValue& operator=(LClosure* val);
		LuaValue& operator=(CClosure* val);

		LuaValue& operator=(lua_State* val);

		LuaValue& operator=(Udata* val);
		LuaValue& operator=(void* val);
		LuaValue& operator=(const LuaValue &val);

		bool operator==(const LuaValue& o);

		//======================================

		const char* GetString(const std::string &def = "");
		struct Table* GetTable(struct Table* def = NULL);
		lua_Integer GetInteger(lua_Integer def = 0);

		int8_t GetInt8(int8_t def = 0);
		int16_t GetInt16(int16_t def = 0);
		int32_t GetInt32(int32_t def = 0);
		int64_t GetInt64(int64_t def = 0);
		uint8_t GetUint8(uint8_t def = 0);
		uint16_t GetUint16(uint16_t def = 0);
		uint32_t GetUint32(uint32_t def = 0);
		uint64_t GetUint64(uint64_t def = 0);

		lua_Number GetNumber(lua_Number def = 0);
		float GetFloat(float def = 0.0f);
		double GetDouble(double def = 0.0);

		bool GetBool(bool def = false);
		lua_CFunction GetFunction(lua_CFunction def = NULL);
		LClosure* GetLClosure(LClosure* def = NULL);
		CClosure* GetCClosure(CClosure* def = NULL);
		lua_State* GetState(lua_State* def = NULL);
		Udata* GetUserData(Udata* def = NULL);
		LuaUserData GetLuaUserData();

		void* GetLightUserData(void* def = NULL);

		//=====================================
		bool isNil();
		bool isString();
		bool isTable();
		bool isInteger();
		bool isNumber();
		bool isBool();
		bool isFunction();
		bool isLClosure();
		bool isCClosure();
		bool isState();
		bool isUserData();
		bool isLightUserData();

		const struct TValue* GetValue();
		unsigned char GetValueType();

	private:
		struct TValue *m_value;
	};
}

#endif