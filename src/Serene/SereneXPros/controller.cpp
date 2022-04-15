#include "main.h"
#include "string.h"
#include "controller.hpp"

/*



*/

void create_controller_instance(lua_State *L) {

    // Remember, The Second Argument Being Passed is the Lua Table
    // stack = ["Component Name",{}] 
    // we will now get our configs

    /*
    
        Constructor Args:
            type: string, requred

    
    */
    // stack - ["Component Name", {}]
    lua_getfield(L,2,"type");
    // stack = ["Component Name", {}, type: string]
    const char *s = lua_tostring(L,3);

    pros::controller_id_e_t id;
    
    if (!strcmp(s,"master")) 
        id = pros::controller_id_e_t::E_CONTROLLER_MASTER;
    else if (!strcmp(s,"partner"))
        id = pros::controller_id_e_t::E_CONTROLLER_PARTNER;
    else
        luaL_error(L,"Serene/Controller: Controller type %s doesn't exist. (only master or partner).\n",s);

    // clearing entire stack to make our life easy
    lua_settop(L,0);

    // creating our metatable if it doesn't exist
    if (luaL_newmetatable(L,SERENE_CONTROLLER_API_METATABLE)) {
        static struct luaL_Reg controller_metatable[] {
            {"bind_to_analog_x", NULL},
            {"bind_to_analog_y", NULL},
            {NULL,NULL},
        };

        lua_pushstring(L,"__index");
        lua_pushvalue(L,-2);
        lua_settable(L,-3);
        //luaL_setfunc(L,controller_metatable,0);

        lua_settop(L,0);
    }


    // Create Pros Controller
    pros::Controller * new_controller = 
        new (lua_newuserdata(L,sizeof(pros::Controller))) pros::Controller(id);


    // Set metatable
    luaL_getmetatable(L,SERENE_CONTROLLER_API_METATABLE);
    lua_setmetatable(L,-2);

    // DONT CLEAR THE STACK
}