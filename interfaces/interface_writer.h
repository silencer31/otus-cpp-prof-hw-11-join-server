#pragma once

#include <string>
#include <atomic>

/**
* @brief Класс - интерфейс для классов многопоточной обработки данных.
*/
class IWriter
{
protected:
	std::atomic<bool> done{ false }; // Больше нет данных для обработки.
	std::atomic<uint16_t> active_threads_number{ 0 }; // Кол-во активных потоков.

public:
	virtual ~IWriter() = default;

	/**
	* Добавление данных в потокобезопасную очередь.
	* @param data данные.
	*/
	virtual void add_data(const std::string&) {}

	/**
	* Добавление данных в потокобезопасную очередь.
	* @param hid номер контекста.
	* @param time время.
	* @param data данные.
	*/
	virtual void add_data(const std::uint32_t&, const std::uint64_t&, const std::string&) {}

	/**
	* @return является ли очередь с данными пустой.
	*/
	virtual bool empty() = 0;

	/**
	* @return есть ли хотя бы один активный поток.
	*/
	virtual bool active() = 0;

	/**
	* Сообщить потокам о прекращении обработки данных.
	*/
	virtual void stop_process() = 0;
};