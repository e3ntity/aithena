#include "benchmark/benchmark.h"

namespace aithena {

using namespace std::chrono;

Benchmark::Benchmark() {}

void Benchmark::Start() {
  running_ = true;
  start_ = high_resolution_clock::now();
}

void Benchmark::End() {
  if (!running_) return;

  auto end = high_resolution_clock::now();
  auto diff = duration_cast<nanoseconds>(end - start_);

  measurements_.push_back(diff);
  running_ = false;
}

bool Benchmark::IsRunning() { return running_; }

int Benchmark::GetSize() { return measurements_.size(); }

void Benchmark::Flush() { measurements_.clear(); }

long Benchmark::GetLast(int unit) {
  if (measurements_.size() < 1) return -1;

  return TypeCast(measurements_.back(), unit);
}

long Benchmark::GetSum(int unit) {
  if (measurements_.size() < 1) return -1;

  nanoseconds sum{0};

  for (auto measurement : measurements_) sum += measurement;

  return TypeCast(sum, unit);
}

long Benchmark::GetAvg(int unit) {
  int count = measurements_.size();

  if (count < 1) return -1;

  nanoseconds sum{0};

  for (auto measurement : measurements_) sum += measurement;

  return TypeCast(sum / count, unit);
}

long Benchmark::TypeCast(nanoseconds time, int unit) {
  switch (unit) {
    case UNIT_SEC:
      return duration_cast<seconds>(time).count();
    case UNIT_MSEC:
      return duration_cast<milliseconds>(time).count();
    case UNIT_USEC:
      return duration_cast<microseconds>(time).count();
    case UNIT_NSEC:
      return duration_cast<nanoseconds>(time).count();
    default:
      return duration_cast<milliseconds>(time).count();
  }
}

}  // namespace aithena