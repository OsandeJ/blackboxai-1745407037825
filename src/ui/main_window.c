#include "main_window.h"
#include "product_window.h"
#include "customer_window.h"
#include "order_window.h"

// Variável global
MainWindow *main_window = NULL;

// Enums para as colunas das TreeViews
enum {
    COL_PRODUTO_ID,
    COL_PRODUTO_NOME,
    COL_PRODUTO_PRECO,
    COL_PRODUTO_IVA,
    COL_PRODUTO_PRECO_TOTAL,
    NUM_COLS_PRODUTO
};

enum {
    COL_CLIENTE_ID,
    COL_CLIENTE_NOME,
    COL_CLIENTE_TELEFONE,
    COL_CLIENTE_NIF,
    NUM_COLS_CLIENTE
};

enum {
    COL_PEDIDO_ID,
    COL_PEDIDO_DATA,
    COL_PEDIDO_CLIENTE,
    COL_PEDIDO_SUBTOTAL,
    COL_PEDIDO_IVA,
    COL_PEDIDO_TOTAL,
    NUM_COLS_PEDIDO
};

static void criar_header_bar(void) {
    main_window->header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(main_window->header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(main_window->header_bar), "Sistema de Gestão");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(main_window->header_bar), "Gestão de Vendas com IVA");
    
    // Adicionar stack switcher ao header bar
    main_window->stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(main_window->stack_switcher), 
                                GTK_STACK(main_window->stack));
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(main_window->header_bar),
                                   main_window->stack_switcher);
}

static void configurar_colunas_produtos_impl(void) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_PRODUTO_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", COL_PRODUTO_NOME, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Preço Base
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço Base", renderer, "text", COL_PRODUTO_PRECO, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(COL_PRODUTO_PRECO), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA (14%)", renderer, "text", COL_PRODUTO_IVA, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(COL_PRODUTO_IVA), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Preço Total
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço com IVA", renderer, "text", COL_PRODUTO_PRECO_TOTAL, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(COL_PRODUTO_PRECO_TOTAL), NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
}

void criar_produtos_view(void) {
    main_window->produtos_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Barra de ferramentas
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_window->produtos_container), toolbar, FALSE, FALSE, 5);
    
    // Botões
    main_window->btn_novo_produto = gtk_button_new_with_label("Novo Produto");
    main_window->btn_editar_produto = gtk_button_new_with_label("Editar");
    main_window->btn_excluir_produto = gtk_button_new_with_label("Excluir");
    
    gtk_box_pack_start(GTK_BOX(toolbar), main_window->btn_novo_produto, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), main_window->btn_editar_produto, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), main_window->btn_excluir_produto, FALSE, FALSE, 0);
    
    // TreeView
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    main_window->tree_produtos = gtk_tree_view_new();
    configurar_colunas_produtos_impl();
    
    gtk_container_add(GTK_CONTAINER(scroll), main_window->tree_produtos);
    gtk_box_pack_start(GTK_BOX(main_window->produtos_container), scroll, TRUE, TRUE, 0);
    
    // Label para total
    main_window->lbl_total_produtos = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(main_window->produtos_container), 
                      main_window->lbl_total_produtos, FALSE, FALSE, 5);
    
    // Conectar sinais
    g_signal_connect(main_window->btn_novo_produto, "clicked", 
                    G_CALLBACK(on_novo_produto_clicked), NULL);
    
    gtk_stack_add_titled(GTK_STACK(main_window->stack), 
                        main_window->produtos_container, "produtos", "Produtos");
}

void atualizar_lista_produtos(void) {
    GtkListStore *store = gtk_list_store_new(NUM_COLS_PRODUTO,
                                            G_TYPE_INT,    // ID
                                            G_TYPE_STRING, // Nome
                                            G_TYPE_DOUBLE, // Preço Base
                                            G_TYPE_DOUBLE, // IVA
                                            G_TYPE_DOUBLE  // Preço Total
                                            );
    
    GtkTreeIter iter;
    for (int i = 0; i < num_produtos; i++) {
        Produto *p = &produtos[i];
        double iva = calcular_iva(p->preco);
        double total = calcular_preco_com_iva(p->preco);
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          COL_PRODUTO_ID, p->id,
                          COL_PRODUTO_NOME, p->nome,
                          COL_PRODUTO_PRECO, p->preco,
                          COL_PRODUTO_IVA, iva,
                          COL_PRODUTO_PRECO_TOTAL, total,
                          -1);
    }
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(main_window->tree_produtos), 
                           GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Atualizar label de total
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Total de Produtos: %d", num_produtos);
    gtk_label_set_text(GTK_LABEL(main_window->lbl_total_produtos), buffer);
}

void formatar_valor_kwanza(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                          GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    (void)col; // Parâmetro intencionalmente não utilizado
    double valor;
    int coluna = GPOINTER_TO_INT(data);
    
    gtk_tree_model_get(model, iter, coluna, &valor, -1);
    
    char buffer[50];
    formatar_kwanza(valor, buffer, sizeof(buffer));
    
    g_object_set(renderer, "text", buffer, NULL);
}

