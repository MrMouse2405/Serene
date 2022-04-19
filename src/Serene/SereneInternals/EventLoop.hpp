#ifndef SERENE_EVENT_LOOP
#define SERENE_EVENT_LOOP

#include <iostream>
#include <functional>

namespace EventLoop {


    class SereneEvent {
        public:
            typedef const std::function<int(int&)> Ttrigger;
            typedef const std::function<void(const int,const int)> Tcallback;
        private:
            Ttrigger trigger;
            Tcallback callback;
            const int lua_registery_key;
            int state ;
        public:
            SereneEvent( 
                Ttrigger Trigger,
                Tcallback Callback,
                const int Lua_registery_key
            ) : 
                trigger(Trigger), 
                callback(Callback),
                lua_registery_key(Lua_registery_key),
                state(0)
            {}

            bool Triggered() {
                return this->trigger(this->state);
            }
            void runCallback() {
                this->callback(this->lua_registery_key,this->state);
            }
    };

    class SerenePeriodicEvent {
        public:
            typedef const std::function<void(const int)> Tcallback;
        private:
            Tcallback callback;
            const int lua_registery_key;
        public:
            SerenePeriodicEvent (Tcallback Callback,const int Lua_registery_key) 
                :callback(Callback),lua_registery_key(Lua_registery_key){}
            void runCallback() {this->callback(this->lua_registery_key);}
    };

    void register_event(SereneEvent);
    void register_periodic_event(SerenePeriodicEvent);

    void run_event_listener();
    void exit_event_listener();

    void run_event_loop();
    void exit_event_loop();

}
#endif