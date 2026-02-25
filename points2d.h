// Abkariyyah Ahmed
/*
Points2D is a template class that represents a dynamically allocated
sequence of 2D points. Each point is stored as a std::array<Object, 2>,
where Object is the template parameter (e.g., int, double).
The class manages its own memory and follows the Rule of Five to ensure
proper deep copying and safe move semantics.
*/

#ifndef CSCI335_HOMEWORK1_POINTS2D_H_
#define CSCI335_HOMEWORK1_POINTS2D_H_

#include <array>
#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>

namespace teaching_project {

template<typename Object>
class Points2D {
  public:
    // This class stores a dynamic sequence of 2D points.
    // Each point is represented as std::array<Object, 2>.
    // Because we manually manage dynamic memory (raw pointer),
    // we implement the Big Five to avoid leaks and double deletes.

    // Zero-parameter constructor:
    // Start with an empty sequence (nullptr) and size 0.
    Points2D() : sequence_{nullptr}, size_{0} {} // empty Points2D: no allocated array yet

    // Copy constructor:
    // Make a deep copy so the new object has its own allocated array.
    Points2D(const Points2D &rhs){
        size_ = rhs.size_;
        sequence_= nullptr;

        // If rhs is empty, we stay empty too.
        if(size_ == 0){
            return;
        }
        // Allocate space for the same number of points as rhs.
        sequence_ = new std::array<Object, 2>[size_];
        // Copy each point over.
        for (size_t i = 0; i < size_; ++i) {
            sequence_[i] = rhs.sequence_[i];
        }
    }

    // Copy assignment:
    // Copy-and-swap approach: make a copy first, then swap.
    // This keeps it safe even if rhs == *this and helps avoid leaks.
    Points2D& operator=(const Points2D &rhs){    
        Points2D copy = rhs;
        std::swap(*this, copy);
        return *this;
    }  

    // Move constructor:
    // "Steal" rhs' array pointer instead of copying.
    // Then reset rhs to a safe empty state.
    Points2D(Points2D &&rhs) noexcept : sequence_{rhs.sequence_}, size_{rhs.size_} { // adopt rhs resources
        rhs.sequence_ = nullptr; // rhs should not own the memory anymore
        rhs.size_ = 0;
    }

    // Move assignment:
    // Swap internal members so ownership transfers efficiently.
    // Marked noexcept because moves should not throw.
    Points2D& operator=(Points2D &&rhs) noexcept{ // move assignment should be fast + safe
        if (this != &rhs) {
            std::swap(size_, rhs.size_);
            std::swap(sequence_, rhs.sequence_);
        }
        return *this;
    }

    // Destructor:
    // Release the dynamic array (if any). delete[] on nullptr is fine.
    ~Points2D() {
        delete[] sequence_;
        sequence_ = nullptr;
    }

    // End of big-five.

    // One-parameter constructor:
    // Build a Points2D with exactly one point in it.
    Points2D(const std::array<Object, 2>& item) { // construct a single-point sequence
        size_ = 1;
        sequence_ = new std::array<Object, 2>[1];  // allocate space for 1 point
        sequence_[0] = item;    
    }

    // Return how many points are stored.
    size_t size() const {
        return size_;
    }

    // Index operator (const):
    // Gives read-only access to a point at a given index.
    // If index is out of bounds, abort as required.
    const std::array<Object, 2>& operator[](size_t location) const {  // bounds-check then return point
        if(location >= size_){  
            std::abort();
        }

        return sequence_[location];

    }

    // Addition operator:
    // Adds two sequences point-by-point where both have elements.
    // If one is longer, copy the remaining points from the longer one.
    friend Points2D operator+(const Points2D &c1, const Points2D &c2) {
        size_t max_length = std::max(c1.size_, c2.size_);
        size_t min_length = std::min(c1.size_, c2.size_);
        Points2D some;
        some.size_ = max_length;
        some.sequence_ = new std::array<Object, 2>[max_length];  // allocate based on larger size

        // Add overlapping portion.
        for (size_t i = 0; i < min_length; i++) {
            some.sequence_[i][0] = c1.sequence_[i][0] + c2.sequence_[i][0]; // add x values
            some.sequence_[i][1] = c1.sequence_[i][1] + c2.sequence_[i][1]; // add y values
        }
        // Copy the leftover points from whichever sequence is longer.
        for (size_t i = min_length; i < max_length; i++) {
            if (c1.size_ > c2.size_) {
                some.sequence_[i] = c1.sequence_[i];
            } else {
                some.sequence_[i] = c2.sequence_[i];
            }
        }
        return some;

    }  

    // Stream output operator:
    // Prints points in the format: (x, y) (x, y) ...
    // For empty sequences, prints ().
    friend std::ostream &operator<<(std::ostream &out, const Points2D &some_points) { // print all points cleanly
        if (some_points.size_ == 0) {
            out << "()" << std::endl;
            return out;
        }

        for (size_t i = 0; i < some_points.size_; i++) {
            out << "(" << some_points.sequence_[i][0] << ", " << some_points.sequence_[i][1] << ") ";
        }

        out << std::endl; // newline at the end (matches the provided tests)
        return out;
    }

    // Stream input operator:
    // Expected format: n x1 y1 x2 y2 ... xn yn
    // If input is invalid/missing, abort as required.
    friend std::istream &operator>>(std::istream &in, Points2D &some_points) {
        size_t num_;
        if (!(in >> num_)) {
            return in;
        }

        // Clear whatever some_points currently holds.
        delete[] some_points.sequence_;
        some_points.sequence_ = nullptr;
        some_points.size_ = 0;

        // If user enters 0 points, we keep it empty and return.
        if (num_ == 0) {
            delete[] some_points.sequence_;
            some_points.sequence_ = nullptr;  
            some_points.size_ = 0;
            return in;
        }

        if(num_ > 0){
            // Use a temporary object so we only assign at the end
            // after everything is successfully read.
            Points2D t;
            t.size_ = num_;
            t.sequence_ = new std::array<Object, 2>[num_];

            for(size_t i = 0; i < num_; i++){
                   if(!(in >> t.sequence_[i][0] >> t.sequence_[i][1])){  
                   std::abort();
                }
            }
            // Move the temporary into some_points (efficient, no extra copy).
            some_points = std::move(t);  // relocate the temp sequence into some_points
            std::cout<<std::endl;
            return in;
        }
        else{
            // Negative sizes aren't expected for size_t, but keeping this for safety.
            std::cerr << "Invalid input: ERROR" << std::endl;
            std::abort();
            some_points.sequence_ = nullptr;
        }
         return in;

    }

  private:
    // Dynamic array of points. This is owned by the object.
    std::array<Object, 2> *sequence_;
    // Number of points in the array.
    size_t size_;
};
}  // namespace teaching_project

#endif // CSCI_335_HOMEWORK1_Points2D_H_
