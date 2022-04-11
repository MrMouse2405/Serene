#ifndef SERENE_VEX_MOTORS_API
#define SERENE_VEX_MOTORS_API

#include "../lua.hpp"
#define SERENE_MOTOR_API_METATABLE "SERENE_MOTORS_META"
void create_motor_instance(lua_State* L);

#endif //SERENE_VEX_MOTORS_API