#include "order_window.h"
#include "../utils.h"
#include "../database.h"

// Declaração de funções internas
static void on_metodo_pagamento_changed(GtkComboBox *widget, gpointer user_data);
static void preencher_dados_pedido(OrderWindow *win, Pedido *pedido);
static void desabilitar_edicao(OrderWindow *win);

static void criar_layout_pedido(OrderWindow *win) {
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(win->dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);
    
    // Container principal
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(content_area), main_box);
    
    // Área do cliente
    GtkWidget *cliente_frame = gtk_frame_new("Cliente");
    gtk_box_pack_start(GTK_BOX(main_box), cliente_frame, FALSE, FALSE, 0);
    
    GtkWidget *cliente_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(cliente_frame), cliente_box);
    
    win->combo_cliente = gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(cliente_box), win->combo_cliente, TRUE, TRUE, 5);
    preencher_combo_clientes(GTK_COMBO_BOX(win->combo_cliente));
    
    // Área de produtos
    GtkWidget *produtos_frame = gtk_frame_new("Produtos Disponíveis");
    gtk_box_pack_start(GTK_BOX(main_box), produtos_frame, TRUE, TRUE, 0);
    
    GtkWidget *produtos_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(produtos_frame), produtos_box);
    
    // Lista de produtos
    GtkWidget *scroll_produtos = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_produtos), 150);
    gtk_box_pack_start(GTK_BOX(produtos_box), scroll_produtos, TRUE, TRUE, 0);
    
    win->tree_produtos = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll_produtos), win->tree_produtos);
    preencher_lista_produtos(GTK_TREE_VIEW(win->tree_produtos));
    
    // Controles de quantidade
    GtkWidget *quantidade_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(produtos_box), quantidade_box, FALSE, FALSE, 5);
    
    GtkWidget *lbl_quantidade = gtk_label_new("Quantidade:");
    gtk_box_pack_start(GTK_BOX(quantidade_box), lbl_quantidade, FALSE, FALSE, 0);
    
    win->spin_quantidade = gtk_spin_button_new_with_range(1, 999, 1);
    gtk_box_pack_start(GTK_BOX(quantidade_box), win->spin_quantidade, FALSE, FALSE, 0);
    
    win->btn_adicionar_item = gtk_button_new_with_label("Adicionar ao Pedido");
    gtk_box_pack_start(GTK_BOX(quantidade_box), win->btn_adicionar_item, FALSE, FALSE, 0);
    
    // Área de itens do pedido
    GtkWidget *itens_frame = gtk_frame_new("Itens do Pedido");
    gtk_box_pack_start(GTK_BOX(main_box), itens_frame, TRUE, TRUE, 0);
    
    GtkWidget *itens_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(itens_frame), itens_box);
    
    // Lista de itens selecionados
    GtkWidget *scroll_itens = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_itens), 150);
    gtk_box_pack_start(GTK_BOX(itens_box), scroll_itens, TRUE, TRUE, 0);
    
    win->tree_itens_selecionados = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll_itens), win->tree_itens_selecionados);
    configurar_colunas_itens_pedido(GTK_TREE_VIEW(win->tree_itens_selecionados));
    
    win->btn_remover_item = gtk_button_new_with_label("Remover Item");
    gtk_box_pack_start(GTK_BOX(itens_box), win->btn_remover_item, FALSE, FALSE, 0);
    
    // Área de totais
    GtkWidget *totais_frame = gtk_frame_new("Resumo do Pedido");
    gtk_box_pack_start(GTK_BOX(main_box), totais_frame, FALSE, FALSE, 0);
    
    GtkWidget *totais_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(totais_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(totais_grid), 10);
    gtk_container_add(GTK_CONTAINER(totais_frame), totais_grid);
    
    // Labels para totais
    GtkWidget *lbl_subtotal_titulo = gtk_label_new("Subtotal:");
    GtkWidget *lbl_iva_titulo = gtk_label_new("IVA (14%):");
    GtkWidget *lbl_total_titulo = gtk_label_new("Total:");
    gtk_label_set_xalign(GTK_LABEL(lbl_subtotal_titulo), 1.0);
    gtk_label_set_xalign(GTK_LABEL(lbl_iva_titulo), 1.0);
    gtk_label_set_xalign(GTK_LABEL(lbl_total_titulo), 1.0);
    
    win->lbl_subtotal = gtk_label_new("0,00 Kz");
    win->lbl_iva = gtk_label_new("0,00 Kz");
    win->lbl_total = gtk_label_new("0,00 Kz");
    gtk_label_set_xalign(GTK_LABEL(win->lbl_subtotal), 0.0);
    gtk_label_set_xalign(GTK_LABEL(win->lbl_iva), 0.0);
    gtk_label_set_xalign(GTK_LABEL(win->lbl_total), 0.0);
    
    // Adicionar labels ao grid
    int row = 0;
    gtk_grid_attach(GTK_GRID(totais_grid), lbl_subtotal_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), win->lbl_subtotal, 1, row, 1, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(totais_grid), lbl_iva_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), win->lbl_iva, 1, row, 1, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(totais_grid), lbl_total_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(totais_grid), win->lbl_total, 1, row, 1, 1);
    
    // Área de pagamento
    GtkWidget *pagamento_frame = gtk_frame_new("Pagamento");
    gtk_box_pack_start(GTK_BOX(main_box), pagamento_frame, FALSE, FALSE, 0);
    
    GtkWidget *pagamento_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(pagamento_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(pagamento_grid), 10);
    gtk_container_add(GTK_CONTAINER(pagamento_frame), pagamento_grid);
    
    // Método de pagamento
    GtkWidget *lbl_metodo = gtk_label_new("Método:");
    win->combo_pagamento = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(win->combo_pagamento), "Dinheiro");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(win->combo_pagamento), "TPA");
    gtk_combo_box_set_active(GTK_COMBO_BOX(win->combo_pagamento), 0);
    
    // Valor pago e troco
    GtkWidget *lbl_valor_pago = gtk_label_new("Valor Pago:");
    GtkWidget *lbl_troco_titulo = gtk_label_new("Troco:");
    win->entry_valor_pago = gtk_entry_new();
    win->lbl_troco = gtk_label_new("0,00 Kz");
    
    // Adicionar widgets ao grid de pagamento
    row = 0;
    gtk_grid_attach(GTK_GRID(pagamento_grid), lbl_metodo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(pagamento_grid), win->combo_pagamento, 1, row, 1, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(pagamento_grid), lbl_valor_pago, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(pagamento_grid), win->entry_valor_pago, 1, row, 1, 1);
    
    row++;
    gtk_grid_attach(GTK_GRID(pagamento_grid), lbl_troco_titulo, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(pagamento_grid), win->lbl_troco, 1, row, 1, 1);
    
    // Conectar sinais
    g_signal_connect(win->btn_adicionar_item, "clicked", 
                    G_CALLBACK(on_adicionar_item_clicked), win);
    g_signal_connect(win->btn_remover_item, "clicked", 
                    G_CALLBACK(on_remover_item_clicked), win);
    g_signal_connect(win->entry_valor_pago, "changed", 
                    G_CALLBACK(on_valor_pago_changed), win);
    g_signal_connect(win->combo_pagamento, "changed", 
                    G_CALLBACK(on_metodo_pagamento_changed), win);
    
    // Inicializar array de itens
    win->itens = g_array_new(FALSE, TRUE, sizeof(ItemPedido));
}

