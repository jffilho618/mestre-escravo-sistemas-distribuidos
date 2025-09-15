# Dockerfile para o Cliente (opcional - pode rodar fora do container)
FROM ubuntu:22.04

# Evitar prompts interativos durante a instalação
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências do sistema
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    pkg-config \
    libssl-dev \
    zlib1g-dev \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Criar diretório de trabalho
WORKDIR /app

# Copiar arquivos de build
COPY CMakeLists.txt .
COPY src/ ./src/
COPY input_files/ ./input_files/

# Compilar a aplicação
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Configurar usuário não-root para segurança
RUN useradd -m -u 1000 appuser && \
    chown -R appuser:appuser /app
USER appuser

# Comando padrão (modo interativo)
CMD ["./build/client"]

# Labels para metadados
LABEL maintainer="Sistema Distribuído C++"
LABEL version="1.0"
LABEL description="Cliente para envio de arquivos de texto"