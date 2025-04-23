#include "product_window.h"
#include "../utils.h"
#include "../database.h"

static void criar_layout_produto(ProductWindow *win) {
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(win->dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
    
    // Grid para organizar os campos
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    
    // Labels
    GtkWidget *label_nome = gtk_label_new("Nome do Produto:");
    GtkWidget *label_descricao = gtk_label_new("Descrição:");
    GtkWidget *label_preco = gtk_label_new("Preço Base (Kz):");
    gtk_label_set_xalign(GTK_LABEL(label_nome), 0);
    gtk_label_set_xalign(GTK_LABEL(label_descricao), 0);
    gtk_label_set_xalign(GTK_LABEL(label_preco), 0);
    
    // Campos de entrada
    win->entry_nome = gtk_entry_new();
    win->entry_descricao = gtk_text_view_new();
    win->entry_preco = gtk_entry_new();
    
    // Configurar campo de descrição
    GtkWidget *scroll_desc = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_desc), 60);
    gtk_container_add(GTK_CONTAINER(scroll_desc), win->entry_descricao);
    
    // Configurar campo de preço
    gtk_entry_set_input_purpose(GTK_ENTRY(win->entry_preco), GTK_INPUT_PURPOSE_NUMBER);
    g_signal_connect(win->entry_preco, "changed", G_CALLBACK(on_preco_changed), win);
    
    // Labels para exibição de valores calculados
    GtkWidget *label_iva_titulo = gtk_label_new("IVA (14%):");
    GtkWidget *label_total_titulo = gtk_label_new("Preço com IVA:");
    win->label_iva = gtk_label_new("0,00 Kz");
    win->label_preco_total = gtk_label_new("0,00 Kz");
    
    gtk_label_set_xalign(GTK_LABEL(label_iva_titulo), 0);
    gtk_label_set_xalign(GTK_LABEL(label_total_titulo), 0);
    gtk_label_set_xalign(GTK_LABEL(win->label_iva), 0);
    gtk_label_set_xalign(GTK_LABEL(win->label_preco_total), 0);
    
    // Adicionar widgets ao grid
    int row = 0;
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->entry_nome, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_descricao, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_desc, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_preco, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->entry_preco, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_iva_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->label_iva, 1, row, 2, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(grid), label_total_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), win->label_preco_total, 1, row, 2, 1);
    
    // Estilizar widgets
    gtk_widget_set_hexpand(win->entry_nome, TRUE);
    gtk_widget_set_hexpand(scroll_desc, TRUE);
    gtk_widget_set_hexpand(win->entry_preco, TRUE);
    
    // Adicionar classes CSS para estilização
    GtkStyleContext *context;
    
    context = gtk_widget_get_style_context(win->label_iva);
    gtk_style_context_add_class(context, "valor-calculado");
    
    context = gtk_widget_get_style_context(win->label_preco_total);
    gtk_style_context_add_class(context, "valor-calculado");
    gtk_style_context_add_class(context, "valor-total");
}

void mostrar_dialogo_produto(GtkWindow *parent, Produto *produto) {
    ProductWindow *win = g_new0(ProductWindow, 1);
    win->produto = produto;
    win->modo_edicao = (produto != NULL);
    
    // Criar diálogo
    const char *titulo = win->modo_edicao ? "Editar Produto" : "Novo Produto";
    win->dialog = gtk_dialog_new_with_buttons(titulo,
                                            parent,
                                            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                            "_Cancelar", GTK_RESPONSE_CANCEL,
                                            "_Salvar", GTK_RESPONSE_ACCEPT,
                                            NULL);
    
    // Criar layout
    criar_layout_produto(win);
    
    // Se estiver editando, preencher campos com dados existentes
    if (win->modo_edicao) {
        gtk_entry_set_text(GTK_ENTRY(win->entry_nome), produto->nome);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->entry_descricao));
        gtk_text_buffer_set_text(buffer, produto->descricao, -1);
        
        char preco[32];
        g_snprintf(preco, sizeof(preco), "%.2f", produto->preco);
        gtk_entry_set_text(GTK_ENTRY(win->entry_preco), preco);
        
        atualizar_calculos_produto(win);
    }
    
    // Conectar sinais
    g_signal_connect(win->dialog, "response", G_CALLBACK(on_produto_response), win);
    
    // Mostrar diálogo
    gtk_widget_show_all(win->dialog);
}

