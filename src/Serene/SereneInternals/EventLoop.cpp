#include "EventLoop.hpp"
#include "main.h"
#include <iostream>
#include <mutex>

using EventLoop::SereneEvent;
using std::vector;

pros::Mutex mutex;

auto RegisteredEvents = vector<SereneEvent>();
auto RegisteredPeriodicEvent = vector<EventLoop::SerenePeriodicEvent>();
auto FiredEvents = vector<SereneEvent *>();

void EventLoop::register_event(SereneEvent newEvent)
{
    mutex.take(TIMEOUT_MAX);
    RegisteredEvents.push_back(newEvent);
    mutex.give();
}

void EventLoop::register_periodic_event(EventLoop::SerenePeriodicEvent newEvent)
{
    mutex.take(TIMEOUT_MAX);
    RegisteredPeriodicEvent.push_back(newEvent);
    mutex.give();
}

// IDEMPOTENT CANNOT SPAWN ANOTHER
void EventLoop::run_event_listener()
{
    printf("Initialized Event Listenered. Up and Running\n");
    pros::Task{[=]
               {
                   for (;;)
                   {

                       mutex.take(TIMEOUT_MAX);

                       for (SereneEvent event : RegisteredEvents)
                           if (event.Triggered())
                               FiredEvents.push_back(&event);

                       mutex.give();

                       pros::Task::delay(10);
                   }
               }};
}

void EventLoop::run_event_loop()
{
    // Runs in Main Thread
    // pros::Task{[=] {
    printf("Event Loop Initialized. Up and Running! \n");
    for (;;)
    {
        mutex.take(TIMEOUT_MAX);

        for (SereneEvent *event : FiredEvents)
            event->runCallback();

        for (auto event : RegisteredPeriodicEvent)
            event.runCallback();

        FiredEvents.clear();
        mutex.give();

        pros::Task::delay(5);
    }
    //}
    //};
}