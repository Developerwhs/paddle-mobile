/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifdef FUSION_FC_OP

#pragma once

#include <string>
#include <vector>

#include "framework/operator.h"
#include "framework/program/program-optimize/fusion_op_register.h"
#include "operators/kernel/fusion_fc_kernel.h"

namespace paddle_mobile {
namespace operators {
using std::string;
using std::vector;
class FusionFcMatcher : public framework::FusionOpMatcher {
 public:
  FusionFcMatcher() {
    node_ = framework::Node(G_OP_TYPE_MUL);
    node_ > std::make_shared<framework::Node>(G_OP_TYPE_ELEMENTWISE_ADD);
  }

  void FolderNodes(
      framework::Node *node,
      std::vector<std::shared_ptr<framework::Node>> *removed_nodes) {
    node->Folder(node_.Depth(), Type(),
                 {{G_OP_TYPE_ELEMENTWISE_ADD, {{"Y", "Z"}}}}, removed_nodes);
  }

  std::string Type() { return G_OP_TYPE_FC; }
};

template <typename DeviceType, typename T>
class FusionFcOp
    : public framework::OperatorWithKernel<
          DeviceType, FusionFcParam, operators::FusionFcKernel<DeviceType, T>> {
 public:
  FusionFcOp(const string &type, const VariableNameMap &inputs,
             const VariableNameMap &outputs,
             const framework::AttributeMap &attrs,
             std::shared_ptr<framework::Scope> scope)
      : framework::OperatorWithKernel<DeviceType, FusionFcParam,
                                      operators::FusionFcKernel<DeviceType, T>>(
            type, inputs, outputs, attrs, scope) {}

  using framework::OperatorWithKernel<
      DeviceType, FusionFcParam,
      operators::FusionFcKernel<DeviceType, T>>::OperatorWithKernel;
  void InferShape() const override;

 protected:
};

#ifdef PADDLE_MOBILE_CPU

#ifndef FUSION_FC_CPU_REGISTER
#define FUSION_FC_CPU_REGISTER
static framework::FusionOpRegistrar fc_registrar(new FusionFcMatcher());
#endif

#endif

#ifdef PADDLE_MOBILE_MALI_GPU

#ifndef CONV_CPU_REGISTER
#define CONV_CPU_REGISTER
static framework::FusionOpRegistrar fc_registrar(new FusionFcMatcher());
#endif

#endif

#ifdef PADDLE_MOBILE_FPGA
#ifndef FUSION_FC_CPU_REGISTER
#define FUSION_FC_CPU_REGISTER
static framework::FusionOpRegistrar fc_registrar(new FusionFcMatcher());
#endif
#endif

}  // namespace operators
}  // namespace paddle_mobile

#ifdef PADDLE_MOBILE_CPU
USE_OP_CPU(fusion_fc);
#endif
#ifdef PADDLE_MOBILE_MALI_GPU
USE_OP_MALI_GPU(fusion_fc);
#endif
#ifdef PADDLE_MOBILE_FPGA
USE_OP_FPGA(fusion_fc);
#endif

#endif
