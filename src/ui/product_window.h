#ifndef PRODUCT_WINDOW_H
#define PRODUCT_WINDOW_H

#include <gtk/gtk.h>
#include "../models.h"

// Estrutura para a janela de produto
typedef struct {
    GtkWidget *dialog;
    GtkWidget *entry_nome;
    GtkWidget *entry_descricao;
    GtkWidget *entry_preco;
    GtkWidget *label_iva;
    GtkWidget *label_preco_total;
    Produto *produto;  // NULL para novo produto, ponteiro para produto existente em caso de edição
    gboolean modo_edicao;
} ProductWindow;

// Funções principais
void mostrar_dialogo_produto(GtkWindow *parent, Produto *produto);
void atualizar_calculos_produto(ProductWindow *win);

// Funções de atualização (definidas em main_window.h)
void atualizar_lista_produtos(void);

// Callbacks
void on_preco_changed(GtkEditable *editable, gpointer user_data);
void on_produto_response(GtkDialog *dialog, gint response_id, gpointer user_data);

// Funções auxiliares
gboolean validar_dados_produto(ProductWindow *win, GtkWindow *parent);
void preencher_dados_produto(ProductWindow *win, Produto *produto);
void limpar_campos_produto(ProductWindow *win);

#endif // PRODUCT_WINDOW_H
