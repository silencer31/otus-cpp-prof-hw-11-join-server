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
	* ������ ������ ������������.
	*/
	void start() {
		do_read();
	}

	/**
	* ������, ��� �� ��������� ������.
	*/
	bool receiving() const {
		return receiving_data;
	}

	/**
	* ������, ��� �� �������� ������.
	*/
	bool sending() const {
		return sending_result;
	}

	/**
	* ���������� ���� ����� ��������, �� ��������� ��������� ��� ����������.
	*/
	void stop_activity() {
		stop_acception = true;
	}

	/**
	* ���������� ������ ������.
	*/
	void shutdown_session();

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
	* ������ ���������� �������.
	*/
	void handle_data_sent();

	/**
	* ������ �������� ������.
	*/
	void handle_write_error();

	/**
	* ��������� ��������� ���������� �������.
	*/
	void send_request_result();

	/**
	* ��������� ��������� ������ �� ���������� ���������� �������.
	*/
	void send_result_string();

	/**
	* �������� ������� �� �������� ��������� �������..
	*/
	void reply_ok();

	/**
	* �������� ������� �� ������.
	*/
	void reply_error(const std::string& error_text);

private: // data
	// ��� ����� � ��������, ��������� ������ ������.
	const join_server_shared join_server_ptr;
	// ����������� ������ � ���������� ������� � ����.
	const parser_shared	parser_ptr;
	// ��������� �������� � ����.
	const req_coll_shared req_coll_ptr;
	// ��������� ����������� ���������� ��������.
	const res_coll_shared res_coll_ptr;
	// �������� ���������� �������� � ����.
	const db_manager_shared db_manager_ptr;


	tcp::socket socket_;
	int session_id; // ������������� ������.
	
	static constexpr std::size_t MAX_LENGTH = 1024;

	char data_read[MAX_LENGTH]; // ��� ��������� ������ �� ����.
	char data_send[MAX_LENGTH];	// ��� �������� ������.

	bool receiving_data{ false }; // ����, ��� � ������ ������ ��� ��������� � ������ ������ �� �������.
	bool sending_result{ false }; // ����, ��� � ������ ������ ��� �������� ������� ���������� ���������� ������� � ���� ������. 
	bool stop_acception{ false }; // ����, ��� ������ ������ ���������� ���� ����� ��������.

	RequestResult cur_req_result{ ResultType::UNKNOWN }; // ������� ��������� �� �������� �������.
	std::queue<RequestResult> results_to_send; // ������� �����������, ��������� �������� �������.
};

using session_shared = std::shared_ptr<ClientSession>;
