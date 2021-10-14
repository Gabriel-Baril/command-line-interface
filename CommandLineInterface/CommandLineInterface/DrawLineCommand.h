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

class DrawLineCommand
{
public:
	DrawLineCommand() = delete;

	static bool invoker(const KeyVal& key_val)
	{
		float x1 = stof(key_val.at("x1"));
		float y1 = stof(key_val.at("y1"));
		float x2 = stof(key_val.at("x2"));
		float y2 = stof(key_val.at("y2"));
		std::cout << "Draw Line Command at x1:" << x1 << "|y1:" << y1 << "|x2:" << x2 << "|y2:" << y2 << '\n';
		return true;
	}

	static std::string_view get_command_name()
	{
		return "draw-line";
	}
private:
	//static CommandTemplate commandTemplate;
};

