#!/bin/bash
# test.sh

echo "🧪 Testando Sistema Distribuído..."

# Teste 1: Health checks
echo "📋 Verificando saúde dos serviços..."
curl -f http://localhost:8080/health && echo "✅ Mestre OK"
curl -f http://localhost:8081/health && echo "✅ Escravo Letras OK"  
curl -f http://localhost:8082/health && echo "✅ Escravo Números OK"

# Teste 2: Processamento
echo "🔄 Testando processamento..."
result=$(curl -s -X POST http://localhost:8080/process \
  -H "Content-Type: application/json" \
  -d '{"text":"Hello123World456"}')

echo "📊 Resultado: $result"
echo "✅ Testes concluídos!"