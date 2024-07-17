#include "join_server.h"

#include <iostream>

JoinServer::JoinServer(boost::asio::io_context& io_context, const unsigned short port, const char* file_path)
	: notifier_ptr(std::make_shared<SessionNotifier>())
	, parser_ptr(std::make_shared<CommandParser>())
	, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
	//std::cout << "Bulk_Constructor" << std::endl;

	do_accept();
}

JoinServer::~JoinServer()
{
	std::cout << "Join_Destructor" << std::endl;
}

void JoinServer::do_accept()
{
	acceptor_.async_accept( // Если будет входящее соединение, выполнить следующую функцию.
		[this](boost::system::error_code errcode, tcp::socket socket)
		{
			if (shutdown_flag) {
				std::cout << "\n -- connection rejected. Server is shutting down!" << std::endl;
				return;
			}

			// Смотрим была ли ошибка boost.
			if (errcode) {
				std::cout << "\n -- connection rejected. Boost system error: " << errcode.message() << std::endl;
				do_accept(); // Переключаемся на ожидание следующего соединения.
				return;
			}

			std::cout << "\n -- connection accepted" << std::endl;

			// Создаем клиентскую сессию и запускаем прием данных.
			session_shared session_ptr = std::make_shared<ClientSession>(
				shared_from_this(),
				std::move(socket),
				session_number // Каждая сессия знает свой номер.
			);

			// Добавляем новую сессию в коллекцию сессий сервера.
			sessions[session_number] = session_ptr;

			// Добавляем новую сессию в коллекцию нотификатора.
			notifier_ptr->add_awaitor(session_number, session_ptr);

			session_number++;

			// Начало работы новой сессии - нового контекста обработки данных.
			session_ptr->start();

			// Переключаемся на ожидание следующего соединения.
			do_accept();
		}
	);

	std::cout << "\nwaiting for new connection" << std::endl;
}

// Получена команда на выключение сервера.
void JoinServer::shutdown_server(int session_id)
{
	std::cout << "Bulk server: Exit request received from session id: " << session_id << std::endl;

	shutdown_flag = true;

	// Выключение вызвавшей сессии.
	notifier_ptr->rem_awaitor(session_id);
	close_session(session_id);

	// В цикле завершаем все сессии.
	for (const auto& [id, session_ptr] : sessions) {
		if (id != session_id) { // Пропускаем сессию, от которой пришел сигнал на завершение.
			notifier_ptr->rem_awaitor(id);
			session_ptr->shutdown();
		}
	}

	// Отключаем прием новых соединений.
	acceptor_.cancel();
	//acceptor_.close();
}

// Закрытие сессии.
void JoinServer::close_session(int session_id)
{
	std::cout << "Join server: Session will be closed: " << session_id << std::endl;

	notifier_ptr->rem_awaitor(session_id);

	sessions[session_id]->shutdown();
	sessions.erase(session_id);
	session_number--;
}