#pragma once

#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::string created_at;
    std::string updated_at;
};

class Database {
public:
    static Database& instance();
    
    std::string create_user(const std::string& username, 
                           const std::string& email,
                           const std::string& password_hash,
                           const std::string& first_name,
                           const std::string& last_name);
    
    std::optional<User> get_user_by_id(const std::string& user_id);
    
    std::vector<User> get_all_users();
    
    bool update_user(const std::string& user_id,
                    const std::string& username,
                    const std::string& email,
                    const std::string& first_name,
                    const std::string& last_name);
    
    bool delete_user(const std::string& user_id);

private:
    Database();
    std::unique_ptr<pqxx::connection> conn_;
    
    std::string get_connection_string();
    User row_to_user(const pqxx::row& row);
};
