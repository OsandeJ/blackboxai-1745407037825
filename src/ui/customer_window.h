#ifndef CUSTOMER_WINDOW_H
#define CUSTOMER_WINDOW_H

#include <gtk/gtk.h>
#include "../models.h"

// Estrutura para a janela de cliente
typedef struct {
    GtkWidget *dialog;
    GtkWidget *entry_nome;
    GtkWidget *entry_telefone;
    GtkWidget *entry_nif;
    GtkWidget *entry_endereco;
    Cliente *cliente;  // NULL para novo cliente, ponteiro para cliente existente em caso de edição
    gboolean modo_edicao;
} CustomerWindow;

// Funções principais
void mostrar_dialogo_cliente(GtkWindow *parent, Cliente *cliente);

// Funções de atualização (definidas em main_window.h)
void atualizar_lista_clientes(void);

// Callbacks
void on_cliente_response(GtkDialog *dialog, gint response_id, gpointer user_data);
void on_nif_changed(GtkEditable *editable, gpointer user_data);
void on_telefone_changed(GtkEditable *editable, gpointer user_data);

// Funções auxiliares
gboolean validar_dados_cliente(CustomerWindow *win, GtkWindow *parent);
void preencher_dados_cliente(CustomerWindow *win, Cliente *cliente);
void limpar_campos_cliente(CustomerWindow *win);

// Funções de validação específicas
gboolean validar_formato_nif(const char *nif);
gboolean validar_formato_telefone(const char *telefone);

// Funções de formatação
void formatar_nif(GtkEntry *entry);
void formatar_telefone(GtkEntry *entry);

#endif // CUSTOMER_WINDOW_H
