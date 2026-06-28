#include <C:/dev/vcpkg/installed/x64-windows/include/boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <string>

using namespace boost::interprocess;

int main() {
    // Open existing queues created by Process A
    message_queue mq_AtoB(open_only, "mq_AtoB");
    message_queue mq_BtoA(open_only, "mq_BtoA");

    std::cout << "[Process B] Started. Waiting for messages from A.\n";

    while (true) {
        // Receive message from A
        char buffer[256];
        size_t received_size;
        unsigned priority;

        mq_AtoB.receive(buffer, sizeof(buffer), received_size, priority);
        buffer[received_size] = '\0';

        std::cout << "[B] Received from A: " << buffer << "\n";

        // Ask user for reply
        std::string reply;
        std::cout << "[B] Enter reply: ";
        std::getline(std::cin, reply);

        // Send reply back to A
        mq_BtoA.send(reply.data(), reply.size(), 0);
    }

    return 0;
}
