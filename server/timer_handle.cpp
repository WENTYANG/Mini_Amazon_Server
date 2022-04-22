#include "timer_handle.h"
#include "server.h"

using Poco::Timer;
using Poco::TimerCallback;

TimerResendWorld::TimerResendWorld(ACommands request): request(request), timer(3000, 3000), callback(*this, &TimerResendWorld::onTimer) {}
TimerResendWorld::~TimerResendWorld() {
  timer.stop();
}

void TimerResendWorld::onTimer(Timer& /*timer*/) {
  Server &s = Server::get_instance();
  s.world_output_queue.push(request);
}

void TimerResendWorld::start() {
  timer.start(callback);
}

TimerResendUps::TimerResendUps(AUCommand request): request(request), timer(3000, 3000), callback(*this, &TimerResendUps::onTimer) {}
TimerResendUps::~TimerResendUps(){
  timer.stop();
}

void TimerResendUps::onTimer(Timer& /*timer*/) {
  Server &s = Server::get_instance();
  s.ups_output_queue.push(request);
}

void TimerResendUps::start() {
  timer.start(callback);
}

