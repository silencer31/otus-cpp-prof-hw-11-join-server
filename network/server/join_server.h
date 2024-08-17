#pragma once

#include "../session/client_session.h"

#include <boost/filesystem/operations.hpp>

using boost::asio::ip::tcp;

using session_map = std::map<int, session_shared>; // Для коллекции сессий пользователей.

/**
* @brief Класс - сервер обработки сетевых подключений и управления сессиями пользователей.
*/
class JoinServer : public std::enable_shared_from_this<JoinServer>
{
public:
	JoinServer() = delete;

	explicit JoinServer(boost::asio::io_context& io_context, unsigned short port, const std::string& db_name);
	
	~JoinServer();

	/**
	* Получена команда на выключение сервера.
	* @param session_id уникальный id сессии, из которой получена команда.
	*/
	void shutdown_server(int session_id);

	/**
	* Закрытие сессии.
	* @param session_id уникальный id сессии.
	*/
	void close_session(int session_id);


private: // methods

	/**
	* Приступить к ожиданию новых подключений.
	*/
	void do_accept();
	
private: // data
	
	// Парсер строки, полученной от клиента, в запрос к базе.
	const parser_shared	parser_ptr;

	// Оповещатель сессий о выполнении запроса к базе.
	const notifier_shared notifier_ptr;

	// Коллектор запросов к базе.
	const req_coll_shared req_coll_ptr;

	// Коллектор результатов выполнения запросов.
	const res_coll_shared res_coll_ptr;

	// Менеджер выполнения запросов к базе.
	const db_manager_shared db_manager_ptr;

	tcp::acceptor acceptor_;

	int session_number{ 0 };   // Кол-во сессий.

	bool shutdown_flag{ false }; // Флаг, что нужно завершать работу сервера.	

	session_map sessions; // Коллекция сессий.			
};