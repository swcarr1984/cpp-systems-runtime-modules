/* ============================================================
// High-resolution timing utilities (use for performance checks)
// Provides: now(), micros(), millis(), measure()
   ============================================================ */

#include <chrono>

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
