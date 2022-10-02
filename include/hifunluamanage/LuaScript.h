//--------------------------------------------------------------------
// 文件名:      LuaScript.h
// 内  容:      Lua脚本,操作lua执行和获取设置lua状态机相关的配置
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUASCRIPT_H__
#define __LUASCRIPT_H__

#include <vector>
#include <string>

#include "LuaValue.h"
#include "LuaExDef.h"

#include "LuaManageDef.h"

#define FunParserArgs LuaScript::LuaParserArgs
#define FunReturn0 {return 0;}
#define FunReturn1(L,v) {LuaScript::LuaReturn1(L,v);return 1;}
#define FunReturn(L,v) {LuaScript::LuaReturn(L,{v});return 1;}

namespace HiFun
{
	class LuaTable;

	class LUAEX_API LuaScript
	{
		friend class LuaManage;
	private:
		LuaScript();
	public:
		~LuaScript();

		struct lua_State* GetState();

		/**
		@brief				设置lua的全局变量(_G)
		@name				变量名称
		@value				变量的值
		*/
		void SetGlobal(const std::string &name,const LuaValue &value);

		/**
		@brief				获取lua的全局变量(_G)
		@name				变量名称
		@return				变量的值
		*/
		LuaValue GetGlobal(const std::string& name);

		/**
		@brief				获取lua的全局变量(_G)
		@name				变量名称
		@ret				变量的值
		*/
		void GetGlobal(const std::string& name, LuaValue &ret);

		/**
		@brief				luaL_loadfile的替代
		@filename			文件路径
		@ret				非0代表加载出错
		*/
		int LuaLoadFile(const std::string &filename);

		/**
		@brief				执行lua文件
		@filename			文件路径
		@nresults			文件的返回值个数
		@return				文件返回变量
		*/
		std::vector<LuaValue> LuaDoFile(const std::string &filename, int nresults);

		/**
		@brief				执行lua文件(使用引用减少临时拷贝)
		@filename			文件路径
		@nresults			文件的返回值个数
		@ret				文件返回变量
		*/
		void LuaDoFile(const std::string& filename, int nresults, std::vector<LuaValue> &ret);

		/**
		@brief				执行lua文件(但返回值)
		@filename			文件路径
		@ret				文件变量
		*/
		void LuaDoFile(const std::string& filename, LuaValue& ret);

		/**
		@brief				执行lua文件(无返回值)
		@filename			文件路径
		*/
		void LuaDoFile(const std::string& filename);

		/**
		@brief				执行lua全局函数
		@funname			函数名称
		@nresults			函数返回值个数
		@nargs				函数参数
		@return				函数返回值
		*/
		std::vector<LuaValue> LuaDoGlobalFunction(const std::string &funname,int nresults, const std::vector<LuaValue> &nargs);

		/**
		@brief				执行lua全局函数(使用引用减少拷贝)
		@funname			函数名称
		@nresults			函数返回值个数
		@nargs				函数参数
		@ret				函数返回值
		*/
		void LuaDoGlobalFunction(const std::string& funname, int nresults, const std::vector<LuaValue>& nargs, std::vector<LuaValue>& ret);

		/**
		@brief				执行lua函数
		@func				函数
		@nresults			函数返回值个数
		@nargs				函数参数
		@return				函数返回值
		*/
		std::vector<LuaValue> LuaDoFunction(const LuaValue& func, int nresults, const std::vector<LuaValue>& nargs);

		/**
		@brief				执行lua函数(无参数，无返回值函数)
		*/
		void LuaDoFunction(const LuaValue& func);

		/**
		@brief				执行lua函数(无参数,有返回值)
		@func				函数
		@nargs				函数参数
		@ret				函数返回值
		*/
		void LuaDoFunction(const LuaValue& func, int nresults, const std::vector<LuaValue>& nargs, std::vector<LuaValue>& ret);

		/**
		@brief				执行lua函数(无返回值,有一个参数)
		@func				函数
		@args				函数参数
		*/
		void LuaDoFunction(const LuaValue& func, const LuaValue& args);

		/**
		@brief				执行lua函数(无返回值,有多个参数)
		@func				函数
		@nargs				函数参数
		*/
		void LuaDoFunction(const LuaValue& func, const std::vector<LuaValue>& nargs);

		/**
		@brief				Require lua模块(返回一个值)
		@filename			模块名称
		@ret				模块返回值
		*/
		LuaValue LuaRequire(const std::string& filename);

		/**
		@brief				设置lua动态库环境变量
		@path				路径+?.lua  (如:c:\\?.lua)
		*/
		void AddLuaCpath(const std::string &path);

		/**
		@brief				设置lua文件环境变量
		@path				路径+?.dll  (如:c:\\?.dll)
		*/
		void AddLuapath(const std::string& path);

		/**
		@brief				获取lua动态库环境变量
		@return				路径
		*/
		const char* GetLuaCpath();

		/**
		@brief				获取lua文件环境变量
		@return				路径
		*/
		const char* GetLuapath();

		/**
		@brief				注册一组函数
		*/
		void RegistryFunctions(luaL_Reg* Funs);

		/**
		@brief				将一组函数放到table里并注册table
		*/
		void RegistryFunctions(const std::string &tablename,luaL_Reg* Funs);

		//注册全局函数
		void RegistryFunction(const std::string &funname, lua_CFunction val);
		//注册userData
		bool RegistryUserdata(const std::string &username, lua_CFunction CreateObj, lua_CFunction DeleteObj);

		//获取元表
		const TValue* GetMetatable(const std::string &name);

		bool CleanLoadedTable();

	private:
		void HookLuaFunction();
		void UtilLuaFunction();

	public:
		/**
		@brief				lua c函数参数解析
		@L					lua状态机
		@return				参数数组
		*/
		static std::vector<LuaValue> LuaParserArgs(lua_State *L);

		/**
		@brief				lua c函数参数解析(引用减少拷贝)
		@L					lua状态机
		@ret				参数数组
		*/
		static void LuaParserArgs(lua_State* L, std::vector<LuaValue>& ret);

		/**
		@brief				lua c函数返回(有多少个返回值,最后就return 多少)
		@L					lua状态机
		@retvalue			返回值数组
		*/
		static void LuaReturn(lua_State* L, const std::vector<LuaValue> &retvalue);

		/**
		@brief				向lua压栈
		@L					lua状态机
		@values				压入的值
		*/
		static void LuaPushValue(lua_State* L, const std::vector<LuaValue>& values);

		/**
		@brief				向lua压栈
		@L					lua状态机
		@value				压入的值
		*/
		static void LuaPushValue(lua_State* L, const LuaValue& value);

		/**
		@brief				获取lua栈中的元素
		@L					lua状态机
		@value				索引
		*/
		static LuaValue GetLuaStack(lua_State* L, int index);

		/**
		@brief				lua c函数返回(单返回值)
		@L					lua状态机
		@retvalue			返回值
		*/
		static void LuaReturn1(lua_State* L, const LuaValue& retvalue);

		static int LuaLoadFile(lua_State* L, const std::string& filename);
	public:
		static struct lua_State *m_L;
	private:
		struct global_State* m_g;
		LuaTable* m_luaRegistry;				//Registry 表代理
		LuaTable* m_luaRidxGlobal;				//_G 表代理
		LuaTable* m_luapackage;					//lua package库代理
	};
}

#endif // !LuaScript