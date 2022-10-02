//--------------------------------------------------------------------
// 文件名:      LuaManage.h
// 内  容:      Lua管理模块, lua环境和缓存相关配置
// 说  明:        
// 创建日期:    2022/07/19
// 创建人:      杨活
// 版权所有:    嗨放网络科技
//--------------------------------------------------------------------

#ifndef __LUAMANAGE_H__
#define __LUAMANAGE_H__

#include "LuaExDef.h"
#include "LuaManageDef.h"
#include "LuaTable.h"
#include <map>
#include <string>
#include <vector>

#define GetLuaManageInstance LuaManage::GetSinstance
#define GetLuaScript GetLuaManageInstance()->GetScript
#define GetLuaState GetLuaScript()->GetState

#define LuaAssert(log) GetLuaManageInstance()->LuaErrorLog(log)

struct Table;

namespace HiFun
{
	class LuaScript;
	class LuaUserData;
	class LuaTable;
	class LuaError;

	typedef void (*LuaErrorFun)(LuaError*);
	typedef int (*LuaMsgFun)(LuaTable& arg);

	typedef void *(*OpenFileCB)(const std::string &filepath);
	typedef int (*CloseFileCB)(void* ud);
	typedef void *(*GetFileBuffCB)(void* ud);
	typedef int (*GetFileSizeCB)(void *ud);

	typedef struct LoadFileApi
	{
		OpenFileCB openfile;
		CloseFileCB closefile;
		GetFileBuffCB getfilebuff;
		GetFileSizeCB getfilesize;
	}LoadFileApi;

	typedef struct LuaManageData
	{
		LuaError* lua_error;							//错误处理函数参数
		LuaErrorFun lua_errorfun;						//错误处理函数
		std::string module_path;						//lua 模块路径
		std::vector<std::string> lua_path;				//lua文件的环境
		std::vector<std::string> lua_cpath;				//lua动态库环境
		std::map<std::string, Table*> module_data;		//加载模块的缓存
		LuaTable *lua_signed;							//lua信号绑定表
		std::map<std::string, LuaMsgFun> luac_signed;	//c信号绑定表
		LoadFileApi lua_file;							//加载文件内容api
	}LuaManageData;

	class LUAEX_API LuaManage
	{
	public:
		LuaManage();
		~LuaManage();

		LuaScript* GetScript();

		/**
		@brief				初始化lua环境
		@module_path		lua模块目录
		@lua_path			lua目录
		@cpath				lua动态库目录
		*/
		void InitLua(const std::string &module_path,const std::string &lua_path, const std::string &cpath);

		/**
		@brief				初始化lua第三方库和util函数
		*/
		void InitLibs();

		/**
		@brief				初始化lua加载文件
		@fileapi			api
		*/
		void InitLoadFileApi(LoadFileApi& fileapi);

		LoadFileApi* GetLoadFileApi();
		/**
		@brief				设置lua的错误处理函数
		@errorfun			回调函数
		*/
		void SetLuaErrorFunction(LuaErrorFun errorfun);

		/**
		@brief				加载模块
		@module_name		模块名字
		@isreload			是否重载
		*/
		LuaTable LuaLoadModule(const std::string& module_name,bool isreload = false);
		bool LuaLoadModule(const std::string& module_name, LuaTable &ret, bool isreload = false);

		/**
		@brief				设置lua执行错误
		@error_log			错误日志
		*/
		void LuaErrorLog(const std::string &error_log);
		/**
		@brief				Lua执行错误数据
		@return				错误对象
		*/
		LuaError* GetLuaError();
		LuaErrorFun GetErrorFun();

		/**
		@brief				给绑定的lua函数发数据
		@msg_name			消息名称
		@arg				table
		*/
		bool SendMsg(const std::string &msg_name, LuaTable& arg);

		/**
		@brief				监听lua发送的消息
		@msg_name			消息名称
		@fun				回调函数
		*/
		void ListenMsg(const std::string& msg_name, LuaMsgFun fun);

		static int ListenMsg(lua_State *L);
		static int SendMsg(lua_State* L);

		/**
		@brief				注册绑定信号的lua函数
		@sig_name			信号名称
		@luafun				回调函数
		*/
		void RegisterSigned(const std::string &sig_name, LClosure* luafun);

		/**
		@brief				获取绑定的c函数
		@sig_name			信号名称
		@return				回调函数
		*/
		LuaMsgFun GetCSignedFun(const std::string& sig_name);

		void CleanLoadedTable();

	public:
		static LuaManage* GetSinstance();

	private:
		static LuaManage* s_instance;

	private:
		lua_State* m_L;
		LuaScript* m_script;
		LuaManageData* m_data;
	};
}

#endif