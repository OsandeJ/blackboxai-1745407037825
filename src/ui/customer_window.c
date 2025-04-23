#include "customer_window.h"
#include "../utils.h"
#include "../database.h"

static void criar_layout_cliente(CustomerWindow *win) {
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(win->dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
    
    // Grid para organizar os campos
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    
    // Labels
    GtkWidget *label_nome = gtk_label_new("Nome do Cliente:");
    GtkWidget *label_telefone = gtk_label_new("Telefone:");
    GtkWidget *label_nif = gtk_label_new("NIF:");
    GtkWidget *label_endereco = gtk_label_new("Endereço:");
    
    gtk_label_set_xalign(GTK_LABEL(label_nome), 0);
    gtk_label_set_xalign(GTK_LABEL(label_telefone), 0);
    gtk_label_set_xalign(GTK_LABEL(label_nif), 0);
    gtk_label_set_xalign(GTK_LABEL(label_endereco), 0);
    
    // Campos de entrada
    win->entry_nome = gtk_entry_new();
    win->entry_telefone = gtk_entry_new();
    win->entry_nif = gtk_entry_new();
    win->entry_endereco = gtk_text_view_new();
    
    // Configurar campo de endereço com scroll
    GtkWidget *scroll_endereco = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_endereco), 60);
    gtk_container_add(GTK_CONTAINER(scroll_endereco), win->entry_endereco);
    
    // Configurar placeholders e limites
    gtk_entry_set_placeholder_text(GTK_ENTRY(win->entry_nome), "Nome completo");
    gtk_entry_set_placeholder_text(GTK_ENTRY(win->entry_telefone), "Ex: 923456789");
    gtk_entry_set_placeholder_text(GTK_ENTRY(win->entry_nif), "Número de Identificação Fiscal");
    
    gtk_entry_set_max_length(GTK_ENTRY(win->entry_telefone), 9);  // Formato angolano: 9 dígitos
    gtk_entry_set_max_length(GTK_ENTRY(win->entry_nif), 9);       // NIF angolano: 9 dígitos
    
    // Conectar sinais para validação em tempo real
    g_signal_connect(win->entry_telefone, "changed", G_CALLBACK(on_telefone_changed), win);
    g_signal_connect(win->entry_nif, "changed", G_CALLBACK(on_nif_changed), win);
    
    // Adicionar widgets ao grid
    int row = 0;
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->entry_nome, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_telefone, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->entry_telefone, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_nif, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->entry_nif, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_endereco, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_endereco, 1, row, 2, 1);
    
    // Estilizar widgets
    gtk_widget_set_hexpand(win->entry_nome, TRUE);
    gtk_widget_set_hexpand(win->entry_telefone, TRUE);
    gtk_widget_set_hexpand(win->entry_nif, TRUE);
    gtk_widget_set_hexpand(scroll_endereco, TRUE);
}

void mostrar_dialogo_cliente(GtkWindow *parent, Cliente *cliente) {
    CustomerWindow *win = g_new0(CustomerWindow, 1);
    win->cliente = cliente;
    win->modo_edicao = (cliente != NULL);
    
    // Criar diálogo
    const char *titulo = win->modo_edicao ? "Editar Cliente" : "Novo Cliente";
    win->dialog = gtk_dialog_new_with_buttons(titulo,
                                            parent,
                                            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                            "_Cancelar", GTK_RESPONSE_CANCEL,
                                            "_Salvar", GTK_RESPONSE_ACCEPT,
                                            NULL);
    
    // Criar layout
    criar_layout_cliente(win);
    
    // Se estiver editando, preencher campos com dados existentes
    if (win->modo_edicao) {
        gtk_entry_set_text(GTK_ENTRY(win->entry_nome), cliente->nome);
        gtk_entry_set_text(GTK_ENTRY(win->entry_telefone), cliente->telefone);
        gtk_entry_set_text(GTK_ENTRY(win->entry_nif), cliente->nif);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->entry_endereco));
        gtk_text_buffer_set_text(buffer, cliente->endereco, -1);
    }
    
    // Conectar sinais
    g_signal_connect(win->dialog, "response", G_CALLBACK(on_cliente_response), win);
    
    // Mostrar diálogo
    gtk_widget_show_all(win->dialog);
}

gboolean validar_formato_nif(const char *nif) {
    // Validar NIF angolano (9 dígitos)
    if (strlen(nif) != 9) {
        return FALSE;
    }
    
    for (int i = 0; nif[i]; i++) {
        if (!g_ascii_isdigit(nif[i])) {
            return FALSE;
        }
    }
    
    return TRUE;
}

