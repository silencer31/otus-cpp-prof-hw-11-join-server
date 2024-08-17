#pragma once

#include <string>

enum class RequestType
{
	UNKNOWN,
	INSERT,			// Добавление данных в таблицу.
	TRUNCATE,		// Очистка указанной таблицы.
	INTERSECTION,	// Пересечение множеств.
	DIFFERENCE		// Симметрическая разница множеств.
};

enum class DataTable
{
	UNKNOWN,
	A,
	B
};

struct DatabaseRequest
{
//public:
	DatabaseRequest()
		: request_type(RequestType::UNKNOWN)
		, data_table(DataTable::UNKNOWN)
		, id(-1)
		, name("")
	{}
	
	explicit DatabaseRequest(RequestType rtype)
		: request_type(rtype)
		, data_table(DataTable::UNKNOWN)
		, id(-1)
		, name("")
	{}
	
	explicit DatabaseRequest(RequestType rtype, DataTable table)
		: request_type(rtype)
		, data_table(table)
		, id(-1)
		, name("")
	{}
	
	explicit DatabaseRequest(RequestType rtype, DataTable table, int id_value, const std::string& txt)
		: request_type(rtype)
		, data_table(table)
		, id(id_value)
		, name(txt)
	{}
	
	~DatabaseRequest() = default;
	/*
	RequestType get_type() const {
		return request_type;
	}

	DataTable get_table() const {
		return data_table;
	}

	int8_t get_id() const {
		return id;
	}

	std::string get_name() const {
		return name;
	}
	*/
//private:
	const RequestType request_type;
	const DataTable data_table;
	const int id;
	const std::string name;
};