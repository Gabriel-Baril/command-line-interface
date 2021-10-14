#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include "CommandTemplate.h"
#include "CommandLibrary.h"
#include "command_util.h"

#include "DrawLineCommand.h"
#include "DrawCircleCommand.h"

struct MemoryProfiler
{
    uint32_t allocated = 00;
} s_memoryProfiler;

void* operator new(unsigned int size)
{
    //std::cout << "New operator overloading : " << size << std::endl;
    s_memoryProfiler.allocated += size;
    void* p = malloc(size);
    return p;
}

CommandLibrary commandLibrary;

static bool testFun(const std::map<std::string, std::string>& key_val)
{
    std::cout << "Test fun called" << '\n';
    return true;
}

void init_command_library()
{
    commandLibrary.add_command<DrawLineCommand>();
    commandLibrary.add_command<DrawCircleCommand>();
    commandLibrary.add_command("test-fun", &testFun);
}


// draw-line y1=23 x1=89 x2=56 y2=90
// draw-circle y=-7 x=11 radius=90
int main()
{
    std::string command;
    std::getline(std::cin, command);

    init_command_library();
    process_command(command.c_str(), commandLibrary);



    std::cout << s_memoryProfiler.allocated << std::endl;
    /*
    std::cout << commandLibrary["draw-line"]["x1"].paramName << '\n';
    
    for (const auto& param : commandLibrary["draw-line"])
    {
        std::cout << param.paramName << "   " << (int)param.paramType << "   " << param.required << '\n';
    }

    std::string command;

    std::getline(std::cin, command);

    auto params = split_command_param(command);
    for (auto param : params)
    {
        std::cout << param << std::endl;
    }

    std::cout << "Command : " << command;
    */
}

