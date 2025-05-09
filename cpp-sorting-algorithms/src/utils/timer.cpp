#include "utils/timer.h"

Timer::Timer() : running(false) {
  // Initialize with default values
}

void Timer::start() {
  // If already running, do nothing
  if (!running) {
    start_time = clock_t::now();
    running = true;
  }
}

void Timer::stop() {
  // If not running, do nothing
  if (running) {
    end_time = clock_t::now();
    running = false;
  }
}

void Timer::reset() { running = false; }

double Timer::getDuration() const {
  // If timer is still running, return time from start to now
  if (running) {
    auto current_time = clock_t::now();
    return std::chrono::duration<double>(current_time - start_time).count();
  }
  // If timer stopped, return time between start and stop
  return std::chrono::duration<double>(end_time - start_time).count();
}

double Timer::elapsed() const { return getDuration(); }

double Timer::getElapsedTime() const { return getDuration(); }

bool Timer::isRunning() const { return running; }