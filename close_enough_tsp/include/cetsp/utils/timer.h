//
// Created by Dominik Krupke on 10.01.23.
//

#ifndef CETSP_TIMER_H
#define CETSP_TIMER_H
#include <chrono>
namespace cetsp::utils {
class Timer {

public:
  explicit Timer(double time_limit)
      : time_limit{time_limit},
        start{std::chrono::high_resolution_clock::now()} {}
  using TimePoint = decltype(std::chrono::high_resolution_clock::now());

  [[nodiscard]] double seconds() const {
    using namespace std::chrono;
    const auto now = high_resolution_clock::now();
    const auto time_used =
        static_cast<double>(duration_cast<milliseconds>(now - start).count()) /
        1000.0;
    return time_used;
  }

  [[nodiscard]] bool timeout() const { return seconds() >= time_limit; }

  double time_limit;
  TimePoint start;
};
} // namespace cetsp::utils
#endif // CETSP_TIMER_H
