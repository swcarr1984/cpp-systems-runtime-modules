/* Created 27/04/26 S.Carr
Raw Input to scaled variable input with below functions:
Range checking on raw 16‑bit input
Signal validation (plausibility + rate‑of‑change)
Scaling to 0–1000 kPa
Diagnostic status output

** To be improved when time, add functionality per IEC61508

*/

#include <cstdint>
#include <cmath>

enum class PressureDiag : uint8_t
{
    OK = 0,
    RAW_OUT_OF_RANGE,
    PRESSURE_OUT_OF_RANGE,
    RATE_OF_CHANGE_EXCEEDED,
    INVALID_INPUT
};

struct PressureResult
{
    float        pressure_kpa;   // Scaled engineering value
    bool         valid;          // True if safe to use
    PressureDiag diag;           // Diagnostic code
};

// Configuration constants (would normally be in a separate, verified config module)
namespace PressureCfg
{
    // Raw ADC range (16-bit)
    constexpr uint16_t RAW_MIN = 0u;
    constexpr uint16_t RAW_MAX = 65535u;

    // Valid electrical range (e.g. 4–20 mA mapped to subset of ADC range)
    // Adjust these to your hardware; here we assume full range is valid.
    constexpr uint16_t RAW_VALID_MIN = RAW_MIN;
    constexpr uint16_t RAW_VALID_MAX = RAW_MAX;

    // Engineering range
    constexpr float ENG_MIN_KPA = 0.0f;
    constexpr float ENG_MAX_KPA = 1000.0f;

    // Precomputed gain: kPa per raw count
    constexpr float SCALE_GAIN =
        (ENG_MAX_KPA - ENG_MIN_KPA) /
        static_cast<float>(RAW_VALID_MAX - RAW_VALID_MIN);

    // Rate-of-change limit (kPa per sample) – tune per application
    constexpr float MAX_DELTA_KPA = 50.0f;
}

// Simple state for validation (keep in a safety-related module)
struct PressureState
{
    bool  initialized;
    float last_pressure_kpa;
};

// Core processing function
PressureResult process_pressure_sample(uint16_t raw, PressureState &state)
{
    PressureResult result{};
    result.pressure_kpa = 0.0f;
    result.valid        = false;
    result.diag         = PressureDiag::INVALID_INPUT;

    // 1. Raw range check (plausibility of ADC value)
    if (raw < PressureCfg::RAW_VALID_MIN || raw > PressureCfg::RAW_VALID_MAX)
    {
        result.diag = PressureDiag::RAW_OUT_OF_RANGE;
        return result; // invalid, do not use
    }

    // 2. Scale to engineering units (0–1000 kPa)
    const float raw_f = static_cast<float>(raw - PressureCfg::RAW_VALID_MIN);
    float pressure_kpa = PressureCfg::ENG_MIN_KPA + raw_f * PressureCfg::SCALE_GAIN;

    // 3. Clamp to engineering range (defensive)
    if (pressure_kpa < PressureCfg::ENG_MIN_KPA || pressure_kpa > PressureCfg::ENG_MAX_KPA)
    {
        result.diag = PressureDiag::PRESSURE_OUT_OF_RANGE;
        return result;
    }

    // 4. Rate-of-change check (simple dynamic plausibility)
    if (state.initialized)
    {
        const float delta = std::fabs(pressure_kpa - state.last_pressure_kpa);
        if (delta > PressureCfg::MAX_DELTA_KPA)
        {
            result.diag = PressureDiag::RATE_OF_CHANGE_EXCEEDED;
            return result;
        }
    }

    // 5. All checks passed → mark valid and update state
    state.initialized      = true;
    state.last_pressure_kpa = pressure_kpa;

    result.pressure_kpa = pressure_kpa;
    result.valid        = true;
    result.diag         = PressureDiag::OK;
    return result;
}