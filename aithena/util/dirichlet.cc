/*
 * Copyright https://github.com/gcant/dirichlet-cpp
 */

#include "util/dirichlet.h"

#include <random>
#include <vector>

template <class RNG>
dirichlet_distribution<RNG>::dirichlet_distribution(const std::vector<double>& alpha) {
  set_params(alpha);
}

template <class RNG>
void dirichlet_distribution<RNG>::set_params(const std::vector<double>& new_params) {
  alpha = new_params;
  std::vector<std::gamma_distribution<>> new_gamma(alpha.size());
  for (int i = 0; i < alpha.size(); ++i) {
    std::gamma_distribution<> temp(alpha[i], 1);
    new_gamma[i] = temp;
  }
  gamma = new_gamma;
}

template <class RNG>
std::vector<double> dirichlet_distribution<RNG>::get_params() {
  return alpha;
}

template <class RNG>
std::vector<double> dirichlet_distribution<RNG>::operator()(RNG& generator) {
  std::vector<double> x(alpha.size());
  double sum = 0.0;
  for (int i = 0; i < alpha.size(); ++i) {
    x[i] = gamma[i](generator);
    sum += x[i];
  }
  for (double& xi : x) xi = xi / sum;
  return x;
}