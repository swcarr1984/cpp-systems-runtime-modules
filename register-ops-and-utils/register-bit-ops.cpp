/* ============================================================
// Register bit operation reference
// ops: set_bit, clear_bit, toggle_bit, check_bit
// Works with any unsigned integer type (uint8_t, uint16_t, uint32_t, uint64_t)
   ============================================================ */

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
