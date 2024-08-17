#include "sql_manager.h"

#include "sql_requests.h"

#include <iostream>


bool SqlManager::init_database(bool db_file_exists)
{
    int result = (db_file_exists
        ? sqlite3_open_v2(db_path, &handle, SQLITE_OPEN_READWRITE, NULL)
        : sqlite3_open_v2(db_path, &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)
        );

    base_opened = (SQLITE_OK == result);

    if ( !base_opened)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(handle) << std::endl;
        sqlite3_close(handle);
        return false;
    }

    std::cout << db_path << " database opened successfully!" << std::endl;

    if (db_file_exists) {
        return true;
    }

    // ���� ����� � ����� ��� �� ����, ������ ������� A � B.
    if (!create_table("A", table_columns)) {
        sqlite3_close(handle);
        return false;
    }

    if (!create_table("B", table_columns)) {
        sqlite3_close(handle);
        return false;
    }

    return true;
}

bool SqlManager::handle_request(const std::string& request)
{
    auto exec_callback = [](void*, int columns, char** data, char** names) -> int {
        for (int i = 0; i < columns; ++i) {
            std::cout << names[i] << " = " << (data[i] ? data[i] : "NULL") << std::endl;
        }

        std::cout << std::endl;

        return 0;
        };

    char* errmsg;

    int rc = sqlite3_exec(handle, request.c_str(), exec_callback, 0, &errmsg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Can't execute query: " << errmsg << std::endl;
        sqlite3_free(errmsg);
        return false;
    }

    return true;
}

// ��������� ������ � ���� ������ � ��������� ������.
bool SqlManager::execute_db_request(const std::string& request, char** err_ptr)
{
    auto exec_callback = [](void*, int, char**, char**) -> int {
        return 0;
        };

    int result = sqlite3_exec(handle, request.c_str(), exec_callback, 0, &(*err_ptr));

    return (result == SQLITE_OK);
}

// ������� ������� � ����.
bool SqlManager::create_table(const std::string& table, const std::string& columns)
{
    const std::string request = "CREATE TABLE " + table
        + " " + columns + ";";

    char* errmsg = nullptr;

    if (execute_db_request(request, &errmsg)) {
        return true;
    }

    std::cout << "Table " << table << " CREATE TABLE error: " << errmsg << std::endl;

    if (errmsg) {
        sqlite3_free(errmsg);
    }

    return false;
}

// �������� � ������� ����� ������ �� ���������� ��� ���� ��������
bool SqlManager::insert_row(const std::string& table, const std::string& values)
{
    const std::string request = "INSERT INTO " + table
        + " VALUES " + values + ";";

    char* errmsg = nullptr;

    if (execute_db_request(request, &errmsg)) {
        return true;
    }

    std::cout << "Table " << table << " INSERT INTO error: " << errmsg << std::endl;

    if (errmsg) {
        sqlite3_free(errmsg);
    }

    return false;
}

// ������, ���� �� ���������� �������� �������� � ��������� ������� �������.
bool SqlManager::get_int_value_presence(const std::string& table, const std::string& col, const int& find_value)
{
    const std::string request = "SELECT " + col + " FROM " + table;

    if (SQLITE_OK != sqlite3_prepare_v2(handle, request.c_str(), -1, &stmt, nullptr)) {
        std::cout << "sqlite prepare error " << sqlite3_errmsg(handle) << std::endl;
        return false;
    }

    // ���� ��������� �����.
    while (SQLITE_ROW == sqlite3_step(stmt)) {
        if (find_value == sqlite3_column_int(stmt, 0)) {
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);

    return false;
}

// �������� ��� ����� �� ���������� ������� � ���� ������� �����.
int SqlManager::get_column_values_int(const std::string& table, const std::string& col, vector_int& values)
{
    const std::string request = "SELECT " + col + " FROM " + table;

    if (SQLITE_OK != sqlite3_prepare_v2(handle, request.c_str(), -1, &stmt, nullptr)) {
        std::cout << "sqlite prepare error " << sqlite3_errmsg(handle) << std::endl;
        return 0;
    }

    int number = 0;

    while (SQLITE_ROW == sqlite3_step(stmt)) {
        values.push_back(sqlite3_column_int(stmt, 0));
        ++number;
    }

    sqlite3_finalize(stmt);

    return number;
}

// ���� ������, ���������� ���������� ��������-����� � ��������� �������.
// ���� �������, ���������� ��������-����� �� ��� �� ������, �� �� ������� ���������� �������.
bool SqlManager::get_corresp_col_value_txt(const std::string& table,
    const std::string& find_col, const std::string& target_col,
    const int& find_value, std::string& get_value)
{
    std::string result;

    const std::string request = "SELECT " + find_col + ", " + target_col + " FROM " + table;

    if (SQLITE_OK != sqlite3_prepare_v2(handle, request.c_str(), -1, &stmt, nullptr)) {
        std::cout << "sqlite prepare error " << sqlite3_errmsg(handle) << std::endl;
        return false;
    }

    while (SQLITE_ROW == sqlite3_step(stmt)) {
        if (find_value == sqlite3_column_int(stmt, 0)) {
            get_value = (char*)sqlite3_column_text(stmt, 1);
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);

    return false;
}



// ������� ��������� �������.
bool SqlManager::truncate_table(const std::string& table)
{
    const std::string request = "DELETE FROM " + table + ";";

    char* errmsg = nullptr;

    if (execute_db_request(request, &errmsg)) {
        if (execute_db_request("VACUUM", &errmsg)) {
            return true;
        }        
    }

    std::cout << "Table " << table << " DELETE TABLE error: " << errmsg << std::endl;

    if (errmsg) {
        sqlite3_free(errmsg);
    }

    return false;
}