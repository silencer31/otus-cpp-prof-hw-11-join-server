#pragma once

#include <string>
#include <atomic>

/**
* @brief ����� - ��������� ��� ������� ������������� ��������� ������.
*/
class IWriter
{
protected:
	std::atomic<bool> done{ false }; // ������ ��� ������ ��� ���������.
	std::atomic<uint16_t> active_threads_number{ 0 }; // ���-�� �������� �������.

public:
	virtual ~IWriter() = default;

	/**
	* ���������� ������ � ���������������� �������.
	* @param data ������.
	*/
	virtual void add_data(const std::string&) {}

	/**
	* ���������� ������ � ���������������� �������.
	* @param hid ����� ���������.
	* @param time �����.
	* @param data ������.
	*/
	virtual void add_data(const std::uint32_t&, const std::uint64_t&, const std::string&) {}

	/**
	* @return �������� �� ������� � ������� ������.
	*/
	virtual bool empty() = 0;

	/**
	* @return ���� �� ���� �� ���� �������� �����.
	*/
	virtual bool active() = 0;

	/**
	* �������� ������� � ����������� ��������� ������.
	*/
	virtual void stop_process() = 0;
};