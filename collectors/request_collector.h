#pragma once

#include "../interfaces/interface_req_collector.h"

#include <queue>
#include <mutex>

// Коллекция пар id сессии и запрос к базе данных.
using request_queue = std::queue<session_request>;

class RequestCollector final : public IReqCollector
{
public:
	void add_request(int session_id, const DatabaseRequest& request) override;

    session_request front() override;

    void pop() override;

    std::size_t size() override;

    bool empty() override;

private:
	request_queue requests;

	mutable std::mutex queue_mtx;
};