static void on_metodo_pagamento_changed(GtkComboBox *widget, gpointer user_data) {
    OrderWindow *win = (OrderWindow *)user_data;
    gboolean is_dinheiro = gtk_combo_box_get_active(widget) == 0;
    
    // Habilitar/desabilitar campo de valor pago conforme método
    gtk_widget_set_sensitive(win->entry_valor_pago, is_dinheiro);
    gtk_widget_set_sensitive(win->lbl_troco, is_dinheiro);
    
    if (!is_dinheiro) {
        // Se for TPA, valor pago = total
        char buffer[32];
        g_snprintf(buffer, sizeof(buffer), "%.2f", win->total);
        gtk_entry_set_text(GTK_ENTRY(win->entry_valor_pago), buffer);
        gtk_label_set_text(GTK_LABEL(win->lbl_troco), "0,00 Kz");
    }
}

static void preencher_dados_pedido(OrderWindow *win, Pedido *pedido) {
    // Selecionar cliente
    gtk_combo_box_set_active(GTK_COMBO_BOX(win->combo_cliente), pedido->idCliente - 1);
    
    // Preencher itens
    for (int i = 0; i < pedido->numProdutos; i++) {
        ItemPedido item;
        item.id_produto = pedido->idProduto[i];
        item.quantidade = pedido->quantidade[i];
        
        Produto *prod = buscar_produto(item.id_produto);
        if (prod) {
            item.preco_unitario = prod->preco;
            item.iva_unitario = calcular_iva(prod->preco);
            item.subtotal = item.preco_unitario * item.quantidade;
            item.iva_total = item.iva_unitario * item.quantidade;
            item.total = item.subtotal + item.iva_total;
            
            g_array_append_val(win->itens, item);
        }
    }
    
    // Atualizar totais
    atualizar_totais_pedido(win);
    
    // Preencher método de pagamento
    if (strcmp(pedido->metodoPagamento, "TPA") == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(win->combo_pagamento), 1);
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(win->combo_pagamento), 0);
    }
    
    // Preencher valor pago
    char buffer[32];
    g_snprintf(buffer, sizeof(buffer), "%.2f", pedido->valorPago);
    gtk_entry_set_text(GTK_ENTRY(win->entry_valor_pago), buffer);
}

