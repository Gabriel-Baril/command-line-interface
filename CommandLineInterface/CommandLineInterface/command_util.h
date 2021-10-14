#pragma once
#include <vector>
#include <string_view>
#include <algorithm>

#include "CommandTemplate.h"
#include "CommandLibrary.h"

using KeyCommandType = std::string;
using ValueCommandType = std::string;
using KeyVal = std::map<KeyCommandType, ValueCommandType>;
using CommandInvokerRetType = bool;
using CommandInvokerFunc = std::function<CommandInvokerRetType(const KeyVal&)>;


std::vector<std::string> parse_command_token(const std::string& rawCommand)
{
    std::vector<std::string> tokens;
    tokens.reserve(std::count(rawCommand.begin(), rawCommand.end(), ' ') + 1);

    size_t previousSpaceIndex = -1;
    size_t currentSpaceIndex = rawCommand.find(' ');
    while (currentSpaceIndex != -1)
    {
        tokens.push_back(rawCommand.substr(previousSpaceIndex + 1, currentSpaceIndex - previousSpaceIndex - 1));
        previousSpaceIndex = currentSpaceIndex;
        currentSpaceIndex = rawCommand.find(' ', currentSpaceIndex + 1);
    }
    tokens.push_back(rawCommand.substr(previousSpaceIndex + 1, currentSpaceIndex - previousSpaceIndex));

    return tokens;
}

std::pair<std::string, std::string> parse_args(const std::string& commandName, const std::string& commandToken, CommandLibrary& library)
{
    constexpr char ASSIGNMENT_CHARACTER = '=';
    size_t assignmentOperatorIndex = commandToken.find(ASSIGNMENT_CHARACTER);
    if (assignmentOperatorIndex != std::string::npos)
    {
        return { commandToken.substr(0, assignmentOperatorIndex), commandToken.substr(assignmentOperatorIndex + 1, commandToken.size() - assignmentOperatorIndex) };
    }
    //throw std::runtime_error("PARSING ERROR");
    return { "", "" };
}

std::map<std::string, std::string> parse_command(const std::string& rawCommand, CommandLibrary& library)
{
    const auto tokens = parse_command_token(rawCommand);
    if (tokens.size() <= 0)
        throw "Unknow error";

    std::string command_name = tokens[0];
    std::map<std::string, std::string> arg_val{ { "_command_name", command_name } };
    for (auto it = tokens.begin() + 1; it != tokens.end(); it++)
    {
        const auto current_arg_val = parse_args(command_name, *it, library);
        if (!current_arg_val.first.empty())
            arg_val.insert(current_arg_val);
        else
            throw "Arg parsing error (Not in the form arg=value)";
    }
    return arg_val;
}

void process_command(const std::string& rawCommand, CommandLibrary& library)
{
    std::map<std::string, std::string> arg_val = parse_command(rawCommand, library);
    std::string command_name = arg_val.at("_command_name");
    
    library[command_name](arg_val);
}