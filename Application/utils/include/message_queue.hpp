#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

#include "logger.hpp"

namespace app {

template <typename T>
class MessageQueue {
public:
    MessageQueue(const uint16_t queue_size) : m_queue_size(queue_size) {
    }

    void Push(const T& item) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stopped) {
                return;
            }
            else if (m_queue.size() >= m_queue_size)
            {
                // LOGW("Queue is full, drop the event %s", ToString(item.event).c_str());
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
    uint16_t m_queue_size{1024};
    bool m_stopped{false};
};

} // namespace app