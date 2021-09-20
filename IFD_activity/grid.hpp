#pragma once

// minimal rectangular grid class
// Hanno 2021


#include <vector>
#include <utility>


template <typename T>
class Grid {
public:
  explicit Grid(int dim) : dim_(dim), buf_(size_t(dim) * dim) {}

  // linear stuff
  size_t size() const noexcept { return buf_.size(); }
  auto begin() noexcept { return buf_.begin(); }
  auto cbegin() const noexcept { return buf_.cbegin(); }
  auto end() noexcept { return buf_.end(); }
  auto cend() const noexcept { return buf_.cend(); }
  auto& operator[](size_t idx) { return buf_[idx]; }
  const auto& operator[](size_t idx) const { return buf_[idx]; }

  // 2D stuff
  int dim() const noexcept { return dim_; }
  auto& operator()(int x, int y) { return buf_[size_t(x) * dim_ + y]; }
  const auto& operator()(int x, int y) const { return buf_[size_t(x) * dim_ + y]; }

  // conversion
  size_t lin_idx(int x, int y) const noexcept { return size_t(x) * dim_ + y; }
  std::pair<int, int> coor(size_t idx) const noexcept { 
    return { static_cast<int>(idx) % dim_, static_cast<int>(idx) / dim_ }; 
  }

private:
  int dim_;
  std::vector<T> buf_;   // linear buffer
};
