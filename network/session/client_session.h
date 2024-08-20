#pragma once

#include "../../interfaces/interface_awaitor.h"

#include "../../parser/command_parser.h"
#include "../../database/database_manager.h"

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

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
		shutdown_session();
	}

	/**
	* Начало сессии пользователя.
	*/
	void start() {
		do_read();
	}

	/**
	* Узнать, идёт ли получение данных.
	*/
	bool receiving() const {
		return receiving_data;
	}

	/**
	* Узнать, идёт ли отправка данных.
	*/
	bool sending() const {
		return sending_result;
	}

	/**
	* Прекратить приём новых запросов, но закончить обработку уже полученных.
	*/
	void stop_activity() {
		stop_acception = true;
	}

	/**
	* Завершение работы сессии.
	*/
	void shutdown_session();

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
	* Данные отправлены клиенту.
	*/
	void handle_data_sent();

	/**
	* Ошибка отправки данных.
	*/
	void handle_write_error();

	/**
	* Отправить результат выполнения запроса.
	*/
	void send_request_result();

	/**
	* Отправить следующую строку из результата выполнения запроса.
	*/
	void send_result_string();

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

	bool receiving_data{ false }; // Флаг, что в данный момент идёт получение и анализ данных от клиента.
	bool sending_result{ false }; // Флаг, что в данный момент идёт отправка клиенту результата выполнения запроса к базе данных. 
	bool stop_acception{ false }; // Флаг, что сессия должна прекратить приём новых запросов.

	RequestResult cur_req_result{ ResultType::UNKNOWN }; // Текущий результат на отправку клиенту.
	std::queue<RequestResult> results_to_send; // Очередь результатов, ожидающих отправки клиенту.
};

using session_shared = std::shared_ptr<ClientSession>;
