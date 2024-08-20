#include "join_server.h"

#include <iostream>


JoinServer::JoinServer(boost::asio::io_context& io_context, const unsigned short port, const std::string& db_name)
	: parser_ptr( std::make_shared<CommandParser>() )
	, notifier_ptr(std::make_shared<SessionNotifier>())
	, req_coll_ptr(std::make_shared<RequestCollector>())
	, res_coll_ptr(std::make_shared<ResultCollector>())
	, db_manager_ptr(std::make_shared<DatabaseManager>(notifier_ptr, req_coll_ptr, res_coll_ptr, db_name))
	, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
	// ������������� ����� � ����� ������ � ������ �, ���� ����� � ����� ��� ���.
	if ( !db_manager_ptr->connect_database()) {
		return;
	}

	// �������� ���� �������� ����������.
	do_accept();
}

JoinServer::~JoinServer()
{
	std::cout << "Join server Destructor" << std::endl;
}

void JoinServer::do_accept()
{
	acceptor_.async_accept( // ���� ����� �������� ����������, ��������� ��������� �������.
		[this](boost::system::error_code errcode, tcp::socket socket)
		{
			if (shutdown_flag) {
				std::cout << "\n -- connection rejected. Server is shutting down!" << std::endl;
				return;
			}

			// ������� ���� �� ������ boost.
			if (errcode) {
				std::cout << "\n -- connection rejected. Boost system error: " << errcode.message() << std::endl;
				do_accept(); // ������������� �� �������� ���������� ����������.
				return;
			}

			std::cout << "\n -- connection accepted" << std::endl;

			// ������� ���������� ������ � ��������� ����� ������.
			session_shared session_ptr = std::make_shared<ClientSession>(
				shared_from_this(),
				parser_ptr,
				req_coll_ptr,
				res_coll_ptr,
				db_manager_ptr,
				std::move(socket),
				session_number // ������ ������ ����� ���� �����.
			);

			// ��������� ����� ������ � ��������� ������ �������.
			sessions[session_number] = session_ptr;

			// ��������� ����� ������ � ��������� ������������.
			notifier_ptr->add_awaitor(session_number, session_ptr);

			session_number++;

			// ������ ������ ����� ������ - ������ ��������� ��������� ������.
			session_ptr->start();

			// ������������� �� �������� ���������� ����������.
			do_accept();
		}
	);

	std::cout << "\nwaiting for new connection" << std::endl;
}

// �������� ������� �� ���������� �������.
void JoinServer::shutdown_server(const int session_id)
{
	// ���������, ��������� �� ��� ������ ������.
	if (shutdown_flag) { return; }

	std::cout << "Join server: Shutdown request received from session id: " << session_id << std::endl;

	// ������ ���� ��� ����������� ����� ����� ����������.
	shutdown_flag = true;

	// �������� ���� �������, ��� ���������� ���������� ���� ����� ��������.
	for (const auto& [id, session_ptr] : sessions) {
		session_ptr->stop_activity();
	}

	// ��� ����� ������ ������ ��������� ��������� ������ �� �������.
	for (const auto& [id, session_ptr] : sessions) {
		while (session_ptr->receiving()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	// ������� ���������� ���������� ���� �������� � ����.
	while (db_manager_ptr->in_progress()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// ��� ����� ������ ������ ��������� �������� ����������� ��������.
	for (const auto& [id, session_ptr] : sessions) {
		while (session_ptr->sending()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	// � ����� ��������� ��� ������.
	for (const auto& [id, session_ptr] : sessions) {
		session_ptr->shutdown_session();
	}

	// ������������� ����� ��������� �������� � ���� ������.
	db_manager_ptr->stop_process();

	// ��� ���������� ������.
	while (db_manager_ptr->active()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// ��������� ����� ����� ����������.
	acceptor_.cancel();	
}

// �������� ������.
void JoinServer::close_session(const int session_id)
{
	std::cout << "Join server: Session will be closed: " << session_id << std::endl;

	sessions[session_id]->shutdown_session();
	sessions.erase(session_id);
	session_number--;
}

// ������� ������ �� ������ ��������, ��������� ��������� ���������� �������.
void JoinServer::remove_awaitor(const int session_id)
{
	notifier_ptr->rem_awaitor(session_id);
}