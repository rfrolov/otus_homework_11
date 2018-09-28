#include <chrono>
#include "gtest/gtest.h"
#include "CommandHandler.h"

TEST(CommandHandler, attach) {
    CommandHandler cmd{1};
    bool is_printer_added{};

    cmd.add_printer([&is_printer_added](std::time_t, const CommandHandler::command_pull_t &){is_printer_added = true;});
    cmd.add_command("test");
    EXPECT_TRUE(is_printer_added);
}

TEST(CommandHandler, detach) {
    CommandHandler cmd{1};
    bool is_printer_added{};

    auto id = cmd.add_printer([&is_printer_added](std::time_t, const CommandHandler::command_pull_t &){is_printer_added = true;});
    cmd.del_printer(id);
    cmd.add_command("test");
    EXPECT_FALSE(is_printer_added);
}

TEST(CommandHandler, first_time_true) {
    CommandHandler cmd{1};
    std::time_t first_time{};

    cmd.add_printer([&first_time](std::time_t time, const CommandHandler::command_pull_t &){first_time = time;});
    cmd.add_command("test");
    EXPECT_NE(first_time, std::time_t{});
}

TEST(CommandHandler, first_time_false) {
    CommandHandler cmd{1};
    std::time_t first_time{};

    cmd.add_printer([&first_time](std::time_t time, const CommandHandler::command_pull_t &){first_time = time;});
    EXPECT_EQ(first_time, std::time_t{});
}

TEST(CommandHandler, second_time_true) {
    CommandHandler cmd{1};
    std::time_t time{};

    cmd.add_printer([&time](std::time_t _time, const CommandHandler::command_pull_t &){time = _time;});
    cmd.add_command("test_1");
    time = std::time_t{};
    cmd.add_command("test_2");

    EXPECT_NE(time, std::time_t{});
}

TEST(CommandHandler, second_time_false) {
    CommandHandler cmd{3};
    std::time_t time{};

    cmd.add_printer([&time](std::time_t _time, const CommandHandler::command_pull_t &){time = _time;});
    cmd.add_command("test_1");
    time = std::time_t{};
    cmd.add_command("test_2");

    EXPECT_EQ(time, std::time_t{});
}

TEST(CommandHandler, add_cmd_true_block_size) {
    CommandHandler cmd{3};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("test_1");
    cmd.add_command("test_2");
    cmd.add_command("test_3");

    std::vector<std::string> v{"test_1", "test_2", "test_3"};
    EXPECT_EQ(cmd_pull, v);
}

TEST(CommandHandler, add_cmd_false_block_size) {
    CommandHandler cmd{3};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("test_1");
    cmd.add_command("test_2");

    std::vector<std::string> v{"test_1", "test_2", "test_3"};
    EXPECT_EQ(cmd_pull, std::vector<std::string>{});
}

TEST(CommandHandler, add_cmd_true_block) {
    CommandHandler cmd{10};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("{");
    cmd.add_command("test");
    cmd.add_command("}");

    EXPECT_EQ(cmd_pull, std::vector<std::string>{"test"});
}

TEST(CommandHandler, add_cmd_true_block_block) {
    CommandHandler cmd{1};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("{");
    cmd.add_command("test_1");
    cmd.add_command("{");
    cmd.add_command("test_2");
    cmd.add_command("test_3");
    cmd.add_command("}");
    cmd.add_command("}");

    std::vector<std::string> v{"test_1", "test_2", "test_3"};
    EXPECT_EQ(cmd_pull, v);
}

TEST(CommandHandler, add_cmd_false_block) {
    CommandHandler cmd{1};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("{");
    cmd.add_command("test_1");
    cmd.add_command("test_2");
    cmd.add_command("test_3");
    EXPECT_EQ(cmd_pull, std::vector<std::string>{});
}

TEST(CommandHandler, add_cmd_false_block_block) {
    CommandHandler cmd{1};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("{");
    cmd.add_command("test_1");
    cmd.add_command("{");
    cmd.add_command("test_2");
    cmd.add_command("test_3");
    cmd.add_command("}");
    EXPECT_EQ(cmd_pull, std::vector<std::string>{});
}

TEST(CommandHandler, end_true) {
    CommandHandler cmd{2};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("test");
    cmd.finish();
    EXPECT_EQ(cmd_pull, std::vector<std::string>{"test"});
}

TEST(CommandHandler, end_false_empty) {
    CommandHandler cmd{1};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("test");
    cmd_pull.clear();
    cmd.finish();
    EXPECT_EQ(cmd_pull, std::vector<std::string>{});
}

TEST(CommandHandler, end_false_braces) {
    CommandHandler cmd{2};
    CommandHandler::command_pull_t cmd_pull{};

    cmd.add_printer([&cmd_pull](std::time_t, const CommandHandler::command_pull_t &_cmd_pull){ cmd_pull = _cmd_pull; });
    cmd.add_command("{");
    cmd.add_command("test");
    cmd.finish();
    EXPECT_EQ(cmd_pull, std::vector<std::string>{});
}