#include "result_collector.h"

void ResultCollector::add_result(int session_id, const RequestResult& result) override
{
	results.push( std::make_pair<int, RequestResult>(session_id, result) );
}