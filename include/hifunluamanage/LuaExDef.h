#ifndef __LUAEXDEF_H__
#define __LUAEXDEF_H__

#if defined LUA_EXPORTS
#define LUAEX_API __declspec(dllexport)
#elif defined LUA_IMPORTS
#define LUAEX_API __declspec(dllimport)
#else
#define LUAEX_API  
#endif

#endif