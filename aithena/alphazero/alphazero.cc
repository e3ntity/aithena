/**
 * Copyright (C) 2020 - All Right Reserved
 */

#include "alphazero/alphazero.h"

#include <float.h>
#include <time.h>
#include <torch/torch.h>

#include <random>

#include "alphazero/nn.h"
#include "chess/util.h"
#include "util/dirichlet.h"

namespace aithena {

AlphaZero::AlphaZero(chess::Game::GamePtr game, AlphaZeroNet net, std::shared_ptr<ReplayMemory> replay_memory)
    : game_{game} {
  network_ = net ? net : AlphaZeroNet(game_);
  replay_memory_ = replay_memory ? replay_memory : std::make_shared<ReplayMemory>();
  random_generator_ = std::mt19937(std::random_device()());

  if (!net) SetUseCUDA(torch::cuda::cudnn_is_available());

  srand(static_cast<unsigned>(time(NULL)));
}

void AlphaZero::SelfPlay(chess::State::StatePtr start) {
  benchmark_.Start("SelfPlay");

  chess::State::StatePtr state = start == nullptr ? game_->GetInitialState() : start;
  AZNode::AZNodePtr node = std::make_shared<AZNode>(game_, state);

  // Run game
  while (!node->IsTerminal() && node->GetStateRepetitions() < 3) node = DrawAction(node);

  int result = node->IsTerminal() ? game_->GetStateResult(node->GetState()) : 0;

  // Store samples in replay memory

  bool negate = false;
  int i = 0;
  while (node != nullptr) {
    double value = pow(discount_factor_, i) * static_cast<double>(negate ? -result : result);
    replay_memory_->AddSample(GetNNInput(node), GetNNOutput(node), value);

    node = node->GetParent();
    negate = !negate;
    ++i;
  }

  benchmark_.End("SelfPlay");
}

void AlphaZero::TrainNetwork() {
  benchmark_.Start("TrainNetwork");

  int width = game_->GetOption("board_width");
  int height = game_->GetOption("board_height");

  torch::optim::Adam optimizer(network_->parameters(), torch::optim::AdamOptions(1e-4).weight_decay(1e-6));

  torch::Tensor input_batch = torch::empty({0, network_->kInputSize, width, height});
  torch::Tensor true_action_value_batch = torch::empty({0, GetNNOutputSize(game_), width, height});
  torch::Tensor true_state_value_batch = torch::empty({0});

  for (int i = 0; i < batch_size_; ++i) {
    auto sample = replay_memory_->GetSample();

    torch::Tensor input = std::get<0>(sample);
    torch::Tensor action_values = std::get<0>(std::get<1>(sample));
    double state_value = std::get<1>(std::get<1>(sample));

    input_batch = torch::cat({input_batch, input});
    true_action_value_batch = torch::cat({true_action_value_batch, action_values});
    true_state_value_batch = torch::cat({true_state_value_batch, torch::from_blob(&state_value, {1}, torch::kFloat64)});
  }

  bool disable_cuda = disable_cuda_update_ && network_->UsesCUDA();
  if (disable_cuda) network_->to(torch::kCPU);

  std::tuple<torch::Tensor, torch::Tensor> output = network_->forward(input_batch, true);

  if (!disable_cuda && network_->UsesCUDA()) {
    true_action_value_batch = true_action_value_batch.to(torch::kCUDA);
    true_state_value_batch = true_state_value_batch.to(torch::kCUDA);
  }

  torch::Tensor predicted_action_value_batch = torch::clamp(std::get<0>(output), 1e-8, 1.);
  torch::Tensor predicted_state_value_batch = std::get<1>(output);

  torch::Tensor state_value_loss = torch::mean(torch::square(true_state_value_batch - predicted_state_value_batch));
  torch::Tensor action_value_loss = torch::sum(true_action_value_batch * torch::log(predicted_action_value_batch));

  torch::Tensor loss = state_value_loss - action_value_loss;

  optimizer.zero_grad();
  loss.backward();
  optimizer.step();

  if (disable_cuda) network_->to(torch::kCUDA);

  benchmark_.End("TrainNetwork");
}

double AlphaZero::EvaluateState(chess::State::StatePtr state) {
  AZNode::AZNodePtr node = std::make_shared<AZNode>(game_, state);

  torch::Tensor input = GetNNInput(node);
  auto output = network_->forward(input);

  double state_value = std::get<1>(output)[0].item<double>();

  return state_value;
}

double AlphaZero::EvaluateState(chess::State::StatePtr state, chess::Player player) {
  double state_value = EvaluateState(state);

  if (state->GetPlayer() != player) state_value = -state_value;

  return state_value;
}

chess::State::StatePtr AlphaZero::DrawAction(chess::State::StatePtr state) {
  AZNode::AZNodePtr node = std::make_shared<AZNode>(game_, state);
  AZNode::AZNodePtr next_node = DrawAction(node);

  return next_node->GetState();
}

AZNode::AZNodePtr AlphaZero::DrawAction(AZNode::AZNodePtr start) {
  benchmark_.Start("DrawAction");

  // Initialize root and add dirchilet noise
  torch::Tensor input = GetNNInput(start);
  std::tuple<torch::Tensor, torch::Tensor> output = network_->forward(input);

  torch::Tensor action_values = std::get<0>(output);

  start->Expand();
  for (auto child : start->GetChildren()) {
    double action_value = GetNNOutput(action_values, child);
    double noise = dirichlet_noise_(random_generator_)[0];

    child->SetPrior(.75 * action_value + .25 * noise);
  }

  // Remove parent-connection from node that would be problematic during backpropagation.
  AZNode::AZNodePtr parent = start->GetParent();
  start->SetParent(nullptr);

  for (int i = 0; i < simulations_; ++i) Simulate(start);

  start->SetParent(parent);

  int max_visited{0};
  AZNode::AZNodePtr max_child{nullptr};

  for (auto child : start->GetChildren()) {
    if (child->GetVisitCount() < max_visited) continue;

    if (child->GetVisitCount() == max_visited) {
      double random = static_cast<double>(rand()) / RAND_MAX;

      max_child = random > 0.5 ? max_child : child;
    } else {
      max_child = child;
      max_visited = child->GetVisitCount();
    }
  }

  benchmark_.End("DrawAction");

  return max_child;
}

void AlphaZero::Simulate(AZNode::AZNodePtr start) {
  benchmark_.Start("Simulate");

  AZNode::AZNodePtr node = start;

  while (node->IsExpanded() && !node->IsTerminal()) node = select_policy_(node);

  node->Expand();

  torch::Tensor input = GetNNInput(node);
  std::tuple<torch::Tensor, torch::Tensor> output = network_->forward(input);

  torch::Tensor action_values = std::get<0>(output);
  double state_value = std::get<1>(output)[0].item<double>();

  for (auto child : node->GetChildren()) child->SetPrior(GetNNOutput(action_values, child));

  if (node->IsTerminal()) state_value = game_->GetStateResult(node->GetState());

  // If node's player is black, a positive state_value means moving into this node is bad for white. As backpass (first)
  // adds state_value to the node's action value, it must be negated to discourage white from moving into it.
  backpass_(node, -state_value);

  benchmark_.End("Simulate");
}

void AlphaZero::SetSimulations(int simulations) { simulations_ = simulations; }

void AlphaZero::SetBatchSize(int batch_size) { batch_size_ = batch_size; }

void AlphaZero::SetUseCUDA(bool use_cuda) {
  if (use_cuda)
    network_->to(torch::kCUDA);
  else
    network_->to(torch::kCPU);
}

void AlphaZero::SetDiscountFactor(int discount_factor) { discount_factor_ = discount_factor; }

void AlphaZero::SetDirichletNoiseAlpha(double alpha) {
  dirichlet_noise_ = dirichlet_distribution<std::mt19937>({alpha});
}

void AlphaZero::SetSelectPolicy(AZNode::AZNodePtr (*select_policy)(AZNode::AZNodePtr)) {
  select_policy_ = select_policy;
}

void AlphaZero::SetBackpass(void (*backpass)(AZNode::AZNodePtr, double)) { backpass_ = backpass; }

double AlphaZero::PUCTValue(AZNode::AZNodePtr child) {
  AZNode::AZNodePtr parent = child->GetParent();

  assert(parent != nullptr);

  double exploitation = child->GetMeanActionValue();
  // TODO(*): use exploration rate C(s) = log((1 + node->GetVisitCount +
  // c_base)/c_base) + c_init instead of const. 4
  double exploration =
      1.41 * child->GetPrior() * sqrt(parent->GetVisitCount()) / static_cast<double>(child->GetVisitCount() + 1);
  double value = exploitation + exploration;

  return value;
}

AZNode::AZNodePtr AlphaZero::PUCTSelect(AZNode::AZNodePtr node) {
  double max_value = -DBL_MAX;
  AZNode::AZNodePtr max_node = nullptr;

  for (auto child : node->GetChildren()) {
    double value = PUCTValue(child);

    if (value < max_value) continue;

    if (value == max_value) {
      double random = static_cast<double>(rand()) / RAND_MAX;

      max_node = random > 0.5 ? max_node : child;
    } else {
      max_node = child;
      max_value = value;
    }
  }

  return max_node;
}

void AlphaZero::AlphaZeroBackpass(AZNode::AZNodePtr start, double state_value) {
  AZNode::AZNodePtr node = start;

  int i = 0;
  while (node != nullptr) {
    node->Update(i % 2 == 0 ? state_value : -state_value);

    node = node->GetParent();
    ++i;
  }
}

std::shared_ptr<ReplayMemory> AlphaZero::GetReplayMemory() { return replay_memory_; }

AlphaZeroNet AlphaZero::GetNetwork() { return network_; }

ReplayMemory::ReplayMemory(int min_size, int max_size) : min_size_{min_size}, max_size_{max_size} {
  random_generator_ = std::default_random_engine(std::random_device()());
}

int ReplayMemory::GetSampleCount() { return samples_.size(); }

bool ReplayMemory::IsReady() { return GetSampleCount() >= min_size_; }

int ReplayMemory::GetMinSize() { return min_size_; }

int ReplayMemory::GetMaxSize() { return max_size_; }

void ReplayMemory::SetMinSize(int size) { min_size_ = size; }

void ReplayMemory::SetMaxSize(int size) { max_size_ = size; }

void ReplayMemory::AddSample(torch::Tensor input, torch::Tensor action_values, double state_value) {
  AddSample(std::make_tuple(input, std::make_tuple(action_values, state_value)));
}

void ReplayMemory::AddSample(std::tuple<torch::Tensor, std::tuple<torch::Tensor, double>> sample) {
  while (max_size_ > 0 && samples_.size() >= static_cast<std::size_t>(max_size_))
    samples_.erase(samples_.begin() + GetRandomSampleIndex());

  samples_.push_back(sample);
}

int ReplayMemory::GetRandomSampleIndex() {
  auto start = samples_.begin();
  auto end = samples_.end();

  std::uniform_int_distribution<> dist(0, std::distance(start, end) - 1);

  return dist(random_generator_);
}

std::tuple<torch::Tensor, std::tuple<torch::Tensor, double>> ReplayMemory::GetSample() {
  auto sample = samples_.begin();

  std::advance(sample, GetRandomSampleIndex());

  return *sample;
}

}  // namespace aithena
