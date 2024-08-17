#include "network/server/join_server.h"

#include <iostream>

#ifdef WIN32
#define appname "join_server.exe"
#else
#define appname "./join_server"
#endif

namespace ba = boost::asio;


int main(int argc, char* argv[])
{
    std::locale::global(std::locale(""));
    
    if (argc != 3) {
        std::cout << "Usage: " << appname << " <port> <data base file name>" << std::endl;
        return -1;
    }

    int port = std::atoi(argv[1]);

    if (port <= 0) {
        std::cerr << "Incorrect port value\n";
        return -1;
    }

    std::string db_name{ argv[2] };
    if (!db_name.ends_with(".sqlite")) {
        std::cout << "Only sqlite data base file is supported!" << std::endl;
        return -1;
    }

    
    try
    {
        ba::io_context io_context;
        
        // Создание сервера.
        std::shared_ptr<JoinServer> join_server_ptr = std::make_shared<JoinServer>(
            io_context,
            static_cast<unsigned short>(port), 
            db_name
        );

        // 
        std::cout << "___ io run ___ waiting" << std::endl;
        io_context.run();
        std::cout << "___ io run ___ finished" << std::endl;

    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return 0;
}