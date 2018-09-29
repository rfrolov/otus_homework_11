#pragma once

#include <iostream>
#include <sstream>
#include <sstream>
#include <algorithm>
#include "CommandHandler.h"
#include "ThreadPool.h"

std::ostream &operator<<(std::ostream &os, const CommandHandler::Statistic &statistic) {
    return os << "строк: " << statistic.string_num << ", блоков: " << statistic.bulk_num << ", команд: "
              << statistic.command_num;
}

std::ostream &operator<<(std::ostream &os, const ThreadPool::Statistic &statistic) {
    return os << "блоков: " << statistic.bulk_num << ", команд: " << statistic.command_num;
}


class CmdProcesser {
public:
    static CmdProcesser &getInstance() {
        static CmdProcesser instance{};
        return instance;
    }

    CmdProcesser(const CmdProcesser &) = delete;

    CmdProcesser(const CmdProcesser &&) = delete;

    CmdProcesser &operator=(const CmdProcesser &) = delete;

    CmdProcesser &operator=(const CmdProcesser &&) = delete;


    CommandHandler *create(std::size_t bulk) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_handlers.emplace_back(new Context{CommandHandler{bulk}, std::string{}});

        auto &context         = m_handlers.back();
        auto &command_handler = context->command_handler;
        context->print_log_id = command_handler.add_printer(
                [this](std::time_t, CommandHandler::command_pull_t command_pull) {
                    m_thread_pool.enqueue([command_pull] { return print_log(command_pull); });
                });

        context->print_file_id = command_handler.add_printer(
                [this](std::time_t time, CommandHandler::command_pull_t command_pull) {
                    static size_t rnd{};
                    m_thread_pool.enqueue([time, command_pull] { return print_file(time, rnd++, command_pull); });
                });
        return &command_handler;
    }

    void process(void *handler, const char *data, std::size_t size) {
        std::unique_lock<std::mutex> lock(m_mutex);

        auto it = std::find(m_handlers.cbegin(), m_handlers.cend(), handler);
        if (it == m_handlers.cend()) { return; }

        auto &context         = *it;
        auto &command_handler = context->command_handler;

        std::string str = context->data + std::string(&data[0], &data[size]);

        const auto *cdata = str.c_str();

        std::size_t      last{};
        for (std::size_t i{0}; i <= size; ++i) {
            if (cdata[i] == '\n') {
                command_handler.add_command(std::string(&cdata[last], &cdata[i]));
                last = i < size ? i + 1 : size;
            }
        }
        context->data = std::string(&cdata[last], &cdata[size]);
    }

    void destroy(void *handler) {
        std::unique_lock<std::mutex> lock(m_mutex);

        auto it = std::find(m_handlers.cbegin(), m_handlers.cend(), handler);
        if (it == m_handlers.cend()) { return; }

        auto &context         = *it;
        auto &command_handler = context->command_handler;

        delete *it;
        m_handlers.erase(it);
    }

private:
    struct Context {
        CommandHandler command_handler;
        std::string    data;
        std::size_t    print_log_id;
        std::size_t    print_file_id;
    };

    CmdProcesser() = default;

    ~CmdProcesser() = default;

    std::mutex             m_mutex{};
    ThreadPool             m_thread_pool{std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1};
    std::vector<Context *> m_handlers{};
};