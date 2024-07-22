#pragma once

#include "../interfaces/interface_parser.h"

class CommandParser final : public IParser
{
public:
	//CommandParser() = default;
	~CommandParser() = default;

	/**
	* Анализ полученной команды.
	* @param command команда, которую нужно распарсить.
	* @param error текст ошибки анализа
	*/
	DatabaseRequest parse_command(const std::string& command, std::string& error) override;
};

using parser_shared = std::shared_ptr<IParser>;