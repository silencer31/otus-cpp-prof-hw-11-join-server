#include "../server/join_server.h"

#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>

#include <iostream>

// ������ ���������� �������.
void ClientSession::handle_data_sent()
{
	// ��������� �������� ���������� � �������. 
	if (cur_req_result.res_type != ResultType::OK) {
		send_request_result();
		return;
	}

	// ���� ��� ����� �� ��������, ���������� ��.
	if (cur_req_result.res_strings.empty()) {
		cur_req_result.clear();
		reply_ok();
		return;
	}

	// ���������� ������ �� ��������� ����� � ����������.
	send_result_string();
}

// ������ �������� ������.
void ClientSession::handle_write_error()
{
	cur_req_result.clear();
	sending_result = false;
	do_read();
	return;
}

// ��������� ��������� ���������� �������.
void ClientSession::send_request_result()
{
	// ���� ����������� �� �������� ������ ���, ��������� ������� �������� �� ����������� ����� �����������.
	if (results_to_send.empty()) {
		cur_req_result.clear();
		sending_result = false;
		do_read();
		return;
	}
	
	cur_req_result = std::move(results_to_send.front());
	results_to_send.pop();

	// ���� �� ������ ��� ������� ��������� ������.
	if (cur_req_result.res_type == ResultType::ERR) {
		reply_error(cur_req_result.error_text);
		return;
	}

	// ������ ������� ��������.
	
	// ���� ��� ����� �� ��������, ���������� ��.
	if (cur_req_result.res_strings.empty()) {
		cur_req_result.clear();
		reply_ok();
		return;
	}
	
	// ���������� ������ �� ��������� ����� � ����������.
	send_result_string();
}

// ��������� ��������� ������ �� ���������� ���������� �������.
void ClientSession::send_result_string()
{
	// ���� ��������� ������ �� ��������� �����.
	const std::string res_str = std::move(cur_req_result.res_strings.front());
	
	cur_req_result.res_strings.pop();

	do_write(boost::str(boost::format("%1%\n") % res_str));
}

// ���������� ��������� ���������� ������� � ����.
void ClientSession::handle_request_result()
{
	results_to_send.push( std::move(res_coll_ptr->front_result(session_id)));

	res_coll_ptr->pop_result(session_id);

	if (sending_result) { return; }
	
	sending_result = true;
	
	send_request_result();	
}