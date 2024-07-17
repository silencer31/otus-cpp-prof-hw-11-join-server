#pragma once

#include "../request/database_request.h"

/**
* @brief ����� ������� �������� � ���� ������.
*/
class IReqCollector
{
public:
	virtual ~IReqCollector() = default;

	/**
	* ���� ����������� ������� � ���� ��� ����������� ����������.
	* @param id ������ ����������� �������.
	* @param request ������ � ����.
	*/
	virtual void collect_request(int , const DatabaseRequest&) = 0;

};