#include "../server/join_server.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

void ClientSession::do_read()
{
	auto self(shared_from_this());

	socket_.async_read_some( // У сокета вызываем async_read_some
		boost::asio::buffer(data_read, MAX_LENGTH),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << " Session: " << session_id << ". Read data boost system error code: " << errcode.message() << std::endl;

				// Сообщить серверу о необходимости выключения сессии
				join_server_ptr->close_session(session_id);

				return;
			}
			
			std::cout << "Session: " << session_id << ". Received: " << length << " bytes\n" << data_read << std::endl;

			// Обработка запроса 
			handle_request();
		}
	);
}

void ClientSession::do_write(const std::string& answer)
{
	auto self(shared_from_this());

	prepare_data_send(answer);

	boost::asio::async_write( // Вызываем async_write, указывая сокет.
		socket_, boost::asio::buffer(data_send, answer.length()),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << "Session: " << session_id << ". Write data boost system error: " << errcode.message() << std::endl;
			}
			else {
				std::cout << "Session: " << session_id << ". To send: " << length << " bytes\n" << data_send << "\n" << std::endl;
			}

			if (shutdown_session_flag) { // Если получена команда на выключение, больше не пытаемся читать из сети.
				return;
			}

			do_read();
		}
	);
}

// Выключение сессии.
void ClientSession::shutdown()
{
	if (!socket_.is_open()) {
		return;
	}

	std::cout << "Shutdown process started. Session: " << session_id << std::endl;

	boost::system::error_code ignore;

	socket_.shutdown(
		boost::asio::ip::tcp::socket::shutdown_both,
		ignore
	);

	socket_.close(ignore);

	std::cout << "Shutdown finished. Session: " << session_id << std::endl;
}

void ClientSession::handle_request_result() override
{

}

// Очистка буфера для приема данных по сети.
void ClientSession::clear_data_read()
{
	for (size_t i = 0; i < MAX_LENGTH; ++i) {
		data_read[i] = 0;
	}
}

// Подготовка данных для отправки по сети.
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

// Обработка запроса от клиента.
void ClientSession::handle_request()
{
	std::string err_text;

	DatabaseRequest db_request = parser_ptr->parse_command();
}
