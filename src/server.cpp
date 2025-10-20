#include "server.h"
#include "handlers/user_handler.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, HttpServer* server)
        : socket_(std::move(socket)), server_(server) {}

    void run() {
        do_read();
    }

private:
    void do_read() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, req_,
            [self](beast::error_code ec, std::size_t) {
                if (!ec) {
                    self->handle_request();
                }
            });
    }

    void handle_request() {
        http::response<http::string_body> res;
        
        std::string target = std::string(req_.target());
        
        if (req_.method() == http::verb::post && target == "/api/users") {
            res = UserHandler::create_user(req_);
        } else if (req_.method() == http::verb::get && target == "/api/users") {
            res = UserHandler::get_all_users();
        } else if (req_.method() == http::verb::get && target.starts_with("/api/users/")) {
            std::string user_id = target.substr(11);
            res = UserHandler::get_user(user_id);
        } else if (req_.method() == http::verb::put && target.starts_with("/api/users/")) {
            std::string user_id = target.substr(11);
            res = UserHandler::update_user(user_id, req_);
        } else if (req_.method() == http::verb::delete_ && target.starts_with("/api/users/")) {
            std::string user_id = target.substr(11);
            res = UserHandler::delete_user(user_id);
        } else {
            res.result(http::status::not_found);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"error": "Not found"})";
            res.prepare_payload();
        }

        do_write(std::move(res));
    }

    void do_write(http::response<http::string_body> res) {
        auto self = shared_from_this();
        auto sp = std::make_shared<http::response<http::string_body>>(std::move(res));
        
        http::async_write(socket_, *sp,
            [self, sp](beast::error_code ec, std::size_t) {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    HttpServer* server_;
};

HttpServer::HttpServer(net::io_context& ioc, const std::string& address, unsigned short port)
    : ioc_(ioc), acceptor_(ioc, tcp::endpoint(net::ip::make_address(address), port)) {
}

void HttpServer::run() {
    do_accept();
}

void HttpServer::do_accept() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), this)->run();
            }
            do_accept();
        });
}
