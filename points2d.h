// --> YOUR NAME here
// Few comments describing the class Points2D

#ifndef CSCI335_HOMEWORK1_POINTS2D_H_
#define CSCI335_HOMEWORK1_POINTS2D_H_

#include <array>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <sstream>

namespace teaching_project {

// Points2D: dynamically allocated sequence of 2D points.
// Each point is std::array<Object, 2>. Supports Big Five, stream I/O, and +.
template<typename Object>
class Points2D {
  public:
    // Zero-parameter constructor. Set size to 0, sequence_ to nullptr.
    Points2D() : size_(0), sequence_(nullptr) {}

    // Copy-constructor. Deep copy of rhs.
    Points2D(const Points2D &rhs) : size_(rhs.size_), sequence_(nullptr) {
      if (rhs.size_ > 0) {
        sequence_ = new std::array<Object, 2>[rhs.size_];
        for (size_t i = 0; i < rhs.size_; ++i)
          sequence_[i] = rhs.sequence_[i];
      }
    }

    // Copy-assignment. Copy-and-swap for strong exception safety.
    Points2D& operator=(const Points2D &rhs) {
      Points2D copy = rhs;
      std::swap(size_, copy.size_);
      std::swap(sequence_, copy.sequence_);
      return *this;
    }

    // Move-constructor. Steal resources; leave rhs empty (valid but empty).
    Points2D(Points2D &&rhs) noexcept
        : size_(rhs.size_), sequence_(rhs.sequence_) {
      rhs.size_ = 0;
      rhs.sequence_ = nullptr;
    }

    // Move-assignment. Swap so both objects remain valid.
    Points2D& operator=(Points2D &&rhs) noexcept {
      std::swap(size_, rhs.size_);
      std::swap(sequence_, rhs.sequence_);
      return *this;
    }

    ~Points2D() {
      delete[] sequence_;
      sequence_ = nullptr;
    }

    // One parameter constructor: single point.
    Points2D(const std::array<Object, 2>& item) : size_(1), sequence_(nullptr) {
      sequence_ = new std::array<Object, 2>[1];
      sequence_[0] = item;
    }

    size_t size() const {
      return size_;
    }

    // @location: index in the sequence. Abort if out-of-range.
    const std::array<Object, 2>& operator[](size_t location) const {
      if (location >= size_) std::abort();
      return sequence_[location];
    }

    std::array<Object, 2>& operator[](size_t location) {
      if (location >= size_) std::abort();
      return sequence_[location];
    }

    // Element-wise sum; if sizes differ, append remainder from larger sequence.
    friend Points2D operator+(const Points2D &c1, const Points2D &c2) {
      size_t n = (c1.size_ < c2.size_) ? c2.size_ : c1.size_;
      size_t m = (c1.size_ < c2.size_) ? c1.size_ : c2.size_;
      Points2D result;
      result.size_ = n;
      if (n > 0) {
        result.sequence_ = new std::array<Object, 2>[n];
        for (size_t i = 0; i < m; ++i) {
          result.sequence_[i][0] = c1.sequence_[i][0] + c2.sequence_[i][0];
          result.sequence_[i][1] = c1.sequence_[i][1] + c2.sequence_[i][1];
        }
        if (c1.size_ > c2.size_) {
          for (size_t i = m; i < n; ++i)
            result.sequence_[i] = c1.sequence_[i];
        } else if (c2.size_ > c1.size_) {
          for (size_t i = m; i < n; ++i)
            result.sequence_[i] = c2.sequence_[i];
        }
      }
      return result;
    }

    friend std::ostream &operator<<(std::ostream &out, const Points2D &some_points) {
      if (some_points.size_ == 0) {
        out << "()\n";
        return out;
      }
      for (size_t i = 0; i < some_points.size_; ++i) {
        out << "(" << some_points.sequence_[i][0] << ", "
            << some_points.sequence_[i][1] << ")";
        if (i + 1 < some_points.size_) out << " ";
      }
      out << "\n";
      return out;
    }

    // Format: n x1 y1 x2 y2 ... xn yn. On invalid input: "ERROR" to stderr and abort.
    friend std::istream &operator>>(std::istream &in, Points2D &some_points) {
      size_t n = 0;
      if (!(in >> n)) {
        std::cerr << "ERROR";
        std::abort();
      }
      delete[] some_points.sequence_;
      some_points.sequence_ = nullptr;
      some_points.size_ = 0;
      if (n == 0) return in;
      some_points.sequence_ = new std::array<Object, 2>[n];
      some_points.size_ = n;
      for (size_t i = 0; i < n; ++i) {
        if (!(in >> some_points.sequence_[i][0] >> some_points.sequence_[i][1])) {
          std::cerr << "ERROR";
          std::abort();
        }
      }
      return in;
    }

  private:
    size_t size_;
    std::array<Object, 2> *sequence_;
};

}  // namespace teaching_project
#endif  // CSCI335_HOMEWORK1_POINTS2D_H_
