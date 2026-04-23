#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace app {

template <typename T>
class MessageQueue {
public:
    void Push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(item);
        }
        m_cv.notify_one();
    }

    T WaitAndPop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty(); });

        T item = m_queue.front();
        m_queue.pop();
        return item;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<T> m_queue;
};

} // namespace app
