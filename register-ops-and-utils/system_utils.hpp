#pragma once
// ============================================================================
// systems_utils.hpp
// A header-only collection of reusable C++ utilities for systems engineering.
// Includes: register ops, bit masks, endian helpers, timing, logging, buffers.
// ============================================================================

#include <cstdint>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

// ---------------------------------------------------------------------------
// Register Bit Operations
// ---------------------------------------------------------------------------
template<typename T>
inline void set_bit(T& reg, uint8_t bit) {
    reg |= (static_cast<T>(1) << bit);
}

template<typename T>
inline void clear_bit(T& reg, uint8_t bit) {
    reg &= ~(static_cast<T>(1) << bit);
}

template<typename T>
inline void toggle_bit(T& reg, uint8_t bit) {
    reg ^= (static_cast<T>(1) << bit);
}

template<typename T>
inline bool check_bit(const T& reg, uint8_t bit) {
    return (reg & (static_cast<T>(1) << bit)) != 0;
}

// ---------------------------------------------------------------------------
// Mask Helpers
// ---------------------------------------------------------------------------
template<typename T>
inline void write_masked(T& reg, T value, T mask) {
    reg = (reg & ~mask) | (value & mask);
}

template<typename T>
inline T read_masked(const T& reg, T mask) {
    return reg & mask;
}

// ---------------------------------------------------------------------------
// Endianness Helpers
// ---------------------------------------------------------------------------
inline uint16_t swap16(uint16_t v) {
    return (v << 8) | (v >> 8);
}

inline uint32_t swap32(uint32_t v) {
    return ((v & 0x000000FF) << 24) |
           ((v & 0x0000FF00) << 8)  |
           ((v & 0x00FF0000) >> 8)  |
           ((v & 0xFF000000) >> 24);
}

inline uint64_t swap64(uint64_t v) {
    return  ((v & 0x00000000000000FFULL) << 56) |
            ((v & 0x000000000000FF00ULL) << 40) |
            ((v & 0x0000000000FF0000ULL) << 24) |
            ((v & 0x00000000FF000000ULL) << 8)  |
            ((v & 0x000000FF00000000ULL) >> 8)  |
            ((v & 0x0000FF0000000000ULL) >> 24) |
            ((v & 0x00FF000000000000ULL) >> 40) |
            ((v & 0xFF00000000000000ULL) >> 56);
}

// ---------------------------------------------------------------------------
// Safe Narrow Cast
// ---------------------------------------------------------------------------
template<typename To, typename From>
To safe_narrow(From value) {
    To converted = static_cast<To>(value);
    if (static_cast<From>(converted) != value) {
        throw std::runtime_error("safe_narrow: value out of range");
    }
    return converted;
}

// ---------------------------------------------------------------------------
// High-Resolution Timing
// ---------------------------------------------------------------------------
inline uint64_t micros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

inline uint64_t millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

template<typename Func>
uint64_t measure(Func&& f) {
    auto start = micros();
    f();
    return micros() - start;
}

// ---------------------------------------------------------------------------
// Lightweight Logging
// ---------------------------------------------------------------------------
inline void log_info(const std::string& msg) {
    std::cout << "[INFO]  " << msg << std::endl;
}

inline void log_warn(const std::string& msg) {
    std::cout << "[WARN]  " << msg << std::endl;
}

inline void log_error(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}

// ---------------------------------------------------------------------------
// Circular Buffer
// ---------------------------------------------------------------------------
template<typename T, size_t N>
class CircularBuffer {
public:
    bool push(const T& item) {
        if (full()) return false;
        buffer_[head_] = item;
        head_ = (head_ + 1) % N;
        count_++;
        return true;
    }

    bool pop(T& out) {
        if (empty()) return false;
        out = buffer_[tail_];
        tail_ = (tail_ + 1) % N;
        count_--;
        return true;
    }

    bool empty() const { return count_ == 0; }
    bool full()  const { return count_ == N; }

private:
    T buffer_[N];
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;
};
