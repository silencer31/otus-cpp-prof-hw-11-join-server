#include "command_parser.h"

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>

#include <iostream>

DatabaseRequest CommandParser::parse_command(const std::string& command, std::string& error)
{
	// ��������� ���������� ������ ����� ������.
	std::vector<std::string> strings;
	boost::split(strings, command, boost::is_any_of(" "));

	// �� ������� ��������� �� ������ ����� ������.
	if (strings.empty()) {
		strings.reserve(1);
		strings.push_back(command);
	}

	// 
	if (strings.size() > 4) {
		error = "command contains too many arguments";
		return DatabaseRequest();
	}

	// INSERT - ���������� ������ � �������.
	if (strings.at(0) == "INSERT") {
		if (strings.size() != 4) {
			error = "unsupported request format";
			return DatabaseRequest();
		}

		// ��� �������.
		DataTable table_name;

		if (strings.at(1) == "A") {
			table_name = DataTable::A;
		}
		else if (strings.at(1) == "B") {
			table_name = DataTable::B;
		}
		else {
			error = "incorrect table name";
			return DatabaseRequest();
		}

		// id
		int id_value;
		std::size_t pos{};

		try {
			id_value = std::stoi(strings.at(2), &pos);
		}
		catch (std::invalid_argument const& ex) {
			error = ex.what();
			return DatabaseRequest();
		}
		catch (std::out_of_range const& ex) {
			error = ex.what();
			return DatabaseRequest();
		}

		return DatabaseRequest(RequestType::INSERT, table_name, id_value, strings.at(3));
	}

	// TRUNCATE - ������� ��������� �������.
	if (strings.at(0) == "TRUNCATE") {
		if (strings.size() != 2) {
			error = "unsupported request format";
			return DatabaseRequest();
		}

		if (strings.at(1) == "A") {
			return DatabaseRequest(RequestType::TRUNCATE, DataTable::A);
		}

		if (strings.at(1) == "B") {
			return DatabaseRequest(RequestType::TRUNCATE, DataTable::B);
		}

		error = "incorrect table name";
		return DatabaseRequest();
	}

	// INTERSECTION - ����������� ��������.
	if (strings.at(0) == "INTERSECTION") {
		return DatabaseRequest(RequestType::INTERSECTION);
	}

	// SYMMETRIC_DIFFERENCE - �������������� ������� ��������.
	if (strings.at(0) == "SYMMETRIC_DIFFERENCE") {
		return DatabaseRequest(RequestType::DIFFERENCE);
	}

	error = "unknown request type";
	return DatabaseRequest();
}