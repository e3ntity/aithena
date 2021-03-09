#include "alphazero/alphazero.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include "alphazero/move.h"

namespace aithena {
namespace alphazero {

#define PUCT_C_BASE 19652.0
#define PUCT_C_INIT 1.25

AlphaZero::AlphaZero(std::shared_ptr<chess::Game> game, AZNeuralNetwork nn)
    : game_{game}, nn_{nn} {}

bool AlphaZero::Train(chess::Game::GameState start,
                      std::shared_ptr<AlphaZero::ReplayMemory> mem,
                      int batch_size = 100000, int simulations = 800) {
  // Play training game

  AZNode::NodePtr root = std::shared_ptr<AZNode>(
      new AZNode(game_, start, static_cast<AZNode::NodePtr>(nullptr), nn_));

  AZNode::NodePtr node = SelfPlayGame(root, simulations);

  int value = game_->GetStateResult(node->GetState());
  for (int i = 0; node != nullptr; ++i) {
    mem->push_back(std::make_tuple(node, i % 2 == 0 ? value : 1 - value));
    node = node->GetParent();
  }

  // Info: works correctly until here

  if (mem->size() < batch_size) return false;

  // Update neural network

  if (nn_->GetUniform()) nn_->SetUniform(false);

  torch::optim::AdamOptions optim_opts{0.01};
  optim_opts.weight_decay() = 0.001;
  torch::optim::Adam optimizer{nn_->parameters(), optim_opts};

  AlphaZero::ReplayMemory samples;
  std::sample(mem->begin(), mem->end(), std::back_inserter(samples), batch_size,
              std::mt19937{std::random_device{}()});

  torch::Tensor input = std::get<0>(samples.at(0))->GetNNInput();
  torch::Tensor prediction =
      torch::cat({std::get<0>(samples.at(0))->GetChildrenPriors(),
                  std::get<1>(samples.at(0)) * torch::ones({1, 1})},
                 1);

  for (int i = 0; i < batch_size; ++i) {
    torch::cat({input, std::get<0>(samples.at(i))->GetNNInput()});
    torch::cat({prediction,
                torch::cat({std::get<0>(samples.at(i))->GetChildrenPriors(),
                            std::get<1>(samples.at(i)) * torch::ones({1, 1})},
                           1)});
  }

  torch::Tensor output = nn_->forward(input);

  torch::Tensor prediction_prior = prediction.slice(1, 0, -1);
  torch::Tensor prediction_value = prediction.slice(1, -1);
  torch::Tensor output_prior = output.slice(1, 0, -1);
  torch::Tensor output_value = output.slice(1, -1);

  torch::Tensor loss = torch::sqrt(torch::square(output - prediction).sum());

  //-(output_prior * torch::log(prediction_prior)).sum() +
  //                    torch::square(output_value - prediction_value).sum();

  // loss.set_requires_grad(true);

  // TODO: check if this is correct.
  optimizer.zero_grad();
  loss.backward();
  optimizer.step();

  return true;
}

AZNode::NodePtr AlphaZero::SelfPlayGame(AZNode::NodePtr root, int simulations) {
  AZNode::NodePtr node = root;

  while (!node->IsTerminal()) {
    for (int i = 0; i < simulations; ++i) Run(node);
    node = PUCTSelect(node);
  }

  return node;
}

void AlphaZero::Run(AZNode::NodePtr root) {
  auto run_start = std::chrono::high_resolution_clock::now();

  auto leaf = Select(root, PUCTSelect);

  double value = leaf->Expand();

  Backpropagate(leaf, value);

  auto run_end = std::chrono::high_resolution_clock::now();
  time_run_.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(
      run_end - run_start));
}

void AlphaZero::Backpropagate(AZNode::NodePtr node, int value) {
  AZNode::NodePtr current = node;

  for (int i = 0; current != nullptr; ++i) {
    current->IncVisits();
    current->SetValue(current->GetValue() + (i % 2 == 0 ? value : 1 - value));

    current = current->GetParent();
  }
}

AZNode::NodePtr AlphaZero::Select(AZNode::NodePtr node,
                                  AZNode::NodePtr (*next)(AZNode::NodePtr)) {
  auto bm_start = std::chrono::high_resolution_clock::now();

  AZNode::NodePtr current = node;

  while (!current->IsLeaf()) current = next(current);

  auto bm_end = std::chrono::high_resolution_clock::now();
  time_select_.push_back(
      std::chrono::duration_cast<std::chrono::milliseconds>(bm_end - bm_start));

  return current;
}

AZNode::NodePtr AlphaZero::PUCTSelect(AZNode::NodePtr node) {
  if (node->IsLeaf()) return node;

  assert(node->GetVisits() > 0);

  int index{0};
  double maximum{0};
  double sum{0};

  auto children = node->GetChildren();

  int i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    double exploration_c =
        log(double(1 + node->GetVisits() + PUCT_C_BASE) / PUCT_C_BASE) +
        PUCT_C_INIT;
    double exploration = exploration_c * double(child->GetPrior()) *
                         sqrt(double(node->GetVisits())) /
                         double(1 + child->GetVisits());
    double exploitation = child->GetMeanValue();
    double value = exploration * exploitation;

    sum += value;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

double AlphaZero::BenchmarkSelect() { return Benchmark(time_select_); }
double AlphaZero::BenchmarkBackprop() { return Benchmark(time_backprop_); }
double AlphaZero::BenchmarkRun() { return Benchmark(time_run_); }

double AlphaZero::Benchmark(
    std::vector<std::chrono::milliseconds> time_vector) {
  std::chrono::milliseconds sum{0};

  for (auto point : time_vector) sum += point;

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      sum / time_vector.size());

  return static_cast<double>(duration.count()) / 1000.;
}

void AlphaZero::Save(std::string path) { torch::save(nn_, path); }
void AlphaZero::Load(std::string path) { torch::load(nn_, path); }

}  // alphazero
}  // aithena
