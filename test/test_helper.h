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

#pragma once

#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "common/common.h"
#include "common/log.h"
#include "framework/ddim.h"
#include "framework/tensor.h"

static const char *g_ocr = "../models/ocr";
static const char *g_mobilenet_ssd = "../models/mobilenet+ssd";
static const char *g_mobilenet_ssd_gesture = "../models/mobilenet+ssd_gesture";
static const char *g_squeezenet = "../models/squeezenet";
static const char *g_googlenet = "../models/googlenet";
static const char *g_mobilenet = "../models/mobilenet";
static const char *g_resnet_50 = "../models/resnet_50";
static const char *g_resnet = "../models/resnet";
static const char *g_googlenet_combine = "../models/googlenet_combine";
static const char *g_yolo = "../models/yolo";
static const char *g_test_image_1x3x224x224 =
    "../images/test_image_1x3x224x224_float";
static const char *g_test_image_1x3x224x224_banana =
    "../images/input_3x224x224_banana";
static const char *g_hand = "../images/hand_image";

using paddle_mobile::framework::DDim;
using paddle_mobile::framework::Tensor;

template <typename T>
void SetupTensor(paddle_mobile::framework::Tensor *input,
                 paddle_mobile::framework::DDim dims, T lower, T upper) {
  static unsigned int seed = 100;
  std::mt19937 rng(seed++);
  std::uniform_real_distribution<double> uniform_dist(0, 1);

  T *input_ptr = input->mutable_data<T>(dims);
  for (int i = 0; i < input->numel(); ++i) {
    input_ptr[i] = static_cast<T>(uniform_dist(rng) * (upper - lower) + lower);
  }
}

template <typename T>
T *CreateInput(Tensor *input, DDim dims, T low, T up) {
  SetupTensor<T>(input, dims, static_cast<float>(low), static_cast<float>(up));
  return input->data<T>();
}

template <typename T>
void GetInput(const std::string &input_name, std::vector<T> *input,
              const std::vector<int64_t> &dims) {
  int size = 1;
  for (const auto &dim : dims) {
    size *= dim;
  }

  T *input_ptr = reinterpret_cast<T *>(malloc(sizeof(T) * size));
  std::ifstream in(input_name, std::ios::in | std::ios::binary);
  in.read(reinterpret_cast<char *>(input_ptr), size * sizeof(T));
  in.close();
  for (int i = 0; i < size; ++i) {
    input->push_back(input_ptr[i]);
  }
  free(input_ptr);
}

template <typename T>
void GetInput(const std::string &input_name,
              paddle_mobile::framework::Tensor *input,
              paddle_mobile::framework::DDim dims) {
  T *input_ptr = input->mutable_data<T>(dims);

  std::ifstream in(input_name, std::ios::in | std::ios::binary);
  in.read(reinterpret_cast<char *>(input_ptr), input->numel() * sizeof(T));
  in.close();
}
