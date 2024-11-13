#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t threads) : stop(false) {
    resize(threads);
}

void ThreadPool::workerThread() {
    for (;;) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
            if (this->stop && this->tasks.empty()) return;
            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        task();
    }
}

bool ThreadPool::isEmpty() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return tasks.empty();
}

std::atomic<int> ThreadPool::getUsedTasks() {
    return std::atomic<int>(tasks.size());
}



void ThreadPool::resize(size_t new_size) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    size_t current_size = workers.size();

    if (new_size > current_size) {
        for (size_t i = current_size; i < new_size; ++i) {
            workers.emplace_back(&ThreadPool::workerThread, this);
        }
    } else if (new_size < current_size) {
        stop = true;
        condition.notify_all();
        for (size_t i = new_size; i < current_size; ++i) {
            if (workers[i].joinable()) {
                workers[i].join();
            }
        }
        workers.resize(new_size);
        stop = false;
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}