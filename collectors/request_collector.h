#pragma once

#include "../interfaces/interface_req_collector.h"

#include <mutex>

// ��������� ��� id ������ � ������ � ���� ������.
using request_queue = std::queue<session_request>;

/**
* @brief ����� ��� ����� �������� � ���� ������ �� ��������.
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