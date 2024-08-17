#pragma once

#include "../request/database_request.h"

// Пара из id сессии и запроса к базе данных.
using session_request = std::pair<int, DatabaseRequest>;

/**
* @brief Класс сборщик запросов к базе данных.
*/
class IReqCollector
{
public:
	virtual ~IReqCollector() = default;

	/**
	* Приём полученного запроса к базе для дальнейшего выполнения.
	* @param id сессии отправителя запроса.
	* @param request запрос к базе.
	*/
	virtual void add_request(int , const DatabaseRequest&) = 0;

	virtual session_request front() = 0;

	virtual void pop() = 0;

	virtual std::size_t size() = 0;

	virtual bool empty() = 0;
};