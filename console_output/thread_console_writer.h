#pragma once

#include "../interfaces/interface_writer.h"

#include <queue>

#include <boost/format.hpp> 
#include <condition_variable> 
#include <thread>

#include <iostream>

/**
* @brief ����� ��� ������ ������ � ������� � ��������� ������.
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
    * ���������� ������ � ���������������� �������.
    * @param data ������.
    */
    void add_data(const std::string& data) override {
        // �������� �������� ������ � ������� � �������.
        std::lock_guard<std::mutex> q_lock(data_mutex);

        // ��������� ������.
        data_to_output.push(data);

        // ��������� ��������� �����.
        cond_var.notify_one();
    }

    /**
    * @return �������� �� ������� � ������� ������.
    */
    bool empty() override {
        // �������� �������� ������ � ������� � �������.
        std::lock_guard<std::mutex> q_lock(data_mutex);

        return data_to_output.empty();
    }

    /**
    * @return ���� �� ���� �� ���� �������� �����.
    */
    bool active() override {
        return (active_threads_number > 0);
    }

    /**
    * �������� ������� � ����������� ��������� ������.
    */
    void stop_process() override {
        done = true;
        cond_var.notify_one();
    }

private: // Methods

    /**
    * �����, ���������� � ��������� ������.
    */
    void worker_thread();

private: // Data
    // ������� ��� ������������� ������ �� ������� �� ������ �������.
    std::shared_ptr<std::mutex> console_mutex_ptr;

    std::condition_variable cond_var; // ��� ����������� ��������� ������ � �������.
    std::mutex data_mutex; // ������� ��� ������������� ������� � ��������� ������.

    std::queue<std::string> data_to_output;

    std::thread console_thread;
};