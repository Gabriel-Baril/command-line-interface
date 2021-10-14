#pragma once
#include "CommandTemplate.h"

class Command
{
public:
	Command(const CommandTemplate& commandTemplate)
		: m_commandTemplate{ commandTemplate } {}
	Command(const std::string& commmandName, const std::vector<CommandParameter>& params)
		: m_commandTemplate(commmandName, params) {}

	virtual void invoke(const std::map<std::string, std::string>& key_val) = 0;

	std::string_view get_command_name()
	{
		return m_commandTemplate.get_command_name();
	}
	virtual ~Command() = default;
private:
	CommandTemplate m_commandTemplate;
};