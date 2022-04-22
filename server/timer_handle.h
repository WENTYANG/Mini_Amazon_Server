#ifndef _TIMER_HANDLE_H
#define _TIMER_HANDLE_H

#include "Poco/Timer.h"
#include "./protobuf/AUprotocolV4.pb.h"
#include "./protobuf/world_amazon.pb.h"

using Poco::Timer;
using Poco::TimerCallback;

class TimerResendWorld
{
public:
    TimerResendWorld() : callback(*this, &TimerResendWorld::onTimer){}
    TimerResendWorld(ACommands request);
    ~TimerResendWorld();
    void onTimer(Timer& timer);
    void start();

private:
    ACommands request;
    Timer timer;
    TimerCallback<TimerResendWorld> callback;
};

class TimerResendUps
{
public:
    TimerResendUps() : callback(*this, &TimerResendUps::onTimer){}
    TimerResendUps(AUCommand request);
    ~TimerResendUps();
    void onTimer(Timer& timer);
    void start();

private:
    AUCommand request;
    Timer timer;
    TimerCallback<TimerResendUps> callback;
};

#endif
