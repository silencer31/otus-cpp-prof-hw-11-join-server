#pragma once

#include "interface_awaitor.h"

/**
* @brief Класс интерфейс для работы с сессиями, ожидающими результат выполнения запроса.
*/
class INotifier
{
public:
	virtual ~INotifier() = default;

	/**
	* .
	* @param .
	*/
	virtual void add_awaitor(int , const awaitor_shared& ) = 0;

	virtual void rem_awaitor(int ) = 0;

	virtual void clear() = 0;

	virtual void notify(int ) = 0;
};