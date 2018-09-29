#include "async.h"
#include "bulk/CommandProcessor.h"

namespace async {
    handle_t connect(std::size_t bulk) {
        auto &cmd_processor = CmdProcesser::getInstance();
        return cmd_processor.create(bulk);
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
        auto &cmd_processor = CmdProcesser::getInstance();
        return cmd_processor.process(handle, data, size);
    }

    void disconnect(handle_t handle) {
        auto &cmd_processor = CmdProcesser::getInstance();
        return cmd_processor.destroy(handle);
    }

}
