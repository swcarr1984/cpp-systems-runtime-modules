// ============================================================
// Circular buffer (header-only)
// simple, fast, ideal for average acquisition pipelines
// ============================================================

template<typename T, size_t N>
class CircularBuffer {
public:
    bool push(const T& item) {
        if (full()) return false;
        buffer_[head_] = item;
        head_ = (head_ + 1) % N;
        count_++;
        return true;
    }

    bool pop(T& out) {
        if (empty()) return false;
        out = buffer_[tail_];
        tail_ = (tail_ + 1) % N;
        count_--;
        return true;
    }

    bool empty() const { return count_ == 0; }
    bool full()  const { return count_ == N; }

private:
    T buffer_[N];
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;
};
