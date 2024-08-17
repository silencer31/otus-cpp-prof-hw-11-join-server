#include "result_collector.h"

void ResultCollector::add_result(const int session_id, const RequestResult& result)
{
	std::lock_guard<std::mutex> guard(data_mtx);
    results[session_id] = result;
}

RequestResult ResultCollector::get_result(const int session_id)
{
    std::lock_guard<std::mutex> guard(data_mtx);

    return results.contains(session_id) ? results[session_id] : RequestResult(ResultType::ERR);
}

void ResultCollector::del_result(const int session_id)
{
    std::lock_guard<std::mutex> guard(data_mtx);
    results.erase(session_id);
}


bool ResultCollector::empty()
{
    std::lock_guard<std::mutex> guard(data_mtx);
    return results.empty();
}

void ResultCollector::clear()
{
    std::lock_guard<std::mutex> guard(data_mtx);
    return results.clear();
}