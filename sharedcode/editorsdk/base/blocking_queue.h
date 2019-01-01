#include <stdio.h>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <cassert>
#include <sstream>
// 一个线程安全的阻塞队列
namespace whensunset {
    namespace base {
        template<typename T>
        class BlockingQueue {
        public:

            explicit BlockingQueue(int capacity,
                                   std::function<T()> default_value_functor = [] { return T(); })
                    : capacity_(capacity) {
                assert(capacity_ > 0);
                items_.reserve(capacity_);
                default_value_functor_ = default_value_functor;
            }

            virtual ~BlockingQueue() {
                std::unique_lock<std::mutex> lock(mutex_);
                items_.clear();
            }

            void Clear() {
                std::unique_lock<std::mutex> lock(mutex_);
                items_.clear();
                not_full_cond_.notify_all();
            }

            bool PushBack(const T &item) {
                return PushBack(item, -1);
            }

            bool PushBack(const T &item, int64_t timeout_ms) {
                std::unique_lock<std::mutex> lock(mutex_);

                if (timeout_ms >= 0) {
                    if (!not_full_cond_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                                 [&] {
                                                     return (items_.size() < capacity_) ||
                                                            is_closed_;
                                                 })) {
                        return false;
                    }
                } else {
                    not_full_cond_.wait(lock,
                                        [&] { return (items_.size() < capacity_) || is_closed_; });
                    if (is_closed_) {
                        return false;
                    }
                }
                // should not use std::move, since item is declared as const
                // in fact, use move has no effect here, it will use copy ctor anyway
                items_.push_back(item);
                not_empty_cond_.notify_one();
                assert(!items_.empty());

                return true;
            }

            bool PushBack(T &&item) {
                // must use std::move(), cause named rvalue reference is deemed as lvalue
                return PushBack(std::move(item), -1);
            }

            bool PushBack(T &&item, int64_t timeout_ms) {
                std::unique_lock<std::mutex> lock(mutex_);

                if (timeout_ms >= 0) {
                    if (!not_full_cond_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                                 [&] {
                                                     return (items_.size() < capacity_) ||
                                                            is_closed_;
                                                 })) {
                        return false;
                    }
                } else {
                    not_full_cond_.wait(lock,
                                        [&] { return (items_.size() < capacity_) || is_closed_; });
                    if (is_closed_) {
                        return false;
                    }
                }

                items_.push_back(std::move(item));
                not_empty_cond_.notify_one();
                assert(!items_.empty());

                return true;
            }

            // Push back regardless of the capacity
            void PushBackForce(T &&item) {
                std::lock_guard<std::mutex> lk(mutex_);
                if (is_closed_) {
                    return;
                }
                items_.push_back(std::move(item));
                not_empty_cond_.notify_one();
                assert(!items_.empty());
            }


            T PopFront() {
                std::unique_lock<std::mutex> lock(mutex_);
                return PopFrontInternal(std::move(lock));
            }

            void BlockUntil(std::function<bool(const std::vector<T> &)> pred) {
                std::unique_lock<std::mutex> lock(mutex_);
                not_empty_cond_.wait(lock, [&] {
                    return is_closed_ || pred(items_);
                });
            }

            std::pair<bool, T> PopFrontIf(std::function<bool(const std::vector<T> &)> predicate) {
                std::unique_lock<std::mutex> lock(mutex_);
                return predicate(items_) ?
                       std::make_pair(true, PopFrontInternal(std::move(lock))) :
                       std::make_pair(false, default_value_functor_());
            }

            T PopFrontInternal(std::unique_lock<std::mutex> lock) {
                // Assume mutex_ is held
                not_empty_cond_.wait(lock, [&] {
                    return !items_.empty() || is_closed_;
                });
                if (is_closed_) {
                    return default_value_functor_();
                }
                T value = std::move(items_.front());
                items_.erase(items_.begin());

                not_full_cond_.notify_one();
                return std::move(value);
            }

            // Atmoic peek operation
            bool Peek(std::function<bool(const std::vector<T> &)> pred) {
                std::unique_lock<std::mutex> lock(mutex_);
                return pred(items_);
            }

            int Size() {
                std::unique_lock<std::mutex> lock(mutex_);
                return static_cast<int>(items_.size());
            }

            bool Empty() {
                std::unique_lock<std::mutex> lock(mutex_);
                return items_.empty();
            }

            // Close the queue, unblocking all pop / push operations and subsequent pushes
            // are treated as noop, subsequent pops will return default value,
            void Close() {
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    items_.clear();
                    is_closed_ = true;
                }
                not_full_cond_.notify_all();
                not_empty_cond_.notify_all();
            }

            // Open the queue, reverse the close operation
            void Open() {
                std::lock_guard<std::mutex> lock(mutex_);
                is_closed_ = false;
            }

            bool is_closed() {
                std::lock_guard<std::mutex> lk(mutex_);
                return is_closed_;
            }

        private:

            std::vector<T> items_;
            std::mutex mutex_;
            std::condition_variable not_empty_cond_;
            std::condition_variable not_full_cond_;
            int capacity_;
            bool is_closed_ = false;
            std::function<T()> default_value_functor_;
        };

    }
}
