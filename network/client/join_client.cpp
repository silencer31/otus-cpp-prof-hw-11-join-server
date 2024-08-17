#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include <queue>
#include <string>

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


    std::string read_reply()
    {
        read_until(_sock, read_buffer, "\n");
        const std::string reply_txt = make_string(read_buffer);
        read_buffer.consume(reply_txt.size());
        return reply_txt;
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

    std::string make_string(boost::asio::streambuf& streambuf)
    {
        return { boost::asio::buffers_begin(streambuf.data()),
                boost::asio::buffers_end(streambuf.data()) };
    }

private:
    ba::io_context _io_context;
    ba::ip::tcp::endpoint _end_point;
    ba::ip::tcp::socket _sock;

    bool connected;

    std::string last_error; // Ошибка, полученная в процессе сетевого взаимодействия.

    std::size_t bytes_written; // Сколько байт удалось отправить.

    boost::asio::streambuf read_buffer;
};

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: join_client <host> <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[2]);

    if (port <= 0) {
        std::cerr << "Incorrect port value\n";
        return -1;
    }

    const std::string server_addr(argv[1]);

    JoinClient join_client(server_addr, static_cast<unsigned short>(port));

    if (!join_client.is_connected()) {
        std::cerr << "Unable to connect to server " << server_addr << ". Port: " << port << std::endl;
        return -1;
    }

    // Наполняем очередь запросов тестовыми данными.
    std::queue<std::string> requests;
    
    requests.push("INSERT A 0 lean\n");
    requests.push("INSERT A 0 understand\n");
    requests.push("INSERT A 1 sweater\n");
    requests.push("INSERT A 2 frank\n");
    
    requests.push("INSERT B 6 flour\n");
    requests.push("INSERT B 7 wonder\n");
    requests.push("INSERT B 8 selection\n");

    requests.push("INSERT A 3 violation\n");
    requests.push("INSERT A 4 quality\n");
    requests.push("INSERT A 5 precision\n");

    requests.push("INSERT B 3 proposal\n");
    requests.push("INSERT B 4 example\n");
    requests.push("INSERT B 5 lake\n");

    requests.push("INTERSECTION\n");
    requests.push("SYMMETRIC_DIFFERENCE\n");
    
    //requests.push("TRUNCATE\n");
    
    requests.push("TRUNCATE A\n");
    requests.push("TRUNCATE B\n");

    // Отправляем в цикле.
    while (!requests.empty()) {
        join_client.send_message(requests.front().c_str());
        requests.pop();

        std::cout << join_client.read_reply() << std::endl;
    }
        
    // Выключаем сервер.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    join_client.send_message("shutdown");
    

    return 0;
}