void atualizar_calculos_produto(ProductWindow *win) {
    const char *texto_preco = gtk_entry_get_text(GTK_ENTRY(win->entry_preco));
    double preco_base = g_ascii_strtod(texto_preco, NULL);
    
    // Calcular IVA e total
    double iva = calcular_iva(preco_base);
    double total = calcular_preco_com_iva(preco_base);
    
    // Formatar valores em Kwanza
    char buffer_iva[32], buffer_total[32];
    formatar_kwanza(iva, buffer_iva, sizeof(buffer_iva));
    formatar_kwanza(total, buffer_total, sizeof(buffer_total));
    
    // Atualizar labels
    gtk_label_set_text(GTK_LABEL(win->label_iva), buffer_iva);
    gtk_label_set_text(GTK_LABEL(win->label_preco_total), buffer_total);
}

void on_preco_changed(GtkEditable *editable, gpointer user_data) {
    (void)editable;  // Parâmetro intencionalmente não utilizado
    ProductWindow *win = (ProductWindow *)user_data;
    atualizar_calculos_produto(win);
}

gboolean validar_dados_produto(ProductWindow *win, GtkWindow *parent) {
    const char *nome = gtk_entry_get_text(GTK_ENTRY(win->entry_nome));
    const char *preco_texto = gtk_entry_get_text(GTK_ENTRY(win->entry_preco));
    
    if (strlen(nome) == 0) {
        mostrar_erro(parent, "O nome do produto é obrigatório.");
        return FALSE;
    }
    
    if (strlen(preco_texto) == 0) {
        mostrar_erro(parent, "O preço do produto é obrigatório.");
        return FALSE;
    }
    
    double preco = g_ascii_strtod(preco_texto, NULL);
    if (preco <= 0) {
        mostrar_erro(parent, "O preço deve ser maior que zero.");
        return FALSE;
    }
    
    return TRUE;
}

void on_produto_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    ProductWindow *win = (ProductWindow *)user_data;
    
    if (response_id == GTK_RESPONSE_ACCEPT) {
        if (!validar_dados_produto(win, GTK_WINDOW(dialog))) {
            return;
        }
        
        // Criar ou atualizar produto
        Produto produto;
        if (win->modo_edicao) {
            produto = *(win->produto);
        } else {
            produto.id = 0; // Será definido pela função adicionar_produto
        }
        
        // Preencher dados do produto
        strncpy(produto.nome, gtk_entry_get_text(GTK_ENTRY(win->entry_nome)), 
                sizeof(produto.nome) - 1);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->entry_descricao));
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *descricao = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        strncpy(produto.descricao, descricao, sizeof(produto.descricao) - 1);
        g_free(descricao);
        
        produto.preco = g_ascii_strtod(gtk_entry_get_text(GTK_ENTRY(win->entry_preco)), NULL);
        produto.preco_iva = calcular_preco_com_iva(produto.preco);
        
        // Salvar produto
        gboolean sucesso;
        if (win->modo_edicao) {
            sucesso = atualizar_produto(&produto);
        } else {
            sucesso = adicionar_produto(&produto);
        }
        
        if (sucesso) {
            mostrar_sucesso(GTK_WINDOW(dialog), 
                           win->modo_edicao ? "Produto atualizado com sucesso!" 
                                          : "Produto cadastrado com sucesso!");
            atualizar_lista_produtos(); // Função definida em main_window.c
        } else {
            mostrar_erro(GTK_WINDOW(dialog), 
                        "Erro ao salvar produto. Por favor, tente novamente.");
            return;
        }
    }
    
    gtk_widget_destroy(GTK_WIDGET(dialog));
    g_free(win);
}
