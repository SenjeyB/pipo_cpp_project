#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <string>

namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class HttpServer {
public:
    HttpServer(net::io_context& ioc, const std::string& address, unsigned short port);
    void run();

private:
    void do_accept();
    void handle_request(http::request<http::string_body> req, 
                       std::function<void(http::response<http::string_body>)> send);

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
};
