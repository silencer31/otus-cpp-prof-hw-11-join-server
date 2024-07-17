#include "thread_console_writer.h"

// �����, ���������� � ��������� ������.
void ConsoleWriter::worker_thread()
{
    {
        std::lock_guard<std::mutex> lg(*console_mutex_ptr);
        std::cout << "Console writer thread started " << std::endl;
    }

    while (!done) {
        // �������� �������� ������ � ��������� � �������.
        std::unique_lock<std::mutex> data_lock(data_mutex);

        // ��������� ������� � �������, ���� � ������� �������� ������.
        cond_var.wait(data_lock, [this]()->bool { return !data_to_output.empty() || done; });

        // ������� ����� ��������.

        if (data_to_output.empty()) {
            data_lock.unlock();
            continue;
        }

        // ���� ������ ��������� ������� � ��������� � �������.
        const auto element = std::move(data_to_output.front());

        // ������� �� ������� �������� � ��������� �������.
        data_to_output.pop();

        // ����������� ������ � ���������.
        data_lock.unlock();

        {
            std::lock_guard<std::mutex> lg(*console_mutex_ptr);
            std::cout << element << std::endl;
        }
    }

    {
        std::lock_guard<std::mutex> lg(*console_mutex_ptr);
        std::cout << "Console writer thread finished " << std::endl;
    }

    --active_threads_number;
}

