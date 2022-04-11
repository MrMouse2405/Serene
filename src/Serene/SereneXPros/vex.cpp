// pros
#include "main.h"
#include "vex.hpp"
#include "string.h"

/*

    Serene Vex APIS

*/
#include "motors.hpp"

/*

    vex.wait()

    args: seconds

    This function yields current pros::Task 
    for a given amount of seconds.
*/

static int vex_lib_wait(lua_State *L) {
    // 1000 milliseconds per second
    const int milliseconds = luaL_optinteger(L,1,1) * 1000;
    pros::Task::delay(milliseconds);
    return 0;
}

/*

    vex_lib_new

    This function is responsible for creating pros objects
    for controlling components. For example, pros::motor
    is a component that controls the motors of the robot.

    This function based on the arguments passed from user (lua code)
    initializes the object, by calling functions from a wrapper
    header file that correspond to that object.

    Note:
        The functions being called by this function to initialize objects, 
        will recieve a Lua Stack, that holds two objects.

        1) "Component Name String" -- used by this function
        2) {configs="-"}  -- a Lua table that holds data

    Lua Example:

        for creating a motor instance

        ```.lua
            
            local motor = vex.init("Motors",{port=1})
        
        ```
*/

static int vex_lib_init(lua_State *L) {
    
    // Discard Any Arguments that are more than 2
    lua_settop(L,2);
    
    // Get First String Argument
    const char *str = luaL_checkstring(L, 1);

    // Get Second Table Argument
    luaL_checktype(L,2,LUA_TTABLE);

    if (!strcmp(str,"Motor"))
        create_motor_instance(L);
    else
        luaL_error(L,"Serene: Component (%s) doesn't exist, maybe a typo?\n",str);
    

    return 1;
}

/*

    open_vex_lib

    called by linit.c for initializing this library
    and making it global.

    This library can be accessed in Lua Code

    ```.lua

        math.tan() -- Global Math Lib
        vex.init() -- Global Vex lib (this function makes it possible)
    
    ```


*/


extern "C" int open_vex_lib(lua_State *L) {
    
    static const luaL_Reg vexlib[] = {
        {"init", vex_lib_init},
        {"wait", vex_lib_wait},
        {NULL,NULL},
    };

    // stack = [vexlib]
    luaL_newlib(L,vexlib);

    return 1;
}
