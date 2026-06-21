/* 
Program that:
User inputted drive - simple test operation.
Converts drive number into a bit position
Uses bitwise OR mask to turn drive “on” in register
Uses bitwise NOT AND mask to turn drive “off” in register
Prints updated register values
Prints a confirmation messages
*/
#include <iostream>
#include <cstdint>

using u32 = std::uint32_t;

int main() {
    u32 register_state = 0u;   // 32-bit register starting at all OFF (0000...)
    int drive = 0;

    std::cout << "Enter drive number to start (1 to 6): ";
    std::cin >> drive;

    // Validate input
    if (drive < 1 || drive > 6) {
        std::cout << "Invalid drive number. Must be 1 to 6.\n";
        return 1;
    }

    // Convert drive number to bit index (drive 1 = bit 0, drive 2 = bit 1, etc.)
    int bit = drive - 1;

    // Create mask and set bit using bitwise 'OR'
    register_state |= (1u << bit);

    // Feedback to user
    std::cout << "Drive " << drive << " started.\n";
    std::cout << "Register state (decimal): " << register_state << "\n";

    // Show register in binary for clarity
    std::cout << "Register state (binary): ";
    for (int i = 7; i >= 0; --i) {  // show 8 bits for readability
        std::cout << ((register_state >> i) & 1u);
    }
    std::cout << "\n";
    std::cout << "Enter drive number to shutdown (1 to 6): ";
    std::cin >> drive;
    // Validate input
    if (drive < 1 || drive > 6) {
        std::cout << "Invalid drive number. Must be 1 to 6.\n";
        return 1;
    }

    // Convert drive number to bit index (drive 1 = bit 0, drive 2 = bit 1, etc.)
    bit = drive - 1;

    // Create mask and set bit using bitwise 'NOT &'
    register_state &= ~(1u << bit);
    // Feedback to user
    std::cout << "Drive " << drive << " stopped.\n";
    std::cout << "Register state (decimal): " << register_state << "\n";

    // Show register in binary for clarity
    std::cout << "Register state (binary): ";
    for (int i = 7; i >= 0; --i) {  // show 8 bits for readability
        std::cout << ((register_state >> i) & 1u);
    }
    return 0;
}
