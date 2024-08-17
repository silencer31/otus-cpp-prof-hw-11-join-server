#pragma once

#include "../request/request_result.h"

/**
* @brief Класс сборщик результатов выполнения запросов к базе данных.
*/
class IResCollector 
{
public:
	virtual ~IResCollector() = default;

	/**
	* Запись результата выполнения запроса к базе.
	* @param id сессии отправителя запроса.
	* @param req_result результат выполнения запроса к базе.
	*/
	virtual void add_result(const int, const RequestResult&) = 0;

	virtual RequestResult get_result(const int) = 0;

	virtual void del_result(const int) = 0;

	virtual bool empty() = 0;

	virtual void clear() = 0;
};