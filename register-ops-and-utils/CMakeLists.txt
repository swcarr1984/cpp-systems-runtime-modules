cmake_minimum_required(VERSION 3.16)
project(systems_utils LANGUAGES CXX)

# Header-only library
add_library(systems_utils INTERFACE)
target_include_directories(systems_utils INTERFACE include)

# Require C++17
target_compile_features(systems_utils INTERFACE cxx_std_17)

# Install support
install(TARGETS systems_utils EXPORT systems_utilsTargets)
install(DIRECTORY include/ DESTINATION include)

# Export package
install(EXPORT systems_utilsTargets
    FILE systems_utilsConfig.cmake
    NAMESPACE systems_utils::
    DESTINATION lib/cmake/systems_utils
)
