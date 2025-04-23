#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include "../models.h"

// Estrutura principal da janela
typedef struct {
    GtkWidget *window;
    GtkWidget *header_bar;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    
    // Containers para cada seção
    GtkWidget *produtos_container;
    GtkWidget *clientes_container;
    GtkWidget *pedidos_container;
    
    // Widgets específicos para produtos
    GtkWidget *tree_produtos;
    GtkWidget *btn_novo_produto;
    GtkWidget *btn_editar_produto;
    GtkWidget *btn_excluir_produto;
    
    // Widgets específicos para clientes
    GtkWidget *tree_clientes;
    GtkWidget *btn_novo_cliente;
    GtkWidget *btn_editar_cliente;
    GtkWidget *btn_excluir_cliente;
    GtkWidget *entry_busca_cliente;
    
    // Widgets específicos para pedidos
    GtkWidget *tree_pedidos;
    GtkWidget *btn_novo_pedido;
    GtkWidget *btn_visualizar_pedido;
    GtkWidget *btn_imprimir_fatura;
} MainWindow;

// Funções principais
void criar_janela_principal(void);
void destruir_janela_principal(void);

// Funções de criação das views
void criar_produtos_view(void);
void criar_clientes_view(void);
void criar_pedidos_view(void);

// Funções de atualização das listas
void atualizar_lista_produtos(void);
void atualizar_lista_clientes(void);
void atualizar_lista_pedidos(void);

// Callbacks
void on_novo_produto_clicked(GtkButton *button, gpointer user_data);
void on_novo_cliente_clicked(GtkButton *button, gpointer user_data);
void on_novo_pedido_clicked(GtkButton *button, gpointer user_data);
void on_pesquisar_cliente_clicked(GtkButton *button, gpointer user_data);

// Variável global para acesso à janela principal
extern MainWindow *main_window;

#endif // MAIN_WINDOW_H
