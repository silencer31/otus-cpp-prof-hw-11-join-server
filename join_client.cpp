#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include <string.h>

using boost::asio::ip::tcp;

namespace ba = boost::asio;

class JoinClient
{
public:
    JoinClient() = delete;

    JoinClient(const std::string& ip_addr, const unsigned short port)
        : _end_point(ba::ip::tcp::endpoint(ba::ip::address::from_string(ip_addr), port))
        , _sock(ba::ip::tcp::socket(_io_context))
    {
        connected = connect_to_server();
    }

    ~JoinClient() {
        disconnect();
    }

    /**
    * Узнать, удалось ли подключиться.
    */
    bool is_connected() const {
        return connected;
    }

    /**
    * Отправка данных.
    * @param data_ptr отправляемые данные.
    * @param data_size размер данных.
    */
    bool send_message(const char* data_ptr)
    {
        if (strlen(data_ptr) == 0) {
            last_error = std::string("Empty data to send");
            return false;
        }

        bytes_written = 0;

        try {
            bytes_written = ba::write(_sock, ba::buffer(data_ptr, strlen(data_ptr)));
        }
        catch (const boost::system::system_error& ex) {
            last_error = std::string("boost exception: ") + ex.what();
            return false;
        }
        catch (const std::exception& ex) {
            last_error = std::string("std exception: ") + ex.what();
            return false;
        }

        // Проверяем, сколько байт удалось записать.
        if (bytes_written == 0) {
            last_error = "Zero bytes were written";
            return false;
        }

        return true;
    }

private:
    /**
    * Подключение к серверу.
    */
    bool connect_to_server()
    {
        try {
            _sock.connect(_end_point);
        }
        catch (const boost::system::system_error& ex) {
            last_error = std::string("boost exception: ") + ex.what();
            return false;
        }
        catch (const std::exception& ex) {
            last_error = std::string("std exception: ") + ex.what();
            return false;
        }

        return true;
    }

    /**
    * Отключение от сервера.
    */
    void disconnect()
    {
        if (!_sock.is_open()) {
            return;
        }

        boost::system::error_code ignore;

        _sock.shutdown(
            boost::asio::ip::tcp::socket::shutdown_both,
            ignore
        );

        _sock.close(ignore);

        connected = false;
    }

private:
    ba::io_context _io_context;
    ba::ip::tcp::endpoint _end_point;
    ba::ip::tcp::socket _sock;

    bool connected;

    std::string last_error; // Ошибка, полученная в процессе сетевого взаимодействия.

    std::size_t bytes_written; // Сколько байт удалось отправить.
};

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: bulk_client <host> <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[2]);

    if (port <= 0) {
        std::cerr << "Incorrect port value\n";
        return -1;
    }

    const std::string server_addr(argv[1]);

    JoinClient bulk_client(server_addr, static_cast<unsigned short>(port));

    if (!bulk_client.is_connected()) {
        std::cerr << "Unable to connect to server " << server_addr << ". Port: " << port << std::endl;
        return -1;
    }
    /*
    char* str_ptr = nullptr;
    size_t data_len = 0;

    // Отправка на сервер данных, указанных в консоли при запуске клиента.
    if (argc > 3) {
        for (int i = 3; i < argc; ++i) {
            if (i == (argc - 1)) {
                bulk_client.send_message(argv[i]);
                break;
            }

            data_len = strlen(argv[i]);
            str_ptr = (char*)malloc(data_len + 2);
            strcpy(str_ptr, argv[i]);
            str_ptr[data_len] = '\n';
            str_ptr[data_len + 1] = 0;
            //std::string str_to_send(argv[i] + "\n");
            bulk_client.send_message(str_ptr);
            free(str_ptr);
            //std::this_thread::sleep_for(std::chrono::seconds(1));
            //std::cout << argv[i] << std::endl;
        }
    }
    else { // Использовать заготовленный набор данных.
        bulk_client.send_message("a");
        bulk_client.send_message("b");
        bulk_client.send_message("\n");
        bulk_client.send_message("c");
    }


    
    client.write("a");
    client.write("b");
    client.write("\n");
    client.write("c");
    client.write("\n");
    client.write("d");
    client.write("\n");
    client.write("\n");
    client.write("\n");
    client.write("f");
    client.write("\n");
    client.write("g");
    client.write("{");
    client.write("\n");
    client.write("h");
    client.write("\n");
    client.write("i");
    client.write("\n");
    client.write("j");
    client.write("\n");
    client.write("k");
    client.write("\n");
    client.write("\n");
    client.write("l");
    client.write("\n");
    */



    return 0;
}
