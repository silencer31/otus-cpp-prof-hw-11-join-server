#pragma once

#include "sqlite3.h"
/**
* ¬ыполнение запроса к базе данных с выводом столбцов в консоль.
* @param handle св€зь с базой данных.
* @param request sql запрос к базе.
* @param table_name им€ базы данных.
*/
void execute_request_output_columns(sqlite3* handle, const char* request, const char* table_name);

/**
* ¬ыполнение запроса к базе данных с выводом в консоль.
* @param handle св€зь с базой данных.
* @param request sql запрос к базе.
* @param table_name им€ базы данных.
*/
void execute_request_output_data(sqlite3* handle, const char* request, const char* table_name);

/**
* ¬ыполнение запроса к базе данных.
* @param handle св€зь с базой данных.
* @param request sql запрос к базе.
* @param table_name им€ базы данных.
* @param create_flag создаЄтс€ ли нова€ таблица или обновл€етс€ существующа€.
*/
bool execute_request(sqlite3* handle, const char* request, const char* table_name, bool create_flag);

/**
* ¬ыполнение запроса к базе данных.
* @param handle св€зь с базой данных.
* @param request sql запрос к базе.
*/
bool execute_simple_request(sqlite3* handle, const char* request);