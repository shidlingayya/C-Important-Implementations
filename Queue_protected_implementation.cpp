#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <system_error>
template<typename T>
class Queue {
public:
    // send through the queue
    void send(T const& t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(t);
        m_condvar.notify_one();
    }

    // receive from the queue
    // If the queue is empty, wait till a element is available.
    T receive() {
        std::unique_lock<std::mutex> lock(m_mutex); // unlocked by wait below.
        while (m_queue.empty()) {
            // releases lock as long as the wait and re-aquire it afterwards.
            m_condvar.wait(lock); // this might wake up spuriously, requiring it to be in the loop
        }

        T rcvd = m_queue.front();
        m_queue.pop();
        return rcvd;
    }

    bool empty() const { return m_queue.empty(); }

private:
    std::queue<T>           m_queue{};
    mutable std::mutex      m_mutex{};
    std::condition_variable m_condvar{};
};

int main()
{
    std::cout << "Inside main function" << std::endl;
    Queue<int> queue;
    queue.send(10);
    queue.send(20);
    queue.send(30);
    
    std::cout << "Pop queue: " << queue.receive() << std::endl;
    std::cout << "Pop queue: " << queue.receive() << std::endl;
    std::cout << "Pop queue: " << queue.receive() << std::endl;
    return 0;
}
