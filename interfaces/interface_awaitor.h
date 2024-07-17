#pragma once

#include <memory>

/**
* @brief Класс
*/
class IAwaitor
{
public:
	virtual ~IAwaitor() = default;

	/**
	* .
	*/
	virtual void handle_request_result() = 0;

};

using awaitor_shared = std::shared_ptr<IAwaitor>;