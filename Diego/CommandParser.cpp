#include "CommandParser.hpp"
#include <sstream>

std::vector<std::string> CommandParser::parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (getline(iss, token, ' ')) {
        tokens.push_back(token);
    }

    return tokens;
}