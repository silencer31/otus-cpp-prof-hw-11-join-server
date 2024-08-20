#pragma once

#include "../interfaces/interface_req_collector.h"

#include <mutex>

// Коллекция пар id сессии и запрос к базе данных.
using request_queue = std::queue<session_request>;

/**
* @brief Класс для сбора запросов к базе данных от клиентов.
*/
class RequestCollector final : public IReqCollector
{
public:
    void add_requests(int session_id, std::queue<DatabaseRequest>& new_requests) override;

    session_request front() override;

    void pop() override;

    std::size_t size() override;

    bool empty() override;

private:
	request_queue requests;

	mutable std::mutex queue_mtx;
};