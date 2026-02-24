# Points2D — C++ Template Class

## Overview

`Points2D<Object>` is a template class that represents a dynamically allocated sequence of 2D points. Each point is stored as `std::array<Object, 2>` (e.g. x and y). The class does not use `std::vector` or `std::list`; it manages a raw pointer to a dynamically allocated array and follows the Rule of Five for correct copy/move semantics and memory safety.

**Private data representation (fixed):**
- `size_t size_` — number of points in the sequence
- `std::array<Object, 2> *sequence_` — pointer to a dynamically allocated array of points

*(Note: The member is named `size_` rather than `size` so that the public accessor can be named `size()` without a C++ name conflict.)*

---

## Big Five Implementation

1. **Destructor**  
   `delete[] sequence_` and set `sequence_ = nullptr`. Safe for `sequence_ == nullptr` (no-op). Prevents leaks when the object goes out of scope.

2. **Copy constructor**  
   Allocates a new array of length `rhs.size_`, then copies each point from `rhs.sequence_[i]` into the new array. If `rhs.size_ == 0`, `sequence_` is left `nullptr`. Ensures deep copy and no shared state.

3. **Copy assignment**  
   Implemented via copy-and-swap: construct a temporary copy of `rhs`, then `std::swap(size_, copy.size_)` and `std::swap(sequence_, copy.sequence_)`. The temporary’s destructor frees the old resources. Provides strong exception safety and avoids self-assignment bugs.

4. **Move constructor**  
   Steals `rhs.size_` and `rhs.sequence_`, then sets `rhs.size_ = 0` and `rhs.sequence_ = nullptr`. Marked `noexcept`. The moved-from object is left in a valid, empty state (no double delete).

5. **Move assignment**  
   Uses `std::swap(size_, rhs.size_)` and `std::swap(sequence_, rhs.sequence_)`. Both objects remain valid; ownership is exchanged. Marked `noexcept`. No allocation and no leak.

---

## Dynamic Memory Handling

- **Allocation:**  
  `sequence_` is allocated with `new std::array<Object, 2>[n]` in the one-parameter constructor (single point), copy constructor, `operator>>`, and `operator+`. No allocation when `size_ == 0`; `sequence_` remains `nullptr`.

- **Deallocation:**  
  Only in the destructor and in `operator>>` (before reassigning `sequence_` for a new read). `delete[] sequence_` is safe when `sequence_` is `nullptr`.

- **Safety:**  
  Copy operations do deep copies. Move constructor nulls the source so it is never deleted twice. Move assignment swaps, so each object still owns one block. Copy-and-swap in copy assignment ensures the old resources are released via the temporary’s destructor.

---

## Operator Overloads

- **`size()`**  
  Returns `size_` (number of points).

- **`operator<<`**  
  Writes the sequence to an ostream: for each point, `(x, y)` with spaces between points; empty sequence prints `()`.

- **`operator>>`**  
  Reads format: `n x1 y1 x2 y2 ... xn yn` (count `n` then `n` pairs). Allocates `sequence_` for `n` points and reads into them. On read failure (e.g. invalid or missing data), writes `"ERROR"` to `std::cerr` and calls `std::abort()`.

- **`operator+`**  
  Element-wise sum of two `Points2D` objects. For indices in range of both, adds corresponding coordinates. If sizes differ, the remainder of the larger sequence is appended unchanged. Returns a new `Points2D`; no modification of operands.

- **`operator[]`**  
  Const and non-const versions. Return `sequence_[location]` (reference to `std::array<Object, 2>`). Supports `a[i][0]` and `a[i][1]`. Call `std::abort()` if `location >= size_`.

---

## Challenges Encountered

1. **Move semantics:** Ensuring that moved-from objects remain valid and empty required carefully resetting `size_` and `sequence_` to avoid double deletion.

2. **Dynamic memory management:** All constructors and assignment operators had to correctly allocate and free memory without leaks. Using copy-and-swap simplified the copy assignment logic and improved safety.

3. **Formatting:** Matching the exact input/output format expected by the provided test files required attention to detail.

---

## Assumptions

- Input in `operator>>` is in the form `n` followed by exactly `n` pairs of values of type `Object`. Any stream failure during that read is treated as invalid input (ERROR + abort).
- For `operator[]`, out-of-range indices are handled by aborting; the test does not require exceptions.
- Empty sequence is represented by `size_ == 0` and `sequence_ == nullptr`.
- Default constructor, copy/move constructors, and copy/move assignment are the only ways to create or assign; no other invariants are assumed.

---

## How to Compile and Run

```bash
make clean
make all
./test_points2d < test_input_file.txt
```

Output can be compared to `expected_output.txt`.
