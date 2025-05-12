#include "utils/timer.h"

Timer::Timer() : running(false) {}

void Timer::start() {
  if (!running) {
    start_time = clock_t::now();
    running = true;
  }
}

void Timer::stop() {
  if (running) {
    end_time = clock_t::now();
    running = false;
  }
}

void Timer::reset() { running = false; }

double Timer::getDuration() const {
  if (running) {
    auto current_time = clock_t::now();
    return std::chrono::duration<double>(current_time - start_time).count();
  }
  return std::chrono::duration<double>(end_time - start_time).count();
}

double Timer::elapsed() const { return getDuration(); }

double Timer::getElapsedTime() const { return getDuration(); }

bool Timer::isRunning() const { return running; }