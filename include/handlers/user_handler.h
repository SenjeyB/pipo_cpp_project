#pragma once

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace http = boost::beast::http;
using json = nlohmann::json;

class UserHandler {
public:
    static http::response<http::string_body> create_user(
        const http::request<http::string_body>& req);
    
    static http::response<http::string_body> get_user(
        const std::string& user_id);
    
    static http::response<http::string_body> get_all_users();
    
    static http::response<http::string_body> update_user(
        const std::string& user_id,
        const http::request<http::string_body>& req);
    
    static http::response<http::string_body> delete_user(
        const std::string& user_id);
    
private:
    static json parse_create_user_request(const std::string& body);
    static bool validate_user_data(const json& data);
    static bool validate_update_data(const json& data);
};
