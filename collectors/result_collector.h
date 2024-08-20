#pragma once

#include "../interfaces/interface_res_collector.h"

#include <map>
#include <queue>
#include <mutex>

// Коллекция id сессий и результатов выполнения запросов.
using result_map = std::map<int, std::queue<RequestResult>>;

/**
* @brief Класс для сбора результатов выполнения запросов к базе данных.
*/
class ResultCollector final : public IResCollector
{
public:
	void add_result(const int session_id, const RequestResult& result) override;

	RequestResult front_result(const int session_id) override;

	void pop_result(const int session_id) override;

	bool has_result(const int session_id) override;

	bool empty() override;

	void clear() override;

private:
	result_map results;

	mutable std::mutex data_mtx;
};
