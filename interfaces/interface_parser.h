#pragma once

#include "../request/database_request.h"

/**
* @brief Класс интерфейс для преобразования строки в сформированный запрос к базе данных.
*/
class IParser
{
public:
	virtual ~IParser() = default;

	/**
	* Анализ полученной команды.
	* @param command команда, которую нужно распарсить.
	*/
	virtual DatabaseRequest parse_command(const std::string&) = 0;

};