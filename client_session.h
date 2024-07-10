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
* @brief ����� - ������ ������������. ��������� ������������� ������������ � ������� � ������ ��������� ������.
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
	* ������ ������ ������������.
	*/
	void start() {
		clear_data_read();
		do_read();
	}

	/**
	* ���������� ������.
	*/
	void shutdown();

private: // methods

	/**
	* ����������� ������ ������ �� ������.
	*/
	void do_read();
	
	/**
	* ����������� ������ ������ � �����.
	*/
	void do_write(const std::string& answer);

	/**
	* ������� ������ ��� ������ ������ �� ����.
	*/
	void clear_data_read();

	/**
	* ���������� ������ ��� �������� �� ����.
	*/
	void prepare_data_send(const std::string& data);

	/**
	* ��������� ������� �� �������.
	*/
	void handle_request();

private: // data
	const join_server_shared join_server_ptr; // ��� ����� � ��������, ��������� ������ ������.

	tcp::socket socket_;
	int session_id; // ������������� ������.
	
	enum { max_length = 1024 };

	char data_read[max_length]; // ��� ��������� ������ �� ����.
	char data_send[max_length];	// ��� �������� ������.

	bool shutdown_session_flag; // ����, ��� ����������� ������ ������.
};

using session_shared = std::shared_ptr<ClientSession>;
