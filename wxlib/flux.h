/*
Copyright 2020 Wuping Xin

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific
*/

#ifndef WXLIB_FLUX_H
#define WXLIB_FLUX_H

#include <any>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace wxlib {
namespace flux {

template<typename E>
using is_scoped_enum = std::integral_constant<bool, std::is_enum_v<E> && !std::is_convertible_v<E, int>>;

class Action final
{
public:
    template <class ScopedEnum, typename = std::enable_if_t<is_scoped_enum<ScopedEnum>::value>>
    Action(ScopedEnum type, std::any &payload, bool error = false)
        : type_(static_cast<int>(type)), payload_(payload), error_(error)
    {
    }

    template <class ScopedEnum, typename = std::enable_if_t<is_scoped_enum<ScopedEnum>::value>>
    Action(ScopedEnum type, std::any &&payload = std::any(), bool error = false)
        : type_(static_cast<int>(type)), payload_(std::move(payload)), error_(error)
    {
    }

    Action(const Action &) = default;
    Action(Action &&) = default;
    ~Action() = default;

    Action &operator=(const Action &) = default;
    Action &operator=(Action &&) = default;

    template <class ScopedEnum, typename = std::enable_if_t<is_scoped_enum<ScopedEnum>::value>>
    auto getType() const
    {
        return static_cast<ScopedEnum>(type_);
    }

    template<class T>
    auto getPayload() const
    {
        return std::any_cast<T>(payload_);
    }

    bool getError() const
    {
        return error_;
    }

private:
    bool error_;
    std::any payload_;
    int type_;
};

class Middleware
{
public:
    virtual ~Middleware() = default;
    virtual std::shared_ptr<Action> process(const std::shared_ptr<Action> &action) = 0;
protected:
    Middleware() = default;
    Middleware(const Middleware &) = default;
    Middleware(Middleware &&) = default;
    
    Middleware &operator=(const Middleware &) = default;
    Middleware &operator=(Middleware &&) = default;
};

class Store
{
public:
    virtual ~Store() = default;
    virtual void process(const std::shared_ptr<Action> &action) = 0;
protected:
    Store() = default;
    Store(const Store &) = default;
    Store(Store &&) = default;
    Store &operator=(const Store &) = default;
    Store &operator=(Store &&) = default;
};

class Dispatcher final
{
public:
    static Dispatcher &instance()
    {
        static Dispatcher self;
        return self;
    }

    template <class... Args>
    void registerMiddleware(Args&&... args)
    {
        middlewares_.emplace_back(std::forward<Args>(args)...);
    }

    template <class... Args>
    void registerStore(Args&&... args)
    {
        stores_.emplace_back(std::forward<Args>(args)...);
    }

    template <class... Args>
    void dispatch(Args&&... args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        actions_.emplace(std::forward<Args>(args)...);

        wake_ = true;
        condition_.notify_one();
    }

private:
    Dispatcher()
    {
        thread_ = std::thread([dispatcher = this] () {
                dispatcher->run();
            });
    }

    Dispatcher(const Dispatcher &) = delete;
    Dispatcher(Dispatcher &&) = delete;
    
    Dispatcher &operator=(const Dispatcher &) = delete;
    Dispatcher &operator=(Dispatcher &&) = delete;

    ~Dispatcher()
    {
        stop();
        thread_.join();
    }

    void run()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while (true) {

            while (!wake_) {
                condition_.wait(lock);
            }

            while (!actions_.empty()) {
                auto action = actions_.front();
                lock.unlock();

                for (const auto &middleware : middlewares_) {
                    action = middleware->process(action);
                }

                for (const auto &store : stores_) {
                    store->process(action);
                }

                lock.lock();
                actions_.pop();
            }

            wake_ = false;

            if (done_) {
                break;
            }
        }
    }

    void stop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        done_ = true;
        wake_ = true;
        condition_.notify_one();
    }

    std::queue<std::shared_ptr<Action>> actions_;
    std::vector<std::shared_ptr<Middleware>> middlewares_;
    std::vector<std::shared_ptr<Store>> stores_;

    std::atomic_bool wake_;
    std::atomic_bool done_;
    std::mutex mutex_;
    std::condition_variable condition_;

    std::thread thread_;
};

} // end of namespace flux
} // end of namespace wxlib
#endif