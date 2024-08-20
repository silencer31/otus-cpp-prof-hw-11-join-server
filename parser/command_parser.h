#pragma once

#include "../interfaces/interface_parser.h"

#include <memory>

/**
* @brief ����� ��������� �������������� ������ � �������������� ������ � ���� ������.
*/
class CommandParser final : public IParser
{
public:
	//CommandParser() = default;
	~CommandParser() = default;

	/**
	* ������ ���������� �������.
	* @param command �������, ������� ����� ����������.
	*/
	DatabaseRequest parse_command(const std::string& command) override;
};

using parser_shared = std::shared_ptr<IParser>;