#include "result_collector.h"

void ResultCollector::add_result(const int session_id, const RequestResult& result)
{
    std::lock_guard<std::mutex> guard(data_mtx);

    if (results.find(session_id) == results.end()) {
        //const 
        results[session_id] = std::queue<RequestResult>();
    }

    results[session_id].push(result);
}

RequestResult ResultCollector::front_result(const int session_id)
{
    std::lock_guard<std::mutex> guard(data_mtx);
    
    if (results.find(session_id) == results.end()) {
        return RequestResult(ResultType::UNKNOWN);
    }
    
    return results[session_id].front();
}

void ResultCollector::pop_result(const int session_id)
{
    std::lock_guard<std::mutex> guard(data_mtx);
    
    if (results.find(session_id) == results.end()) {
        return;
    }

    results[session_id].pop();
}

bool ResultCollector::has_result(const int session_id)
{
    std::lock_guard<std::mutex> guard(data_mtx);

    if (results.find(session_id) == results.end()) {
        return false;
    }

    return !results[session_id].empty();
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