gboolean validar_formato_telefone(const char *telefone) {
    // Validar telefone angolano (9 dígitos, começando com 9)
    if (strlen(telefone) != 9 || telefone[0] != '9') {
        return FALSE;
    }
    
    for (int i = 0; telefone[i]; i++) {
        if (!g_ascii_isdigit(telefone[i])) {
            return FALSE;
        }
    }
    
    return TRUE;
}

void on_telefone_changed(GtkEditable *editable, gpointer user_data) {
    (void)user_data;  // Parâmetro intencionalmente não utilizado
    const char *texto = gtk_entry_get_text(GTK_ENTRY(editable));
    
    // Remover caracteres não numéricos
    GString *numeros = g_string_new("");
    for (const char *c = texto; *c; c++) {
        if (g_ascii_isdigit(*c)) {
            g_string_append_c(numeros, *c);
        }
    }
    
    // Limitar a 9 dígitos
    if (numeros->len > 9) {
        g_string_truncate(numeros, 9);
    }
    
    // Atualizar o campo se o texto foi modificado
    if (strcmp(texto, numeros->str) != 0) {
        gtk_entry_set_text(GTK_ENTRY(editable), numeros->str);
    }
    
    g_string_free(numeros, TRUE);
}

void on_nif_changed(GtkEditable *editable, gpointer user_data) {
    (void)user_data;  // Parâmetro intencionalmente não utilizado
    const char *texto = gtk_entry_get_text(GTK_ENTRY(editable));
    
    // Remover caracteres não numéricos
    GString *numeros = g_string_new("");
    for (const char *c = texto; *c; c++) {
        if (g_ascii_isdigit(*c)) {
            g_string_append_c(numeros, *c);
        }
    }
    
    // Limitar a 9 dígitos
    if (numeros->len > 9) {
        g_string_truncate(numeros, 9);
    }
    
    // Atualizar o campo se o texto foi modificado
    if (strcmp(texto, numeros->str) != 0) {
        gtk_entry_set_text(GTK_ENTRY(editable), numeros->str);
    }
    
    g_string_free(numeros, TRUE);
}

gboolean validar_dados_cliente(CustomerWindow *win, GtkWindow *parent) {
    const char *nome = gtk_entry_get_text(GTK_ENTRY(win->entry_nome));
    const char *telefone = gtk_entry_get_text(GTK_ENTRY(win->entry_telefone));
    const char *nif = gtk_entry_get_text(GTK_ENTRY(win->entry_nif));
    
    if (strlen(nome) == 0) {
        mostrar_erro(parent, "O nome do cliente é obrigatório.");
        return FALSE;
    }
    
    if (!validar_formato_telefone(telefone)) {
        mostrar_erro(parent, "Telefone inválido. Deve ter 9 dígitos e começar com 9.");
        return FALSE;
    }
    
    if (strlen(nif) > 0 && !validar_formato_nif(nif)) {
        mostrar_erro(parent, "NIF inválido. Deve ter 9 dígitos numéricos.");
        return FALSE;
    }
    
    return TRUE;
}

void on_cliente_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    CustomerWindow *win = (CustomerWindow *)user_data;
    
    if (response_id == GTK_RESPONSE_ACCEPT) {
        if (!validar_dados_cliente(win, GTK_WINDOW(dialog))) {
            return;
        }
        
        // Criar ou atualizar cliente
        Cliente cliente;
        if (win->modo_edicao) {
            cliente = *(win->cliente);
        } else {
            cliente.id = 0; // Será definido pela função adicionar_cliente
        }
        
        // Preencher dados do cliente
        strncpy(cliente.nome, gtk_entry_get_text(GTK_ENTRY(win->entry_nome)), 
                sizeof(cliente.nome) - 1);
        strncpy(cliente.telefone, gtk_entry_get_text(GTK_ENTRY(win->entry_telefone)), 
                sizeof(cliente.telefone) - 1);
        strncpy(cliente.nif, gtk_entry_get_text(GTK_ENTRY(win->entry_nif)), 
                sizeof(cliente.nif) - 1);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->entry_endereco));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *endereco = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        strncpy(cliente.endereco, endereco, sizeof(cliente.endereco) - 1);
        g_free(endereco);
        
        // Salvar cliente
        gboolean sucesso;
        if (win->modo_edicao) {
            sucesso = atualizar_cliente(&cliente);
        } else {
            sucesso = adicionar_cliente(&cliente);
        }
        
        if (sucesso) {
            mostrar_sucesso(GTK_WINDOW(dialog), 
                           win->modo_edicao ? "Cliente atualizado com sucesso!" 
                                          : "Cliente cadastrado com sucesso!");
            atualizar_lista_clientes(); // Função definida em main_window.c
        } else {
            mostrar_erro(GTK_WINDOW(dialog), 
                        "Erro ao salvar cliente. Por favor, tente novamente.");
            return;
        }
    }
    
    gtk_widget_destroy(GTK_WIDGET(dialog));
    g_free(win);
}
