#pragma once

#include "../../interfaces/interface_awaitor.h"

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
class ClientSession : public std::enable_shared_from_this<ClientSession>, public IAwaitor
{
public:
	
	ClientSession(
		const join_server_shared& js_ptr,
		const parser_shared& prsr_ptr,
		const req_coll_shared& rqcl_ptr,
		const res_coll_shared& rscl_ptr,
		tcp::socket socket, int s_id)
		: join_server_ptr(js_ptr)
		, parser_ptr(prsr_ptr)
		, req_coll_ptr(rqcl_ptr)
		, res_coll_ptr(rscl_ptr)
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

	/**
	* ���������� ��������� ���������� ������� � ����.
	*/
	void handle_request_result() override;

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
	void handle_request(const std::size_t& length);

	/**
	* �������� ������� �� �������� ��������� �������..
	*/
	void reply_ok();

	/**
	* �������� ������� �� ������.
	*/
	void reply_error(const std::string& error_text);

private: // data
	const join_server_shared join_server_ptr; // ��� ����� � ��������, ��������� ������ ������.

	const parser_shared	parser_ptr;
	const req_coll_shared req_coll_ptr;
	const res_coll_shared res_coll_ptr;


	//const request_parser_shared request_parser_ptr; // ��� �������� �������� � ���� ������.

	tcp::socket socket_;
	int session_id; // ������������� ������.
	
	static constexpr std::size_t MAX_LENGTH = 1024;

	char data_read[MAX_LENGTH]; // ��� ��������� ������ �� ����.
	char data_send[MAX_LENGTH];	// ��� �������� ������.

	bool shutdown_session_flag; // ����, ��� ����������� ������ ������.
};

using session_shared = std::shared_ptr<ClientSession>;
