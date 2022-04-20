/*

    Serene.hpp

    Note: Read main.cpp's documentation if you havent
    
    File responsible for initializing, and running Serene
    Framework. This file is also responsible for calling
    and running lua.

*/

#ifndef EzVex
#define EzVex 

#include "lua.hpp"
#include "main.h"
#include "SereneXPros/vex.hpp"
#include "SereneInternals/EventLoop.hpp"
#include <stdio.h>

/*

    Points to the Lua Interpreter

*/

static lua_State* L;

/*

    A clean up function
    
    Clears lua stack, and runs garbage collector

*/

static inline void cleanup (lua_State *L) {
    lua_settop(L,0);
    lua_gc(L,LUA_GCCOLLECT);
}

/*

    serene_initialize()

    When: Runs during initialization phase
    Why:  To quickly initializes Serene and Lua
          To run vex.on_init() if assigned by the user

*/

void serene_initialize() {

    printf("\n================INITIALISING===========\n");

    /*
    
        Initialize Lua
    
    */

    L = luaL_newstate();
    luaL_openlibs(L);
    

    /*
    
        Load our Lua File
    
    */

    int Error = luaL_dofile(L,"/usd/Serene/App.lua");


    if(Error) {
        printf("Serene: Failed to load lua file\n"); 
        printf("Lua: %s\n",lua_tostring(L,-1));
        exit(-1);
    }
    
    else
        printf("Serene: Lua File Loaded Successfully\n");
    

    // get our global vex table, get field on_init, if it's assigned with a function, run it.
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_init");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        if(lua_pcall(L,0,0,0))
            luaL_error(L, "Lua: %s",lua_tostring(L,-1));

    cleanup(L);

    printf("\n================INITIALISED===========\n");

}

/*

    serene_autonomous()

    When: Runs during autonomous phase
    Why:  To run vex.on_autonomous() if assigned by the user

*/


void serene_autonomous() {

    /*
    
    */

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