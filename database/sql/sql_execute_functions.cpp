#include "sql_execute_functions.h"

#include <cstring>
#include <iostream>

#define COLUMN_WIDTH 28

void execute_request_output_columns(sqlite3* handle, const char* request, const char* table_name)
{
    auto exec_callback = [](void*, int columns, char** data, char** names) -> int {
        size_t len = 0;

        for (int i = 0; i < columns; ++i) {
            if (0 == strcmp(names[i], "name")) {
                std::cout << data[i];

                len = strlen(data[i]);
                break;
            }
        }

        for (size_t x = len; x < COLUMN_WIDTH; ++x) {
            std::cout << " ";
        }

        return 0;
    };

    char* errmsg = nullptr;

    if (SQLITE_OK != sqlite3_exec(handle, request, exec_callback, 0, &errmsg)) {
        std::cout << "An error occured while getting column names from table " << table_name << "!" << std::endl;
        std::cout << "Error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }

    std::cout << std::endl;
}

void execute_request_output_data(sqlite3* handle, const char* request, const char* table_name)
{
    auto exec_callback = [](void*, int columns, char** data, char**) -> int {
        for (int i = 0; i < columns; ++i) {
            std::cout << (data[i] ? data[i] : "NULL");

            for (size_t x = strlen(data[i] ? data[i] : "NULL"); x < COLUMN_WIDTH; ++x) {
                std::cout << " ";
            }
        }

        std::cout << std::endl;

        return 0;
    };

    char* errmsg = nullptr;

    if (SQLITE_OK != sqlite3_exec(handle, request, exec_callback, 0, &errmsg)) {
        std::cout << "An error occured while getting of content from table " << table_name << "!" << std::endl;
        std::cout << "Error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }

    std::cout << std::endl;
}

bool execute_request(sqlite3* handle, const char* request, const char* table_name, bool create_flag)
{
    auto exec_callback = [](void*, int, char**, char**) -> int {
        return 0;
    };

    char* errmsg = nullptr;

    if (SQLITE_OK == sqlite3_exec(handle, request, exec_callback, 0, &errmsg)) {
        std::cout << "DB table " << table_name << " successfully " << (create_flag ? "created" : "updated") << std::endl;
        return true;
    }

    std::cout << "An error occured while " << (create_flag ? "creation" : "updation") << " of DB table " << table_name << "!" << std::endl;
    std::cout << "Error: " << errmsg << std::endl;
    sqlite3_free(errmsg);

    return false;
}

bool execute_simple_request(sqlite3* handle, const char* request)
{
    auto exec_callback = [](void*, int, char**, char**) -> int {
        return 0;
    };

    char* errmsg = nullptr;

    if (SQLITE_OK == sqlite3_exec(handle, request, exec_callback, 0, &errmsg)) {
        std::cout << "OK" << std::endl;
        return true;
    }

    std::cout << "Error: " << errmsg << std::endl;
    sqlite3_free(errmsg);

    return false;
}
