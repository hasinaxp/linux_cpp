#pragma once
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace sp {

        template<typename Job>
        struct ThreadPool
        {
            std::vector<std::thread> _workers;
            std::queue<Job> _jobs;
            std::mutex _mutex;
            std::condition_variable _cv;
            std::function<void(Job)> _workerFunc = nullptr;
            bool _stop = false;

            void create(
                    std::function<void(Job &)> workerFunction,
                    uint32_t threadCount = std::thread::hardware_concurrency())
            {
                _workers = {};
                _workers.reserve(threadCount);
                _stop = false;
                for(int i = 0; i < threadCount; i++)
                {
                    _workers.emplace_back([&]{

                            std::unique_lock<std::mutex> lock(_mutex);
                            _cv.wait(lock, [&] { return !_jobs.empty() || _stop; });

                            if(_stop && _jobs.empty()) return;

                            workerFunction(_jobs.front());
                            _jobs.pop();


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
                for(auto & w: _workers)
                    w.join();
                _workers = {};
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

}; //sp;
