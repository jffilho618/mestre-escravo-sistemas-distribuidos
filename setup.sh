#!/bin/bash

# Sistema Distribuído C++ - Script de Setup
# Este script configura o ambiente e verifica dependências

set -e  # Parar em caso de erro

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para log colorido
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Banner
print_banner() {
    echo ""
    echo "================================================"
    echo "   SISTEMA DISTRIBUÍDO C++ - SETUP SCRIPT"
    echo "================================================"
    echo ""
}

# Verificar se comando existe
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Verificar dependências
check_dependencies() {
    log_info "Verificando dependências..."
    
    local missing_deps=()
    
    if ! command_exists docker; then
        missing_deps+=("docker")
    fi
    
    if ! command_exists docker-compose; then
        missing_deps+=("docker-compose")
    fi
    
    if ! command_exists git; then
        missing_deps+=("git")
    fi
    
    if ! command_exists curl; then
        missing_deps+=("curl")
    fi
    
    if [ ${#missing_deps[@]} -eq 0 ]; then
        log_success "Todas as dependências estão instaladas!"
        return 0
    else
        log_error "Dependências faltando: ${missing_deps[*]}"
        log_info "Por favor, instale as dependências e execute o script novamente."
        return 1
    fi
}

# Verificar se Docker está rodando
check_docker_running() {
    log_info "Verificando se Docker está rodando..."
    
    if docker info >/dev/null 2>&1; then
        log_success "Docker está rodando!"
        return 0
    else
        log_error "Docker não está rodando ou você não tem permissões."
        log_info "Inicie o Docker e/ou adicione seu usuário ao grupo docker:"
        log_info "  sudo systemctl start docker"
        log_info "  sudo usermod -aG docker \$USER"
        log_info "  # Depois faça logout/login"
        return 1
    fi
}

# Criar estrutura de diretórios
create_structure() {
    log_info "Criando estrutura de diretórios..."
    
    # Criar diretórios se não existirem
    mkdir -p client/src client/input_files
    mkdir -p master/src
    mkdir -p slave_letters/src  
    mkdir -p slave_numbers/src
    
    log_success "Estrutura de diretórios criada!"
}

# Verificar se os arquivos fonte existem
check_source_files() {
    log_info "Verificando arquivos fonte..."
    
    local required_files=(
        "client/src/main.cpp"
        "client/src/http_client.cpp"
        "client/src/http_client.h"
        "client/CMakeLists.txt"
        "client/Dockerfile"
        "master/src/main.cpp"
        "master/src/master_server.cpp"
        "master/src/master_server.h"
        "master/CMakeLists.txt"
        "master/Dockerfile"
        "slave_letters/src/main.cpp"
        "slave_letters/src/letters_server.cpp"
        "slave_letters/CMakeLists.txt"
        "slave_numbers/src/main.cpp"
        "slave_numbers/src/numbers_server.cpp"
        "slave_numbers/CMakeLists.txt"
        "docker-compose.yml"
    )
    
    local missing_files=()
    
    for file in "${required_files[@]}"; do
        if [ ! -f "$file" ]; then
            missing_files+=("$file")
        fi
    done
    
    if [ ${#missing_files[@]} -eq 0 ]; then
        log_success "Todos os arquivos fonte estão presentes!"
        return 0
    else
        log_warning "Arquivos faltando:"
        for file in "${missing_files[@]}"; do
            echo "  - $file"
        done
        log_info "Certifique-se de copiar todos os arquivos gerados pelo Claude."
        return 1
    fi
}

# Criar arquivo de exemplo se não existir
create_example_file() {
    local example_file="client/input_files/exemplo.txt"
    
    if [ ! -f "$example_file" ]; then
        log_info "Criando arquivo de exemplo..."
        
        cat > "$example_file" << 'EOF'
Sistema Distribuído em C++ - Teste de Funcionalidade

Este arquivo contém uma mistura de letras e números para testar o sistema distribuído.

Dados de exemplo:
- CPF: 123.456.789-00
- Telefone: (11) 99999-8888
- CEP: 01234-567
- Idade: 25 anos
- Salário: R$ 5.500,00

Texto descritivo:
Lorem ipsum dolor sit amet, consectetur adipiscing elit. 
Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco.

Sequências mistas:
abc123def456ghi789
XYZ987PQR654MNO321
test2024version1.5.3

Caracteres especiais e pontuação:
!@#$%^&*()_+-={}[]|\:";'<>?,./

Números diversos:
Pi = 3.14159265359
E = 2.71828182846
Ano = 2024
Versão = 1.0

Fim do arquivo de teste.
Total esperado aproximado:
- Letras: ~180-200
- Números: ~40-60
EOF

        log_success "Arquivo de exemplo criado: $example_file"
    else
        log_info "Arquivo de exemplo já existe: $example_file"
    fi
}

# Verificar portas disponíveis
check_ports() {
    log_info "Verificando disponibilidade das portas..."
    
    local ports=(8080 8081 8082)
    local busy_ports=()
    
    for port in "${ports[@]}"; do
        if ss -tuln | grep -q ":$port "; then
            busy_ports+=("$port")
        fi
    done
    
    if [ ${#busy_ports[@]} -eq 0 ]; then
        log_success "Todas as portas necessárias estão disponíveis!"
        return 0
    else
        log_warning "Portas ocupadas: ${busy_ports[*]}"
        log_info "Você pode:"
        log_info "  1. Parar os serviços que usam essas portas"
        log_info "  2. Modificar as portas no docker-compose.yml"
        return 1
    fi
}

# Build dos containers
build_containers() {
    log_info "Construindo containers Docker..."
    
    if docker-compose build; then
        log_success "Containers construídos com sucesso!"
        return 0
    else
        log_error "Falha ao construir containers."
        return 1
    fi
}

# Compilar cliente localmente (opcional)
compile_client() {
    log_info "Compilando cliente localmente..."
    
    if command_exists cmake && command_exists make; then
        cd client
        
        if [ ! -d "build" ]; then
            mkdir build
        fi
        
        cd build
        
        if cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc); then
            log_success "Cliente compilado com sucesso!"
            log_info "Executável em: client/build/client"
        else
            log_warning "Falha ao compilar cliente localmente."
            log_info "Você ainda pode usar o cliente no container."
        fi
        
        cd ../..
    else
        log_warning "CMake ou Make não encontrado. Pulando compilação local."
        log_info "Você pode usar o cliente no container."
    fi
}

# Criar script de teste
create_test_script() {
    local test_script="test_system.sh"
    
    log_info "Criando script de teste..."
    
    cat > "$test_script" << 'EOF'
#!/bin/bash

# Script de teste do sistema distribuído

echo "🧪 Testando Sistema Distribuído..."

# Aguardar serviços iniciarem
echo "⏳ Aguardando serviços iniciarem..."
sleep 10

# Teste 1: Health checks
echo ""
echo "📋 Verificando saúde dos serviços..."

if curl -f -s http://localhost:8080/health > /dev/null; then
    echo "✅ Mestre OK"
else
    echo "❌ Mestre FALHOU"
    exit 1
fi

if curl -f -s http://localhost:8081/health > /dev/null; then
    echo "✅ Escravo Letras OK"
else
    echo "❌ Escravo Letras FALHOU"
    exit 1
fi

if curl -f -s http://localhost:8082/health > /dev/null; then
    echo "✅ Escravo Números OK"
else
    echo "❌ Escravo Números FALHOU"  
    exit 1
fi

# Teste 2: Processamento
echo ""
echo "🔄 Testando processamento..."

result=$(curl -s -X POST http://localhost:8080/process \
  -H "Content-Type: application/json" \
  -d '{"text":"Hello123World456"}')

if echo "$result" | grep -q '"success"'; then
    echo "✅ Processamento OK"
else
    echo "❌ Processamento FALHOU"
    echo "Resposta: $result"
    exit 1
fi

echo ""
echo "🎉 Todos os testes passaram!"
echo "📊 Resultado do teste: $result"
EOF

    chmod +x "$test_script"
    log_success "Script de teste criado: $test_script"
}

# Mostrar próximos passos
show_next_steps() {
    echo ""
    echo "================================================"
    echo "   SETUP CONCLUÍDO COM SUCESSO!"
    echo "================================================"
    echo ""
    log_info "Próximos passos:"
    echo ""
    echo "1. 🚀 Iniciar o sistema:"
    echo "   docker-compose up --build"
    echo ""
    echo "2. 🧪 Testar o sistema:"
    echo "   ./test_system.sh"
    echo ""
    echo "3. 💻 Usar o cliente:"
    echo "   # Opção A: Cliente local (se compilado)"
    echo "   cd client/build && ./client"
    echo ""
    echo "   # Opção B: Cliente no container"
    echo "   docker-compose --profile client up client"
    echo ""
    echo "4. 📋 Monitorar logs:"
    echo "   docker-compose logs -f"
    echo ""
    echo "5. 🛑 Parar o sistema:"
    echo "   docker-compose down"
    echo ""
    log_success "Divirta-se com seu sistema distribuído! 🎉"
}

# Função principal
main() {
    print_banner
    
    # Verificações
    check_dependencies || exit 1
    check_docker_running || exit 1
    
    # Setup
    create_structure
    check_source_files
    create_example_file
    check_ports
    
    # Build (opcional, pode ser feito depois)
    read -p "Construir containers agora? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        build_containers
    else
        log_info "Você pode construir depois com: docker-compose build"
    fi
    
    # Compilação local (opcional)
    read -p "Compilar cliente localmente? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        compile_client
    fi
    
    # Criar utilitários
    create_test_script
    
    # Finalizar
    show_next_steps
}

# Executar se script for chamado diretamente
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi