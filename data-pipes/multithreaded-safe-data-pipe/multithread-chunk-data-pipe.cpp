/* Created 12/06/26 S.Carr
 
Multi-threaded data ingestion pipeline for embedded control.
Framework program simulates 5 (can be increased) independent threads 
(e.g. sensor readers, controllers) that generate data and push it in chunks to a
common thread-safe data pipe. Consumer thread monitors and
processes chunks (simulating a database / endpoint).

Aim to reduce lock contention and improves throughput for data stores.
Thread-safe queue with condition_variable for efficient waiting.
Graceful shutdown with remaining data flush.
Use in test for real-time / embedded systems feeding a central point.

*/

#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <sstream>

// ============================================================================
// 1. DATA STRUCTURES

/*
  Struct for data samples read from sensor loops.
  Adjust for various inputs:
  - axis positions, velocities, temp, pressure, status flags, error codes, etc.
*/
struct DataItem {
    int thread_id; // Which sensor thread being read
    int sequence; // Per-thread sequence number
    double value; // Sensor reading or computed value
    std::chrono::system_clock::time_point timestamp;
};

// A chunk (batch) of data items — sent together to the data store
using Chunk = std::vector<DataItem>;

// ============================================================================
// 2. THREAD-SAFE CHUNK QUEUE (Data Pipe definition)

/*
   Thread-safe queue specialized for moving chunks of data between producers
   and consumer. Uses move semantics for efficiency.
 */
class ThreadSafeChunkQueue {
private:
    std::queue<Chunk> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool finished_ = false; // Signal that no more data will arrive

public:
    
    // Push a completed chunk into the pipe. Uses 'move' to avoid copying large vectors.
    void push(Chunk chunk) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(std::move(chunk));
        }
        cv_.notify_one(); // Wake up consumer
    }

    // Try to pop a chunk. Returns false when queue is empty and finished flag is set.
    bool pop(Chunk& chunk) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { 
            return !queue_.empty() || finished_; 
        });
        
        if (queue_.empty() && finished_) {
            return false;
        }
        
        chunk = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    // Called at shutdown, wakes consumer and lets it exit.
    void finish() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            finished_ = true;
        }
        cv_.notify_all();
    }
};

// ============================================================================
// 3. MAIN FUNCTION

int main() {
    ThreadSafeChunkQueue dataPipe; // Central data pipe
    std::atomic<bool> prog_online{true}; // Global shutdown flag
    const int CHUNK_SIZE = 8; // Update based on your data reqs
    const int NUM_PRODUCERS = 5;

    std::vector<std::thread> producers;

    std::cout << "---Multi-Threaded Data Ingestion Pipeline Started ---\n\n";

    // -------------------------------------------------------------------------
    // 3.1 Producer Threads (5 x independent data sources)
    
    for (int t = 0; t < NUM_PRODUCERS; ++t) {
        // emplace_back constructs a new thread based on 'producers' with use of lambda function
        // Isolates t by assigning to thread_id (safe) and & references other vars directly for scope use
        producers.emplace_back([&, thread_id = t]() {
            int seq = 0;
            std::vector<DataItem> currentChunk;
            currentChunk.reserve(CHUNK_SIZE); // Protect reallocation

            while (prog_online) {
                // Simulate actual sensor / edge data
                DataItem item{
                    thread_id,
                    seq++,
                    10.0 * thread_id + (seq % 100) * 0.1, // Change value each scan for realism
                    std::chrono::system_clock::now()
                };
                // Move data 'item' into currentChunk
                currentChunk.push_back(std::move(item));

                // When chunk reaches desired size, push to common pipe
                if (currentChunk.size() >= CHUNK_SIZE) {
                    dataPipe.push(std::move(currentChunk));
                    currentChunk.clear();
                }

                // Simulate sampling / control loop period
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }

            // Flush any remaining partial chunk on shutdown
            if (!currentChunk.empty()) {
                dataPipe.push(std::move(currentChunk));
            }
        });
    }

    // ------------------------------------------------------------------------
    // 3.2 Consumer Thread (Data Store + Real-time Monitoring)

    std::thread consumer([&]() {
        std::cout << "Consumer started - monitoring data pipe...\n";
        std::cout << std::string(85, '-') << "\n";

        int chunkCount = 0;
        Chunk chunk;

        while (true) {
            if (!dataPipe.pop(chunk)) {
                break; // Shutdown signal received
            }

            chunkCount++;

            // Display chunk header
            auto first_ts = std::chrono::system_clock::to_time_t(chunk[0].timestamp);
            std::cout << "Chunk #" << std::setw(3) << chunkCount 
                      << " | Items: " << std::setw(2) << chunk.size()
                      << " | Time: " << std::ctime(&first_ts); // ctime - add newline

            // Individual diag items displayed for monitoring/debugging
            for (const auto& item : chunk) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    item.timestamp.time_since_epoch()).count() % 1000000;

                std::cout << " T" << item.thread_id 
                          << " seq:" << std::setw(4) << item.sequence
                          << " val:" << std::fixed << std::setprecision(2) << item.value
                          << " ts:" << ms << "ms\n";
            }
            std::cout << std::string(70, '.') << "\n";

            // Simulate writing to database / file / historian
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
        
        std::cout << "\nConsumer finished. Total chunks processed: " << chunkCount << "\n";
    });

    // ------------------------------------------------------------------------
    // Run demonstration for a limited time then shutdown cleanly
    
    std::this_thread::sleep_for(std::chrono::seconds(10));
    prog_online = false;

    std::cout << "\n=== Shutdown signal sent. Waiting for producers to finish ===\n";

    for (auto& t : producers) {
        if (t.joinable()) t.join();
    }

    dataPipe.finish(); // Let consumer exit

    if (consumer.joinable()) {
        consumer.join();
    }

    std::cout << "=== Program completed successfully ===\n";
    return 0;
}
