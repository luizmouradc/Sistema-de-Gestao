# Sistema de Gestão — Projeto da disciplina de Programação em C

Este projeto é uma aplicação de terminal em **C** para gerenciamento de **Clientes** e **Produtos**.  
O objetivo é praticar o uso de **ponteiros**, **vetores dinâmicos** e **estruturas**, criando um sistema simples de cadastro e controle de estoque sem persistência em arquivos.

---

## Funcionalidades

### Clientes
- Adicionar um novo cliente  
- Listar todos os clientes cadastrados  
- Editar dados de um cliente (nome, email, telefone)  
- Remover cliente pelo ID  

### Produtos
- Adicionar um novo produto  
- Listar todos os produtos cadastrados  
- Editar informações do produto (nome, preço, estoque)  
- Entrada e saída de estoque (com validações)  
- Remover produto pelo ID  

### Gerais
- IDs autoincrementais  
- Menus interativos no terminal  
- Validação de entradas numéricas  

---

## Tecnologias Utilizadas

- **C (padrão C11)**  
- **Ponteiros** para manipulação de vetores dinâmicos  
- **Alocação dinâmica de memória** com `malloc`, `realloc` e `free`  
- Entrada e saída padrão (stdio)  

---

## Como Executar o Projeto

1. Clone este repositório:

```bash
git clone https://github.com/usuario/gestao-clientes-produtos.git
```
2. Compile o código:
```bash
gcc -std=c11 -O2 -Wall -Wextra gestao.c -o gestao
```
3. Execute o programa:
```bash
./gestao
```

## Exemplo de uso
```diff
=== Sistema de Gestao: Clientes e Produtos ===

Menu principal
1) Clientes
2) Produtos
0) Sair
> 1
```
