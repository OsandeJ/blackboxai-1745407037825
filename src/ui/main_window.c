#include "main_window.h"
#include "product_window.h"
#include "customer_window.h"
#include "order_window.h"
#include "../database.h"
#include "../utils.h"

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
    
    main_window->stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(main_window->stack_switcher), 
                                GTK_STACK(main_window->stack));
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(main_window->header_bar),
                                   main_window->stack_switcher);
}

void criar_produtos_view(void) {
    main_window->produtos_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Botões de ação
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    main_window->btn_novo_produto = gtk_button_new_with_label("Novo Produto");
    main_window->btn_editar_produto = gtk_button_new_with_label("Editar");
    main_window->btn_excluir_produto = gtk_button_new_with_label("Excluir");
    
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_novo_produto, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_editar_produto, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_excluir_produto, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_window->produtos_container), btn_box, FALSE, FALSE, 5);
    
    // TreeView para listar produtos
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    main_window->tree_produtos = gtk_tree_view_new();
    
    // Colunas da lista de produtos
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_PRODUTO_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", COL_PRODUTO_NOME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Preço sem IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço", renderer, "text", COL_PRODUTO_PRECO, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA (14%)", renderer, "text", COL_PRODUTO_IVA, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    // Coluna Preço com IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Preço com IVA", renderer, "text", COL_PRODUTO_PRECO_TOTAL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_produtos), column);
    
    gtk_container_add(GTK_CONTAINER(scroll), main_window->tree_produtos);
    gtk_box_pack_start(GTK_BOX(main_window->produtos_container), scroll, TRUE, TRUE, 0);
    
    // Conectar sinais
    g_signal_connect(main_window->btn_novo_produto, "clicked", G_CALLBACK(on_novo_produto_clicked), NULL);
}

void criar_clientes_view(void) {
    main_window->clientes_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Botões de ação
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    main_window->btn_novo_cliente = gtk_button_new_with_label("Novo Cliente");
    main_window->btn_editar_cliente = gtk_button_new_with_label("Editar");
    main_window->btn_excluir_cliente = gtk_button_new_with_label("Excluir");
    
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_novo_cliente, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_editar_cliente, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_excluir_cliente, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_window->clientes_container), btn_box, FALSE, FALSE, 5);
    
    // Campo de pesquisa
    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    main_window->entry_busca_cliente = gtk_entry_new();
    GtkWidget *btn_pesquisar = gtk_button_new_with_label("Pesquisar");
    gtk_entry_set_placeholder_text(GTK_ENTRY(main_window->entry_busca_cliente), "Pesquisar cliente por nome...");
    gtk_box_pack_start(GTK_BOX(search_box), main_window->entry_busca_cliente, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(search_box), btn_pesquisar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_window->clientes_container), search_box, FALSE, FALSE, 5);
    
    // TreeView para listar clientes
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    main_window->tree_clientes = gtk_tree_view_new();
    
    // Colunas da lista de clientes
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_CLIENTE_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_clientes), column);
    
    // Coluna Nome
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nome", renderer, "text", COL_CLIENTE_NOME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_clientes), column);
    
    // Coluna Telefone
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Telefone", renderer, "text", COL_CLIENTE_TELEFONE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_clientes), column);
    
    // Coluna NIF
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("NIF", renderer, "text", COL_CLIENTE_NIF, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_clientes), column);
    
    gtk_container_add(GTK_CONTAINER(scroll), main_window->tree_clientes);
    gtk_box_pack_start(GTK_BOX(main_window->clientes_container), scroll, TRUE, TRUE, 0);
    
    // Conectar sinais
    g_signal_connect(main_window->btn_novo_cliente, "clicked", G_CALLBACK(on_novo_cliente_clicked), NULL);
    g_signal_connect(btn_pesquisar, "clicked", G_CALLBACK(on_pesquisar_cliente_clicked), main_window->entry_busca_cliente);
}

