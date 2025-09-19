#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <future>

namespace imp {
    class ThreadPool
    {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
    public:
        explicit ThreadPool(size_t threads= std::thread::hardware_concurrency()-1);
        ~ThreadPool();

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using ReturnType = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<ReturnType> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queueMutex);

                if (stop)
                    throw std::runtime_error("enqueue on stopped ThreadPool");

                tasks.emplace([task]() { (*task)(); });
            }
            condition.notify_one();
            return res;
        }


    };



}