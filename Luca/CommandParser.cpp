/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmasetti <lmasetti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 14:25:53 by lmasetti          #+#    #+#             */
/*   Updated: 2024/03/21 14:25:54 by lmasetti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandParser.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> CommandParser::parseCommand(const std::string& input)
{
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
