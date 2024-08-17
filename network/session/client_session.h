#pragma once

#include "../../interfaces/interface_awaitor.h"

#include "../../parser/command_parser.h"
#include "../../database/database_manager.h"

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <utility>

using boost::asio::ip::tcp;

class JoinServer;

using join_server_shared = std::shared_ptr<JoinServer>;

/**
* @brief Класс - сессия пользователя. Позволяет пользователям подключаться к серверу в рамках отдельной сессии.
*/
class ClientSession : public std::enable_shared_from_this<ClientSession>, public IAwaitor
{
public:
	
	ClientSession(
		const join_server_shared& js_ptr,
		const parser_shared& prsr_ptr,
		const req_coll_shared& rqcl_ptr,
		const res_coll_shared& rscl_ptr,
		const db_manager_shared& dbm_ptr,
		tcp::socket socket, int s_id)
		: join_server_ptr(js_ptr)
		, parser_ptr(prsr_ptr)
		, req_coll_ptr(rqcl_ptr)
		, res_coll_ptr(rscl_ptr)
		, db_manager_ptr(dbm_ptr)
		, socket_(std::move(socket))
		, session_id(s_id)
	{}

	~ClientSession() {
		std::cout << "session destr. id " << session_id << std::endl;
		shutdown();
		//std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	/**
	* Начало сессии пользователя.
	*/
	void start() {
		do_read();
	}

	/**
	* Выключение сессии.
	*/
	void shutdown();

	/**
	* Обработать результат выполнения запроса к базе.
	*/
	void handle_request_result() override;

private: // methods

	/**
	* Асинхронное чтение данных из сокета.
	*/
	void do_read();
	
	/**
	* Асинхронная запись данных в сокет.
	*/
	void do_write(const std::string& answer);

	/**
	* Очистка буфера для приема данных по сети.
	*/
	void clear_data_read();

	/**
	* Подготовка данных для отправки по сети.
	*/
	void prepare_data_send(const std::string& data);

	/**
	* Обработка запроса от клиента.
	*/
	void handle_request(const std::size_t& length);

	/**
	* Сообщить клиенту об успешной обработке запроса..
	*/
	void reply_ok();

	/**
	* Сообщить клиенту об ошибке.
	*/
	void reply_error(const std::string& error_text);

private: // data
	// Для связи с сервером, создавшим данную сессию.
	const join_server_shared join_server_ptr;
	// Оповещатель сессий о выполнении запроса к базе.
	const parser_shared	parser_ptr;
	// Коллектор запросов к базе.
	const req_coll_shared req_coll_ptr;
	// Коллектор результатов выполнения запросов.
	const res_coll_shared res_coll_ptr;
	// Менеджер выполнения запросов к базе.
	const db_manager_shared db_manager_ptr;


	tcp::socket socket_;
	int session_id; // Идентификатор сессии.
	
	static constexpr std::size_t MAX_LENGTH = 1024;

	char data_read[MAX_LENGTH]; // Для получения данных из сети.
	char data_send[MAX_LENGTH];	// Для отправки данных.

	bool shutdown_session_flag{false}; // Флаг, что завершается работа сессии.
	bool shutdown_server_flag{false};  // Флаг, что завершается работа сервера.
};

using session_shared = std::shared_ptr<ClientSession>;
