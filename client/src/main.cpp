#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>
#include "http_client.h"
#include "logger.h"

void print_banner() {
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "   CLIENTE SISTEMA DISTRIBUÍDO  \n";
    std::cout << "     Contador de Letras/Números \n";
    std::cout << "================================\n";
    std::cout << "\n";
}

void print_help() {
    std::cout << "\nComandos disponíveis:\n";
    std::cout << "  1 - Processar arquivo de texto\n";
    std::cout << "  2 - Processar texto digitado\n";
    std::cout << "  3 - Verificar status do servidor\n";
    std::cout << "  4 - Configurar endereço do servidor\n";
    std::cout << "  h - Mostrar esta ajuda\n";
    std::cout << "  q - Sair\n";
    std::cout << "\n";
}

void print_result(const ProcessingResult& result) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "           RESULTADO DO PROCESSAMENTO\n";
    std::cout << std::string(50, '=') << "\n";
    
    if (result.success) {
        std::cout << "✅ Processamento concluído com sucesso!\n\n";
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "📊 ESTATÍSTICAS:\n";
        std::cout << "   Letras encontradas:    " << std::setw(8) << result.letters_count << "\n";
        std::cout << "   Números encontrados:   " << std::setw(8) << result.numbers_count << "\n";
        std::cout << "   Total de caracteres:   " << std::setw(8) << result.total_characters << "\n";
        std::cout << "   Tempo de processamento: " << std::setw(8) << result.processing_time_ms << " ms\n";
        
        // Calcular percentuais
        int total = result.letters_count + result.numbers_count;
        if (total > 0) {
            double letter_pct = (static_cast<double>(result.letters_count) / total) * 100.0;
            double number_pct = (static_cast<double>(result.numbers_count) / total) * 100.0;
            
            std::cout << "\n📈 DISTRIBUIÇÃO:\n";
            std::cout << "   Letras:  " << std::setw(6) << letter_pct << "%\n";
            std::cout << "   Números: " << std::setw(6) << number_pct << "%\n";
        }
        
    } else {
        std::cout << "❌ Falha no processamento!\n\n";
        std::cout << "💥 ERRO: " << result.error_message << "\n";
        
        if (!result.raw_response.empty()) {
            std::cout << "\n📄 Resposta do servidor:\n";
            std::cout << result.raw_response.substr(0, 500);
            if (result.raw_response.length() > 500) {
                std::cout << "... (truncado)";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << std::string(50, '=') << "\n";
}

void process_file_interactive(HttpClient& client) {
    std::cout << "\n📁 PROCESSAMENTO DE ARQUIVO\n";
    std::cout << std::string(30, '-') << "\n";
    
    std::string file_path;
    std::cout << "Digite o caminho do arquivo: ";
    std::getline(std::cin, file_path);
    
    if (file_path.empty()) {
        std::cout << "❌ Caminho do arquivo não pode estar vazio.\n";
        return;
    }
    
    // Verificar se arquivo existe
    if (!std::filesystem::exists(file_path)) {
        std::cout << "❌ Arquivo não encontrado: " << file_path << "\n";
        return;
    }
    
    std::cout << "🔄 Processando arquivo: " << file_path << "\n";
    
    auto result = client.process_file(file_path);
    print_result(result);
}

void process_text_interactive(HttpClient& client) {
    std::cout << "\n✏️  PROCESSAMENTO DE TEXTO\n";
    std::cout << std::string(30, '-') << "\n";
    
    std::string text;
    std::cout << "Digite o texto (termine com Enter): ";
    std::getline(std::cin, text);
    
    if (text.empty()) {
        std::cout << "❌ Texto não pode estar vazio.\n";
        return;
    }
    
    std::cout << "🔄 Processando texto (" << text.length() << " caracteres)...\n";
    
    auto result = client.process_text(text);
    print_result(result);
}

void check_server_status(HttpClient& client) {
    std::cout << "\n🏥 VERIFICAÇÃO DE STATUS\n";
    std::cout << std::string(30, '-') << "\n";
    std::cout << "🔄 Verificando status do servidor...\n";
    
    bool healthy = client.check_master_health();
    
    if (healthy) {
        std::cout << "✅ Servidor está operacional e pronto para processar requisições!\n";
    } else {
        std::cout << "❌ Servidor não está disponível ou com problemas.\n";
        std::cout << "   Verifique se o servidor está rodando em: " << client.get_master_url() << "\n";
    }
}

void configure_server(HttpClient& client) {
    std::cout << "\n⚙️  CONFIGURAÇÃO DO SERVIDOR\n";
    std::cout << std::string(30, '-') << "\n";
    std::cout << "Servidor atual: " << client.get_master_url() << "\n\n";
    
    std::string host;
    std::string port_str;
    
    std::cout << "Digite o novo endereço do servidor (Enter para manter 'localhost'): ";
    std::getline(std::cin, host);
    if (host.empty()) {
        host = "localhost";
    }
    
    std::cout << "Digite a nova porta (Enter para manter '8080'): ";
    std::getline(std::cin, port_str);
    
    int port = 8080;
    if (!port_str.empty()) {
        try {
            port = std::stoi(port_str);
        } catch (const std::exception& e) {
            std::cout << "❌ Porta inválida, mantendo 8080.\n";
            port = 8080;
        }
    }
    
    client.set_master_address(host, port);
    std::cout << "✅ Configuração atualizada: " << client.get_master_url() << "\n";
}

int main(int argc, char* argv[]) {
    // Configurar logs
    Logger::set_component_name("CLIENT");
    Logger::set_log_level(LogLevel::INFO);
    
    // Argumentos de linha de comando
    std::string master_host = "localhost";
    int master_port = 8080;
    
    if (argc >= 2) {
        master_host = argv[1];
    }
    if (argc >= 3) {
        try {
            master_port = std::stoi(argv[2]);
        } catch (const std::exception& e) {
            std::cout << "⚠️  Porta inválida, usando 8080.\n";
        }
    }
    
    // Criar cliente HTTP
    HttpClient client(master_host, master_port);
    
    print_banner();
    
    std::cout << "🌐 Servidor configurado: " << client.get_master_url() << "\n";
    std::cout << "💡 Use 'h' para ver os comandos disponíveis.\n";
    
    // Loop principal
    std::string command;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        // Limpar espaços e converter para minúsculo
        command.erase(0, command.find_first_not_of(" \t"));
        command.erase(command.find_last_not_of(" \t") + 1);
        
        if (command == "q" || command == "quit" || command == "exit") {
            std::cout << "👋 Até logo!\n";
            break;
            
        } else if (command == "h" || command == "help") {
            print_help();
            
        } else if (command == "1") {
            process_file_interactive(client);
            
        } else if (command == "2") {
            process_text_interactive(client);
            
        } else if (command == "3") {
            check_server_status(client);
            
        } else if (command == "4") {
            configure_server(client);
            
        } else {
            std::cout << "❓ Comando não reconhecido: '" << command << "'\n";
            std::cout << "   Digite 'h' para ver os comandos disponíveis.\n";
        }
    }
    
    Logger::info("Cliente finalizado");
    return 0;
}