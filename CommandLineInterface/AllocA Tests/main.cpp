#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <array>

int allocated = 0;

void* operator new(unsigned int size)
{
    //std::cout << "New operator overloading : " << size << std::endl;
    allocated += size;
    void* p = malloc(size);
    return p;
}

constexpr char ASSIGNMENT_CHARACTER = '=';

bool is_special_character(char current)
{
    constexpr std::array<char, 4> special_chars = { { '=', ',', '[', ']' } };
    for (char c : special_chars)
        if (c == current) return true;
    return false;
}


std::string trim_quotes(const std::string& expression)
{
    constexpr char QUOTE_CAHRACTER = '\"';
    if (expression.size() > 2)
    {
        int startIndex = 0;
        int endIndex = expression.size() - 1;
        if (expression.front() == QUOTE_CAHRACTER)
            startIndex = 1;
        if (expression.back() == QUOTE_CAHRACTER)
            endIndex -= 1;
        return expression.substr(startIndex, endIndex - startIndex + 1);
    }
    return expression;
}

// Normalize the whitespace character in a raw command
std::string normalize(const std::string& rawCommand)
{
    constexpr char EMPTY_CHARACTER = ' ';
    std::string commandBuffer;
    size_t index = 0;

    while (index < rawCommand.size() && index != std::string::npos)
    {
        if (rawCommand[index] != EMPTY_CHARACTER)
        {
            commandBuffer.push_back(rawCommand[index]);
            if (is_special_character(rawCommand[index]))
            {
                char previousChar = rawCommand[index];
                index = rawCommand.find_first_not_of(EMPTY_CHARACTER, index + 1);
                if (index != std::string::npos && previousChar == ']' && (rawCommand[index] != ']' && rawCommand[index] != ',') )
                {
                    commandBuffer.push_back(EMPTY_CHARACTER);
                }
            }
            else
            {
                index++;
            }
        }
        else
        {
            index = rawCommand.find_first_not_of(EMPTY_CHARACTER, index);
            if (!is_special_character(rawCommand[index]))
            {
                commandBuffer.push_back(EMPTY_CHARACTER);
            }
        }
    }
    return commandBuffer;
}

// Return the partitions that compose the command
std::vector<std::string> partition(const std::string& normalizedCommand)
{
    std::vector<std::string> output;
    output.reserve(std::count(normalizedCommand.begin(), normalizedCommand.end(), ' ') + 1);

    size_t previousSpaceIndex = -1;
    size_t currentSpaceIndex = normalizedCommand.find(' ');
    while (currentSpaceIndex != -1)
    {
        output.push_back(normalizedCommand.substr(previousSpaceIndex + 1, currentSpaceIndex - previousSpaceIndex - 1));
        previousSpaceIndex = currentSpaceIndex;
        currentSpaceIndex = normalizedCommand.find(' ', currentSpaceIndex + 1);
    }
    output.push_back(normalizedCommand.substr(previousSpaceIndex + 1, currentSpaceIndex - previousSpaceIndex));

    return output;
}

std::pair<std::string, std::string> parse_assignment(const std::string& assignment)
{
    size_t assignmentOperatorIndex = assignment.find(ASSIGNMENT_CHARACTER);
    if (assignmentOperatorIndex != std::string::npos)
    {
        std::string param = trim_quotes(assignment.substr(0, assignmentOperatorIndex));
        std::string value = trim_quotes(assignment.substr(assignmentOperatorIndex + 1, assignment.size() - assignmentOperatorIndex));
        return { param, value };
    }
    return { "error", "parsing" };
}

std::vector<std::string> partition_value(const std::string& value)
{
    std::vector<std::string> output;

    std::string buffer;
    size_t index = 1; // find_first_of('[')
    uint32_t openingBracketCount = 0;
    uint32_t closingBracketCount = 0;
    while (index < value.size() - 1)
    {
        if (value[index] == '[')
            openingBracketCount++;
        else if (value[index] == ']')
            closingBracketCount++;

        if ((openingBracketCount - closingBracketCount) == 0)
        {
            if (value[index] != ',')
            {
                buffer.push_back(value[index]);
            }
            else
            {
                output.push_back(buffer);
                buffer.clear();
            }
        }
        else
        {
            buffer.push_back(value[index]);
        }
        index++;
    }
    output.push_back(buffer);
    return output;
}

bool is_aggregate(const std::string& value)
{
    // TODO: Error checking
    // TODO: Better parsing
    return value.front() == '[';
}