static void desabilitar_edicao(OrderWindow *win) {
    gtk_widget_set_sensitive(win->combo_cliente, FALSE);
    gtk_widget_set_sensitive(win->tree_produtos, FALSE);
    gtk_widget_set_sensitive(win->spin_quantidade, FALSE);
    gtk_widget_set_sensitive(win->btn_adicionar_item, FALSE);
    gtk_widget_set_sensitive(win->btn_remover_item, FALSE);
    gtk_widget_set_sensitive(win->combo_pagamento, FALSE);
    gtk_widget_set_sensitive(win->entry_valor_pago, FALSE);
}

void mostrar_dialogo_pedido(GtkWindow *parent, Pedido *pedido) {
    OrderWindow *win = g_new0(OrderWindow, 1);
    win->pedido = pedido;
    win->modo_visualizacao = (pedido != NULL);
    
    // Criar diálogo
    const char *titulo = win->modo_visualizacao ? "Visualizar Pedido" : "Novo Pedido";
    win->dialog = gtk_dialog_new_with_buttons(titulo,
                                            parent,
                                            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                            "_Cancelar", GTK_RESPONSE_CANCEL,
                                            win->modo_visualizacao ? "_Fechar" : "_Finalizar",
                                            GTK_RESPONSE_ACCEPT,
                                            NULL);
    
    // Criar layout
    criar_layout_pedido(win);
    
    // Se estiver em modo visualização, preencher e desabilitar campos
    if (win->modo_visualizacao) {
        preencher_dados_pedido(win, pedido);
        desabilitar_edicao(win);
    }
    
    // Conectar sinal de resposta
    g_signal_connect(win->dialog, "response", G_CALLBACK(on_pedido_response), win);
    
    // Mostrar diálogo
    gtk_widget_show_all(win->dialog);
}

void atualizar_totais_pedido(OrderWindow *win) {
    win->subtotal = 0;
    win->iva_total = 0;
    win->total = 0;
    
    for (guint i = 0; i < win->itens->len; i++) {
        ItemPedido *item = &g_array_index(win->itens, ItemPedido, i);
        win->subtotal += item->subtotal;
        win->iva_total += item->iva_total;
        win->total += item->total;
    }
    
    // Atualizar labels com os valores formatados
    char buffer[32];
    
    formatar_kwanza(win->subtotal, buffer, sizeof(buffer));
    gtk_label_set_text(GTK_LABEL(win->lbl_subtotal), buffer);
    
    formatar_kwanza(win->iva_total, buffer, sizeof(buffer));
    gtk_label_set_text(GTK_LABEL(win->lbl_iva), buffer);
    
    formatar_kwanza(win->total, buffer, sizeof(buffer));
    gtk_label_set_text(GTK_LABEL(win->lbl_total), buffer);
    
    // Atualizar troco se necessário
    atualizar_troco(win);
}

void atualizar_troco(OrderWindow *win) {
    const char *valor_pago_texto = gtk_entry_get_text(GTK_ENTRY(win->entry_valor_pago));
    win->valor_pago = g_ascii_strtod(valor_pago_texto, NULL);
    win->troco = win->valor_pago - win->total;
    
    char buffer[32];
    formatar_kwanza(win->troco, buffer, sizeof(buffer));
    gtk_label_set_text(GTK_LABEL(win->lbl_troco), buffer);
}

gboolean validar_pedido(OrderWindow *win, GtkWindow *parent) {
    if (win->itens->len == 0) {
        mostrar_erro(parent, "Adicione pelo menos um item ao pedido.");
        return FALSE;
    }

    if (gtk_combo_box_get_active(GTK_COMBO_BOX(win->combo_cliente)) < 0) {
        mostrar_erro(parent, "Selecione um cliente.");
        return FALSE;
    }

    const char *valor_pago_texto = gtk_entry_get_text(GTK_ENTRY(win->entry_valor_pago));
    if (strlen(valor_pago_texto) == 0) {
        mostrar_erro(parent, "Informe o valor pago.");
        return FALSE;
    }

    double valor_pago = g_ascii_strtod(valor_pago_texto, NULL);
    if (valor_pago < win->total) {
        mostrar_erro(parent, "O valor pago é menor que o total do pedido.");
        return FALSE;
    }

    return TRUE;
}

