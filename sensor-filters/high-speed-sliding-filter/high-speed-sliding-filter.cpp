/* Created 13/06/26 S.Carr
   Very High Speed Sliding average Filter
   ** More to add, revisit and speed test 
*/

#include <random>
#include <cstddef>

template <size_t N>
class FastSlidingAverage {
    static_assert((N & (N - 1)) == 0, "N must be a power of two");

public:
    FastSlidingAverage() : index(0), sum(0.0) {
        for (size_t i = 0; i < N; i++) buffer[i] = 0.0;
    }

    inline double add(double x) {
        sum -= buffer[index];
        buffer[index] = x;
        sum += x;

        index = (index + 1) & (N - 1);   // bitmask wraparound

        return sum * invN;
    }

private:
    double buffer[N];
    size_t index;
    double sum;
    static constexpr double invN = 1.0 / double(N);
};