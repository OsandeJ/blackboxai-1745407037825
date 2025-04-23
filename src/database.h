#ifndef DATABASE_H
#define DATABASE_H

#include <gtk/gtk.h>
#include "models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Variáveis globais para armazenamento em memória
extern Produto produtos[MAX_PRODUTOS];
extern Cliente clientes[MAX_CLIENTES];
extern Pedido pedidos[MAX_PEDIDOS];
extern int num_produtos;
extern int num_clientes;
extern int num_pedidos;

// Funções para produtos
gboolean salvar_produtos(void);
gboolean carregar_produtos(void);
gboolean adicionar_produto(Produto *produto);
gboolean atualizar_produto(Produto *produto);
gboolean remover_produto(int id);
Produto* buscar_produto(int id);
void calcular_valores_produto(Produto *produto); // Calcula preço com IVA

// Funções para clientes
gboolean salvar_clientes(void);
gboolean carregar_clientes(void);
gboolean adicionar_cliente(Cliente *cliente);
gboolean atualizar_cliente(Cliente *cliente);
gboolean remover_cliente(int id);
Cliente* buscar_cliente(int id);
Cliente* buscar_cliente_por_nome(const char *nome);

// Funções para pedidos
gboolean salvar_pedidos(void);
gboolean carregar_pedidos(void);
gboolean adicionar_pedido(Pedido *pedido);
Pedido* buscar_pedido(int id);
void calcular_valores_pedido(Pedido *pedido); // Calcula subtotal, IVA e total

// Funções para geração de documentos
void gerar_fatura_pdf(Pedido *pedido);
void gerar_relatorio_vendas_pdf(const char *data_inicio, const char *data_fim);

// Funções de backup
gboolean fazer_backup(const char *diretorio);
gboolean restaurar_backup(const char *arquivo);

// Funções de inicialização e finalização
gboolean inicializar_database(void);
void finalizar_database(void);

// Funções auxiliares
void criar_diretorios_necessarios(void);
gboolean verificar_integridade_dados(void);

// Constantes para nomes de arquivos
#define ARQUIVO_PRODUTOS "data/produtos.dat"
#define ARQUIVO_CLIENTES "data/clientes.dat"
#define ARQUIVO_PEDIDOS "data/pedidos.dat"
#define DIRETORIO_BACKUP "data/backup"
#define DIRETORIO_FATURAS "data/faturas"

// Códigos de erro
typedef enum {
    DB_SUCCESS = 0,
    DB_ERROR_ARQUIVO_NAO_ENCONTRADO,
    DB_ERROR_PERMISSAO_NEGADA,
    DB_ERROR_MEMORIA_INSUFICIENTE,
    DB_ERROR_DADOS_INVALIDOS,
    DB_ERROR_LIMITE_EXCEDIDO,
    DB_ERROR_REGISTRO_NAO_ENCONTRADO,
    DB_ERROR_BACKUP_FALHOU,
    DB_ERROR_RESTAURACAO_FALHOU
} DatabaseError;

// Estrutura para resultado de operações
typedef struct {
    DatabaseError erro;
    char mensagem[256];
} DatabaseResult;

// Função para obter mensagem de erro
const char* obter_mensagem_erro(DatabaseError erro);

#endif // DATABASE_H
