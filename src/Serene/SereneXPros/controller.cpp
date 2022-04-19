#include "main.h"
#include "string.h"
#include "controller.hpp"
#include "../SereneInternals/EventLoop.hpp"
#include <vector>
#include <cstdint>
#include <typeinfo>



typedef struct serene_controller {
    pros::Controller controller;
    std::vector<int> registery_keys;
} Controller;

typedef int (*func) (pros::Controller*);

/*

    All this for avoiding writing

    function y;
    variable x;

    return ([] (okapi::Controller *self)->int{return self->y(x);});

*/
#define lambda return ([] (pros::Controller *self)->int{return self->
#define end );});
#define INVALID_TYPE_ERROR luaL_error(L,"Serene/Controller: Invalid Action {%s}",string); return 0;
#define END );
#define ANALOG_TYPE get_analog(pros::controller_analog_e_t::
#define DIGITAL_TYPE get_digital(pros::controller_digital_e_t::
#define lambda_a lambda ANALOG_TYPE
#define lambda_d lambda DIGITAL_TYPE


/*

    get_corresponding_trigger

    used to get the correct lambda function
    that can be used to trigger events in the event loop

*/

func get_corresponding_trigger(lua_State *L, const char *string) {
    switch (string[0])
    {
    case 'R': switch(string[1]) {
            case 'X': lambda_a E_CONTROLLER_ANALOG_RIGHT_X end   
            case 'Y': lambda_a E_CONTROLLER_ANALOG_RIGHT_Y end
            case '1': lambda_d E_CONTROLLER_DIGITAL_R1 end
            case '2': lambda_d E_CONTROLLER_DIGITAL_R2 end
            case '\0': lambda_d E_CONTROLLER_DIGITAL_RIGHT end
            default:   INVALID_TYPE_ERROR
        }
    case 'L':switch(string[1]) {
            case 'X': lambda_a E_CONTROLLER_ANALOG_LEFT_X end
            case 'Y': lambda_a E_CONTROLLER_ANALOG_LEFT_Y end
            case '1': lambda_d E_CONTROLLER_DIGITAL_L1 end
            case '2': lambda_d E_CONTROLLER_DIGITAL_L2 end
            case '\0': lambda_d E_CONTROLLER_DIGITAL_LEFT end
            default: INVALID_TYPE_ERROR
        }
    case 'A': lambda_d E_CONTROLLER_DIGITAL_A end
    case 'B': lambda_d E_CONTROLLER_DIGITAL_B end
    case 'X': lambda_d E_CONTROLLER_DIGITAL_X end
    case 'Y': lambda_d E_CONTROLLER_DIGITAL_Y end
    case 'U': lambda_d E_CONTROLLER_DIGITAL_UP end
    case 'D': lambda_d E_CONTROLLER_DIGITAL_DOWN end
    default: INVALID_TYPE_ERROR
    }
}

bool is_digital(const char *string) {
    switch(string[0]) {
        case 'R':
        case 'L':
            switch(string[1]) {
                case 'X': return false;
                case 'Y': return false;
                default: return true;
            }
        default:
            return true;
    }
}

/*


    bind_action()

    args:
        string, required |OR| table, required
        function, required

    case 1:
        argument 1 = string, required
        binds given function to the following action specified by
        string in the event loop
    
        Passes the value obtained by the action to the call back
        function

    case 2:
        argument 1 = table, required
        binds given function to all actions provided by the table,
        in the event loop

        This is a special case, because for example
        ` bind_action({"E_CONTROLLER_DIGITAL_A","E_CONTROLLER_DIGITAL_B"},function(a,bend) `
        will be called if either E_CONTROLLER_DIGITAL_A or E_CONTROLLER_DIGITAL_B is triggered
        and the value obtained by the action is passed to the
        call back function, if any of the actions are not triggered
        it will pass 0
*/

static int bind_action(lua_State* L) {

    // stack = [self]
    Controller *self = (Controller*) luaL_checkudata(L, 1,SERENE_CONTROLLER_API_METATABLE);
    
    // stack - [self,string|OR|table,function]

    // First check our function
    if (!lua_isfunction(L,3)) {
        luaL_error(L, "Serene/Controller->bind_action: Invalid Argument #2, Expected a Callback function");
        return 0;
    }

    // Great Register it
    int registered_key = luaL_ref(L,LUA_REGISTRYINDEX);

    // Keep Track of it, for deletion, garbage collection, etc
    self->registery_keys.push_back(registered_key);
    
    // Now If The First Argument is String
    if (lua_isstring(L,2)) {

        // get our string
        const char *s = luaL_checkstring(L,2);

        // get matching event trigger
        func EventTrigger = get_corresponding_trigger(L,s);

        // handle errors
        if (!EventTrigger) return 0;

        // is digital?
        bool iz_digital = is_digital(s);

        // create our event
        EventLoop::SereneEvent event(
                // our event trigger
                iz_digital ?
                (std::function<int(int&)>)
                // for digital
                [EventTrigger,self] (int& state) mutable->int {        
                    return EventTrigger(&self->controller);
                } :
                // for analog
                [EventTrigger,self] (int& state) mutable->int {        
                    state = EventTrigger(&self->controller);
                    return state;
                }
                ,
                // our event call back
                iz_digital ?  
               (std::function<void(int,int)>)
                // for digital
                [L] (int reg_key,int state) {
                    // load our function
                    lua_rawgeti(L,LUA_REGISTRYINDEX,reg_key);
                    // call it + error handling
                    if(lua_pcall(L,0,0,0)) {
                        printf("Lua: %s\n",lua_tostring(L,-1));
                    }
                } :
                // for analog
                [L] (int reg_key,int state) {
                    // load our function
                    lua_rawgeti(L,LUA_REGISTRYINDEX,reg_key);
                    // push values
                    lua_pushnumber(L,state);
                    // call it + error handling
                    if(lua_pcall(L,1,0,0)) {
                        printf("Lua: %s\n",lua_tostring(L,-1));
                    }
                },
                // our lua registery key
                registered_key
            );

        // register to event loop
        EventLoop::register_event(event);
    } 
    else if (lua_istable(L,2)) 
    {
        
    } 
    else
        luaL_error(L, "Serene/Controller->bind_action: Invalid Action Type (check argument 1)");

    return 0;
}

