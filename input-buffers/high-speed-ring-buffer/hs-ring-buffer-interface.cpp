/* 
   Created 12/06/26 S.Carr
   Simple High-Speed Ring Buffer for interfacing device data pre supervisory ingestion stage

   High-performance lock-free ring can feed into Chunk Datapipe Frame Component
   Currently optimized for single-producer / single-consumer (SPSC) i.e. One Device stream
   Bitwise masking for fast wrapping.
*/

#include <atomic>
#include <thread>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>
  
/*
   SINGLE-PRODUCER / SINGLE-CONSUMER RING BUFFER
   This ring buffer is lock-free for 1 producer thread and 1 consumer thread.
   It uses two atomic indices (head and tail) and a fixed-size array.
   One slot is intentionally wasted to distinguish full vs empty states.
*/

template<typename T, std::size_t Capacity>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0) {}

    //
    // push(): Called ONLY by the producer thread.
    // Non-blocking. Returns false if the buffer is full.
    //
    bool push(const T& value) {
        // Load current head index (relaxed: producer is the only writer)
        auto head = head_.load(std::memory_order_relaxed);

        // Compute next index in circular buffer
        auto next = increment(head);

        // If next == tail, buffer is full.
        // Acquire ensures we see the latest tail value from consumer.
        if (next == tail_.load(std::memory_order_acquire)) {
            return false; // full
        }

        // Write the value into the buffer slot.
        // Safe because only producer writes to buffer_[head].
        buffer_[head] = value;

        // Publish the new head index.
        // Release ensures the write to buffer_ becomes visible to consumer.
        head_.store(next, std::memory_order_release);
        return true;
    }

    //
    // pop(): Called ONLY by the consumer thread.
    // Non-blocking. Returns false if the buffer is empty.
    //
    bool pop(T& value) {
        // Load current tail index (relaxed: consumer is the only writer)
        auto tail = tail_.load(std::memory_order_relaxed);

        // If tail == head, buffer is empty.
        // Acquire ensures we see the latest head value from producer.
        if (tail == head_.load(std::memory_order_acquire)) {
            return false; // empty
        }

        // Read the value from the buffer.
        // Safe because only consumer reads buffer_[tail].
        value = buffer_[tail];

        // Publish updated tail index.
        // Release ensures consumer's read happens-before producer overwrites slot.
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

private:
    // hAVE allocated Capacity+1 so one slot is always unused.
    // Assists to confirm full / empty
    static constexpr std::size_t capacity_ = Capacity + 1;

    // Circular increment
    std::size_t increment(std::size_t idx) const noexcept {
        return (idx + 1) % capacity_;
    }

    // The actual buffer storage
    T buffer_[capacity_];

    // head_ = next write position (producer)
    // tail_ = next read position (consumer)
    std::atomic<std::size_t> head_;
    std::atomic<std::size_t> tail_;
};

/*
    MAIN PROGRAM
*/

int main() {
    using value_type = uint32_t;

    // 65536 entries (plus 1 wasted slot)
    constexpr std::size_t BufferSize = 1 << 16;

    // Instantiate ring buffer
    RingBuffer<value_type, BufferSize> rb;

    // Flag to signal consumer when producer is done
    std::atomic<bool> done{false};

    /*
           PRODUCER THREAD
    */
    std::thread producer([&]() {
        // Create RNG
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<value_type> dist(0, 1'000'000);

        // Generate 5 million random numbers
        for (std::size_t i = 0; i < 5'000'000; ++i) {
            value_type v = dist(rng);

            // Try to push until successful.
            // This is a lock-free spin loop.
            while (!rb.push(v)) {
                // Yield reduces CPU burn if consumer is slow.
                std::this_thread::yield();
            }
        }

        // Signal completion
        done.store(true, std::memory_order_release);
    });

    /*
          CONSUMER THREAD
    */
    
    std::thread consumer([&]() {
        value_type value;
        std::size_t count = 0;
        uint64_t checksum = 0;

        // Loop until producer is done AND buffer is empty.
        while (!done.load(std::memory_order_acquire) ||
               rb.pop(value)) {

            // Try to pop a value
            if (rb.pop(value)) {
                ++count;
                checksum += value;
            } else {
                // Nothing available right now
                std::this_thread::yield();
            }
        }

        // Print results
        std::cout << "Consumed: " << count << " values\n";
        std::cout << "Checksum: " << checksum << "\n";
    });

    // Wait for both threads
    producer.join();
    consumer.join();

    return 0;
}

