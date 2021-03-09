#ifndef ALPHAZERO_NEURAL_NETWORK_H
#define ALPHAZERO_NEURAL_NETWORK_H

#include <torch/torch.h>

#include <memory>
#include <string>

namespace aithena {
namespace alphazero {

class ResidualBlockImpl : public torch::nn::Cloneable<ResidualBlockImpl> {
 public:
  ResidualBlockImpl() = default;
  ResidualBlockImpl(unsigned index, unsigned inplanes, unsigned num_filters,
                    unsigned ker_size);
  void reset();
  torch::Tensor forward(torch::Tensor x);

 private:
  torch::nn::Conv2d conv1 = nullptr;
  torch::nn::BatchNorm2d bn1 = nullptr;
  torch::nn::Conv2d conv2 = nullptr;
  torch::nn::BatchNorm2d bn2 = nullptr;
  int64_t index_;
  int64_t inplanes_;
  int64_t num_filters_;
  int64_t ker_size_;
};

TORCH_MODULE(ResidualBlock);

class PolicyHeadImpl : public torch::nn::Cloneable<PolicyHeadImpl> {
 public:
  PolicyHeadImpl() = default;
  PolicyHeadImpl(unsigned board_width, unsigned board_height);
  void reset();
  torch::Tensor forward(torch::Tensor x);

 private:
  torch::nn::Conv2d conv1 = nullptr;
  torch::nn::BatchNorm2d bn = nullptr;
  torch::nn::Conv2d conv2 = nullptr;
  unsigned board_width_;
  unsigned board_height_;
};

TORCH_MODULE(PolicyHead);

class ValueHeadImpl : public torch::nn::Cloneable<ValueHeadImpl> {
 public:
  ValueHeadImpl() = default;
  explicit ValueHeadImpl(unsigned board_width, unsigned board_height);
  void reset();
  torch::Tensor forward(torch::Tensor x);

 private:
  torch::nn::Conv2d conv = nullptr;
  torch::nn::BatchNorm2d bn = nullptr;
  torch::nn::Linear hidden = nullptr;
  torch::nn::Linear out = nullptr;
  unsigned board_width_;
  unsigned board_height_;
};

TORCH_MODULE(ValueHead);

class AZNeuralNetworkImpl : public torch::nn::Cloneable<AZNeuralNetworkImpl> {
 public:
  AZNeuralNetworkImpl() = default;
  AZNeuralNetworkImpl(unsigned board_width, unsigned board_height,
                      unsigned num_residual_blocks, torch::Device device,
                      bool uniform = false);
  void reset();
  void SetUniform(bool uniform);
  torch::Tensor forward(torch::Tensor);

 private:
  torch::nn::Conv2d conv = nullptr;
  torch::nn::BatchNorm2d bn = nullptr;
  torch::nn::Sequential residual_layer = nullptr;
  PolicyHead policy_head = nullptr;
  ValueHead value_head = nullptr;
  unsigned board_width_;
  unsigned board_height_;
  unsigned num_residual_blocks_;
  bool uniform_;
  torch::Tensor uniform_out_;
  torch::Device device_ = torch::Device{torch::kCPU};
};

TORCH_MODULE(AZNeuralNetwork);

}  // namespace alphazero
}  // namespace aithena

#endif  // ALPHAZERO_NEURAL_NETWORK_H