#pragma once

#include "../../parser/command_parser.h"
#include "../../database/database_manager.h"

#include "../session/client_session.h"

using boost::asio::ip::tcp;

using session_map = std::map<int, session_shared>; // ��� ��������� ������ �������������.

/**
* @brief ����� - ������ ��������� ������� ����������� � ���������� �������� �������������.
*/
class JoinServer : public std::enable_shared_from_this<JoinServer>
{
public:
	JoinServer() = delete;

	explicit JoinServer(boost::asio::io_context& io_context, unsigned short port, const char* file_path);
	
	~JoinServer();

	/**
	* �������� ������� �� ���������� �������.
	* @param session_id ���������� id ������, �� ������� �������� �������.
	*/
	void shutdown_server(int session_id);

	/**
	* �������� ������.
	* @param session_id ���������� id ������.
	*/
	void close_session(int session_id);


private: // methods
	/**
	* ���������� � �������� ����� �����������.
	*/
	void do_accept();
	
private: // data
	
	const parser_shared	parser_ptr;
	const notifier_shared notifier_ptr;
	const req_coll_shared req_coll_ptr;
	const res_coll_shared res_coll_ptr;
	const db_manager_shared db_manager_ptr;

	tcp::acceptor acceptor_;

	int session_number{ 0 };   // ���-�� ������.

	bool shutdown_flag{ false }; // ����, ��� ����� ��������� ������ �������.	

	session_map sessions; // ��������� ������.			
};