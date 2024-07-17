#include "thread_console_writer.h"

// Метод, работающий в отдельном потоке.
void ConsoleWriter::worker_thread()
{
    {
        std::lock_guard<std::mutex> lg(*console_mutex_ptr);
        std::cout << "Console writer thread started " << std::endl;
    }

    while (!done) {
        // Пытаемся получить доступ к коллекции с данными.
        std::unique_lock<std::mutex> data_lock(data_mutex);

        // Отпускаем мьютекс и ожидаем, пока в очереди появятся данные.
        cond_var.wait(data_lock, [this]()->bool { return !data_to_output.empty() || done; });

        // Мьютекс снова захвачен.

        if (data_to_output.empty()) {
            data_lock.unlock();
            continue;
        }

        // Берём первый доступный элемент в коллекции с данными.
        const auto element = std::move(data_to_output.front());

        // Удаляем из очереди принятый в обработку элемент.
        data_to_output.pop();

        // Освобождаем доступ к коллекции.
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

