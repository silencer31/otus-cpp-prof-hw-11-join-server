#pragma once

#include "../request/database_request.h"

/**
* @brief Класс
*/
class IParser
{
public:
	virtual ~IParser() = default;

	/**
	* Анализ полученной команды.
	* @param command команда, которую нужно распарсить.
	* @param error текст ошибки анализа
	*/
	virtual DatabaseRequest parse_command(const std::string&, std::string&) = 0;

};