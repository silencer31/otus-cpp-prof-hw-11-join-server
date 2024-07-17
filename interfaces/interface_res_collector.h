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
	virtual void add_request_result(int , const RequestResult&) = 0;

};