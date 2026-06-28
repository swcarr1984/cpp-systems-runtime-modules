#include <C:/dev/vcpkg/installed/x64-windows/include/boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace boost::interprocess;

int main() {
    // Remove old queues if they exist
    message_queue::remove("mq_AtoB");
    message_queue::remove("mq_BtoA");

    // Create two message queues
    message_queue mq_AtoB(create_only, "mq_AtoB", 10, 256);
    message_queue mq_BtoA(create_only, "mq_BtoA", 10, 256);

    std::cout << "[Process A] Started. Type messages to send to Process B.\n";

    while (true) {
        std::string msg;
        std::cout << "[A] Enter message: ";
        std::getline(std::cin, msg);

        // Send message to B
        mq_AtoB.send(msg.data(), msg.size(), 0);

        // Wait for reply
        char buffer[256];
        size_t received_size;
        unsigned priority;

        mq_BtoA.receive(buffer, sizeof(buffer), received_size, priority);
        buffer[received_size] = '\0';

        std::cout << "[A] Received from B: " << buffer << "\n";
    }

    return 0;
}
