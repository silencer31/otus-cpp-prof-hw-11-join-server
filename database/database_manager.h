#pragma once

#include "../collectors/request_collector.h"
#include "../collectors/result_collector.h"
#include "../notifier/session_notifier.h"

#include "sqlite3.h"

#include <memory>

/****
* @brief Класс реализует отправку запросов к базе данных.
*/

using req_coll_shared = std::shared_ptr<IReqCollector>;
using res_coll_shared = std::shared_ptr<IResCollector>;

class DatabaseManager
{
public:
	DatabaseManager() = delete;

	explicit DatabaseManager(const notifier_shared& ntf_ptr,
		const req_coll_shared& rqcl_ptr,
		const res_coll_shared& rscl_ptr)
		: notifier_ptr(ntf_ptr)
		, req_coll_ptr(rqcl_ptr)
		, res_coll_ptr(rscl_ptr)
	{}

private:
	const notifier_shared notifier_ptr;
	const req_coll_shared req_coll_ptr;
	const res_coll_shared res_coll_ptr;
};

using db_manager_shared = std::shared_ptr<DatabaseManager>;