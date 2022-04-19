#include <stack>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>

class SynchronisedStack
{
public:


    void Push(const T& data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_stack.push(data);
        m_cond.notify_one();

    }

    void TryPop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        while (m_stack.empty())
        {
            m_cond.wait(lock);
        }

        m_stack.pop();

    }
    void TryTop(T& result)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        while (m_stack.empty())
        {
            m_cond.wait(lock);
        }
        result= m_stack.top();

    }

    int Size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_stack.size();

    }

private:

    std::stack<T> m_stack;
    std::mutex m_mutex;
    std::condition_variable m_cond;

};
