#pragma once

#include "../collectors/request_collector.h"
#include "../collectors/result_collector.h"
#include "../notifier/session_notifier.h"
#include "sql/sql_manager.h"

#include <atomic>
#include <thread>
#include <condition_variable>

using req_coll_shared = std::shared_ptr<IReqCollector>;
using res_coll_shared = std::shared_ptr<IResCollector>;

/****
* @brief Класс реализует отправку запросов к базе данных.
* 
*
*/

class DatabaseManager
{
public:
	DatabaseManager() = delete;

	explicit DatabaseManager(const notifier_shared& ntf_ptr,
		const req_coll_shared& rqcl_ptr,
		const res_coll_shared& rscl_ptr,
		const std::string& db_name)
		: notifier_ptr(ntf_ptr)
		, req_coll_ptr(rqcl_ptr)
		, res_coll_ptr(rscl_ptr)
		, sql_mngr_ptr(std::make_unique<SqlManager>(db_name.c_str()))
		, database_path(db_name)
		, requests_thread(&DatabaseManager::worker_thread, this)
	{}

	~DatabaseManager() {
		if (requests_thread.joinable()) {
			requests_thread.join();
		}
		
		//std::cout << "Database Manager - Destructor" << std::endl;
	}

	/**
	* Подключение к базе данных.
	*/
	bool connect_database();

	/**
	* Узнаём о появлении нового запроса в очереди запросов.
	*/
	void new_request_notify();

	void stop_process() {
		done = true;
		cond_var.notify_one();
	}

	bool active() const {
		return thread_active;
	}

private: // Methods

	/**
	* Добавить данные к базе.
	*/
	bool insert_data(const std::string& table, const int& id_val, const std::string& name_val);

	/**
	* Узнать, есть ли уже строка с таким id в указанной таблице.
	*/
	bool id_present(const std::string& table, const int& id);

	/**
	* Получить вектор строк из столбца name для указанных id.
	*/
	void get_names_by_ids(const std::string& table, const vector_int& ids, vector_str& names);

	/**
	* Получить вектор из id, которые есть в обеих таблицах.
	*/
	void get_equal_ids(vector_int& id_values);

	/**
	* Получить вектор из id, которые есть в первой таблице, но отсутствуют во второй.
	*/
	void get_unique_ids(const std::string& tab_1, const std::string& tab_2, vector_int& id_values);

	/**
	* Выполнить запрос к базе данных.
	*/
	RequestResult commit_db_request(const DatabaseRequest& db_request);

	/**
	* Выполнить поиск строк с одинаковым id.
	*/
	RequestResult commit_intersection();

	/**
	* Выполнить поиск строк с уникальными id.
	*/
	RequestResult commit_difference();

	/**
	* Метод, работающий в отдельном потоке.
	*/
	void worker_thread();

private:
	const notifier_shared notifier_ptr;
	const req_coll_shared req_coll_ptr;
	const res_coll_shared res_coll_ptr;
	const sql_mngr_unique sql_mngr_ptr;

	const std::string database_path;

	std::atomic<bool> done{ false }; // Больше нет данных для обработки.
	bool thread_active{ false };

	std::thread requests_thread;

	std::mutex wait_mutex; // Мьютекс для ожидания появления нового запроса в коллекции с запросами.
	std::condition_variable cond_var; // Для нотификации появления нового запроса в коллекции с запросами.
};

using db_manager_shared = std::shared_ptr<DatabaseManager>;