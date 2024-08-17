#pragma once

#include "../interfaces/interface_res_collector.h"

#include <map>
#include <mutex>

// Коллекция пар id сессии и результат выполнения запроса.
using result_map = std::map<int, RequestResult>;

class ResultCollector final : public IResCollector
{
public:
	void add_result(const int session_id, const RequestResult& result) override;

	RequestResult get_result(const int session_id) override;

	void del_result(const int session_id) override;

	bool empty() override;

	void clear() override;

private:
	result_map results;

	mutable std::mutex data_mtx;
};
