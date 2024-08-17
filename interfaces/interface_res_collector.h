#pragma once

#include "../request/request_result.h"

/**
* @brief ����� ������� ����������� ���������� �������� � ���� ������.
*/
class IResCollector 
{
public:
	virtual ~IResCollector() = default;

	/**
	* ������ ���������� ���������� ������� � ����.
	* @param id ������ ����������� �������.
	* @param req_result ��������� ���������� ������� � ����.
	*/
	virtual void add_result(const int, const RequestResult&) = 0;

	virtual RequestResult get_result(const int) = 0;

	virtual void del_result(const int) = 0;

	virtual bool empty() = 0;

	virtual void clear() = 0;
};