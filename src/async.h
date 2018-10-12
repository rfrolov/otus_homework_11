#pragma once

#include <cstddef>

namespace async {

    /// Тип контекста обработчика команд.
    using handle_t = void *;

    /**
     * Подключается к обработчику команд.
     * @param bulk Размер блока команд.
     * @return Контекст обработчика команд.
     */
    handle_t connect(std::size_t bulk);

    /**
     * Принимает данные на обработку.
     * @param handle Контекст обработчика команд.
     * @param data Данные для обработки.
     * @param size Размер данных для обработки.
     */
    void receive(handle_t handle, const char *data, std::size_t size);

    /**
     * Отключается от обработчика команд.
     * @param handle Контекст обработчика команд.
     */
    void disconnect(handle_t handle);
}
