#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
 private:
  using clock_t = std::chrono::high_resolution_clock;
  using time_point_t = std::chrono::time_point<clock_t>;

  time_point_t start_time;
  time_point_t end_time;
  bool running;

 public:
  // Constructor
  Timer();

  // Control methods
  void start();
  void stop();
  void reset();

  // Different ways to get the elapsed time (to maintain compatibility)
  double getDuration() const;
  double elapsed() const;
  double getElapsedTime() const;

  // Check if timer is running
  bool isRunning() const;
};

#endif  // TIMER_H