#ifndef EzVex
#define EzVex 

// Lua Libs
#include "lua.hpp"

// Vex Pros Lib
#include "main.h"

// Serene Libs
#include "SereneXPros/vex.hpp"
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


    // initialize Lua interpreter
    L = luaL_newstate();

    // load Lua base libraries (print / math / etc)
    luaL_openlibs(L);
    
    // Loading Lua File
    int Error = luaL_dofile(L,"/usd/Serene/App.lua");

    using namespace pros::lcd;    

    if(Error) {
        set_text(1,"Serene: Failed to load lua file");
        set_text(2,lua_tostring(L,-1)); //print the error message
        lua_close(L);
        exit(-1);
    }
    else {
        set_text(2, "Serene: Lua File Loaded Succesfully");
    }

    // Run Init Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_init");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        lua_call(L,0,0);

    // clean up
    cleanup(L);

    
    
    // // Cleanup:  Deallocate all space assocatated with the lua state */
    // lua_close(L);
    
    // // Close Log File
    // fclose(logfile);

}

void serene_autonomous() {
    //open log file
    
 // Run Autonomous Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_autonomous");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        lua_call(L,0,0);

    // clean up
    cleanup(L);
}

void serene_competition_initialize() {
 // Run Competition Initialize Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_comp_init");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        lua_call(L,0,0);

    // clean up
    cleanup(L);
}

void serene_opcontrol() {
    // Run Operator Control Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_opcontrol");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        lua_call(L,0,0);

    // clean up
    cleanup(L);
}

void serene_disabled() {
     // Run Init Lua Code
    lua_getglobal(L,"vex");
    lua_getfield(L,-1,"on_disabled");
    if(!lua_isnil(L,-1) && lua_isfunction(L,-1))
        lua_call(L,0,0);

    // clean up
    cleanup(L);
}

#endif // EzVex