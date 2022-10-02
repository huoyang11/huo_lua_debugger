#ifndef __LUACOMMON_H__
#define __LUACOMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "lapi.h"
#include "lauxlib.h"
#include "lcode.h"
#include "lctype.h"
#include "ldebug.h"
#include "lfunc.h"
#include "lua.h"
#include "lvm.h"
#include "ltm.h"
#include "ltable.h"
#include "lstring.h"
#include "lobject.h"
#include "luaconf.h"
#include "lualib.h"
#include "lauxlib.h"
#include "mime.h"
#include "luasocket.h"
#include "bson.h"

int luaopen_cjson(lua_State* l);
int luaopen_bson(lua_State* L);
#ifdef __cplusplus
}
#endif

#endif