#pragma once

#include "../request/database_request.h"
#include <queue>

// ���� �� id ������ � ������� � ���� ������.
using session_request = std::pair<int, DatabaseRequest>;

/**
* @brief ����� ��������� - ������� �������� � ���� ������.
*/
class IReqCollector
{
public:
	virtual ~IReqCollector() = default;

	/**
	* �������� � ������� ��������� �������� � ���� ������ ��� ����������� ����������.
	* @param id ������ ����������� �������.
	* @param requests ������� � ����.
	*/
	virtual void add_requests(int, std::queue<DatabaseRequest>&) = 0;

	virtual session_request front() = 0;

	virtual void pop() = 0;

	virtual std::size_t size() = 0;

	virtual bool empty() = 0;
};