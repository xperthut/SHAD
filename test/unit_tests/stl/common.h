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

#ifndef TEST_UNIT_TESTS_STL_COMMON_H_
#define TEST_UNIT_TESTS_STL_COMMON_H_

#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "gtest/gtest.h"

#include "shad/data_structures/array.h"
#include "shad/data_structures/set.h"

namespace shad_test_stl {

static constexpr size_t kNumElements = 1024;

// container creation and expected checksum
template <typename T, bool even>
struct create_vector_;

template <typename T, bool even>
struct create_array_;

template <typename T, bool even>
struct create_set_;

template <typename T, bool even>
struct create_map_;

template <typename U, bool even>
struct create_vector_<std::vector<U>, even> {
  using T = std::vector<U>;
  std::shared_ptr<T> operator()(size_t size) {
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < size; ++i) res->push_back(2 * i + !even);
    return res;
  }
};

template <typename U, size_t size, bool even>
struct create_array_<std::array<U, size>, even> {
  using T = std::array<U, size>;
  std::shared_ptr<T> operator()() {
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < size; ++i) res->at(i) = 2 * i + !even;
    return res;
  }
};

template <typename U, size_t size, bool even>
struct create_array_<shad::array<U, size>, even> {
  using T = shad::array<U, size>;
  std::shared_ptr<T> operator()() {
    auto res = T::Create();
    for (size_t i = 0; i < size; ++i) res->at(i) = 2 * i + !even;
    return res;
  }
};

template <typename U, bool even>
struct create_set_<std::unordered_set<U>, even> {
  using T = std::unordered_set<U>;
  std::shared_ptr<T> operator()(size_t size) {
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < size; ++i) res->insert(2 * i + !even);
    return res;
  }
};

template <typename U, bool even>
struct create_set_<shad::Set<U>, even> {
  using T = shad::Set<U>;
  std::shared_ptr<T> operator()(size_t size) {
    auto res = T::Create(size);
    for (size_t i = 0; i < size; ++i) res->Insert(2 * i + !even);
    return res;
  }
};

template <typename U, typename V, bool even>
struct create_map_<std::unordered_map<U, V>, even> {
  using T = std::unordered_map<U, V>;
  std::shared_ptr<T> operator()(size_t size) {
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < size; ++i) (*res)[i] = 2 * i + !even;
    return res;
  }
};

template <typename U, typename V, bool even>
struct create_map_<shad::Hashmap<U, V>, even> {
  using T = shad::Hashmap<U, V>;
  std::shared_ptr<T> operator()(size_t size) {
    auto res = T::Create(size);
    for (size_t i = 0; i < size; ++i) (*res).Insert(i, 2 * i + !even);
    return res;
  }
};

template <bool even>
int64_t expected_checksum_(size_t size) {
  int64_t res = 0;
  for (size_t i = 0; i < size; ++i) res += 2 * i + !even;
  return res;
}

// container destruction
template <typename T>
struct destroy_container_ {
  void operator()(std::shared_ptr<T>) {}
};

template <typename T>
void destroy_shad_contanier_(std::shared_ptr<T> c) {
  T::Destroy(c.get()->GetGlobalID());
}

template <typename U, size_t size>
struct destroy_container_<shad::array<U, size>> {
  using T = shad::array<U, size>;
  void operator()(std::shared_ptr<T> c) { destroy_shad_contanier_(c); }
};

template <typename U, typename V>
struct destroy_container_<shad::Hashmap<U, V>> {
  using T = shad::Hashmap<U, V>;
  void operator()(std::shared_ptr<T> c) { destroy_shad_contanier_(c); }
};

template <typename U>
struct destroy_container_<shad::Set<U>> {
  using T = shad::Set<U>;
  void operator()(std::shared_ptr<T> c) { destroy_shad_contanier_(c); }
};

// sub-sequencing from dynamic-size containers
template <typename T>
typename T::iterator it_seek_(std::shared_ptr<T> in, size_t start_idx) {
  auto first = in->begin();
  size_t i = 0;
  while (i++ < start_idx && first != in->end()) {
    assert(first != in->end());
    ++first;
  }
  return first;
}

template <typename T>
struct subseq_from_;

template <typename U>
struct subseq_from_<std::vector<U>> {
  using T = std::vector<U>;
  std::shared_ptr<T> operator()(std::shared_ptr<T> in, size_t start_idx,
                                size_t len) {
    assert(start_idx < in->size());
    auto first = it_seek_(in, start_idx);
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < len; ++i) {
      assert(first != in->end());
      res->push_back(*first++);
    }
    return res;
  }
};

template <typename U>
struct subseq_from_<std::unordered_set<U>> {
  using T = std::unordered_set<U>;
  std::shared_ptr<T> operator()(std::shared_ptr<T> in, size_t start_idx,
                                size_t len) {
    assert(start_idx < in->size());
    auto first = it_seek_(in, start_idx);
    auto res = std::make_shared<T>();
    for (size_t i = 0; i < len; ++i) {
      assert(first != in->end());
      res->insert(*first++);
    }
    return res;
  }
};

