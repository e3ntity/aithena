#ifndef AITHENA_BENCHMARK_BENCHMARK_H
#define AITHENA_BENCHMARK_BENCHMARK_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace aithena {

class Benchmark {
 public:
  Benchmark();

  // Starts a benchmark. If a benchmark is already running, it will be
  // discarded.
  void Start();
  // Ends a benchmark. Must be preceded by a call to Start().
  void End();

  // Returns whether a benchmark is currently running.
  bool IsRunning();
  // Returns the number of recorded benchmarks.
  int GetSize();
  // Removes all recorded benchmarks
  void Flush();

  long GetLast(int unit = UNIT_MSEC);
  long GetSum(int unit = UNIT_MSEC);
  long GetAvg(int unit = UNIT_MSEC);

  const static int UNIT_SEC = 0x01;
  const static int UNIT_MSEC = 0x02;
  const static int UNIT_USEC = 0x03;
  const static int UNIT_NSEC = 0x04;

 private:
  // Typecasts a timepoint in nanoseconds to some given unit.
  long TypeCast(std::chrono::nanoseconds time, int unit);

  // Stores the time point of the last call to Start().
  std::chrono::high_resolution_clock::time_point start_;
  // Stores all measurements
  std::vector<std::chrono::nanoseconds> measurements_;
  // Indicates that the clock is running
  bool running_{false};
};

class BenchmarkSet {
 public:
  template <typename E>
  using Entry = std::tuple<std::string, E>;

  // Creates a new, empty set of benchmarks.
  BenchmarkSet();

  // Starts the benchmark registered under the given name. If no benchmark can
  // be found, a new one is created and started.
  void Start(std::string name);

  // Ends a running benchmark registered under the given name. If no benchmark
  // can be found, nothing happens.
  void End(std::string name);

  // Returns a benchmark registered under the given name or nullptr if not
  // found.
  std::shared_ptr<Benchmark> Get(std::string name);

  // Returns the last benchmarked time for all registered benchmarks.
  std::vector<Entry<long>> GetLast(int unit = Benchmark::UNIT_MSEC);
  // Returns the summed benchmarked time for all registered benchmarks.
  std::vector<Entry<long>> GetSum(int unit = Benchmark::UNIT_MSEC);
  // Returns the average benchmarked time for all registered benchmarks.
  std::vector<Entry<long>> GetAvg(int unit = Benchmark::UNIT_MSEC);

 private:
  // A list of all registered benchmarks.
  std::vector<Entry<std::shared_ptr<Benchmark>>> entries_;
};

}  // namespace aithena

#endif  // AITHENA_BENCHMARK_BENCHMARK_H