/* ============================================================
// Register Mask Helpers
// Useful for multi-bit fields inside registers
   ============================================================ */

template<typename T>
inline void write_masked(T& reg, T value, T mask) {
    reg = (reg & ~mask) | (value & mask);
}

template<typename T>
inline T read_masked(const T& reg, T mask) {
    return reg & mask;
}
