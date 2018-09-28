#pragma once

#include <cassert>
#include <functional>
#include "print.h"

struct CommandHandler {
    using command_pull_t = std::vector<std::string>;
    using fn_printer_t   = std::function<void(std::time_t, const command_pull_t &)>;

    /// Структура статискики.
    struct Statistic {
        size_t string_num;  ///< Количество строк.
        size_t bulk_num;    ///< Количество блоков.
        size_t command_num; ///< Количество команд.

        Statistic &operator+=(const Statistic &other) {
            string_num  += other.string_num;
            bulk_num    += other.bulk_num;
            command_num += other.command_num;
            return *this;
        }

        bool operator==(const Statistic &other) {
            return string_num  == other.string_num &&
                   bulk_num    == other.bulk_num &&
                   command_num == other.command_num;
        }
    };

    /**
     * Конструктор.
     * @param block_size Размер блока.
     */
    explicit CommandHandler(const std::size_t block_size) : m_block_size{block_size} {
        assert(block_size != 0);
    }

    /**
     * Добавляет принтер.
     * @param printer Указатель на функцию печати блока.
     * @return Идентификатор добавленной функции печати блока.
     */
    size_t add_printer(const fn_printer_t &printer) {
        m_printers.emplace_back(printer);
        return m_printers.size() - 1;
    }

    /**
     * Удаляет принтер
     * @param printer_id Идентификатор функции печати блока.
     */
    void del_printer(size_t printer_id) {
        if (printer_id < m_printers.size()) {
            m_printers.erase(std::next(m_printers.begin(), printer_id));
        }
    }

    /**
     * Обрабатывает команду.
     * @param command Команда.
     */
    void add_command(const std::string &command) {
        ++m_statistic.string_num;
        if (command == "{") {
            print_pool();
            ++m_braces_num;
        } else if (command == "}") {
            if (m_braces_num) {
                --m_braces_num;
                print_pool();
            }
        } else {
            if (m_command_pool.empty()) {
                m_first_command_time = std::time(nullptr);
            }
            m_command_pool.emplace_back(command);
            if (m_command_pool.size() == m_block_size) {
                print_pool();
            }
        }
    }

    /// Выводит оставшиеся команды и выдает статистику.
    const auto& finish() {
        print_pool();
        return m_statistic;
    }

private:
    void print_pool() {
        if (m_command_pool.empty() || m_braces_num) {
            return;
        }

        for (const auto &printer:m_printers) {
            printer(m_first_command_time, m_command_pool);
        }

        ++m_statistic.bulk_num;
        m_statistic.command_num += m_command_pool.size();
        m_command_pool.clear();
    }

private:
    size_t                    m_block_size{};
    size_t                    m_braces_num{};
    command_pull_t                m_command_pool{};
    std::vector<fn_printer_t> m_printers{};
    std::time_t               m_first_command_time{};
    Statistic                 m_statistic{};
};