void criar_pedidos_view(void) {
    main_window->pedidos_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Botões de ação
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    main_window->btn_novo_pedido = gtk_button_new_with_label("Novo Pedido");
    main_window->btn_visualizar_pedido = gtk_button_new_with_label("Visualizar");
    main_window->btn_imprimir_fatura = gtk_button_new_with_label("Imprimir Fatura");
    
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_novo_pedido, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_visualizar_pedido, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(btn_box), main_window->btn_imprimir_fatura, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_window->pedidos_container), btn_box, FALSE, FALSE, 5);
    
    // TreeView para listar pedidos
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    main_window->tree_pedidos = gtk_tree_view_new();
    
    // Colunas da lista de pedidos
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    // Coluna ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COL_PEDIDO_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    // Coluna Data
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Data", renderer, "text", COL_PEDIDO_DATA, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    // Coluna Cliente
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Cliente", renderer, "text", COL_PEDIDO_CLIENTE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    // Coluna Subtotal
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Subtotal", renderer, "text", COL_PEDIDO_SUBTOTAL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    // Coluna IVA
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IVA", renderer, "text", COL_PEDIDO_IVA, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    // Coluna Total
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", COL_PEDIDO_TOTAL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(main_window->tree_pedidos), column);
    
    gtk_container_add(GTK_CONTAINER(scroll), main_window->tree_pedidos);
    gtk_box_pack_start(GTK_BOX(main_window->pedidos_container), scroll, TRUE, TRUE, 0);
    
    // Conectar sinais
    g_signal_connect(main_window->btn_novo_pedido, "clicked", G_CALLBACK(on_novo_pedido_clicked), NULL);
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
    criar_clientes_view();
    criar_pedidos_view();
    
    // Adicionar as views ao stack com títulos
    gtk_stack_add_titled(GTK_STACK(main_window->stack), 
                        main_window->produtos_container, "produtos", "Produtos");
    gtk_stack_add_titled(GTK_STACK(main_window->stack), 
                        main_window->clientes_container, "clientes", "Clientes");
    gtk_stack_add_titled(GTK_STACK(main_window->stack), 
                        main_window->pedidos_container, "pedidos", "Pedidos");
    
    // Adicionar stack à janela principal
    gtk_container_add(GTK_CONTAINER(main_window->window), main_window->stack);
    
    // Conectar sinais para fechar o programa corretamente
    g_signal_connect(main_window->window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(main_window->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Mostrar todos os widgets
    gtk_widget_show_all(main_window->window);
    
    // Carregar dados iniciais
    atualizar_lista_produtos();
    atualizar_lista_clientes();
    atualizar_lista_pedidos();
}

void destruir_janela_principal(void) {
    if (main_window) {
        g_free(main_window);
        main_window = NULL;
    }
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

void on_novo_produto_clicked(GtkButton *button, gpointer user_data) {
    (void)button;      // Parâmetro intencionalmente não utilizado
    (void)user_data;   // Parâmetro intencionalmente não utilizado
    mostrar_dialogo_produto(GTK_WINDOW(main_window->window), NULL);
}

void on_novo_cliente_clicked(GtkButton *button, gpointer user_data) {
    (void)button;      // Parâmetro intencionalmente não utilizado
    (void)user_data;   // Parâmetro intencionalmente não utilizado
    mostrar_dialogo_cliente(GTK_WINDOW(main_window->window), NULL);
}

void on_novo_pedido_clicked(GtkButton *button, gpointer user_data) {
    (void)button;      // Parâmetro intencionalmente não utilizado
    (void)user_data;   // Parâmetro intencionalmente não utilizado
    mostrar_dialogo_pedido(GTK_WINDOW(main_window->window), NULL);
}

void on_pesquisar_cliente_clicked(GtkButton *button, gpointer user_data) {
    (void)button;      // Parâmetro intencionalmente não utilizado
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *texto = gtk_entry_get_text(entry);
    
    // TODO: Implementar pesquisa de cliente
    g_print("Pesquisando cliente: %s\n", texto);
}
