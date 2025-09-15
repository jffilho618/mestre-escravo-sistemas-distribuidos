#pragma once

#include <string>
#include <memory>

// Estrutura para resultado do processamento
struct ProcessingResult {
    bool success = false;
    int letters_count = 0;
    int numbers_count = 0;
    int total_characters = 0;
    double processing_time_ms = 0.0;
    std::string error_message;
    std::string raw_response;
};

// Cliente HTTP para comunicação com o servidor mestre
class HttpClient {
private:
    std::string master_host;
    int master_port;
    std::string master_url;

    void update_master_url();

public:
    HttpClient(const std::string& host, int port);

    // Configuração do servidor
    void set_master_address(const std::string& host, int port);
    std::string get_master_url() const;

    // Operações principais
    ProcessingResult process_file(const std::string& file_path);
    ProcessingResult process_text(const std::string& text);
    bool check_master_health();

private:
    // Métodos auxiliares
    std::string read_file_content(const std::string& file_path);
    ProcessingResult parse_response(const std::string& response);
    ProcessingResult make_request(const std::string& endpoint, const std::string& data);
};