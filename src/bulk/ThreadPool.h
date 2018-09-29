#pragma once

#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>

/// Класс пула потоков.
struct ThreadPool {

    /// Структура статистики.
    struct Statistic {
        size_t bulk_num;        ///< Количество блоков.
        size_t command_num;     ///< Количество команд.

        Statistic &operator+=(const Statistic &other) {
            bulk_num += other.bulk_num;
            command_num += other.command_num;
            return *this;
        }

        bool operator==(const Statistic &other) {
            return bulk_num == other.bulk_num && command_num == other.command_num;
        }
    };

    using func_t = std::function<Statistic()>;

    /**
     * Конструктор.
     * @param threads_num Количество потоков в пуле.
     */
    explicit ThreadPool(size_t threads_num) : m_max_queue_size{threads_num * 3} {
        m_statistics.resize(threads_num);
        m_workers.reserve(threads_num);

        for (size_t i = 0; i < threads_num; ++i)
            m_workers.emplace_back(
                    [this, i] {
                        for (;;) {
                            func_t task;
                            {
                                std::unique_lock<std::mutex> lock(m_queue_mutex);
                                m_condition.wait(lock, [this] { return m_stop || !m_task_queue.empty(); });
                                if (m_stop && m_task_queue.empty()) {
                                    return;
                                }
                                task = std::move(m_task_queue.front());
                                m_task_queue.pop();
                            }
                            auto   statistic = task();
                            {
                                std::unique_lock<std::mutex> lock(m_statistic_mutex);
                                m_statistics[i] += statistic;
                            }
                        }
                    }
            );
    }

    /// Добавляет задачу в очередь.
    template<typename F, typename... Args>
    void enqueue(F &&f, Args &&... args) {
        for (;;) {
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);

                if (m_stop) {
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                }

                if (m_task_queue.size() < m_max_queue_size) {
                    m_task_queue.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                    break;
                }
                std::this_thread::yield();
            }
        }
        m_condition.notify_one();
    }


    /**
     * Завершает все потоки и выводит статистику.
     * @return Статистика.
     */
    const auto &finish() {
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_stop = true;
        }
        m_condition.notify_all();
        for (std::thread &worker: m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        // Все потоки остановлены, поэтому мьютекс здесь не нужен.
        return m_statistics;
    }

    /// Деструктор.
    ~ThreadPool() {
        finish();
    }


private:
    std::vector<std::thread> m_workers{};

    std::mutex              m_queue_mutex{};
    std::condition_variable m_condition{};
    std::queue<func_t>      m_task_queue{};
    bool                    m_stop{};

    std::mutex             m_statistic_mutex{};
    std::vector<Statistic> m_statistics{};

    size_t m_max_queue_size;
};
