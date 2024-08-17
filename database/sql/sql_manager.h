#pragma once

#include <vector>
#include <string>
#include <memory>

#include <sqlite3.h>

using vector_str = std::vector<std::string>;
using vector_int = std::vector<int>;

/**
* @brief Класс - хранилище данных. Реализует методы, позволяющие работать непосредственно с базой.
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
	* Создать соединение с БД.
	*/
	bool init_database(bool db_file_exists);

	/**
	* Обработка запроса к базе данных.
	* @param request запрос к базе данных
	*/
	bool handle_request(const std::string& request);

	/**
	* Выполнить запрос к базе данных с возвратом ошибки.
	* @param request запрос к базе данных
	* @param err_ptr параметр для получения ошибки выполнения запроса
	*/
	bool execute_db_request(const std::string& request, char** err_ptr);

	/**
	* Создать таблицу в базе.
	* @param table имя таблицы.
	* @param columns столбцы таблицы.
	*/
	bool create_table(const std::string& table, const std::string& columns);

	/**
	* Добавить в таблицу новую строку со значениями для всех столбцов.
	* @param table имя таблицы
	* @param values строка с набором значений для всех столбцов
	*/
	bool insert_row(const std::string& table, const std::string& values);

	/**
	* Узнать, есть ли переданное числовое значение в указанном столбце таблицы.
	*
	*/
	bool get_int_value_presence(const std::string& table, const std::string& col, const int& find_value);

	/**
	* Получить все числа из указанного столбца в виде вектора чисел.
	*
	*/
	int get_column_values_int(const std::string& table, const std::string& col, vector_int& values);

	/**
	* Ищем строку, содержащую переданное значение в указанном столбце.
	* Если находим, возвращаем значение-текст из той же строки, но из второго указанного столбца.
	* @param table имя таблицы
	* @param find_col искомый столбец
	* @param target_col целевой столбец
	* @param find_value искомое значение - условие
	* @param get_value параметр для получения строкового значения
	*/
	bool get_corresp_col_value_txt(const std::string& table,
		const std::string& find_col, const std::string& target_col,
		const int& find_value, std::string& get_value);

	/**
	* Очистка указанной таблицы.
	*/
	bool truncate_table(const std::string& table);

	

private:
	sqlite3* handle;
	sqlite3_stmt* stmt;

	const char* db_path; // Путь к файлу с базой данных.

	bool base_opened; // Флаг успешного открытия базы данных.	
};

using sql_mngr_unique = std::unique_ptr<SqlManager>;
