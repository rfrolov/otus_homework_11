#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include "ThreadPool.h"

/**
 * Печатает блок команд в консоль.
 * @param command_pool Блок команд.
 * @param os Ссылка на поток вывода.
 * @return Статистика.
 */
ThreadPool::Statistic print_log(const std::vector<std::string> &command_pool, std::ostream &os = std::cout);

/**
 * Печатает блок команд в файл.
 * @param time Время поступления первой команды.
 * @param rnd Некое значение, необходимое для уникальности названия файла.
 * @param command_pool Блок команд.
 * @return Статистика.
 */
ThreadPool::Statistic print_file(std::time_t time, size_t rnd, const std::vector<std::string> &command_pool);