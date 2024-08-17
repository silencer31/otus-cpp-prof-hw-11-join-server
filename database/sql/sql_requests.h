#pragma once

// Создание таблицы A.
const char* create_a_table = "CREATE TABLE A ("
"id SMALLINT PRIMARY KEY,"
"name CHAR(16) NOT NULL"
");";

// Создание таблицы B.
const char* create_b_table = "CREATE TABLE B ("
"id SMALLINT PRIMARY KEY,"
"name CHAR(16) NOT NULL"
");";

// Столбцы таблицы.
const char* table_columns = "(id SMALLINT PRIMARY KEY,name CHAR(16) NOT NULL)";

// Получение содержимого таблицы A
const char* select_from_a = "SELECT * FROM A";

// Получение содержимого таблицы B
const char* select_from_b = "SELECT * FROM B";




