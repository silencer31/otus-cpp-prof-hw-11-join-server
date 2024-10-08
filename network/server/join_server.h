#pragma once

#include "../session/client_session.h"

#include <boost/filesystem/operations.hpp>

using boost::asio::ip::tcp;

using session_map = std::map<int, session_shared>; // ��� ��������� ������ �������������.

/**
* @brief ����� - ������ ��������� ������� ����������� � ���������� �������� �������������.
*/
class JoinServer : public std::enable_shared_from_this<JoinServer>
{
public:
	JoinServer() = delete;

	explicit JoinServer(boost::asio::io_context& io_context, unsigned short port, const std::string& db_name);
	
	~JoinServer();

	/**
	* �������� ������� �� ���������� �������.
	* @param session_id ���������� id ������, �� ������� �������� �������.
	*/
	void shutdown_server(const int session_id);

	/**
	* �������� ������.
	* @param session_id ���������� id ������.
	*/
	void close_session(const int session_id);

	/**
	* ������� ������ �� ������ ��������, ��������� ��������� ���������� �������.
	* @param session_id ���������� id ������.
	*/
	void remove_awaitor(const int session_id);

private: // methods

	/**
	* ���������� � �������� ����� �����������.
	*/
	void do_accept();
	
private: // data
	
	// ������ ������, ���������� �� �������, � ������ � ����.
	const parser_shared	parser_ptr;

	// ����������� ������ � ���������� ������� � ����.
	const notifier_shared notifier_ptr;

	// ��������� �������� � ����.
	const req_coll_shared req_coll_ptr;

	// ��������� ����������� ���������� ��������.
	const res_coll_shared res_coll_ptr;

	// �������� ���������� �������� � ����.
	const db_manager_shared db_manager_ptr;

	tcp::acceptor acceptor_;

	int session_number{ 0 };   // ���-�� ������.

	bool shutdown_flag{ false }; // ����, ��� ����� ��������� ������ �������.	

	session_map sessions; // ��������� ������.			
};