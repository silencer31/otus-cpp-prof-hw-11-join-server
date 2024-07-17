#pragma once

#include "../interfaces/interface_res_collector.h"

#include <queue>

using result_queue = std::queue<std::pair<int, RequestResult>>;

class ResultCollector final : public IResCollector
{
public:
	void void add_result(int session_id, const RequestResult& result) override;

private:
	result_queue results;
};
