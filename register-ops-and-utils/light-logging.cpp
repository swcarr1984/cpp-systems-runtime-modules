/* ============================================================
// Lightweight logger
// Provides: log_info, log_warn, log_error
   ============================================================ */

#include <iostream>
#include <string>

inline void log_info(const std::string& msg) {
    std::cout << "[INFO]  " << msg << std::endl;
}

inline void log_warn(const std::string& msg) {
    std::cout << "[WARN]  " << msg << std::endl;
}

inline void log_error(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}
