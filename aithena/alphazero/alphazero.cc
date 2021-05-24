/**
 * Copyright (C) 2020 - All Right Reserved
 */

#include "alphazero/alphazero.h"

#include <float.h>
#include <torch/torch.h>

#include <random>

#include "alphazero/nn.h"
#include "chess/util.h"

namespace aithena {

AlphaZero::AlphaZero(chess::Game::GamePtr game, AlphaZeroNet net,
                     std::shared_ptr<ReplayMemory> replay_memory)
    : game_{game} {
  network_ = net ? net : AlphaZeroNet(game_);
  replay_memory_ =
      replay_memory ? replay_memory : std::make_shared<ReplayMemory>();
}

void AlphaZero::SelfPlay(chess::State::StatePtr start) {
  chess::State::StatePtr state =
      start == nullptr ? game_->GetInitialState() : start;
  AZNode::AZNodePtr root = std::make_shared<AZNode>(game_, state);
  AZNode::AZNodePtr node = root;

  // Run game

  while (!node->IsTerminal() && node->GetStateRepetitions() < 3)
    node = DrawAction(node);

  int result = node->IsTerminal() ? game_->GetStateResult(node->GetState()) : 0;

  // Store samples in replay memory

  bool negate = false;
  while (node != nullptr) {
    replay_memory_->AddSample(node, negate ? result : -result);

    node = node->GetParent();
    negate = !negate;
  }

  TrainNetwork();

  auto output = network_->forward(GetNNInput(root));
  torch::Tensor action_values = std::get<0>(output);
  torch::Tensor state_value = std::get<1>(output);

  std::cout << "State value: " << state_value.item<double>() << std::endl;

  std::cout << "Root dist.: " << std::flush;
  for (auto child : root->GetChildren()) {
    double estimate = GetNNOutput(action_values, child);
    std::cout << std::fixed << std::setprecision(2) << estimate << " "
              << std::flush;
  }
  std::cout << std::endl;
}

void AlphaZero::TrainNetwork() {
  int width = game_->GetOption("board_width");
  int height = game_->GetOption("board_height");

  torch::optim::Adam optimizer(
      network_->parameters(),
      torch::optim::AdamOptions(1e-2).weight_decay(1e-3));

  network_->zero_grad();

  torch::Tensor true_action_values =
      torch::empty({0, GetNNOutputSize(game_), width, height});
  torch::Tensor true_state_values = torch::empty({0});
  torch::Tensor input = torch::empty({0, network_->kInputSize, width, height});

  for (int i = 0; i < batch_size_; ++i) {
    auto sample = replay_memory_->GetSample();

    AZNode::AZNodePtr sample_node = std::get<0>(sample);
    double sample_value = std::get<1>(sample);

    true_action_values =
        torch::cat({true_action_values, GetNNOutput(sample_node)});
    true_state_values =
        torch::cat({true_state_values,
                    torch::from_blob(&sample_value, {1}, torch::kFloat64)});
    input = torch::cat({input, GetNNInput(sample_node)});
  }

  std::tuple<torch::Tensor, torch::Tensor> output = network_->forward(input);

  torch::Tensor predicted_action_values = std::get<0>(output);
  torch::Tensor predicted_state_values = std::get<1>(output);

  torch::Tensor state_value_error =
      torch::square(true_state_values - predicted_state_values);
  torch::Tensor state_value_loss =
      state_value_error.sum() / state_value_error.size({0});

  torch::Tensor action_value_loss =
      torch::dot(true_action_values.reshape({-1}),
                 torch::log(predicted_action_values.reshape({-1}) + 1e-7));

  torch::Tensor loss = state_value_loss - action_value_loss;

  loss.backward();
  optimizer.step();
}

chess::State::StatePtr AlphaZero::DrawAction(chess::State::StatePtr state) {
  AZNode::AZNodePtr node = std::make_shared<AZNode>(game_, state);
  AZNode::AZNodePtr next_node = DrawAction(node);

  return next_node->GetState();
}

AZNode::AZNodePtr AlphaZero::DrawAction(AZNode::AZNodePtr start) {
  benchmark_.Start("DrawAction");

  AZNode::AZNodePtr node = start;

  for (int i = 0; i < simulations_; ++i) Simulate(node);

  int max_visited{0};
  AZNode::AZNodePtr max_child{nullptr};

  for (auto child : node->GetChildren()) {
    if (child->GetVisitCount() < max_visited) continue;

    max_visited = child->GetVisitCount();
    max_child = child;
  }

  benchmark_.End("DrawAction");

  return max_child;
}

void AlphaZero::Simulate(AZNode::AZNodePtr start) {
  benchmark_.Start("Simulate");

  AZNode::AZNodePtr node = start;
  while (node->IsExpanded() && !node->IsTerminal()) {
    node = select_policy_(node);
    assert(node != nullptr);
  }

  torch::Tensor input = GetNNInput(node);
  auto output = network_->forward(input);

  torch::Tensor action_values = std::get<0>(output);
  double state_value = std::get<1>(output)[0].item<double>();

  node->Expand();

  for (auto child : node->GetChildren())
    child->SetPrior(GetNNOutput(action_values, child));

  backpass_(node, state_value);

  benchmark_.End("Simulate");
}

void AlphaZero::SetSimulations(int simulations) { simulations_ = simulations; }

void AlphaZero::SetBatchSize(int batch_size) { batch_size_ = batch_size; }

void AlphaZero::SetSelectPolicy(
    AZNode::AZNodePtr (*select_policy)(AZNode::AZNodePtr)) {
  select_policy_ = select_policy;
}

void AlphaZero::SetBackpass(void (*backpass)(AZNode::AZNodePtr, double)) {
  backpass_ = backpass;
}

double AlphaZero::PUCTValue(AZNode::AZNodePtr child) {
  AZNode::AZNodePtr parent = child->GetParent();

  assert(parent != nullptr);

  double exploitation = child->GetMeanActionValue();
  // TODO(*): use exploration rate C(s) = log((1 + node->GetVisitCount +
  // c_base)/c_base) + c_init instead of const. 0.1
  double exploration = 0.1 * child->GetPrior() * sqrt(parent->GetVisitCount()) /
                       static_cast<double>(child->GetVisitCount() + 1);
  double value = exploitation + exploration;

  return value;
}

AZNode::AZNodePtr AlphaZero::PUCTSelect(AZNode::AZNodePtr node) {
  double max_value = -DBL_MAX;
  AZNode::AZNodePtr max_node = nullptr;

  for (auto child : node->GetChildren()) {
    double value = PUCTValue(child);

    if (value <= max_value) continue;

    max_value = value;
    max_node = child;
  }

  return max_node;
}

void AlphaZero::AlphaZeroBackpass(AZNode::AZNodePtr start, double state_value) {
  AZNode::AZNodePtr node = start;

  for (int i = 0; node != nullptr; ++i, node = node->GetParent())
    node->Update(i % 2 == 0 ? state_value : -state_value);
}

ReplayMemory::ReplayMemory() {
  random_generator_ = std::default_random_engine(std::random_device()());
}

void ReplayMemory::SetSize(int size) { size_ = size; }

int ReplayMemory::GetSampleCount() { return samples_.size(); }

void ReplayMemory::AddSample(AZNode::AZNodePtr node, int result) {
  AddSample(std::make_tuple(node, result));
}

void ReplayMemory::AddSample(std::tuple<AZNode::AZNodePtr, int> sample) {
  while (size_ > 0 && samples_.size() >= static_cast<std::size_t>(size_))
    samples_.erase(samples_.begin() + GetRandomSampleIndex());

  samples_.push_back(sample);
}

int ReplayMemory::GetRandomSampleIndex() {
  auto start = samples_.begin();
  auto end = samples_.end();

  std::uniform_int_distribution<> dist(0, std::distance(start, end) - 1);

  return dist(random_generator_);
}

std::tuple<AZNode::AZNodePtr, int> ReplayMemory::GetSample() {
  auto sample = samples_.begin();

  std::advance(sample, GetRandomSampleIndex());

  return *sample;
}

}  // namespace aithena
