#include "handlers/user_handler.h"
#include "db/database.h"
#include <regex>

http::response<http::string_body> UserHandler::create_user(
    const http::request<http::string_body>& req) {
    
    http::response<http::string_body> res;
    res.set(http::field::content_type, "application/json");
    
    try {
        json data = parse_create_user_request(req.body());
        
        if (!validate_user_data(data)) {
            res.result(http::status::bad_request);
            res.body() = R"({"error": "Invalid user data"})";
            res.prepare_payload();
            return res;
        }
        
        std::string user_id = Database::instance().create_user(
            data["username"],
            data["email"],
            data["password"],
            data.value("first_name", ""),
            data.value("last_name", "")
        );
        
        json response;
        response["id"] = user_id;
        response["username"] = data["username"];
        response["email"] = data["email"];
        
        res.result(http::status::created);
        res.body() = response.dump();
        res.prepare_payload();
        
    } catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        json error;
        error["error"] = e.what();
        res.body() = error.dump();
        res.prepare_payload();
    }
    
    return res;
}

http::response<http::string_body> UserHandler::get_user(const std::string& user_id) {
    http::response<http::string_body> res;
    res.set(http::field::content_type, "application/json");
    
    try {
        auto user_opt = Database::instance().get_user_by_id(user_id);
        
        if (!user_opt.has_value()) {
            res.result(http::status::not_found);
            res.body() = R"({"error": "User not found"})";
            res.prepare_payload();
            return res;
        }
        
        User user = user_opt.value();
        json response;
        response["id"] = user.id;
        response["username"] = user.username;
        response["email"] = user.email;
        response["first_name"] = user.first_name;
        response["last_name"] = user.last_name;
        response["created_at"] = user.created_at;
        response["updated_at"] = user.updated_at;
        
        res.result(http::status::ok);
        res.body() = response.dump();
        res.prepare_payload();
        
    } catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        json error;
        error["error"] = e.what();
        res.body() = error.dump();
        res.prepare_payload();
    }
    
    return res;
}

http::response<http::string_body> UserHandler::get_all_users() {
    http::response<http::string_body> res;
    res.set(http::field::content_type, "application/json");
    
    try {
        std::vector<User> users = Database::instance().get_all_users();
        
        json response = json::array();
        for (const auto& user : users) {
            json user_json;
            user_json["id"] = user.id;
            user_json["username"] = user.username;
            user_json["email"] = user.email;
            user_json["first_name"] = user.first_name;
            user_json["last_name"] = user.last_name;
            user_json["created_at"] = user.created_at;
            user_json["updated_at"] = user.updated_at;
            response.push_back(user_json);
        }
        
        res.result(http::status::ok);
        res.body() = response.dump();
        res.prepare_payload();
        
    } catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        json error;
        error["error"] = e.what();
        res.body() = error.dump();
        res.prepare_payload();
    }
    
    return res;
}

json UserHandler::parse_create_user_request(const std::string& body) {
    return json::parse(body);
}

http::response<http::string_body> UserHandler::update_user(
    const std::string& user_id,
    const http::request<http::string_body>& req) {
    
    http::response<http::string_body> res;
    res.set(http::field::content_type, "application/json");
    
    try {
        json data = json::parse(req.body());
        
        if (!validate_update_data(data)) {
            res.result(http::status::bad_request);
            res.body() = R"({"error": "Invalid update data"})";
            res.prepare_payload();
            return res;
        }
        
        bool updated = Database::instance().update_user(
            user_id,
            data.value("username", ""),
            data.value("email", ""),
            data.value("first_name", ""),
            data.value("last_name", "")
        );
        
        if (!updated) {
            res.result(http::status::not_found);
            res.body() = R"({"error": "User not found"})";
            res.prepare_payload();
            return res;
        }
        
        auto user_opt = Database::instance().get_user_by_id(user_id);
        if (user_opt.has_value()) {
            User user = user_opt.value();
            json response;
            response["id"] = user.id;
            response["username"] = user.username;
            response["email"] = user.email;
            response["first_name"] = user.first_name;
            response["last_name"] = user.last_name;
            response["updated_at"] = user.updated_at;
            
            res.result(http::status::ok);
            res.body() = response.dump();
            res.prepare_payload();
        }
        
    } catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        json error;
        error["error"] = e.what();
        res.body() = error.dump();
        res.prepare_payload();
    }
    
    return res;
}

http::response<http::string_body> UserHandler::delete_user(const std::string& user_id) {
    http::response<http::string_body> res;
    res.set(http::field::content_type, "application/json");
    
    try {
        bool deleted = Database::instance().delete_user(user_id);
        
        if (!deleted) {
            res.result(http::status::not_found);
            res.body() = R"({"error": "User not found"})";
            res.prepare_payload();
            return res;
        }
        
        json response;
        response["message"] = "User deleted successfully";
        response["id"] = user_id;
        
        res.result(http::status::ok);
        res.body() = response.dump();
        res.prepare_payload();
        
    } catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        json error;
        error["error"] = e.what();
        res.body() = error.dump();
        res.prepare_payload();
    }
    
    return res;
}

bool UserHandler::validate_user_data(const json& data) {
    if (!data.contains("username") || !data.contains("email") || !data.contains("password")) {
        return false;
    }
    
    std::string email = data["email"];
    std::regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, email_pattern)) {
        return false;
    }
    
    std::string username = data["username"];
    if (username.length() < 3 || username.length() > 255) {
        return false;
    }
    
    std::string password = data["password"];
    if (password.length() < 6) {
        return false;
    }
    
    return true;
}

bool UserHandler::validate_update_data(const json& data) {
    if (data.contains("email")) {
        std::string email = data["email"];
        std::regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        if (!std::regex_match(email, email_pattern)) {
            return false;
        }
    }
    
    if (data.contains("username")) {
        std::string username = data["username"];
        if (username.length() < 3 || username.length() > 255) {
            return false;
        }
    }
    
    return true;
}
