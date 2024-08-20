#pragma once

#include "../request/database_request.h"
#include <queue>

// Пара из id сессии и запроса к базе данных.
using session_request = std::pair<int, DatabaseRequest>;

/**
* @brief Класс интерфейс - сборщик запросов к базе данных.
*/
class IReqCollector
{
public:
	virtual ~IReqCollector() = default;

	/**
	* Добавить в очередь несколько запросов к базе данных для дальнейшего выполнения.
	* @param id сессии отправителя запроса.
	* @param requests запросы к базе.
	*/
	virtual void add_requests(int, std::queue<DatabaseRequest>&) = 0;

	virtual session_request front() = 0;

	virtual void pop() = 0;

	virtual std::size_t size() = 0;

	virtual bool empty() = 0;
};