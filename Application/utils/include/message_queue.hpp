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
            if (m_stopped) {
                return;
            }
            m_queue.push(item);
        }
        m_cv.notify_one();
    }

    bool WaitAndPop(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_stopped || !m_queue.empty(); });

        if (m_stopped && m_queue.empty()) {
            return false;
        }

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopped = true;
        }
        m_cv.notify_all();
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<T> m_queue;
    bool m_stopped{false};
};

} // namespace app