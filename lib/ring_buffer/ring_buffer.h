/**
 * @file ring_buffer.h
 * @brief Simple ring-buffer implementation
 */

#pragma once
#include "main.h"
#include <cstdint>
#include <array>

/**
 * @brief Non-overwriting ring buffer implementation
 *
 * @tparam T type in buffer
 * @tparam N number of elements in buffer
 */
template <class T = uint8_t, uint16_t N = 64, bool ADD_VOLATILE = false>
class RingBuffer {
private:
  using index_t = typename std::conditional<ADD_VOLATILE, volatile uint16_t, uint16_t>::type;

public:
  using data_t = T;
  std::array<T, N> buff_;
  index_t head_{}, tail_{};
  bool is_full_ = false;

  /// Place and element into the buffer
  uint16_t push(const data_t& d) {
    if (is_full()) {
      return 0;
    }
    buff_[head_] = d;
    head_ = (head_ + 1) % N;
    is_full_ = is_full_ || (head_ == tail_);

    return 1;
  }

  /// Place @p n elements into the buffer
  /// @return Number of elements written
  uint16_t push(const T* data, uint16_t n) {
    int cnt = 0;
    for (; cnt < n && not is_full(); ++cnt) {
      buff_[head_] = data[cnt];
      head_ = (head_ + 1) % N;
      is_full_ = is_full_ || (head_ == tail_);
    }
    return cnt;
  }

  /// Take one element from the buffer
  [[nodiscard]] data_t pop() {
    assert_param(!is_empty());

    data_t ret = buff_[tail_];
    tail_ = (tail_ + 1) % N;
    is_full_ = false;
    return ret;
  }

  /// Move tail_ pointer by @p n. Essentially deletes the elements
  void pop(size_t n) {
    assert_param(!is_empty());
    tail_ = (tail_ + n) % N;
    is_full_ = false;
  }

  /// First element in the buffer
  [[nodiscard]] const data_t& peek() const {
    assert_param(!is_empty());
    return buff_[tail_];
  }

  [[nodiscard]] bool is_full() const {
    return is_full_;
  }
  [[nodiscard]] bool is_empty() const {
    return ((not is_full_) && (head_ == tail_));
  }

  [[nodiscard]] uint16_t get_num_occupied() const {
    if (is_full()) {
      return N;
    }

    if (head_ >= tail_) {
      return (head_ - tail_);
    } else {
      return (N + head_ - tail_);
    }
  }

  /// size of continuously occupied space after tail_
  [[nodiscard]] uint16_t get_num_occupied_continuous() const {
    if (head_ < tail_) {
      return N - tail_;
    } else {
      return get_num_occupied();
    }
  }

  [[nodiscard]] uint16_t get_num_free() const {
    return N - get_num_occupied();
  }

  /// size of continuous free space after head_
  [[nodiscard]] uint16_t get_num_free_continuous() const {
    if (head_ < tail_) {
      return tail_ - head_;
    } else {
      return N - head_;
    }
  }

  /// @brief Moves head by @p n, if @p n number of continuous space is available
  /// @returns pointer to beginning of the reserved space
  [[nodiscard]] data_t* reserve(uint16_t n) {
    if (get_num_free_continuous() < n) {
      return nullptr;
    }

    data_t* ret = &(buff_[head_]);
    head_ = (head_ + n) % N;

    return ret;
  }

  [[nodiscard]] uint16_t size() const {
    return N;
  }

  void reset() {
    is_full_ = false;
    head_ = 0;
    tail_ = 0;
  }


  /// @name iterating
  /// @{
  /// @brief first occupied index in buffer
  [[nodiscard]] size_t begin() const {
    return tail_;
  }
  /// @brief once past last occupied
  [[nodiscard]] size_t end() const {
    return head_;
  }
  /// @brief move the index @p idx by one
  [[nodiscard]] size_t increment_idx(size_t idx) const {
    ++idx;
    idx = idx % buff_.size();
    return idx;
  }
  /// @brief decrement index @p idx by one
  [[nodiscard]] size_t decrement_idx(size_t idx) const {
    if (idx == 0) {
      idx = buff_.size() - 1;
    } else {
      --idx;
    }
    return idx;
  }

  [[nodiscard]] size_t rbegin() const {
    return decrement_idx(head_);
  }

  [[nodiscard]] size_t rend() const {
    return decrement_idx(tail_);
  }
  /// @}

  /// @brief move head position by N
  /// Used if data was written externally, e.g. by the DMA
  void advance_head(size_t n) {
    for (unsigned i = 0; i < n; ++i) {
      // data is written by DMA, so if full, data is overwritten, which shouldn't happen
      assert_param(not is_full());
      ++head_;
      head_ = head_ % buff_.size();
    }
  }
};
