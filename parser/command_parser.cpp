#include "command_parser.h"

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>

#include <iostream>

DatabaseRequest CommandParser::parse_command(const std::string& command)
{
	// Разделяем полученные данные через пробел.
	std::vector<std::string> strings;
	boost::split(strings, command, boost::is_any_of(" "));

	// Не удалось разделить на строки через пробел.
	if (strings.empty()) {
		strings.reserve(1);
		strings.push_back(command);
	}

	// 
	if (strings.size() > 4) {
		return DatabaseRequest("command contains too many arguments");
	}

	// INSERT - добавление данных в таблицу.
	if (strings.at(0) == "INSERT") {
		if (strings.size() != 4) {
			return DatabaseRequest("unsupported request format");
		}

		// Имя таблицы.
		DataTable table_name;

		if (strings.at(1) == "A") {
			table_name = DataTable::A;
		}
		else if (strings.at(1) == "B") {
			table_name = DataTable::B;
		}
		else {
			return DatabaseRequest("incorrect table name");
		}

		// id
		int id_value;
		std::size_t pos{};

		try {
			id_value = std::stoi(strings.at(2), &pos);
		}
		catch (std::invalid_argument const& ex) {
			return DatabaseRequest(ex.what());
		}
		catch (std::out_of_range const& ex) {
			return DatabaseRequest(ex.what());
		}

		return DatabaseRequest(RequestType::INSERT, table_name, id_value, strings.at(3));
	}

	// TRUNCATE - очистка указанной таблицы.
	if (strings.at(0) == "TRUNCATE") {
		if (strings.size() != 2) {
			return DatabaseRequest("unsupported request format");
		}

		if (strings.at(1) == "A") {
			return DatabaseRequest(RequestType::TRUNCATE, DataTable::A);
		}

		if (strings.at(1) == "B") {
			return DatabaseRequest(RequestType::TRUNCATE, DataTable::B);
		}

		return DatabaseRequest("incorrect table name");
	}

	// INTERSECTION - пересечение множеств.
	if (strings.at(0) == "INTERSECTION") {
		return DatabaseRequest(RequestType::INTERSECTION);
	}

	// SYMMETRIC_DIFFERENCE - симметрическая разница множеств.
	if (strings.at(0) == "SYMMETRIC_DIFFERENCE") {
		return DatabaseRequest(RequestType::DIFFERENCE);
	}

	return DatabaseRequest("unknown request type");
}