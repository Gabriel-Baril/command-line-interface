#include "CommandTemplate.h"

CommandTemplate::CommandTemplate(const std::string& name, const std::vector<CommandParameter>& parameters)
	: m_commandName{ name }, m_commandParameters{ parameters }
{
}

std::string_view CommandTemplate::get_command_name() const
{
	return m_commandName;
}
