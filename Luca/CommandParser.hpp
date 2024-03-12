#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <vector>

class CommandParser {
public:
    static std::vector<std::string> parseCommand(const std::string& input);
};

#endif
