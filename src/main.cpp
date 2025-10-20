#include "server.h"
#include <iostream>
#include <boost/asio.hpp>

int main() {
    try {
        boost::asio::io_context ioc;
        
        const std::string address = "0.0.0.0";
        const unsigned short port = 8080;
        
        HttpServer server(ioc, address, port);
        server.run();
        
        std::cout << "Server running on http://" << address << ":" << port << std::endl;
        
        ioc.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
