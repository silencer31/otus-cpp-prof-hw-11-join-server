#pragma once

#include "../interfaces/interface_writer.h"

#include <queue>

#include <boost/format.hpp> 
#include <condition_variable> 
#include <thread>

#include <iostream>

/**
* @brief Класс для вывода данных в консоль в отдельном потоке.
*/
class ConsoleWriter final : public IWriter
{

public:
    ConsoleWriter(std::shared_ptr<std::mutex>& cm_ptr)
        : console_mutex_ptr(cm_ptr)
        , console_thread(&ConsoleWriter::worker_thread, this)
    {
        ++active_threads_number;

        std::cout << "Console writer - Constructor" << std::endl;
    }

    ~ConsoleWriter() {
        if (!done) {
            if (console_thread.joinable()) {
                console_thread.join();
            }
        }

        std::cout << "Console writer - Destructor" << std::endl;
    }

    /**
    * Добавление данных в потокобезопасную очередь.
    * @param data данные.
    */
    void add_data(const std::string& data) override {
        // Пытаемся получить доступ к очереди с данными.
        std::lock_guard<std::mutex> q_lock(data_mutex);

        // Добавляем данные.
        data_to_output.push(data);

        // Оповещаем ожидающий поток.
        cond_var.notify_one();
    }

    /**
    * @return является ли очередь с данными пустой.
    */
    bool empty() override {
        // Пытаемся получить доступ к очереди с данными.
        std::lock_guard<std::mutex> q_lock(data_mutex);

        return data_to_output.empty();
    }

    /**
    * @return есть ли хотя бы один активный поток.
    */
    bool active() override {
        return (active_threads_number > 0);
    }

    /**
    * Сообщить потокам о прекращении обработки данных.
    */
    void stop_process() override {
        done = true;
        cond_var.notify_one();
    }

private: // Methods

    /**
    * Метод, работающий в отдельном потоке.
    */
    void worker_thread();

private: // Data
    // Мьютекс для синхронизации вывода на консоль из разных потоков.
    std::shared_ptr<std::mutex> console_mutex_ptr;

    std::condition_variable cond_var; // Для нотификации появления данных в очереди.
    std::mutex data_mutex; // Мьютекс для синхронизации доступа к коллекции данных.

    std::queue<std::string> data_to_output;

    std::thread console_thread;
};