/*
Copyright 2020-2021 Wuping Xin

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
#include <vector>

namespace wxlib {
namespace flux {

// C++/17 SFINAE
template<typename E>
using is_scoped_enum = std::integral_constant<bool, std::is_enum_v<E> && !std::is_convertible_v<E, int>>;

// C++/20 Concept
template<typename E>
concept ScopedEnum = std::is_enum_v<E> && !std::is_convertible_v<E, int>;

class Action final
{
public:
    template <ScopedEnum T>
    Action(T type, std::any &payload, bool error = false)
        : type_(static_cast<int>(type)), payload_(payload), error_(error)
    {
    }

    template <ScopedEnum T>
    Action(T type, std::any &&payload = std::any(), bool error = false)
        : type_(static_cast<int>(type)), payload_(std::move(payload)), error_(error)
    {
    }

    Action(const Action&) = default;
    Action(Action&&) = default;
    ~Action() = default;

    Action &operator=(const Action&) = default;
    Action &operator=(Action&&) = default;

    template <ScopedEnum T>
    auto getType() const
    {
        return static_cast<T>(type_);
    }

    template<typename T>
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
    virtual void process(const std::shared_ptr<Action>& action) = 0;
protected:
    Middleware() = default;
    Middleware(const Middleware&) = default;
    Middleware(Middleware&&) = default;

    Middleware &operator=(const Middleware&) = default;
    Middleware &operator=(Middleware&&) = default;
};

class Store
{
public:
    virtual ~Store() = default;
    virtual void process(const std::shared_ptr<Action>& action) = 0;
protected:
    Store() = default;
    Store(const Store&) = default;
    Store(Store&&) = default;
    Store &operator=(const Store&) = default;
    Store &operator=(Store&&) = default;
};

class Dispatcher final
{
public:
    static Dispatcher& instance()
    {
        static Dispatcher self;
        return self;
    }

    template <typename... Ts>
    void registerMiddleware(Ts&&... args)
    {
        middlewares_.emplace_back(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void registerStore(Ts&&... args)
    {
        stores_.emplace_back(std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    void dispatch(Ts&&... args)
    {
        std::unique_lock<std::mutex> lk(mutex_);
        actions_.emplace(std::forward<Ts>(args)...);
        lk.unlock();

        wake_ = true;
        condition_.notify_one();
    }

private:
    Dispatcher()
    {
        thread_ = std::thread([=] { this->run(); });
    }

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher(Dispatcher&&) = delete;

    Dispatcher &operator=(const Dispatcher&) = delete;
    Dispatcher &operator=(Dispatcher&&) = delete;

    ~Dispatcher()
    {
        stop();
        thread_.join();
    }

    void run()
    {
        std::unique_lock<std::mutex> lk(mutex_);

        while (!done_) {
            while (!wake_) {
                condition_.wait(lk);
            }

            while (!actions_.empty()) {
                const auto& action = actions_.front();                
                lk.unlock();

                for (const auto& middleware: middlewares_) {
                    if (done_) break;
                    middleware->process(action);
                }

                for (const auto& store: stores_) {
                    if (done_) break;
                    store->process(action);
                }

                lk.lock();
                // action is a lvalue reference of the shared_ptr. If pop right after actions_.front(), '
                // the shared_ptr object will be prematurely released.
                actions_.pop(); 
            }

            wake_ = false;
        }
    }

    void stop()
    {
        std::unique_lock<std::mutex> lk(mutex_);
        done_ = true;
        wake_ = true;        
        actions_ = { };
        lk.unlock();
        
        condition_.notify_one();
    }

    std::queue<std::shared_ptr<Action>> actions_;
    std::vector<std::shared_ptr<Middleware>> middlewares_;
    std::vector<std::shared_ptr<Store>> stores_;

    std::atomic_bool wake_{false};
    std::atomic_bool done_{false};
    std::mutex mutex_;
    std::condition_variable condition_;

    std::thread thread_;
};

} // end of namespace flux
} // end of namespace wxlib
#endif