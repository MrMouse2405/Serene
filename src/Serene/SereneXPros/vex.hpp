// Common Includes
#include "../lua.hpp"


#ifndef LUA_VEX_LIBRARY
#define LUA_VEX_LIBRARY

// EzVexLibs
#define SERENE_VEX_LIBRARY "vex"
#define SERENE_VEX_LIB_META "metatable_vex"

#ifdef __cplusplus
    extern "C" {
#endif

    int open_vex_lib(lua_State *L);

#ifdef __cplusplus
    }
#endif




#endif // LUA_VEX_LIBRARY