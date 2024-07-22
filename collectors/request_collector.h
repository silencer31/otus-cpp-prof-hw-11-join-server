#pragma once

#include "../interfaces/interface_req_collector.h"

#include <queue>

// Коллекция пар id сессии и запрос к базе данных.
using request_queue = std::queue<std::pair<int, DatabaseRequest>>;

class RequestCollector final : public IReqCollector
{
public:
	void add_request(int session_id, const DatabaseRequest& request) override;

private:
	request_queue requests;
};