/*


    bind_periodic()

    Same as bind_action(), but reads the value no matter even if it's triggered or not.
    
    Passed in variable value will be 0 if it's not triggered.

    args:
        string, required |OR| table, required
        function, required

*/

static int bind_periodic(lua_State* L) {

    // stack = [self]
    Controller *self = (Controller*) luaL_checkudata(L, 1,SERENE_CONTROLLER_API_METATABLE);
    
    // stack - [self,string|OR|table,function]

    // First check our function
    if (!lua_isfunction(L,3)) {
        luaL_error(L, "Serene/Controller->bind_action: Invalid Argument #2, Expected a Callback function");
        return 0;
    }

    // Great Register it
    int registered_key = luaL_ref(L,LUA_REGISTRYINDEX);

    // Keep Track of it, for deletion, garbage collection, etc
    self->registery_keys.push_back(registered_key);

    printf("Registery Key %i",registered_key);

    // Now If The First Argument is String
    if (lua_isstring(L,2)) {

        // get our string
        const char *s = luaL_checkstring(L,2);

        // get matching event trigger
        func EventTrigger = get_corresponding_trigger(L,s);

        // handle errors
        if (!EventTrigger) return 0;

        // create our event
        EventLoop::SerenePeriodicEvent event(
                // our event call back
                is_digital(s)?
                (std::function<void (const int)>)
                // Digital
                [L,EventTrigger,self] (const int reg_key)->void {
                    // load our function
                    lua_rawgeti(L,LUA_REGISTRYINDEX,reg_key);
                    // call it + error handling
                    if(lua_pcall(L,0,0,0)) {
                        printf("Lua: %s\n",lua_tostring(L,-1));
                    }
                } :
                [L,EventTrigger,self] (const int reg_key)->void {
                    // load our function
                    lua_rawgeti(L,LUA_REGISTRYINDEX,reg_key);
                    // push values
                    lua_pushnumber(L,EventTrigger(&self->controller));
                    // call it + error handling
                    if(lua_pcall(L,1,0,0)) {
                        printf("Lua: %s\n",lua_tostring(L,-1));
                    }
                },
                // our lua registery key
                registered_key
            );

        printf("Created Event");

        // register to event loop
        EventLoop::register_periodic_event(event);

        printf("Added Event");
    } 
    else if (lua_istable(L,2)) 
    {
        
    } 
    else
        luaL_error(L, "Serene/Controller->bind_periodic: Invalid Action Type (check argument 1)");

    return 0;
}


/*

    get_action()

    immediately returns the action

    DIGITAL:
        returns 1 if triggered, else 0
    
    ANALOG:
        returns 2
*/

static int get_action(lua_State *L) {

    // stack = [self]
    pros::Controller *self = &((Controller*) luaL_checkudata(L, 1,SERENE_CONTROLLER_API_METATABLE))->controller;
    
    // stack = [self,string]
    if (lua_isstring(L,2)) {    
        const char *s = luaL_checkstring(L, 2);


        if (is_digital(s))
            lua_pushboolean(L, get_corresponding_trigger(L,s)(self));
        else
            lua_pushinteger(L,get_corresponding_trigger(L,s)(self));

        return 1;
    }

    else if(lua_istable(L,1)) {

        lua_len(L,2);
        int size = lua_tonumber(L,3);

        if (size == 0) 
            luaL_error(L,"Serene/Controller->get_action: Attempt to pass in an empty table.. bruh\n");
        
        for (int i = 1; i <= size; i++) {
            lua_rawgeti(L,2,i);
            const char *s = luaL_checkstring(L,-1);
            if (is_digital(s))
                lua_pushboolean(L,get_corresponding_trigger(L,s)(self));
            else
                lua_pushnumber(L,get_corresponding_trigger(L,s)(self));                 
        }

        return size;
    }

    else
        luaL_error(L, "Serene/Controller->get_action: Invalid Action Type (check argument 1)");


    return 1;
}


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
            {"bind_action",bind_action},
            {"bind_periodic",bind_periodic},
            {"get_action",get_action},
            // {"unbind_action",NULL},
            // {"get_action",NULL},
            {NULL,NULL},
        };

        lua_pushstring(L,"__index");
        lua_pushvalue(L,-2);
        lua_settable(L,-3);
        luaL_setfuncs(L,controller_metatable,0);
        lua_settop(L,0);
    }


    // Create Pros Controller
    Controller * new_controller = (Controller*) lua_newuserdata(L,sizeof(Controller));
 
    new (&new_controller->controller) pros::Controller(id);

    // Set metatable
    luaL_getmetatable(L,SERENE_CONTROLLER_API_METATABLE);
    lua_setmetatable(L,-2);

    // DONT CLEAR THE STACK
}