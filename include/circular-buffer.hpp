#pragma once

#include <array>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

template <typename T, std::size_t N> class CircularBuffer {
public:
  explicit CircularBuffer() : head(0), tail(0), count(0) {}

  auto push_back(const T &value) -> std::optional<T> {
    auto out = std::optional<T>();

    if (count < N) {
      out = std::nullopt;
    } else {
      out = std::make_optional<T>(buffer[tail]);

      head = (head + 1) % N;
    }

    buffer[tail] = value;
    tail = (tail + 1) % N;

    if (count < N) {
      ++count;
    }

    return out;
  }

  auto operator[](std::size_t index) -> T & {
    if (index >= count) {
      throw std::out_of_range("Index out of range.");
    }
    return buffer[(head + index) % N];
  }

  auto operator[](std::size_t index) const -> const T & {
    if (index >= count) {
      throw std::out_of_range("Index out of range.");
    }
    return buffer[(head + index) % N];
  }

  auto size() const -> std::size_t { return count; }

  auto get_underlying_array() -> const std::array<T, N> & { return buffer; }

  auto mean() -> double {
    if (count == 0) {
      return 0.0;
    }
    return sum / static_cast<decltype(*buffer.begin() + 0.0)>(count);
  }

  class iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::forward_iterator_tag;

    explicit iterator(CircularBuffer &buf, std::size_t pos)
        : buffer(buf), index(pos) {}

    T &operator*() { return buffer[index]; }
    const T &operator*() const { return buffer[index]; }

    iterator &operator++() {
      index = (index + 1) % N;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator &other) const {
      return &buffer == &other.buffer && index == other.index;
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }

  private:
    CircularBuffer &buffer;
    std::size_t index;
  };

  iterator begin() { return iterator(*this, head); }
  iterator end() { return iterator(*this, (head + count) % N); }

private:
  std::array<T, N> buffer;
  std::size_t head;
  std::size_t tail;
  std::size_t count;
  double sum;
};
