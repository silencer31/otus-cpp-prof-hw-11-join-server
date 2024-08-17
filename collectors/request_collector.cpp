#include "request_collector.h"

void RequestCollector::add_request(int session_id, const DatabaseRequest& request)
{
	std::lock_guard<std::mutex> guard(queue_mtx);
    const session_request id_req_pair = std::make_pair(session_id, request);
	requests.push(id_req_pair);
}

session_request RequestCollector::front()
{
    std::lock_guard<std::mutex> guard(queue_mtx);
    return requests.front();
}

void RequestCollector::pop()
{
    std::lock_guard<std::mutex> guard(queue_mtx);
    requests.pop();
}

std::size_t RequestCollector::size()
{
    std::lock_guard<std::mutex> guard(queue_mtx);
    return requests.size();
}

bool RequestCollector::empty()
{
    std::lock_guard<std::mutex> guard(queue_mtx);
    return requests.empty();
}