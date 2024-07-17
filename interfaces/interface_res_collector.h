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
	virtual void add_request_result(int , const RequestResult&) = 0;

};