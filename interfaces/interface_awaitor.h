#pragma once

#include <memory>

/**
* @brief Класс интерфейс для реагирования на выполненный запрос.
*/
class IAwaitor
{
public:
	virtual ~IAwaitor() = default;

	/**
	* @brief Обработать результат выполнения запроса к базе.
	*/
	virtual void handle_request_result() = 0;

};

using awaitor_shared = std::shared_ptr<IAwaitor>;