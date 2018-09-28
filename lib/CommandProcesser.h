#pragma once

#include <iostream>
#include <sstream>
#include "CommandHandler.h"
#include "ThreadPool.h"

std::ostream &operator<<(std::ostream &os, const CommandHandler::Statistic &statistic) {
    return os << "строк: " << statistic.string_num
              << ", блоков: " << statistic.bulk_num
              << ", команд: " << statistic.command_num;
}

std::ostream &operator<<(std::ostream &os, const ThreadPool::Statistic &statistic) {
    return os << "блоков: " << statistic.bulk_num << ", команд: " << statistic.command_num;
}


struct CommandProcesser {
    /**
     *
     * @param block_size Размер блока.
     * @param is Ссылка на поток ввода.
     * @param os Ссылка на поток вывода.
     */
    explicit CommandProcesser(const std::size_t block_size,
                              std::istream &is = std::cin,
                              std::ostream &os = std::cout) :
            m_is{is}
            , m_os{os}
            , m_command_handler{block_size}
            , m_log_pool{1}
            , m_file_pool{2} {

        m_print_log_id = m_command_handler.add_printer([this](std::time_t, CommandHandler::command_pull_t command_pull) {
            m_log_pool.enqueue([command_pull] { return print_log(command_pull); });
        });

        m_print_file_id = m_command_handler.add_printer([this](std::time_t time, CommandHandler::command_pull_t command_pull) {
            static size_t rnd{};
            m_file_pool.enqueue([time, command_pull] { return print_file(time, rnd++, command_pull); });
        });
    }

    /// Деструктор.
    ~CommandProcesser() {
        m_command_handler.del_printer(m_print_file_id);
        m_command_handler.del_printer(m_print_log_id);
    }

    /// Обработать поток ввода.
    void process() {
        for (std::string command; getline(m_is, command);) {
            m_command_handler.add_command(command);
        }

        auto cmd_statistic  = m_command_handler.finish();
        auto log_statistic  = m_log_pool.finish();
        auto file_statistic = m_file_pool.finish();

        m_os << "main поток: " << cmd_statistic << std::endl;

        for (const auto &statistic: log_statistic) {
            m_os << "log  поток(" << (&statistic - &log_statistic[0] + 1)
                 << "), " << statistic << std::endl;
        }

        for (const auto &statistic: file_statistic) {
            m_os << "file поток(" << (&statistic - &file_statistic[0] + 1)
                 << "), " << statistic << std::endl;
        }

    }

private:
    std::istream   &m_is;
    std::ostream   &m_os;
    CommandHandler m_command_handler;
    size_t         m_print_log_id;
    size_t         m_print_file_id;
    ThreadPool     m_log_pool;
    ThreadPool     m_file_pool;
};