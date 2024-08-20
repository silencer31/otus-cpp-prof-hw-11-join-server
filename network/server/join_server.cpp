#include "join_server.h"

#include <iostream>


JoinServer::JoinServer(boost::asio::io_context& io_context, const unsigned short port, const std::string& db_name)
	: parser_ptr( std::make_shared<CommandParser>() )
	, notifier_ptr(std::make_shared<SessionNotifier>())
	, req_coll_ptr(std::make_shared<RequestCollector>())
	, res_coll_ptr(std::make_shared<ResultCollector>())
	, db_manager_ptr(std::make_shared<DatabaseManager>(notifier_ptr, req_coll_ptr, res_coll_ptr, db_name))
	, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
	// Устанавливаем связь с базой данных и создаём её, если файла с базой ещё нет.
	if ( !db_manager_ptr->connect_database()) {
		return;
	}

	// Начинаем приём входящих соединений.
	do_accept();
}

JoinServer::~JoinServer()
{
	std::cout << "Join server Destructor" << std::endl;
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
				parser_ptr,
				req_coll_ptr,
				res_coll_ptr,
				db_manager_ptr,
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
void JoinServer::shutdown_server(const int session_id)
{
	// Проверяем, завершает ли уже сервер работу.
	if (shutdown_flag) { return; }

	std::cout << "Join server: Shutdown request received from session id: " << session_id << std::endl;

	// Ставим флаг для прекращения приёма новых соединений.
	shutdown_flag = true;

	// Сообщаем всем сессиям, что необходимо прекратить приём новых запросов.
	for (const auto& [id, session_ptr] : sessions) {
		session_ptr->stop_activity();
	}

	// Ждём когда каждая сессия прекратит получение данных от клиента.
	for (const auto& [id, session_ptr] : sessions) {
		while (session_ptr->receiving()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	// Ожидаем завершения выполнения всех запросов к базе.
	while (db_manager_ptr->in_progress()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// Ждём когда каждая сессия прекратит отправку результатов запросов.
	for (const auto& [id, session_ptr] : sessions) {
		while (session_ptr->sending()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	// В цикле завершаем все сессии.
	for (const auto& [id, session_ptr] : sessions) {
		session_ptr->shutdown_session();
	}

	// Останавливаем поток обработки запросов к базе данных.
	db_manager_ptr->stop_process();

	// Ждём завершения потока.
	while (db_manager_ptr->active()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// Отключаем прием новых соединений.
	acceptor_.cancel();	
}

// Закрытие сессии.
void JoinServer::close_session(const int session_id)
{
	std::cout << "Join server: Session will be closed: " << session_id << std::endl;

	sessions[session_id]->shutdown_session();
	sessions.erase(session_id);
	session_number--;
}

// Удалить сессию из списка клиентов, ожидающих результат выполнения запроса.
void JoinServer::remove_awaitor(const int session_id)
{
	notifier_ptr->rem_awaitor(session_id);
}