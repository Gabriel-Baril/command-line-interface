#pragma once
#include <vector>
#include <string_view>

enum class CommandParameterType : uint8_t
{
	STRING,
	INT,
	DECIMAL,
	BOOL
};

struct CommandParameter
{
	std::string paramName;
	CommandParameterType paramType;
	bool required = true;
};

class CommandTemplate
{
public:
	CommandTemplate(const std::string& name, const std::vector<CommandParameter>& parameters);
	//CommandTemplate(const CommandTemplate&) = delete;
	//CommandTemplate(CommandTemplate&&) = delete;
	CommandTemplate() = default;

	std::string_view get_command_name() const;

	const CommandParameter& get_param(uint32_t index)
	{
		return m_commandParameters[index];
	}

	bool has_param(const std::string_view& name)
	{
		return std::find_if(m_commandParameters.begin(), m_commandParameters.end(), [&name](const CommandParameter& param) { return param.paramName == name; }) != m_commandParameters.end();
	}

	std::vector<CommandParameter>::iterator begin() { return m_commandParameters.begin(); }
	std::vector<CommandParameter>::iterator end() { return m_commandParameters.end(); }

	std::vector<CommandParameter>::const_iterator begin() const { return m_commandParameters.begin(); }
	std::vector<CommandParameter>::const_iterator end() const { return m_commandParameters.end(); }

	const CommandParameter& operator[](uint32_t paramIndex)
	{
		return m_commandParameters[paramIndex];
	}

	const CommandParameter& operator[](std::string_view paramName)
	{
		uint32_t indexOfParam = -1;
		for (int i = 0;i < m_commandParameters.size();i++)
		{
			if (m_commandParameters[i].paramName == paramName)
			{
				indexOfParam = i;
				break;
			}
		}

		return m_commandParameters[indexOfParam];
	}
private:
	std::string m_commandName = "__unknown";
	std::vector<CommandParameter> m_commandParameters{};
};

