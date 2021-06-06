/*
 * Copyright https://github.com/gcant/dirichlet-cpp
 */

#pragma once
#include <random>
#include <vector>

template <class RNG>
class dirichlet_distribution {
 public:
  explicit dirichlet_distribution(const std::vector<double>&);
  void set_params(const std::vector<double>&);
  std::vector<double> get_params();
  std::vector<double> operator()(RNG&);

 private:
  std::vector<double> alpha;
  std::vector<std::gamma_distribution<>> gamma;
};

template class dirichlet_distribution<std::mt19937>;
