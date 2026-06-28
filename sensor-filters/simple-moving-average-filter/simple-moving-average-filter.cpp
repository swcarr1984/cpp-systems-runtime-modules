/* Created 12/06/26 S.Carr
   Sensor data pre-processing filter
   Apply within acquisition block prior to transmit as required
   Uses std::mt19937 to generate random numbers, seeded with std::random_device.
   Applies a sliding (simple) moving average filter over the generated values.
   Maintains a deque for the last N values.
   Keeps a running sum for O(1) average calculation.
   Automatically removes the oldest value when the window is full.
*/

#include <iostream>
#include <deque>
#include <random>
#include <numeric>
#include <stdexcept>

// Sliding Moving Average Filter Class
class SlidingMovingAverage {
private:
    std::deque<double> window; // stores the last N values
    size_t maxSize;            // window size
    double sum;                // running sum for efficiency

public:
    // Constructor with validation
    explicit SlidingMovingAverage(size_t size) : maxSize(size), sum(0.0) {
        if (size == 0) {
            throw std::invalid_argument("Window size must be greater than zero.");
        }
    }

    // Add a new value and return the current moving average
    double addValue(double value) {
        window.push_back(value);
        sum += value;

        // Remove oldest value if window is full
        if (window.size() > maxSize) {
            sum -= window.front();
            window.pop_front();
        }

        return sum / window.size();
    }
};

int main() {
    try {
        const size_t windowSize = 5; // moving average window size
        const size_t numSamples = 20; // number of random samples to process

        SlidingMovingAverage sma(windowSize);

        // Random number generator setup
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 100); // random integers [0, 100]

        std::cout << "Sliding Moving Average Filter (" << windowSize << "-point)\n";
        std::cout << "---------------------------------------------\n";
        std::cout << "Value\tMoving Average\n";

        for (size_t i = 0; i < numSamples; ++i) {
            double value = dist(gen); // generate random value
            double avg = sma.addValue(value);
            std::cout << value << "\t" << avg << "\n";
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}



