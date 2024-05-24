#pragma once
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cstdio>

namespace sp {

    template<typename Job>
    struct ThreadPool
    {
        std::vector<std::thread> _workers;
        std::queue<Job> _jobs;
        std::mutex _mutex;
        std::condition_variable _cv;
        bool _stop = false;
        std::function<void(void **dataptr)> _onInit = nullptr;
        std::function<void(void *dataptr)> _onDestroy = nullptr;

        void create(
                std::function<void(Job &, void*)> workerFunction,
                uint32_t threadCount = std::thread::hardware_concurrency())
        {
            _workers.clear();
            _workers.reserve(threadCount);
            _stop = false;
            for(uint32_t i = 0; i < threadCount; ++i)
            {
                _workers.emplace_back([this, workerFunction]{
                    
                    void * dataptr = nullptr;
                    
                    if(_onInit) {
                        _onInit(&dataptr);
                    }
                    while (true) {
                        Job job;
                        {
                            std::unique_lock<std::mutex> lock(_mutex);
                            _cv.wait(lock, [this]{ return !_jobs.empty() || _stop; });

                            if (_stop && _jobs.empty())
                                return;

                            job = std::move(_jobs.front());
                            _jobs.pop();
                        }

                        workerFunction(job, dataptr);
                    }

                    if(_onDestroy)
                        _onDestroy(dataptr);
                });
            }
        }

        void destroy()
        {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _stop = true;
            }
            _cv.notify_all();
            for(auto & worker : _workers)
                worker.join();
            _workers.clear();
        }

        void push(Job && j) {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _jobs.emplace(std::move(j));
            }
            _cv.notify_one();
        }

        void push(const Job & j) {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _jobs.push(j);
            }
            _cv.notify_one();
        }
    };

} // namespace sp
