#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include "../models.h"
#include "../database.h"
#include "../utils.h"

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
    GtkWidget *dashboard_container;
    
    // Widgets específicos para produtos
    GtkWidget *tree_produtos;
    GtkWidget *btn_novo_produto;
    GtkWidget *btn_editar_produto;
    GtkWidget *btn_excluir_produto;
    GtkWidget *lbl_total_produtos;
    
    // Widgets específicos para clientes
    GtkWidget *tree_clientes;
    GtkWidget *btn_novo_cliente;
    GtkWidget *btn_editar_cliente;
    GtkWidget *btn_excluir_cliente;
    GtkWidget *entry_busca_cliente;
    GtkWidget *lbl_total_clientes;
    
    // Widgets específicos para pedidos
    GtkWidget *tree_pedidos;
    GtkWidget *btn_novo_pedido;
    GtkWidget *btn_visualizar_pedido;
    GtkWidget *btn_imprimir_fatura;
    GtkWidget *lbl_total_vendas;
    GtkWidget *lbl_total_iva;
    
    // Widgets para dashboard
    GtkWidget *chart_vendas_diarias;
    GtkWidget *chart_produtos_populares;
    GtkWidget *lbl_resumo_dia;
} MainWindow;

// Funções da janela principal
void criar_janela_principal(void);
void destruir_janela_principal(void);

// Funções de atualização das listas
void atualizar_lista_produtos(void);
void atualizar_lista_clientes(void);
void atualizar_lista_pedidos(void);
void atualizar_dashboard(void);

// Funções de callback para eventos
void on_novo_produto_clicked(GtkButton *button, gpointer user_data);
void on_novo_cliente_clicked(GtkButton *button, gpointer user_data);
void on_novo_pedido_clicked(GtkButton *button, gpointer user_data);

// Funções auxiliares
void configurar_colunas_produtos(GtkTreeView *tree_view);
void configurar_colunas_clientes(GtkTreeView *tree_view);
void configurar_colunas_pedidos(GtkTreeView *tree_view);
void preencher_combo_clientes(GtkComboBox *combo);
void preencher_lista_produtos(GtkTreeView *tree_view);
void configurar_colunas_itens_pedido(GtkTreeView *tree_view);

// Funções de formatação e exibição
void atualizar_totais(void);
void formatar_valor_kwanza(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                          GtkTreeModel *model, GtkTreeIter *iter, gpointer data);

// Funções de exportação
void exportar_relatorio_pdf(void);
void exportar_dados_excel(void);

// Variável global para acesso à janela principal
extern MainWindow *main_window;

#endif // MAIN_WINDOW_H
