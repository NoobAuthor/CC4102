#ifndef TIMER_H
#define TIMER_H

#include <chrono>

/**
 * @brief A simple timer class to measure elapsed time.
 */
class Timer {
 private:
  using clock_t = std::chrono::high_resolution_clock;
  using time_point_t = std::chrono::time_point<clock_t>;

  time_point_t start_time;
  time_point_t end_time;
  bool running;

 public:
  Timer();

  void start();
  void stop();
  void reset();

  double getDuration() const;
  double elapsed() const;
  double getElapsedTime() const;

  bool isRunning() const;
};

#endif