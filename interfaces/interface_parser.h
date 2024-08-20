#pragma once

#include "../request/database_request.h"

/**
* @brief ����� ��������� ��� �������������� ������ � �������������� ������ � ���� ������.
*/
class IParser
{
public:
	virtual ~IParser() = default;

	/**
	* ������ ���������� �������.
	* @param command �������, ������� ����� ����������.
	*/
	virtual DatabaseRequest parse_command(const std::string&) = 0;

};