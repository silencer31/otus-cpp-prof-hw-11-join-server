#pragma once

#include "../../notifier/session_notifier.h"
#include "../../parser/command_parser.h"

#include "../session/client_session.h"

using boost::asio::ip::tcp;

using session_map = std::map<int, session_shared>; // Для коллекции сессий пользователей.

/**
* @brief Класс - сервер обработки сетевых подключений и управления сессиями пользователей.
*/
class JoinServer : public std::enable_shared_from_this<JoinServer>
{
public:
	JoinServer() = delete;

	explicit JoinServer(boost::asio::io_context& io_context, unsigned short port, const char* file_path);
	
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
	const std::shared_ptr<INotifier> notifier_ptr;
	const std::shared_ptr<IParser>	 parser_ptr;


	tcp::acceptor acceptor_;

	int session_number{ 0 };   // Кол-во сессий.

	bool shutdown_flag{ false }; // Флаг, что нужно завершать работу сервера.	

	session_map sessions; // Коллекция сессий.			
};