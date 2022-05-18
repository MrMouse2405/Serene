#include "main.h"
#include "motors.hpp"
#include <iostream>

/*

    motor_instance_set_speed(lua_State *l)

    This method is responsible for setting speed of an motor object
    given it's magnitude defined by the user.


    Lua Example:

    ```.lua

        motor:set_speed(100)
        
    ```

*/

static int motor_instance_set_speed(lua_State *L)
{

    // stack = [self]
    pros::Motor *self = (pros::Motor *)luaL_checkudata(L, 1, SERENE_MOTOR_API_METATABLE);

    // stack = [self, magnitude]
    int magnitude = luaL_checkinteger(L, 2);

    self->move(magnitude);

    return 0;
}

/*

    motor_instance_set_brake(lua_State *l)

    This method is responsible for setting braking options of a motor object
    given it's task defined by the user.


    Lua Example:

    ```.lua

        motor:set_brake(coast)
        motor:set_brake(hold)

    ```

*/

static int motor_instance_set_brake(lua_State *L)
{

    /*
        typedef enum motor_brake_mode_e {
            E_MOTOR_BRAKE_COAST = 0, // Motor coasts when stopped, traditional behavior
            E_MOTOR_BRAKE_BRAKE = 1, // Motor brakes when stopped
            E_MOTOR_BRAKE_HOLD = 2, // Motor actively holds position when stopped
            E_MOTOR_BRAKE_INVALID = INT32_MAX
        } motor_brake_mode_e_t;
    */

    // stack = [self]
    pros::Motor *self = (pros::Motor *)luaL_checkudata(L, 1, SERENE_MOTOR_API_METATABLE);
    const char *brakeType = luaL_checkstring(L, 2);
    pros::motor_brake_mode_e_t id;

    if (!strcmp(brakeType, "coast"))
        id = pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST;
    else if (!strcmp(brakeType, "hold"))
        id = pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD;
    else
        luaL_error(L, "Serene/Motors: Brake type %s doesn't exist. (only coast or hold).\n", brakeType);

    // stack = [self, magnitude]

    self->set_brake_mode(id);

    return 0;
}

/*

    create_motor_instance(lua_State *L)


    In a nutshell, this creates a pros::motor,
    stores it as a Lua light user data, and
    allows people to call methods on it via
    lua interface.

    This function is only to be called by vex.cpp/vex_lib_init()

*/

void create_motor_instance(lua_State *L)
{

    // Remember, The Second Argument Being Passed is the Lua Table
    // stack = ["Component Name",{}]
    // we will now get our configs

    /*

        Constructor Args:
            port: int, requred


    */

    // stack = ["Component Name",{},port]
    lua_getfield(L, 2, "port");

    int port = luaL_checkinteger(L, 3);
    if (port > 21 || port < 1)
    {
        luaL_error(L, "Serene/Motor: Port %s doesn't exist. Only ports 1-21 are available", std::to_string(port));
        return;
    }

    /*

        reverse: boolean, default = false

    */
    // stack = ["Component Name",{},port,reverse]
    lua_getfield(L, 2, "reverse");
    bool reverse = false;
    // This field is optional
    if (!lua_isnil(L, 4))
    {
        if (lua_isboolean(L, 4))
        {
            reverse = lua_toboolean(L, 4) ? true : false;
        }
        else
            luaL_error(L, "Serene/Motor: Expected boolean value (true/false) for reverse, recieved invalid value");
    }

    // Clearing Stack to make our life easy
    lua_settop(L, 0);

    // If Metatable isn't initialized, then initialize it
    if (luaL_newmetatable(L, SERENE_MOTOR_API_METATABLE))
    {
        static struct luaL_Reg motors_metatable[]
        {
            {"set_speed", motor_instance_set_speed},
                {"set_brake", motor_instance_set_brake},
                {NULL, NULL},
        };
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2); /* pushes the metatable */
        lua_settable(L, -3);

        luaL_setfuncs(L, motors_metatable, 0);

        lua_settop(L, 0);
    }

    // Create Pros Motor
    pros::Motor *new_motor = new (lua_newuserdata(L, sizeof(pros::Motor))) pros::Motor(port, reverse);

    // Set Metatable
    luaL_getmetatable(L, SERENE_MOTOR_API_METATABLE);
    lua_setmetatable(L, -2);
}