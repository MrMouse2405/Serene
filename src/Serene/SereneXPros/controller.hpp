#ifndef SERENE_VEX_CONTROLLER_API
#define SERENE_VEX_CONTROLLER_API
#include "../lua.hpp"
#define SERENE_CONTROLLER_API_METATABLE "SERENE_CONTROLLER_META"

void create_controller_instance(lua_State *L);
#endif