template <typename U>
struct subseq_from_<shad::Set<U>> {
  using T = shad::Set<U>;
  std::shared_ptr<T> operator()(std::shared_ptr<T> in, size_t start_idx,
                                size_t len) {
    assert(start_idx < in->Size());
    auto first = it_seek_(in, start_idx);
    auto res = T::Create(len);
    for (size_t i = 0; i < len; ++i) {
      assert(first != in->end());
      res->Insert(*first);
      ++first;
    }
    return res;
  }
};

template <typename U, typename V>
struct subseq_from_<std::unordered_map<U, V>> {
  using T = std::unordered_map<U, V>;
  std::shared_ptr<T> operator()(std::shared_ptr<T> in, size_t start_idx,
                                size_t len) {
    assert(start_idx < in->size());
    auto first = it_seek_(in, start_idx);
    auto res = std::make_shared<T>();
    std::unordered_map<U, V> x;
    for (size_t i = 0; i < len; ++i) {
      assert(first != in->end());
      res->operator[]((*first).first) = (*first).second;
      ++first;
    }
    return res;
  }
};

template <typename U, typename V>
struct subseq_from_<shad::Hashmap<U, V>> {
  using T = shad::Hashmap<U, V>;
  std::shared_ptr<T> operator()(std::shared_ptr<T> in, size_t start_idx,
                                size_t len) {
    assert(start_idx < (*in).Size());
    auto first = it_seek_(in, start_idx);
    auto res = T::Create(len);
    std::unordered_map<U, V> x;
    for (size_t i = 0; i < len; ++i) {
      assert(first != in->end());
      res->Insert((*first).first, (*first).second);
      ++first;
    }
    return res;
  }
};

// sub-sequencing from static-size containers
template <typename T, size_t size_>
struct static_subseq_from_;

template <typename U, size_t size, size_t size_>
struct static_subseq_from_<std::array<U, size>, size_> {
  using T = std::array<U, size>;
  using T_ = std::array<U, size_>;
  std::shared_ptr<T_> operator()(std::shared_ptr<T> in, size_t start_idx) {
    assert(start_idx < size);
    auto first = it_seek_(in, start_idx);
    auto res = std::make_shared<T_>();
    for (size_t i = 0; i < size_; ++i) {
      assert(first != in->end());
      res->at(i) = *first++;
    }
    return res;
  }
};

template <typename U, size_t size, size_t size_>
struct static_subseq_from_<shad::array<U, size>, size_> {
  using T = shad::array<U, size>;
  using T_ = shad::array<U, size_>;
  std::shared_ptr<T_> operator()(std::shared_ptr<T> in, size_t start_idx) {
    assert(start_idx < size);
    auto first = it_seek_(in, start_idx);
    auto res = T_::Create();
    for (size_t i = 0; i < size_; ++i) {
      assert(first != in->end());
      res->at(i) = *first++;
    }
    return res;
  }
};

// evenness/oddness test
template <typename T>
struct is_even {
  bool operator()(const T &x) { return !(x % 2); }
};

template <typename T, typename U>
struct is_even<std::pair<T, U>> {
  bool operator()(const std::pair<T, U> &x) { return is_even<U>{}(x.second); }
};

template <typename T>
struct is_odd {
  bool operator()(const T &x) { return !(is_even<T>{}(x)); }
};

// accumulate a pair into a numeric value
template <typename acc_t, typename pair_t>
struct pair_acc {
  acc_t operator()(const acc_t &acc, const pair_t &kv) {
    return acc + kv.second;
  }
};

// test fixtures
template <typename T>
class TestFixture : public ::testing::Test {
  void TearDown() { destroy_container_<T>{}(this->in); }

 public:
  template <typename F, typename... args_>
  void test(F &&sub_f, F &&obj_f, args_... args) {
    auto obs = sub_f(in->begin(), in->end(), args...);
    auto exp = obj_f(in->begin(), in->end(), args...);
    ASSERT_EQ(obs, exp);
  }

 protected:
  virtual ~TestFixture() {}
  std::shared_ptr<T> in;
};

template <typename T>
class VectorTestFixture : public TestFixture<T> {
  void SetUp() { this->in = create_vector_<T, true>{}(kNumElements); }

 protected:
  int64_t expected_checksum() { return expected_checksum_<true>(kNumElements); }
};

template <typename T>
class ArrayTestFixture : public TestFixture<T> {
  void SetUp() { this->in = create_array_<T, true>{}(); }

 protected:
  int64_t expected_checksum() { return expected_checksum_<true>(kNumElements); }
};

template <typename T>
class SetTestFixture : public TestFixture<T> {
  void SetUp() { this->in = create_set_<T, true>{}(kNumElements); }

 protected:
  int64_t expected_checksum() { return expected_checksum_<true>(kNumElements); }
};

template <typename T>
class MapTestFixture : public TestFixture<T> {
  void SetUp() { this->in = create_map_<T, true>{}(kNumElements); }

 protected:
  int64_t expected_checksum() { return expected_checksum_<true>(kNumElements); }
};
}  // namespace shad_test_stl

#endif
