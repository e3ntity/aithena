#include "alphazero/neural_network.h"

#include <iostream>

#include "alphazero/move.h"

namespace aithena {
namespace alphazero {

ResidualBlockImpl::ResidualBlockImpl(unsigned index, unsigned inplanes,
                                     unsigned num_filters, unsigned ker_size)
    : index_{index},
      inplanes_{inplanes},
      num_filters_{num_filters},
      ker_size_{ker_size} {
  reset();
}

void ResidualBlockImpl::reset() {
  conv1 =
      register_module("ResBlock_" + std::to_string(index_) + "_conv1",
                      torch::nn::Conv2d{torch::nn::Conv2dOptions(
                                            inplanes_, num_filters_, ker_size_)
                                            .padding(1)
                                            .padding_mode(torch::kZeros)});
  bn1 = register_module("ResBlock_" + std::to_string(index_) + "_bn1",
                        torch::nn::BatchNorm2d{num_filters_});
  conv2 =
      register_module("ResBlock_" + std::to_string(index_) + "_conv2",
                      torch::nn::Conv2d{torch::nn::Conv2dOptions(
                                            inplanes_, num_filters_, ker_size_)
                                            .padding(1)
                                            .padding_mode(torch::kZeros)});
  bn2 = register_module("ResBlock_" + std::to_string(index_) + "_bn2",
                        torch::nn::BatchNorm2d{num_filters_});
}

torch::Tensor ResidualBlockImpl::forward(torch::Tensor x) {
  torch::Tensor res = x;

  res = conv1(res);
  res = bn1(res);
  res = torch::relu(res);
  res = conv2(res);
  res = bn2(res);
  res = torch::relu(x + res);

  return res;
}

PolicyHeadImpl::PolicyHeadImpl(unsigned board_width, unsigned board_height)
    : board_width_{board_width}, board_height_{board_height} {
  reset();
}

void PolicyHeadImpl::reset() {
  auto conv2options = torch::nn::Conv2dOptions(
                          256, MaxChessMoveValue(board_width_, board_height_) /
                                   board_width_ / board_height_,
                          3)
                          .padding(1)
                          .padding_mode(torch::kZeros);

  conv1 = register_module(
      "policy_conv1", torch::nn::Conv2d{torch::nn::Conv2dOptions(256, 256, 3)
                                            .padding(1)
                                            .padding_mode(torch::kZeros)});
  bn = register_module("policy_bn", torch::nn::BatchNorm2d{256});
  conv2 = register_module("policy_conv2", torch::nn::Conv2d{conv2options});
}

torch::Tensor PolicyHeadImpl::forward(torch::Tensor x) {
  return conv2(torch::relu(bn(conv1(x))));
}

ValueHeadImpl::ValueHeadImpl(unsigned board_width, unsigned board_height)
    : board_width_{board_width}, board_height_{board_height} {
  reset();
}

void ValueHeadImpl::reset() {
  conv = register_module("value_conv", torch::nn::Conv2d(256, 1, 1));
  bn = register_module("value_bn", torch::nn::BatchNorm2d(1));
  hidden =
      register_module("value_hidden_layer",
                      torch::nn::Linear{board_width_ * board_height_, 256});
  out = register_module("value_output", torch::nn::Linear{256, 1});
}

torch::Tensor ValueHeadImpl::forward(torch::Tensor x) {
  torch::Tensor res = x;
  res = conv(res);
  res = bn(res);
  res = torch::relu(res);
  res = torch::flatten(res, 1);
  res = hidden(res);
  res = torch::relu(res);
  res = out(res);
  res = torch::tanh(res);
  return res;
}

AZNeuralNetworkImpl::AZNeuralNetworkImpl(unsigned board_width,
                                         unsigned board_height,
                                         unsigned num_residual_blocks,
                                         torch::Device device, bool uniform)
    : board_width_{board_width},
      board_height_{board_height},
      uniform_{uniform},
      num_residual_blocks_{num_residual_blocks},
      conv(torch::nn::Conv2dOptions(119, 256, 3)
               .padding(1)
               .padding_mode(torch::kZeros)),
      bn(256),
      residual_layer(),
      policy_head(board_width, board_height),
      value_head(board_width, board_height),
      device_{device} {
  unsigned out_size = MaxChessMoveValue(board_width_, board_height_) + 1;

  uniform_out_ =
      torch::ones({1, out_size}).to(device_) / static_cast<int>(out_size - 1);
  uniform_out_.index_put_({0, -1}, 0.5);

  reset();
}

void AZNeuralNetworkImpl::reset() {
  conv = register_module("AZ_conv",
                         torch::nn::Conv2d{torch::nn::Conv2dOptions(119, 256, 3)
                                               .padding(1)
                                               .padding_mode(torch::kZeros)});

  bn = register_module("AZ_bn", torch::nn::BatchNorm2d(256));
  torch::nn::Sequential reslayer{};

  for (unsigned i = 0; i < num_residual_blocks_; i++) {
    reslayer->push_back(ResidualBlock(i, 256, 256, 3));
  }
  residual_layer = register_module("AZ_reslayer", reslayer);

  policy_head =
      register_module("AZ_policy", PolicyHead(board_width_, board_height_));

  value_head =
      register_module("AZ_value", ValueHead(board_width_, board_height_));
}

torch::Tensor AZNeuralNetworkImpl::forward(torch::Tensor x) {
  if (uniform_) {
    return uniform_out_;
  }

  torch::Tensor res_tower = x;
  res_tower = conv(res_tower);
  res_tower = bn(res_tower);
  res_tower = torch::relu(res_tower);
  res_tower = residual_layer->forward(res_tower);
  torch::Tensor policy = policy_head->forward(res_tower);
  torch::Tensor val = value_head->forward(res_tower);

  return torch::cat({torch::flatten(policy, 1), val}, 1);
}

void AZNeuralNetworkImpl::SetUniform(bool uniform) { uniform_ = uniform; }
bool AZNeuralNetworkImpl::GetUniform() { return uniform_; }

}  // namespace alphazero
}  // namespace aithena