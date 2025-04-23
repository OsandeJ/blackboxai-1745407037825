#ifndef ORDER_WINDOW_H
#define ORDER_WINDOW_H

#include <gtk/gtk.h>
#include "../models.h"

// Estrutura para item do pedido
typedef struct {
    int id_produto;
    int quantidade;
    double preco_unitario;
    double iva_unitario;
    double subtotal;
    double iva_total;
    double total;
} ItemPedido;

// Estrutura para a janela de pedido
typedef struct {
    GtkWidget *dialog;
    GtkWidget *combo_cliente;
    GtkWidget *tree_produtos;
    GtkWidget *tree_itens_selecionados;
    GtkWidget *spin_quantidade;
    GtkWidget *btn_adicionar_item;
    GtkWidget *btn_remover_item;
    GtkWidget *lbl_subtotal;
    GtkWidget *lbl_iva;
    GtkWidget *lbl_total;
    GtkWidget *combo_pagamento;
    GtkWidget *entry_valor_pago;
    GtkWidget *lbl_troco;
    
    // Armazenamento temporário dos itens
    GArray *itens;
    
    // Totalizadores
    double subtotal;
    double iva_total;
    double total;
    double valor_pago;
    double troco;
    
    Pedido *pedido;  // NULL para novo pedido, ponteiro para pedido existente em visualização
    gboolean modo_visualizacao;
} OrderWindow;

// Funções principais
void mostrar_dialogo_pedido(GtkWindow *parent, Pedido *pedido);
void mostrar_dialogo_visualizar_pedido(GtkWindow *parent, Pedido *pedido);
void atualizar_totais_pedido(OrderWindow *win);
void atualizar_troco(OrderWindow *win);

// Funções de preenchimento de widgets
void preencher_combo_clientes(GtkComboBox *combo);
void preencher_lista_produtos(GtkTreeView *tree_view);
void configurar_colunas_itens_pedido(GtkTreeView *tree_view);

// Funções de atualização (definidas em main_window.h)
void atualizar_lista_pedidos(void);

// Callbacks
void on_cliente_changed(GtkComboBox *widget, gpointer user_data);
void on_produto_selecionado(GtkTreeSelection *selection, gpointer user_data);
void on_quantidade_changed(GtkSpinButton *spin_button, gpointer user_data);
void on_adicionar_item_clicked(GtkButton *button, gpointer user_data);
void on_remover_item_clicked(GtkButton *button, gpointer user_data);
void on_valor_pago_changed(GtkEditable *editable, gpointer user_data);
void on_pedido_response(GtkDialog *dialog, gint response_id, gpointer user_data);

// Funções auxiliares
void atualizar_totais_pedido(OrderWindow *win);
void atualizar_troco(OrderWindow *win);
gboolean validar_pedido(OrderWindow *win, GtkWindow *parent);
void gerar_fatura(Pedido *pedido);
void limpar_campos_pedido(OrderWindow *win);

// Funções de formatação e exibição
void preencher_combo_clientes(GtkComboBox *combo);
void preencher_lista_produtos(GtkTreeView *tree_view);
void configurar_colunas_itens_pedido(GtkTreeView *tree_view);

#endif // ORDER_WINDOW_H
