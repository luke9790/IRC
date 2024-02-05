#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <string>
#include <vector>

class Handler {
public:
    static std::vector<std::string> parseCommand(const std::string& input);
    // Other utility methods as needed
};

#endif // HANDLER_HPP
