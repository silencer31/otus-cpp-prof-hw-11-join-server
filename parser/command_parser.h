#pragma once

#include "../interfaces/interface_parser.h"

#include <memory>

/**
* @brief Класс реализует преобразование строки в сформированный запрос к базе данных.
*/
class CommandParser final : public IParser
{
public:
	//CommandParser() = default;
	~CommandParser() = default;

	/**
	* Анализ полученной команды.
	* @param command команда, которую нужно распарсить.
	*/
	DatabaseRequest parse_command(const std::string& command) override;
};

using parser_shared = std::shared_ptr<IParser>;