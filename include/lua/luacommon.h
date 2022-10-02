#ifndef __LUACOMMON_H__
#define __LUACOMMON_H__

extern "C"
{
#include "lapi.h"
#include "lauxlib.h"
#include "lcode.h"
#include "lctype.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "llimits.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lopnames.h"
#include "lparser.h"
#include "lprefix.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lua.h"
#include "lua.hpp"
#include "luaconf.h"
#include "lualib.h"
#include "lundump.h"
#include "lvm.h"
#include "lzio.h"

	int luaopen_cjson(lua_State* l);
}

#endif