struct ParamMetadata
{
    size_t relativeParamOrder;
    size_t absoluteParamOrder;
    // nbAttribute, isAggregate, datatype, paramname, paramvalue
    // parse_command -> parse_parameter -> parse_assignment
    // parse_assignment(std::string assignment) -> std::vector<ParamData>
};

std::string parse_partition(const std::string& partition, const std::string& parentParam, std::map<std::string, std::string>& parentMap, const ParamMetadata& metadata)
{
    std::pair<std::string, std::string> nameValuePair = parse_assignment(partition);
    const std::string& paramName = std::get<0>(nameValuePair);
    const std::string& paramValue = std::get<1>(nameValuePair);
    if (is_aggregate(paramValue))
    {
        std::vector<std::string> attributes = partition_value(paramValue);

        size_t paramOrder = 0;
        for (const auto& attribute : attributes)
        {
            // parentMap.insert({ parentParam + '.' + paramName + ".__order", std::to_string(metadata.relativeParamOrder) });
            parse_partition(attribute, parentParam + '.' + paramName, parentMap, { .relativeParamOrder = paramOrder });// , .absoluteParamOrder = metadata.absoluteParamOrder + 1
            paramOrder++;
        }
    }
    else
    {
        parentMap.insert({ parentParam + '.' + paramName, paramValue });
        // parentMap.insert({ parentParam + '.' + paramName + ".__order", std::to_string(metadata.relativeParamOrder) });
        // parentMap.insert({ parentParam + '.' + paramName + ".__order_absolute", std::to_string(metadata.absoluteParamOrder) });
    }
    return paramName;
}

std::map<std::string, std::string> parse_command(const std::string& rawCommand)
{
    std::string normalizedRawCommand = normalize(rawCommand);
    std::vector<std::string> commandPartitions = partition(normalizedRawCommand);

    std::map<std::string, std::string> commandKeyVal;
    commandKeyVal.insert({ "meta.command_namespace", commandPartitions[0] });
    commandKeyVal.insert({ "meta.command_name", commandPartitions[1] });

    size_t paramOrder = 0;
    for (auto it = commandPartitions.begin() + 2; it != commandPartitions.end(); it++)
    {
        std::string fullParamName = parse_partition(*it, "root", commandKeyVal, { .relativeParamOrder = paramOrder, .absoluteParamOrder = 0 });
        //commandKeyVal.insert({ "meta." + fullParamName + ".__order", std::to_string(paramOrder) });
        paramOrder++;
    }
    return commandKeyVal;
}

int main()
{
    // hdr draw object=[radius=10,pos=[x=14,y=7,z=90],vel=[abc=12,def=34],obj=[data=[axe=[x=2,y=6,z=78]],allo=12],bonjour=5]
    // hdr    draw   object =  [   radius   =   10  ,     pos  = [ x  =14   , y   =7  , z   = 90  ]      ]    object2 =  [   data          = [ vel = [x = 34,   y = 12, z=-90],   x  =14   , y   =7  , z   = 90  ], mydata=[text=bonjour, angle=34deg]      ]
    // "     hdr  draw-line   \"x1\"     =            3 y1                     =   -12   x2  =             2 y2       =4              "
    



    auto command = parse_command("hdr    draw   object =  [   radius   =   10  ,     pos  = [ x  =14   , y   =7  , z   = 90  ]      ]    object2 =  [   data          = [ vel = [x = 34,   y = 12, z=-90],   x  =14   , y   =7  , z   = 90  ], mydata=[text=bonjour, angle=34deg]      ]");

    for (const auto& param : command)
    {
        std::cout << "param => " << std::get<0>(param) << " | arg => " << std::get<1>(param) << std::endl;
    }

    /*
    std::string command;
    std::getline(std::cin, command);

    std::cout << normalize(command) << std::endl;
    auto norm = partition(normalize(command));

    std::string commandNamespace = norm[0];
    std::string commandName = norm[1];

    for (const auto& assignment : norm)
    {
        auto param_value = parse_assignment(assignment);

        std::cout << "param:" << std::get<0>(param_value) << " | arg:" << std::get<1>(param_value) << std::endl;
    }
    */
    //std::cout << trim_quotes("\"value\"") << std::endl;
    std::cout << "Allocated : " << allocated << std::endl;
    //auto p = parse_assignment("\"x\"=\"37\"");
    //std::cout << "param:" << std::get<0>(p) << std::endl;
    //std::cout << "arg:" << std::get<1>(p) << std::endl;
}
