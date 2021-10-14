#pragma once
#include <string_view>
#include <map>
#include <stdexcept>
#include <functional>
#include "Command.h"


using KeyCommandType = std::string;
using ValueCommandType = std::string;
using KeyVal = std::map<KeyCommandType, ValueCommandType>;
using CommandInvokerRetType = bool;
using CommandInvokerFunc = std::function<CommandInvokerRetType(const KeyVal&)>;

class CommandLibrary
{
public:
	CommandLibrary() = default;
	template<typename CommandClass>
	void add_command()
	{
		if (m_commandLibrary.find(CommandClass::get_command_name().data()) == m_commandLibrary.end())
		{
			m_commandLibrary.emplace(CommandClass::get_command_name().data(), &CommandClass::invoker);
		}
	}

	void add_command(const std::string& commandName, const CommandInvokerFunc& commandInvokerFn)
	{
		if (m_commandLibrary.find(commandName) == m_commandLibrary.end())
		{
			m_commandLibrary.emplace(commandName, commandInvokerFn);
		}
	}

	// Make sure that the key exist
	CommandInvokerFunc& operator[](std::string commandName)
	{
		if (m_commandLibrary.find(commandName) == m_commandLibrary.end())
		{
			throw std::runtime_error("ERROR");
		}
		return m_commandLibrary.at(commandName);
	}
private:
	std::map<std::string, CommandInvokerFunc> m_commandLibrary{};
};