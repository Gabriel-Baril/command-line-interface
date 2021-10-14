#pragma once
#include <iostream>
#include <string>
#include <map>
#include <functional>

using KeyCommandType = std::string;
using ValueCommandType = std::string;
using KeyVal = std::map<KeyCommandType, ValueCommandType>;
using CommandInvokerRetType = bool;
using CommandInvokerFunc = std::function<CommandInvokerRetType(const KeyVal&)>;

class DrawCircleCommand
{
public:
	DrawCircleCommand() = delete;

	static bool invoker(const std::map<std::string, std::string>& key_val)
	{
		float x = stof(key_val.at("x"));
		float y = stof(key_val.at("y"));
		float radius = stof(key_val.at("radius"));
		std::cout << "Draw Circle Command at x:" << x << "|y:" << y << "|radius:" << radius << '\n';
		return true;
	}

	static std::string_view get_command_name()
	{
		return "draw-circle";
	}
private:
	//static CommandTemplate commandTemplate;
};

