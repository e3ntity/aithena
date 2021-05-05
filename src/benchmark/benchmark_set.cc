#include "benchmark/benchmark.h"

#include <string>

namespace aithena {

BenchmarkSet::BenchmarkSet() {}

void BenchmarkSet::Start(std::string name) {
  std::shared_ptr<Benchmark> bm = Get(name);

  if (bm == nullptr) {
    bm = std::make_shared<Benchmark>();
    entries_.push_back(std::make_tuple(name, bm));
  }

  bm->Start();
}

void BenchmarkSet::End(std::string name) {
  std::shared_ptr<Benchmark> bm = Get(name);

  if (bm == nullptr) return;

  bm->End();
}

std::shared_ptr<Benchmark> BenchmarkSet::Get(std::string name) {
  for (auto entry : entries_) {
    if (std::get<0>(entry) != name) continue;

    return std::get<1>(entry);
  }

  return nullptr;
}

std::vector<BenchmarkSet::Entry<long>> BenchmarkSet::GetLast(int unit) {
  std::vector<Entry<long>> timings;

  for (auto entry : entries_) {
    std::string name = std::get<0>(entry);
    long time = std::get<1>(entry)->GetLast(unit);

    timings.push_back(std::make_tuple(name, time));
  }

  return timings;
}

std::vector<BenchmarkSet::Entry<long>> BenchmarkSet::GetSum(int unit) {
  std::vector<Entry<long>> timings;

  for (auto entry : entries_) {
    std::string name = std::get<0>(entry);
    long time = std::get<1>(entry)->GetSum(unit);

    timings.push_back(std::make_tuple(name, time));
  }

  return timings;
}

std::vector<BenchmarkSet::Entry<long>> BenchmarkSet::GetAvg(int unit) {
  std::vector<Entry<long>> timings;

  for (auto entry : entries_) {
    std::string name = std::get<0>(entry);
    long time = std::get<1>(entry)->GetAvg(unit);

    timings.push_back(std::make_tuple(name, time));
  }

  return timings;
}

}  // namespace aithena