# C++ Systems Engineering Utilities

A header‑only collection of reusable C++ modules for systems‑engineering work.  
Designed for device‑level software, acquisition pipelines, IPC frameworks, and  
low‑latency runtime components.

This library provides:

- Register bit operations  
- Masked field helpers  
- Endian utilities  
- Safe narrowing casts  
- High‑resolution timing tools  
- Lightweight logging  
- Circular buffer template for acquisition pipelines  


## Usage

### CMake

```cmake
find_package(systems_utils REQUIRED)
target_link_libraries(my_app PRIVATE systems_utils::systems_utils)

Example usage:

#include <systems_utils.hpp>

int main() {
    uint32_t reg = 0;
    set_bit(reg, 3);
    log_info("Bit 3 set");

    CircularBuffer<int, 8> buf;
    buf.push(42);

    return 0;
}

Features
Register Operations
Set, clear, toggle, and check individual bits.

Mask Helpers
Write and read multi‑bit fields safely.

Endianness Tools
swap16 / swap32 / swap64 for cross‑platform data handling.

Timing Utilities
Microsecond‑resolution timers for profiling and acquisition loops.

Logging
Zero‑dependency logging for embedded or runtime systems.

Circular Buffer
Fast, fixed‑size FIFO for acquisition or streaming

MIT License

---

# **4. General Folder Structure**

cpp-systems-runtime-modules/
│
├── include/
│   └── systems_utils.hpp
│
├── CMakeLists.txt
└── README.md
