//===------------------------------------------------------------*- C++ -*-===//
//
//                                     SHAD
//
//      The Scalable High-performance Algorithms and Data Structure Library
//
//===----------------------------------------------------------------------===//
//
// Copyright 2018 Battelle Memorial Institute
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
//===----------------------------------------------------------------------===//

#ifndef INCLUDE_SHAD_RUNTIME_MAPPING_TRAITS_H_
#define INCLUDE_SHAD_RUNTIME_MAPPING_TRAITS_H_

#include <string>

#include "shad/config/config.h"

namespace shad {

namespace rt {
namespace impl {

template <typename TargetSystemTag>
struct HandleTrait {
  /// @brief The type of Handle
  using HandleTy = typename TargetSystemTag::UndefinedHandlerTypeError;

  /// @brief The type to be used when passing the handle as argument
  using ParameterTy = typename TargetSystemTag::UndefinedHandlerTypeError;

  static constexpr HandleTy NullValue();

  static bool Equal(ParameterTy lhs, ParameterTy rhs);

  static std::string toString(ParameterTy H);

  static HandleTy CreateNewHandle();
  static void WaitFor(ParameterTy H);
};

template <typename TargetSystemTag>
struct LockTrait {
  /// @brief The type of Handle
  using LockTy = typename TargetSystemTag::UndefinedLockTypeError;

  static void lock(LockTy& L);
  static void unlock(LockTy& L);
};

template <typename TargetSystemTag>
struct RuntimeInternalsTrait {
  static void Initialize(int argc, char* argv[]);
  static void Finalize();

  static size_t Concurrency();
  static void Yield();

  static uint32_t ThisLocality();
  static uint32_t NullLocality();
  static uint32_t NumLocalities();
};

}  // namespace impl
}  // namespace rt

}  // namespace shad

#endif  // INCLUDE_SHAD_RUNTIME_MAPPING_TRAITS_H_
