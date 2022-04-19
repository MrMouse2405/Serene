#ifndef EzVex
#define EzVex 

// Lua Libs
#include "lua.hpp"

// Vex Pros Lib
#include "main.h"

// Serene Libs
#include "SereneXPros/vex.hpp"
#include "SereneInternals/EventLoop.hpp"
#include <stdio.h>

// Lua Interpreter Stack
static lua_State* L;

static inline void cleanup (lua_State *L) {
    lua_settop(L,0);
    lua_gc(L,LUA_GCCOLLECT);
}

/*
    Quickly initializes Serene and Lua

    Runs vex.on_init() if assigned by the user

*/

void serene_initialize() {

    printf("\n================INITIALISING===========\n");

    // initialize Lua interpreter
    L = luaL_newstate();

    printf("1\n");

    // load Lua base libraries (print / math / etc)
    luaL_openlibs(L);

    
    printf("2\n");
    
    // Loading Lua File
    // MICRO SD BROKEN OR WHATEVER
    int Error = luaL_dofile(L,"/usd/Serene/App.lua");


    //  int Error = luaL_dostring(L,R""""(
    //     local controller = vex.init("Controller",{type="master"})
    //     local motor = vex.init("Motor",{port=1})

    //     local function run_motor(power)
    //         print(power)
    //         motor:set_speed(power)
    //     end


    //     controller:bind_periodic("RX",run_motor)

    //     vex.on_opcontrol = function()
    //         print("In OP CONTROL")
    //     end
    //  )"""");

    printf("3\n");

    using namespace pros::lcd;    

    if(Error) {
        printf("Serene: Failed to load lua file\n"); 
        printf("Lua: %s\n",lua_tostring(L,-1));
        //print the error message
        //lua_close(L);
        exit(-1);
    }
    else {
        printf("Serene: Lua File Loaded Successfully\n");
        set_text(2, "Serene: Lua File Loaded Succesfully");
    }

    // Run Init Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_init");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    // clean up
    cleanup(L);
    
    // // Cleanup:  Deallocate all space assocatated with the lua state */
    // lua_close(L);
    
    // // Close Log File
    // fclose(logfile);

    printf("\n================INITIALISED===========\n");

}

void serene_autonomous() {
    //open log file
    
 // Run Autonomous Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_autonomous");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
                if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    // Run Event Loop
    EventLoop::run_event_listener();
    EventLoop::run_event_loop();

    // clean up
    cleanup(L);
}

void serene_competition_initialize() {
 // Run Competition Initialize Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_comp_init");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    // clean up
    cleanup(L);
}

void serene_opcontrol() {

    printf("\n================RUNNING OP CONTROL===========\n");

    // Run Operator Control Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_opcontrol");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    // Run Event Loop
    EventLoop::run_event_listener();
    EventLoop::run_event_loop();

    // clean up
    cleanup(L);

    printf("\n================RAN OP CONTROL===========\n");
}

void serene_disabled() {
     // Run Init Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_disabled");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    // clean up
    cleanup(L);
}

#endif // EzVex