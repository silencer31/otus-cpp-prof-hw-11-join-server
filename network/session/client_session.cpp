#include "../server/join_server.h"

#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>

#include <iostream>

// 
void ClientSession::do_read()
{
	clear_data_read();

	auto self(shared_from_this());

	socket_.async_read_some( // � ������ �������� async_read_some
		boost::asio::buffer(data_read, MAX_LENGTH),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << " Session: " << session_id << ". Read data boost system error code: " << errcode.message() << std::endl;

				// �������� ������� � ������������� ���������� ������
				if (!shutdown_server_flag) {
					join_server_ptr->close_session(session_id);
				}

				return;
			}
			
			std::cout << "Session: " << session_id << ". Received: " << length << " bytes\n" << data_read << std::endl;

			// ����� �� ��������� ������.
			if (0 == strcmp(data_read, "exit")) {
				join_server_ptr->close_session(session_id);
				return;
			}

			// ����� �� ��������� ������.
			if (0 == strcmp(data_read, "shutdown")) {
				std::cout << "closing server..." << std::endl;
				//std::this_thread::sleep_for(std::chrono::seconds(1));
				shutdown_server_flag = true;
				join_server_ptr->shutdown_server(session_id);
				return;
			}

			// ��������� ������������ ������� �������.
			if ((length < 2) || (data_read[length-1] != '\n')) {
				reply_error("incorrect request");
				return;
			}

			// ��������� �������. 
			handle_request(length-1);
		}
	);
}

// 
void ClientSession::do_write(const std::string& answer)
{
	auto self(shared_from_this());

	prepare_data_send(answer);

	boost::asio::async_write( // �������� async_write, �������� �����.
		socket_, boost::asio::buffer(data_send, answer.length()),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << "Session: " << session_id << ". Write data boost system error: " << errcode.message() << std::endl;
			}
			else {
				std::cout << "Session: " << session_id << ". To send: " << length << " bytes\n" << data_send << "\n" << std::endl;
			}

			if (shutdown_session_flag) { // ���� �������� ������� �� ����������, ������ �� �������� ������ �� ����.
				return;
			}
		}
	);
}

// ���������� ������.
void ClientSession::shutdown()
{
	if (!socket_.is_open()) {
		return;
	}

	std::cout << "Session closing process started. id: " << session_id << std::endl;

	boost::system::error_code ignore;

	socket_.shutdown(
		boost::asio::ip::tcp::socket::shutdown_both,
		ignore
	);

	socket_.close(ignore);

	std::cout << "Session closing finished. id: " << session_id << std::endl;
}

// 
void ClientSession::handle_request_result()
{
	RequestResult result = res_coll_ptr->get_result(session_id);

	res_coll_ptr->del_result(session_id);

	// ���� �� ������ ��� ������� ��������� ������.
	if (result.res_type == ResultType::ERR) {
		reply_error(result.error_text);
		return;
	}

	// ������ ������� ��������.
	// ���������� �� ������� �������������� ������, ���� ��� ����.
	while (!result.res_strings.empty()) {
		do_write(boost::str(boost::format("%1%\n") % result.res_strings.front()));
		result.res_strings.pop();
	}

	// ���������� ��.
	reply_ok();
}

// ������� ������ ��� ������ ������ �� ����.
void ClientSession::clear_data_read()
{
	for (size_t i = 0; i < MAX_LENGTH; ++i) {
		data_read[i] = 0;
	}
}

// ���������� ������ ��� �������� �� ����.
void ClientSession::prepare_data_send(const std::string& data)
{
	for (size_t i = 0; i < MAX_LENGTH; ++i) {
		data_send[i] = 0;
	}

	for (size_t i = 0; i < data.length(); ++i) {
		data_send[i] = data.c_str()[i];
		if (i == (MAX_LENGTH - 1)) {
			break;
		}
	}
}

// ��������� ������� �� �������.
void ClientSession::handle_request(const std::size_t& length)
{
	std::string request(data_read, length);

	std::string error_text;

	DatabaseRequest db_request = parser_ptr->parse_command(request, error_text);

	// ���� �� ���������� ������ �� ������� ��������� ���������� ������, �������� ������� �� ������.
	if (db_request.request_type == RequestType::UNKNOWN) {
		reply_error(error_text);
		return;
	}

	// ���������� ������ � ������� �� ����������.
	req_coll_ptr->add_request(session_id, db_request);

	// ��������� �������� �������� � ���� � ����������� ������ ������� � ������� ��������.
	db_manager_ptr->new_request_notify();
}

// 
void ClientSession::reply_ok()
{
	do_write("OK\n");
	do_read();
}

// 
void ClientSession::reply_error(const std::string& error_text)
{
	do_write(boost::str(boost::format("ERR %1%\n") % error_text));
	do_read();
}