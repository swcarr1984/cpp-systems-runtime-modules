/* ============================================================
// Safe narrow casting
// Prevents accidental truncation when converting between types
   ============================================================ */

#include <type_traits>
#include <stdexcept>

template<typename To, typename From>
To safe_narrow(From value) {
    To converted = static_cast<To>(value);
    if (static_cast<From>(converted) != value) {
        throw std::runtime_error("safe_narrow: value out of range");
    }
    return converted;
}
