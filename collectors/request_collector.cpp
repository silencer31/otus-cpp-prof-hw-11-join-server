#include "request_collector.h"

void RequestCollector::add_request(int session_id, const DatabaseRequest& request) override
{
	requests.push(std::make_pair<int, DatabaseRequest>(session_id, request));
}