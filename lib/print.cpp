#include "print.h"
#include <fstream>

ThreadPool::Statistic print_log(const std::vector<std::string> &command_pool, std::ostream &os) {
    os << "Bulk: ";
    for (const auto &command : command_pool) {
        os << command << (&command != &command_pool.back() ? ", " : "");
    }
    os << std::endl;
    return ThreadPool::Statistic{1, command_pool.size()};
}

ThreadPool::Statistic print_file(std::time_t time, size_t rnd, const std::vector<std::string> &command_pool) {
    std::fstream fs{};
    std::string  file_name = "bulk_num" + std::to_string(time) + "." + std::to_string(rnd) + ".log";
    fs.open(file_name, std::ios::out);
    if (!fs.is_open()) { return ThreadPool::Statistic{0, 0}; }

    fs << "Bulk: ";
    for (const auto &command : command_pool) {
        fs << command << (&command != &command_pool.back() ? ", " : "");
    }
    fs << std::endl;
    return ThreadPool::Statistic{1, command_pool.size()};
}
