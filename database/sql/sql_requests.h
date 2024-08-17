#pragma once

// �������� ������� A.
const char* create_a_table = "CREATE TABLE A ("
"id SMALLINT PRIMARY KEY,"
"name CHAR(16) NOT NULL"
");";

// �������� ������� B.
const char* create_b_table = "CREATE TABLE B ("
"id SMALLINT PRIMARY KEY,"
"name CHAR(16) NOT NULL"
");";

// ������� �������.
const char* table_columns = "(id SMALLINT PRIMARY KEY,name CHAR(16) NOT NULL)";

// ��������� ����������� ������� A
const char* select_from_a = "SELECT * FROM A";

// ��������� ����������� ������� B
const char* select_from_b = "SELECT * FROM B";




