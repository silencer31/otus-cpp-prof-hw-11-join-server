#include "command_parser.h"

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>

DatabaseRequest CommandParser::parse_command(const std::string& command, std::string& error) override
{
	// –аздел€ем полученные данные через пробел.
	std::vector<std::string> strings;
	boost::split(strings, data_read, boost::is_any_of(" "));

	// 
	if (strings.empty()) {
		error = "request split by space error";
		return DatabaseRequest();
	}

	// 
	if (strings.size() > 4) {
		error = "command contains too many arguments";
		return DatabaseRequest();
	}


	if (strings.at(0) == "INSERT") {
		if (strings.size() != 4) {
			error = "unsupported request format";
			return DatabaseRequest();
		}


	}


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

		error = "unsupported table name";
		return DatabaseRequest();
	}


	if (strings.at(0) == "INTERSECTION") {
		return DatabaseRequest(RequestType::INTERSECTION);
	}


	if (strings.at(0) == "SYMMETRIC_DIFFERENCE") {
		return DatabaseRequest(RequestType::DIFFERENCE);
	}

	error = "unknown request type";
	return DatabaseRequest();
}