void criar_janela_principal(void) {
    // Alocar e inicializar a estrutura principal
    main_window = g_malloc(sizeof(MainWindow));
    
    // Criar janela principal
    main_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window->window), "Sistema de Gestão");
    gtk_window_set_default_size(GTK_WINDOW(main_window->window), 1024, 768);
    
    // Criar stack para as diferentes views
    main_window->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(main_window->stack), 
                                GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    
    // Criar header bar
    criar_header_bar();
    gtk_window_set_titlebar(GTK_WINDOW(main_window->window), main_window->header_bar);
    
    // Criar as diferentes views
    criar_produtos_view();
    // TODO: Implementar outras views (clientes, pedidos, dashboard)
    
    // Adicionar stack à janela principal
    gtk_container_add(GTK_CONTAINER(main_window->window), main_window->stack);
    
    // Conectar sinais para fechar o programa corretamente
    g_signal_connect(main_window->window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(main_window->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Mostrar todos os widgets
    gtk_widget_show_all(main_window->window);
    
    // Carregar dados iniciais
    atualizar_lista_produtos();
}

void destruir_janela_principal(void) {
    if (main_window) {
        g_free(main_window);
        main_window = NULL;
    }
}

void on_novo_produto_clicked(GtkButton *button, gpointer user_data) {
    (void)button;      // Parâmetro intencionalmente não utilizado
    (void)user_data;   // Parâmetro intencionalmente não utilizado
    mostrar_dialogo_produto(GTK_WINDOW(main_window->window), NULL);
}
void atualizar_lista_clientes(void) {
    GtkListStore *store = gtk_list_store_new(NUM_COLS_CLIENTE,
                                            G_TYPE_INT,    // ID
                                            G_TYPE_STRING, // Nome
                                            G_TYPE_STRING, // Telefone
                                            G_TYPE_STRING  // NIF
                                            );
    
    GtkTreeIter iter;
    for (int i = 0; i < num_clientes; i++) {
        Cliente *c = &clientes[i];
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          COL_CLIENTE_ID, c->id,
                          COL_CLIENTE_NOME, c->nome,
                          COL_CLIENTE_TELEFONE, c->telefone,
                          COL_CLIENTE_NIF, c->nif,
                          -1);
    }
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(main_window->tree_clientes), 
                           GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void atualizar_lista_pedidos(void) {
    GtkListStore *store = gtk_list_store_new(NUM_COLS_PEDIDO,
                                            G_TYPE_INT,    // ID
                                            G_TYPE_STRING, // Data
                                            G_TYPE_STRING, // Cliente
                                            G_TYPE_DOUBLE, // Subtotal
                                            G_TYPE_DOUBLE, // IVA
                                            G_TYPE_DOUBLE  // Total
                                            );
    
    GtkTreeIter iter;
    for (int i = 0; i < num_pedidos; i++) {
        Pedido *p = &pedidos[i];
        Cliente *c = buscar_cliente(p->idCliente);
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          COL_PEDIDO_ID, p->id,
                          COL_PEDIDO_DATA, p->data,
                          COL_PEDIDO_CLIENTE, c ? c->nome : "Cliente não encontrado",
                          COL_PEDIDO_SUBTOTAL, p->subtotal,
                          COL_PEDIDO_IVA, p->iva_total,
                          COL_PEDIDO_TOTAL, p->total,
                          -1);
    }
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(main_window->tree_pedidos), 
                           GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void preencher_combo_clientes(GtkComboBox *combo) {
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    GtkTreeIter iter;
    
    for (int i = 0; i < num_clientes; i++) {
        Cliente *c = &clientes[i];
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, c->id,
                          1, c->nome,
                          -1);
    }
    
    gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer,
                                 "text", 1,
                                 NULL);
}

void preencher_lista_produtos(GtkTreeView *tree_view) {
    GtkListStore *store = gtk_list_store_new(5,
                                            G_TYPE_INT,    // ID
                                            G_TYPE_STRING, // Nome
                                            G_TYPE_DOUBLE, // Preço Base
                                            G_TYPE_DOUBLE, // IVA
                                            G_TYPE_DOUBLE  // Preço Total
                                            );
    
    GtkTreeIter iter;
    for (int i = 0; i < num_produtos; i++) {
        Produto *p = &produtos[i];
        double iva = calcular_iva(p->preco);
        double total = calcular_preco_com_iva(p->preco);
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, p->id,
                          1, p->nome,
                          2, p->preco,
                          3, iva,
                          4, total,
                          -1);
    }
    
    gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Configurar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(2), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(3), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(4), NULL);
    gtk_tree_view_append_column(tree_view, column);
}

void configurar_colunas_itens_pedido(GtkTreeView *tree_view) {
    GtkListStore *store = gtk_list_store_new(7,
                                            G_TYPE_STRING, // Produto
                                            G_TYPE_INT,    // Quantidade
                                            G_TYPE_DOUBLE, // Preço Unit
                                            G_TYPE_DOUBLE, // IVA Unit
                                            G_TYPE_DOUBLE, // Subtotal
                                            G_TYPE_DOUBLE, // IVA Total
                                            G_TYPE_DOUBLE  // Total
                                            );
    
    gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Configurar colunas
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Produto", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Qtd", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço Unit.", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(2), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA Unit.", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(3), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Subtotal", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(4), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA Total", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(5), NULL);
    gtk_tree_view_append_column(tree_view, column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer, formatar_valor_kwanza, 
                                           GINT_TO_POINTER(6), NULL);
    gtk_tree_view_append_column(tree_view, column);
}
