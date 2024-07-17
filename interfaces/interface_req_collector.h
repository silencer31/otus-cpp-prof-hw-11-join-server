#pragma once

#include "../request/database_request.h"

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
	virtual void collect_request(int , const DatabaseRequest&) = 0;

};