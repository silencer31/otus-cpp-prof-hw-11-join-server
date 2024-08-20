#pragma once

#include "../request/request_result.h"

/**
* @brief ����� ��������� - ������� ����������� ���������� �������� � ���� ������.
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

	virtual RequestResult front_result(const int) = 0;

	virtual void pop_result(const int) = 0;

	virtual bool has_result(const int) = 0;

	virtual bool empty() = 0;

	virtual void clear() = 0;
};