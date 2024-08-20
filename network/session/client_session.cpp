#include "../server/join_server.h"

#include <boost/format.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <cstring>
#include <iostream>

// 
void ClientSession::do_read()
{
	if (stop_acception) { return; }

	clear_data_read();

	auto self(shared_from_this());

	socket_.async_read_some( // У сокета вызываем async_read_some
		boost::asio::buffer(data_read, MAX_LENGTH),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << " Session: " << session_id << ". Read data boost system error code: " << errcode.message() << "Length: " << length << std::endl;

				// Сообщить серверу о проблеме чтения из сокета.
				receiving_data = false;
				join_server_ptr->close_session(session_id);
				return;

				return;
			}

			//std::cout << "Session: " << session_id << ". Received: " << length << " bytes\n" << data_read << std::endl;
			// Ставим флаг, что идёт процесс обработки запросов.
			receiving_data = true;

			const std::string req_txt(data_read);

			// Проверяем наличие \n в полученной от клиента строке.
			if (req_txt.find('\n') == std::string::npos) {
				reply_error("incorrect request");
				return;
			}
			
			std::vector<std::string> commands;
			boost::split(commands, req_txt, boost::is_any_of("\n"));

			std::queue<DatabaseRequest> requests;

			for (auto it = commands.begin(); it != commands.end(); ++it)
			{
				if ((*it).empty()) { continue; }

				std::cout << "COMMAND: " << *it << std::endl;

				// Нужно ли завершить сессию.
				if (*it == "exit") {
					receiving_data = false;
					join_server_ptr->close_session(session_id);
					return;
				}

				// Нужно ли выключить сервер.
				if (*it == "shutdown") {
					receiving_data = false;
					join_server_ptr->shutdown_server(session_id);
					return;
				}

				// Формирование запроса из строки. 
				//DatabaseRequest db_request = parser_ptr->parse_command(*it, error_text);
				requests.push(parser_ptr->parse_command(*it));
			}

			// Отправляем запросы в очередь на исполнение.
			// Если от клиента получено несколько запросов, они должны идти по порядку в очереди запросов к базе данных. 
			req_coll_ptr->add_requests(session_id, requests);

			// Оповещаем менеджер запросов к базе о поступлении нового запроса в очередь запросов.
			db_manager_ptr->new_request_notify();

			receiving_data = false;
		}
	);
}

// 
void ClientSession::do_write(const std::string& answer)
{
	auto self(shared_from_this());

	prepare_data_send(answer);

	boost::asio::async_write( // Вызываем async_write, указывая сокет.
		socket_, boost::asio::buffer(data_send, answer.length()),
		[this, self](boost::system::error_code errcode, std::size_t length)
		{
			if (errcode) {
				std::cout << "Session: " << session_id << ". Write data boost system error: " << errcode.message() << " Length: " << length << " bytes" << std::endl;
				handle_write_error();
				return;
			}
			
			std::cout << "ANSWER: " << data_send << std::endl;

			handle_data_sent();
		}
	);
}

// Выключение сессии.
void ClientSession::shutdown_session()
{
	if (!socket_.is_open()) {
		return;
	}

	std::cout << "Session closing process started. id: " << session_id << std::endl;

	// Удаляем эту сессию из списка ожидающих результат выполнения запроса к базе.
	join_server_ptr->remove_awaitor(session_id);

	boost::system::error_code ignore;

	socket_.shutdown(
		boost::asio::ip::tcp::socket::shutdown_both,
		ignore
	);

	socket_.close(ignore);

	std::cout << "Session closing finished. id: " << session_id << std::endl;
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

	const char* cstr = data.c_str();

	//std::memcpy(data_send, cstr, sizeof data_send);
	for (size_t i = 0; i < data.length(); ++i) {
		data_send[i] = cstr[i];
		if (i == (MAX_LENGTH - 1)) {
			break;
		}
	}
}

// 
void ClientSession::reply_ok()
{
	do_write("OK\n");
}

// 
void ClientSession::reply_error(const std::string& error_text)
{
	do_write(boost::str(boost::format("ERR %1%\n") % error_text));
}