#include "http_client.h"
#include "logger.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <chrono>

using json = nlohmann::json;

HttpClient::HttpClient(const std::string& host, int port)
    : master_host(host), master_port(port) {
    update_master_url();
}

void HttpClient::update_master_url() {
    master_url = "http://" + master_host + ":" + std::to_string(master_port);
}

void HttpClient::set_master_address(const std::string& host, int port) {
    master_host = host;
    master_port = port;
    update_master_url();
    Logger::info_f("Servidor configurado para: %s", master_url.c_str());
}

std::string HttpClient::get_master_url() const {
    return master_url;
}

std::string HttpClient::read_file_content(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

ProcessingResult HttpClient::process_file(const std::string& file_path) {
    Logger::info_f("Processando arquivo: %s", file_path.c_str());

    try {
        std::string content = read_file_content(file_path);
        return process_text(content);
    } catch (const std::exception& e) {
        ProcessingResult result;
        result.success = false;
        result.error_message = "Erro ao ler arquivo: " + std::string(e.what());
        Logger::error(result.error_message);
        return result;
    }
}

ProcessingResult HttpClient::process_text(const std::string& text) {
    Logger::info_f("Processando texto de %zu caracteres", text.length());

    json request_data;
    request_data["text"] = text;

    return make_request("/process", request_data.dump());
}

bool HttpClient::check_master_health() {
    Logger::info("Verificando saúde do servidor mestre");

    try {
        httplib::Client client(master_host, master_port);
        client.set_connection_timeout(5, 0); // 5 segundos
        client.set_read_timeout(10, 0);      // 10 segundos

        auto result = client.Get("/health");

        if (result && result->status == 200) {
            Logger::info("Servidor mestre está saudável");
            return true;
        } else {
            Logger::warning_f("Health check falhou - Status: %d",
                             result ? result->status : -1);
            return false;
        }
    } catch (const std::exception& e) {
        Logger::error_f("Erro no health check: %s", e.what());
        return false;
    }
}

ProcessingResult HttpClient::make_request(const std::string& endpoint, const std::string& data) {
    ProcessingResult result;

    try {
        httplib::Client client(master_host, master_port);
        client.set_connection_timeout(10, 0); // 10 segundos
        client.set_read_timeout(30, 0);       // 30 segundos

        httplib::Headers headers = {
            {"Content-Type", "application/json"}
        };

        auto start_time = std::chrono::high_resolution_clock::now();

        auto response = client.Post(endpoint.c_str(), headers, data, "application/json");

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        if (!response) {
            result.success = false;
            result.error_message = "Falha na conexão com o servidor";
            Logger::error(result.error_message);
            return result;
        }

        result.raw_response = response->body;
        result.processing_time_ms = duration.count();

        if (response->status == 200) {
            return parse_response(response->body);
        } else {
            result.success = false;
            result.error_message = "Erro HTTP " + std::to_string(response->status);
            Logger::error_f("Erro HTTP %d: %s", response->status, response->body.c_str());
            return result;
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Exceção: " + std::string(e.what());
        Logger::error(result.error_message);
        return result;
    }
}

ProcessingResult HttpClient::parse_response(const std::string& response) {
    ProcessingResult result;

    try {
        json response_json = json::parse(response);

        result.success = response_json.value("success", false);
        result.letters_count = response_json.value("letters_count", 0);
        result.numbers_count = response_json.value("numbers_count", 0);
        result.total_characters = response_json.value("total_characters", 0);
        result.processing_time_ms = response_json.value("processing_time_ms", 0.0);
        result.error_message = response_json.value("error_message", "");
        result.raw_response = response;

        if (result.success) {
            Logger::info_f("Processamento bem-sucedido: %d letras, %d números",
                          result.letters_count, result.numbers_count);
        } else {
            Logger::error_f("Processamento falhou: %s", result.error_message.c_str());
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Erro ao parsear resposta JSON: " + std::string(e.what());
        result.raw_response = response;
        Logger::error(result.error_message);
    }

    return result;
}