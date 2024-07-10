#pragma once

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
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
	
	ClientSession(
		const join_server_shared& js_ptr,
		tcp::socket socket, int s_id)
		: join_server_ptr(js_ptr)
		, socket_(std::move(socket))
		, session_id(s_id)
	{}

	~ClientSession() {
		//std::cout << "session destr. id " << session_id << std::endl;
		shutdown();
		//std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	/**
	* Начало сессии пользователя.
	*/
	void start() {
		clear_data_read();
		do_read();
	}

	/**
	* Выключение сессии.
	*/
	void shutdown();

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
	void handle_request();

private: // data
	const join_server_shared join_server_ptr; // Для связи с сервером, создавшим данную сессию.

	tcp::socket socket_;
	int session_id; // Идентификатор сессии.
	
	enum { max_length = 1024 };

	char data_read[max_length]; // Для получения данных из сети.
	char data_send[max_length];	// Для отправки данных.

	bool shutdown_session_flag; // Флаг, что завершается работа сессии.
};

using session_shared = std::shared_ptr<ClientSession>;
