#include "db/database.h"
#include <cstdlib>
#include <stdexcept>

Database& Database::instance() {
    static Database db;
    return db;
}

Database::Database() {
    try {
        conn_ = std::make_unique<pqxx::connection>(get_connection_string());
        if (!conn_->is_open()) {
            throw std::runtime_error("Failed to open database connection");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database connection error: ") + e.what());
    }
}

std::string Database::get_connection_string() {
    const char* host = std::getenv("DB_HOST");
    const char* port = std::getenv("DB_PORT");
    const char* dbname = std::getenv("DB_NAME");
    const char* user = std::getenv("DB_USER");
    const char* password = std::getenv("DB_PASSWORD");
    
    std::string conn_str = "postgresql://";
    conn_str += (user ? user : "postgres");
    conn_str += ":";
    conn_str += (password ? password : "postgres");
    conn_str += "@";
    conn_str += (host ? host : "localhost");
    conn_str += ":";
    conn_str += (port ? port : "5432");
    conn_str += "/";
    conn_str += (dbname ? dbname : "pipo_db");
    
    return conn_str;
}

std::string Database::create_user(const std::string& username,
                                   const std::string& email,
                                   const std::string& password_hash,
                                   const std::string& first_name,
                                   const std::string& last_name) {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result result = txn.exec_params(
            "INSERT INTO users (username, email, password_hash, first_name, last_name) "
            "VALUES ($1, $2, $3, $4, $5) "
            "RETURNING id",
            username, email, password_hash, first_name, last_name
        );
        
        txn.commit();
        
        return result[0][0].as<std::string>();
        
    } catch (const pqxx::unique_violation& e) {
        throw std::runtime_error("Username or email already exists");
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

std::optional<User> Database::get_user_by_id(const std::string& user_id) {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result result = txn.exec_params(
            "SELECT id, username, email, first_name, last_name, "
            "created_at, updated_at FROM users WHERE id = $1",
            user_id
        );
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        return row_to_user(result[0]);
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

std::vector<User> Database::get_all_users() {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result result = txn.exec(
            "SELECT id, username, email, first_name, last_name, "
            "created_at, updated_at FROM users ORDER BY created_at DESC"
        );
        
        std::vector<User> users;
        for (const auto& row : result) {
            users.push_back(row_to_user(row));
        }
        
        return users;
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

bool Database::update_user(const std::string& user_id,
                          const std::string& username,
                          const std::string& email,
                          const std::string& first_name,
                          const std::string& last_name) {
    try {
        pqxx::work txn(*conn_);
        
        std::string query = "UPDATE users SET updated_at = CURRENT_TIMESTAMP";
        std::vector<std::string> params;
        int param_index = 1;
        
        if (!username.empty()) {
            query += ", username = $" + std::to_string(param_index++);
            params.push_back(username);
        }
        if (!email.empty()) {
            query += ", email = $" + std::to_string(param_index++);
            params.push_back(email);
        }
        if (!first_name.empty()) {
            query += ", first_name = $" + std::to_string(param_index++);
            params.push_back(first_name);
        }
        if (!last_name.empty()) {
            query += ", last_name = $" + std::to_string(param_index++);
            params.push_back(last_name);
        }
        
        query += " WHERE id = $" + std::to_string(param_index);
        params.push_back(user_id);
        
        pqxx::result result;
        if (params.size() == 1) {
            result = txn.exec_params(query, user_id);
        } else if (params.size() == 2) {
            result = txn.exec_params(query, params[0], user_id);
        } else if (params.size() == 3) {
            result = txn.exec_params(query, params[0], params[1], user_id);
        } else if (params.size() == 4) {
            result = txn.exec_params(query, params[0], params[1], params[2], user_id);
        } else if (params.size() == 5) {
            result = txn.exec_params(query, params[0], params[1], params[2], params[3], user_id);
        }
        
        txn.commit();
        
        return result.affected_rows() > 0;
        
    } catch (const pqxx::unique_violation& e) {
        throw std::runtime_error("Username or email already exists");
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

bool Database::delete_user(const std::string& user_id) {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result result = txn.exec_params(
            "DELETE FROM users WHERE id = $1",
            user_id
        );
        
        txn.commit();
        
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Database error: ") + e.what());
    }
}

User Database::row_to_user(const pqxx::row& row) {
    User user;
    user.id = row["id"].as<std::string>();
    user.username = row["username"].as<std::string>();
    user.email = row["email"].as<std::string>();
    user.first_name = row["first_name"].is_null() ? "" : row["first_name"].as<std::string>();
    user.last_name = row["last_name"].is_null() ? "" : row["last_name"].as<std::string>();
    user.created_at = row["created_at"].as<std::string>();
    user.updated_at = row["updated_at"].as<std::string>();
    return user;
}
