# 📸 Screenshots do Sistema

Esta pasta contém as imagens utilizadas na documentação do projeto.

## 🖼️ Imagens Necessárias

### 1. Screenshot do Servidor
**Arquivo:** `servidor_screenshot.png`
**Descrição:** Captura de tela mostrando:
- Terminal com docker-compose logs
- Status dos contêineres rodando
- Logs em tempo real dos serviços

### 2. Screenshot do Cliente
**Arquivo:** `cliente_screenshot.png`
**Descrição:** Captura de tela da interface PyQt5 mostrando:
- Interface gráfica do cliente
- Campos de configuração do servidor
- Área de entrada de texto
- Resultados de processamento
- Barra de status

## 📝 Instruções para Captura

### Para o Servidor:
1. Execute: `docker-compose up`
2. Capture o terminal mostrando os logs
3. Inclua na imagem: status dos contêineres e logs em tempo real

### Para o Cliente:
1. Execute: `python client.py`
2. Configure um servidor
3. Processe um texto de exemplo
4. Capture a tela com os resultados visíveis

## 🎨 Especificações das Imagens

- **Formato:** PNG
- **Resolução:** Mínimo 800x600, Máximo 1920x1080
- **Qualidade:** Alta (sem compressão excessiva)
- **Conteúdo:** Interface limpa, texto legível

## 📂 Estrutura Final

```
docs/images/
├── README.md (este arquivo)
├── servidor_screenshot.png
└── cliente_screenshot.png
```

Após adicionar as imagens, elas aparecerão automaticamente no README principal do projeto.