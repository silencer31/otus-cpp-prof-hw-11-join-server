#pragma once

#include <vector>
#include <string>
#include <memory>

#include <sqlite3.h>

using vector_str = std::vector<std::string>;
using vector_int = std::vector<int>;

/**
* @brief ����� - ��������� ������. ��������� ������, ����������� �������� ��������������� � �����.
*/
class SqlManager
{
public:
	SqlManager() = delete;

	explicit SqlManager(const char* file_path) :
		handle(nullptr),
		stmt(nullptr),
		db_path(file_path),
		base_opened(false)
	{}

	~SqlManager() {
		if (stmt != nullptr) {
			sqlite3_finalize(stmt);
		}

		if (base_opened) {
			sqlite3_close(handle);
		}
	}
	
	/**
	* ������� ���������� � ��.
	*/
	bool init_database(bool db_file_exists);

	/**
	* ��������� ������� � ���� ������.
	* @param request ������ � ���� ������
	*/
	bool handle_request(const std::string& request);

	/**
	* ��������� ������ � ���� ������ � ��������� ������.
	* @param request ������ � ���� ������
	* @param err_ptr �������� ��� ��������� ������ ���������� �������
	*/
	bool execute_db_request(const std::string& request, char** err_ptr);

	/**
	* ������� ������� � ����.
	* @param table ��� �������.
	* @param columns ������� �������.
	*/
	bool create_table(const std::string& table, const std::string& columns);

	/**
	* �������� � ������� ����� ������ �� ���������� ��� ���� ��������.
	* @param table ��� �������
	* @param values ������ � ������� �������� ��� ���� ��������
	*/
	bool insert_row(const std::string& table, const std::string& values);

	/**
	* ������, ���� �� ���������� �������� �������� � ��������� ������� �������.
	*
	*/
	bool get_int_value_presence(const std::string& table, const std::string& col, const int& find_value);

	/**
	* �������� ��� ����� �� ���������� ������� � ���� ������� �����.
	*
	*/
	int get_column_values_int(const std::string& table, const std::string& col, vector_int& values);

	/**
	* ���� ������, ���������� ���������� �������� � ��������� �������.
	* ���� �������, ���������� ��������-����� �� ��� �� ������, �� �� ������� ���������� �������.
	* @param table ��� �������
	* @param find_col ������� �������
	* @param target_col ������� �������
	* @param find_value ������� �������� - �������
	* @param get_value �������� ��� ��������� ���������� ��������
	*/
	bool get_corresp_col_value_txt(const std::string& table,
		const std::string& find_col, const std::string& target_col,
		const int& find_value, std::string& get_value);

	/**
	* ������� ��������� �������.
	*/
	bool truncate_table(const std::string& table);

	

private:
	sqlite3* handle;
	sqlite3_stmt* stmt;

	const char* db_path; // ���� � ����� � ����� ������.

	bool base_opened; // ���� ��������� �������� ���� ������.	
};

using sql_mngr_unique = std::unique_ptr<SqlManager>;
