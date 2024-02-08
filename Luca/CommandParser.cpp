#include "CommandParser.hpp"
#include <sstream>

// std::vector<std::string> CommandParser::parseCommand(const std::string& input) {
//     std::vector<std::string> tokens;
//     std::istringstream iss(input);
//     std::string token;

//     while (getline(iss, token, ' ')) {
//         tokens.push_back(token);
//     }

//     return tokens;
// }

#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> CommandParser::parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss;
    
    // Rimuovi i caratteri di newline e carriage return dall'input
    std::string sanitizedInput = input;
    sanitizedInput.erase(std::remove(sanitizedInput.begin(), sanitizedInput.end(), '\r'), sanitizedInput.end());
    sanitizedInput.erase(std::remove(sanitizedInput.begin(), sanitizedInput.end(), '\n'), sanitizedInput.end());

    iss.str(sanitizedInput);
    std::string token;

    while (getline(iss, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}
