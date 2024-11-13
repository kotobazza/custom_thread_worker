#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <future>



class ThreadPool {
public:
    ThreadPool(size_t threads);
    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::result_of<F()>::type>;
    void resize(size_t new_size);
    ~ThreadPool();
    bool isEmpty();
    std::atomic<int> getUsedTasks();

private:
    void workerThread();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};


template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<typename std::result_of<F()>::type> {
    using return_type = typename std::result_of<F()>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}