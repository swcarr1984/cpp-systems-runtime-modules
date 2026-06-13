/* Created 12/06/26 S.Carr 
   Development of High-Speed Real-Time sensor acquisition module
   chrono - used for maintained precision interval. PTP/NTP disruption mitigation.
   Framework piece only - Replace pseudo-gen (mt19937) and distribution() with initialised sensor connection readings   
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <random>

// Interface for selected / potential hardware sensor
class ISensor {
    public:
        virtual ~ISensor() = default;
        virtual bool intialize() = 0;
        virtual float readData() = 0;
};

// Simulated Hardware Sensor (e.g. Pressure, Accel, Temp)
class TemperatureSensor1HA105 : public ISensor {
    private:
        std::mt19937 generator;
        std::uniform_real_distribution<float> distribution;

    
    public:
        // Use random_device{} - non-deterministic seed source, distribution() - class to shape output within range
        TemperatureSensor1HA105() : generator(std::random_device{}()), distribution(20.0f, 25.0f) {}

        // Member function declarations - explicitly 'override' virtual base class with virtual derived class (Compiler check)

        bool intialize() override {
            // Open serial port file descriptor here (like /dev/ttyACM0)
            // or intialise an I2C connection bus here
            return true;
        }

        float readData() override {
            // Simulates the physical hardware conversion time and returns a data
            return distribution(generator);
        }
};

int main() {
    TemperatureSensor1HA105 sensor;
    if (!sensor.intialize()) {
        std::cerr << "F080 - Failed to initialize sensor hardware. Check bus/sensor unit." << std::endl;
        return 1;
    }

    // Set real-time sampling frequency (e.g. 1000Hz = 1 mS Interval)
    const std::chrono::milliseconds sample_interval(1000);

    using clock = std::chrono::steady_clock;
    auto next_sample_time = clock::now();

    std::cout << "Starting real-time sensor data acquisition loop" << std::endl;

    // Continuous real-time execution loop
    while (true) {
        // Scan and return physical sensor I/O data
        float raw_value = sensor.readData();

        // Log sensor raw data prior to transport or storage pre-processing
        std::cout << "[Sensor Data] Telemetry raw reading " << raw_value << " deg C" << std::endl;

        // Calculate next deadline trigger for loop aqcuisition
        next_sample_time += sample_interval;

        // Monitor real-time deadline violation (cycle overrun)
        // Assists in catching scheduling problems, hardware blocking events or thread priority inversions early.
        if (clock::now() > next_sample_time) {
            std::cerr << "[WARNING] Real-Time deadline missed on sensor acquisition. Cycle overun, Latency error. " << std::endl;
            // Cycle Correction, Recovery - catch up next execution cycle to the current system clock time.
            next_sample_time = clock::now(); 
        }

        // Sleep efficiently until the exact target tick arrives (sleep_until accounts for execution overhead not just fixed duration)
        std::this_thread::sleep_until(next_sample_time);
    }

    return 0;
}