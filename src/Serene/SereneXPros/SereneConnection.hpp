#ifndef EZ_VEX_CONNECTION
#define EZ_VEX_CONNECTION

#include "../Lua54/lua.hpp"


typedef int (*function) ();
typedef struct {
    int ID;
    // Used by Event Listener
    function VexListener;
    // Used by Event Loop
    function ConnectionLauncher;
    // Used by Interpreter, but launched by Event Loop
    function LuaHandler;
} EzVexConnection;

EzVexConnection * newEzVexConnection(
    function VexListener, function ConnectionLauncher, function LuaHandler);



#endif /* EZ_VEX_CONNECTION */