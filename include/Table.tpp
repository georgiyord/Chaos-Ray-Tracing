#ifndef RenderEngine_Table_TPP
#define RenderEngine_Table_TPP

#include <cstddef>
#include <memory>
#include <utility>

namespace RenderEngine {
template <typename T> class Table {
protected:
  size_t rows_;
  size_t cols_;
  std::unique_ptr<T[]> arr_;

public:
  Table(const size_t rows, const size_t cols)
      : rows_(rows), cols_(cols), arr_(new T[rows * cols]) {}

  Table(const Table &rhs)
      : rows_(rhs.rows_), cols_(rhs.cols_), arr_(new T[rows_ * cols_]) {
    for (size_t i = 0; i < rows_ * cols_; ++i) {
      arr_[i] = rhs.arr_[i];
    }
  }

  Table &operator=(const Table &rhs) {
    if (this != &rhs) {
      if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
        rows_ = rhs.rows_;
        cols_ = rhs.cols_;
        arr_ = std::make_unique<T[]>(rows_ * cols_);
      }
      for (size_t i = 0; i < rows_ * cols_; ++i) {
        arr_[i] = rhs.arr_[i];
      }
    }
    return *this;
  }

  [[nodiscard]] const T *begin() const noexcept { return arr_; }

  [[nodiscard]] const T *end() const noexcept { return arr_ + rows_ * cols_; }

  [[nodiscard]] T *begin() noexcept { return arr_; }

  [[nodiscard]] T *end() noexcept { return arr_ + rows_ * cols_; }

  [[nodiscard]] T &get(const size_t x, const size_t y) {
    if (x >= cols_ || y >= rows_) {
      throw std::out_of_range("");
    }
    return arr_[(y * cols_) + x];
  }

  [[nodiscard]] const T &get(size_t x, size_t y) const {
    if (x >= cols_ || y >= rows_) {
      throw std::out_of_range("");
    }
    return arr_[(y * cols_) + x];
  }

  [[nodiscard]] size_t getSize() const noexcept { return cols_ * rows_; }
  [[nodiscard]] std::pair<size_t, size_t> getDimensions() const noexcept {
    return {cols_, rows_};
  }
};
} // namespace RenderEngine

#endif // RenderEngine_Table_TPP