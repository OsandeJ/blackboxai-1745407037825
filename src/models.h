#ifndef MODELS_H
#define MODELS_H

#include <gtk/gtk.h>

#define MAX_PRODUTOS 100
#define MAX_CLIENTES 100
#define MAX_PEDIDOS 100
#define IVA_RATE 0.14  // 14% IVA em Angola

typedef struct {
    int id;
    char nome[50];
    char descricao[100];
    double preco;          // Preço sem IVA
    double preco_iva;      // Preço com IVA incluído
} Produto;

typedef struct {
    int id;
    char nome[50];
    char telefone[15];
    char nif[20];         // Número de Identificação Fiscal
    char endereco[100];   // Endereço para fatura
} Cliente;

typedef struct {
    int id;
    int idCliente;
    int idProduto[MAX_PRODUTOS];
    int quantidade[MAX_PRODUTOS];
    int numProdutos;
    double subtotal;       // Valor total sem IVA
    double iva_total;      // Valor total do IVA
    double total;          // Valor total com IVA
    double valorPago;
    double troco;
    char metodoPagamento[20];
    char data[20];        // Data do pedido
    char hora[20];        // Hora do pedido
} Pedido;

// Funções auxiliares para cálculos
double calcular_preco_com_iva(double preco);
double calcular_iva(double preco);

// Estruturas para widgets GTK reutilizáveis
typedef struct {
    GtkWidget *window;
    GtkWidget *entry_nome;
    GtkWidget *entry_descricao;
    GtkWidget *entry_preco;
    GtkWidget *label_preco_iva;
} ProdutoWindow;

typedef struct {
    GtkWidget *window;
    GtkWidget *entry_nome;
    GtkWidget *entry_telefone;
    GtkWidget *entry_nif;
    GtkWidget *entry_endereco;
} ClienteWindow;

typedef struct {
    GtkWidget *window;
    GtkWidget *combo_cliente;
    GtkWidget *tree_produtos;
    GtkWidget *label_subtotal;
    GtkWidget *label_iva;
    GtkWidget *label_total;
    GtkWidget *entry_valor_pago;
    GtkWidget *label_troco;
    GtkWidget *combo_pagamento;
} PedidoWindow;

#endif // MODELS_H
