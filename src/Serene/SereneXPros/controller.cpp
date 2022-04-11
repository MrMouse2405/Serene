#include "controller.hpp"

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



}