#ifndef __BSON_H__
#define __BSON_H__

#include <stdint.h>

struct lua_State;

int lua_encode(lua_State* L);

int lua_decode(lua_State* L, uint8_t* data);

int32_t
get_length(const uint8_t* data);

#endif