void on_adicionar_item_clicked(GtkButton *button, gpointer user_data) {
    (void)button;  // Parâmetro intencionalmente não utilizado
    OrderWindow *win = (OrderWindow *)user_data;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(win->tree_produtos));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id_produto;
        char *nome;
        double preco;
        gtk_tree_model_get(model, &iter,
                          0, &id_produto,
                          1, &nome,
                          2, &preco,
                          -1);

        int quantidade = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(win->spin_quantidade));
        
        // Criar item do pedido
        ItemPedido item;
        item.id_produto = id_produto;
        item.quantidade = quantidade;
        item.preco_unitario = preco;
        item.iva_unitario = calcular_iva(preco);
        item.subtotal = preco * quantidade;
        item.iva_total = item.iva_unitario * quantidade;
        item.total = item.subtotal + item.iva_total;

        // Adicionar à lista
        g_array_append_val(win->itens, item);

        // Atualizar TreeView
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(win->tree_itens_selecionados)));
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, nome,
                          1, quantidade,
                          2, item.preco_unitario,
                          3, item.iva_unitario,
                          4, item.subtotal,
                          5, item.iva_total,
                          6, item.total,
                          -1);

        g_free(nome);
        atualizar_totais_pedido(win);
    }
}

void on_remover_item_clicked(GtkButton *button, gpointer user_data) {
    (void)button;  // Parâmetro intencionalmente não utilizado
    OrderWindow *win = (OrderWindow *)user_data;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(win->tree_itens_selecionados));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gint index;
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        index = gtk_tree_path_get_indices(path)[0];
        gtk_tree_path_free(path);

        // Remover do array
        g_array_remove_index(win->itens, index);

        // Remover da TreeView
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

        atualizar_totais_pedido(win);
    }
}

void on_valor_pago_changed(GtkEditable *editable, gpointer user_data) {
    (void)editable;  // Parâmetro intencionalmente não utilizado
    OrderWindow *win = (OrderWindow *)user_data;
    atualizar_troco(win);
}

void on_pedido_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    OrderWindow *win = (OrderWindow *)user_data;
    
    if (response_id == GTK_RESPONSE_ACCEPT && !win->modo_visualizacao) {
        if (!validar_pedido(win, GTK_WINDOW(dialog))) {
            return;
        }
        
        // Criar novo pedido
        Pedido pedido = {0};
        pedido.id = 0; // Será definido pela função adicionar_pedido
        
        // Preencher dados do cliente
        pedido.idCliente = gtk_combo_box_get_active(GTK_COMBO_BOX(win->combo_cliente)) + 1;
        
        // Preencher dados dos produtos
        pedido.numProdutos = win->itens->len;
        for (guint i = 0; i < win->itens->len; i++) {
            ItemPedido *item = &g_array_index(win->itens, ItemPedido, i);
            pedido.idProduto[i] = item->id_produto;
            pedido.quantidade[i] = item->quantidade;
        }
        
        // Preencher valores
        pedido.subtotal = win->subtotal;
        pedido.iva_total = win->iva_total;
        pedido.total = win->total;
        pedido.valorPago = win->valor_pago;
        pedido.troco = win->troco;
        
        // Método de pagamento
        const char *metodo = gtk_combo_box_text_get_active_text(
            GTK_COMBO_BOX_TEXT(win->combo_pagamento));
        strncpy(pedido.metodoPagamento, metodo, sizeof(pedido.metodoPagamento) - 1);
        
        // Data e hora atual
        obter_data_atual(pedido.data, sizeof(pedido.data));
        obter_hora_atual(pedido.hora, sizeof(pedido.hora));
        
        // Salvar pedido
        if (adicionar_pedido(&pedido)) {
            mostrar_sucesso(GTK_WINDOW(dialog), "Pedido realizado com sucesso!");
            gerar_fatura(&pedido); // Gerar fatura em PDF
            atualizar_lista_pedidos(); // Função definida em main_window.c
        } else {
            mostrar_erro(GTK_WINDOW(dialog), 
                        "Erro ao salvar pedido. Por favor, tente novamente.");
            return;
        }
    }
    
    // Limpar memória
    g_array_free(win->itens, TRUE);
    gtk_widget_destroy(GTK_WIDGET(dialog));
    g_free(win);
}
