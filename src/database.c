#include "database.h"
#include "utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <cairo.h>
#include <cairo-pdf.h>



// Variáveis globais
Produto produtos[MAX_PRODUTOS];
Cliente clientes[MAX_CLIENTES];
Pedido pedidos[MAX_PEDIDOS];
int num_produtos = 0;
int num_clientes = 0;
int num_pedidos = 0;

// Mensagens de erro
static const char* mensagens_erro[] = {
    "Operação realizada com sucesso",
    "Arquivo não encontrado",
    "Permissão negada",
    "Memória insuficiente",
    "Dados inválidos",
    "Limite máximo excedido",
    "Registro não encontrado",
    "Falha no backup",
    "Falha na restauração"
};

const char* obter_mensagem_erro(DatabaseError erro) {
    if (erro >= 0 && erro <= DB_ERROR_RESTAURACAO_FALHOU) {
        return mensagens_erro[erro];
    }
    return "Erro desconhecido";
}

// Funções auxiliares
static void criar_diretorio_se_nao_existe(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        #ifdef _WIN32
            mkdir(path);
        #else
            mkdir(path, 0700);
        #endif
    }
}

void criar_diretorios_necessarios(void) {
    criar_diretorio_se_nao_existe("data");
    criar_diretorio_se_nao_existe("data/backup");
    criar_diretorio_se_nao_existe("data/faturas");
}

// Implementação das funções de produtos
void calcular_valores_produto(Produto *produto) {
    produto->preco_iva = calcular_preco_com_iva(produto->preco);
}

gboolean salvar_produtos(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "wb");
    if (!arquivo) {
        return FALSE;
    }

    fwrite(&num_produtos, sizeof(int), 1, arquivo);
    fwrite(produtos, sizeof(Produto), num_produtos, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean carregar_produtos(void) {
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "rb");
    if (!arquivo) {
        num_produtos = 0;
        return FALSE;
    }

    fread(&num_produtos, sizeof(int), 1, arquivo);
    fread(produtos, sizeof(Produto), num_produtos, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean adicionar_produto(Produto *produto) {
    if (num_produtos >= MAX_PRODUTOS) {
        return FALSE;
    }

    produto->id = num_produtos + 1;
    calcular_valores_produto(produto);
    produtos[num_produtos++] = *produto;
    return salvar_produtos();
}

gboolean atualizar_produto(Produto *produto) {
    for (int i = 0; i < num_produtos; i++) {
        if (produtos[i].id == produto->id) {
            calcular_valores_produto(produto);
            produtos[i] = *produto;
            return salvar_produtos();
        }
    }
    return FALSE;
}

gboolean atualizar_cliente(Cliente *cliente) {
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].id == cliente->id) {
            clientes[i] = *cliente;
            return salvar_clientes();
        }
    }
    return FALSE;
}

Produto* buscar_produto(int id) {
    for (int i = 0; i < num_produtos; i++) {
        if (produtos[i].id == id) {
            return &produtos[i];
        }
    }
    return NULL;
}

// Implementação das funções de clientes
gboolean salvar_clientes(void) {
    FILE *arquivo = fopen(ARQUIVO_CLIENTES, "wb");
    if (!arquivo) {
        return FALSE;
    }

    fwrite(&num_clientes, sizeof(int), 1, arquivo);
    fwrite(clientes, sizeof(Cliente), num_clientes, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean carregar_clientes(void) {
    FILE *arquivo = fopen(ARQUIVO_CLIENTES, "rb");
    if (!arquivo) {
        num_clientes = 0;
        return FALSE;
    }

    fread(&num_clientes, sizeof(int), 1, arquivo);
    fread(clientes, sizeof(Cliente), num_clientes, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean adicionar_cliente(Cliente *cliente) {
    if (num_clientes >= MAX_CLIENTES) {
        return FALSE;
    }

    cliente->id = num_clientes + 1;
    clientes[num_clientes++] = *cliente;
    return salvar_clientes();
}

Cliente* buscar_cliente(int id) {
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].id == id) {
            return &clientes[i];
        }
    }
    return NULL;
}

// Implementação das funções de pedidos
void calcular_valores_pedido(Pedido *pedido) {
    pedido->subtotal = 0;
    pedido->iva_total = 0;
    pedido->total = 0;

    for (int i = 0; i < pedido->numProdutos; i++) {
        Produto *prod = buscar_produto(pedido->idProduto[i]);
        if (prod) {
            double subtotal_item = prod->preco * pedido->quantidade[i];
            double iva_item = calcular_iva(subtotal_item);
            
            pedido->subtotal += subtotal_item;
            pedido->iva_total += iva_item;
        }
    }
    
    pedido->total = pedido->subtotal + pedido->iva_total;
}

gboolean salvar_pedidos(void) {
    FILE *arquivo = fopen(ARQUIVO_PEDIDOS, "wb");
    if (!arquivo) {
        return FALSE;
    }

    fwrite(&num_pedidos, sizeof(int), 1, arquivo);
    fwrite(pedidos, sizeof(Pedido), num_pedidos, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean carregar_pedidos(void) {
    FILE *arquivo = fopen(ARQUIVO_PEDIDOS, "rb");
    if (!arquivo) {
        num_pedidos = 0;
        return FALSE;
    }

    fread(&num_pedidos, sizeof(int), 1, arquivo);
    fread(pedidos, sizeof(Pedido), num_pedidos, arquivo);
    fclose(arquivo);
    return TRUE;
}

gboolean adicionar_pedido(Pedido *pedido) {
    if (num_pedidos >= MAX_PEDIDOS) {
        return FALSE;
    }

    pedido->id = num_pedidos + 1;
    obter_data_atual(pedido->data, sizeof(pedido->data));
    obter_hora_atual(pedido->hora, sizeof(pedido->hora));
    
    calcular_valores_pedido(pedido);
    pedidos[num_pedidos++] = *pedido;
    
    return salvar_pedidos();
}

// Inicialização e finalização
gboolean inicializar_database(void) {
    criar_diretorios_necessarios();
    
    // Carregar todos os dados
    gboolean success = TRUE;
    success &= carregar_produtos();
    success &= carregar_clientes();
    success &= carregar_pedidos();
    
    return success;
}

void finalizar_database(void) {
    // Salvar todos os dados antes de finalizar
    salvar_produtos();
    salvar_clientes();
    salvar_pedidos();
}

// Função para gerar fatura em PDF
void gerar_fatura(Pedido *pedido) {
    char filename[256];
    snprintf(filename, sizeof(filename), "data/faturas/fatura_%d_%s.pdf", 
             pedido->id, pedido->data);
    
    // Criar superfície PDF
    cairo_surface_t *surface = cairo_pdf_surface_create(filename, 595.0, 842.0); // A4
    cairo_t *cr = cairo_create(surface);
    
    // Configurar fonte
    cairo_select_font_face(cr, "Arial", 
                          CAIRO_FONT_SLANT_NORMAL, 
                          CAIRO_FONT_WEIGHT_NORMAL);
    
    // Cabeçalho da fatura
    cairo_set_font_size(cr, 20);
    cairo_move_to(cr, 50, 50);
    cairo_show_text(cr, "FATURA");
    
    // Informações da empresa
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, 50, 80);
    cairo_show_text(cr, "Sistema de Gestão");
    cairo_move_to(cr, 50, 95);
    cairo_show_text(cr, "Luanda, Angola");
    
    // Data e número da fatura
    char buffer[100];
    cairo_move_to(cr, 400, 80);
    snprintf(buffer, sizeof(buffer), "Fatura Nº: %d", pedido->id);
    cairo_show_text(cr, buffer);
    cairo_move_to(cr, 400, 95);
    snprintf(buffer, sizeof(buffer), "Data: %s", pedido->data);
    cairo_show_text(cr, buffer);
    cairo_move_to(cr, 400, 110);
    snprintf(buffer, sizeof(buffer), "Hora: %s", pedido->hora);
    cairo_show_text(cr, buffer);
    
    // Informações do cliente
    Cliente *cliente = buscar_cliente(pedido->idCliente);
    if (cliente) {
        cairo_move_to(cr, 50, 130);
        cairo_show_text(cr, "Cliente:");
        cairo_move_to(cr, 50, 145);
        snprintf(buffer, sizeof(buffer), "Nome: %s", cliente->nome);
        cairo_show_text(cr, buffer);
        cairo_move_to(cr, 50, 160);
        snprintf(buffer, sizeof(buffer), "Telefone: %s", cliente->telefone);
        cairo_show_text(cr, buffer);
        if (strlen(cliente->nif) > 0) {
            cairo_move_to(cr, 50, 175);
            snprintf(buffer, sizeof(buffer), "NIF: %s", cliente->nif);
            cairo_show_text(cr, buffer);
        }
    }
    
    // Cabeçalho da tabela
    cairo_set_font_size(cr, 10);
    cairo_move_to(cr, 50, 220);
    cairo_show_text(cr, "Produto");
    cairo_move_to(cr, 200, 220);
    cairo_show_text(cr, "Qtd");
    cairo_move_to(cr, 250, 220);
    cairo_show_text(cr, "Preço Unit.");
    cairo_move_to(cr, 320, 220);
    cairo_show_text(cr, "IVA Unit.");
    cairo_move_to(cr, 390, 220);
    cairo_show_text(cr, "Subtotal");
    cairo_move_to(cr, 460, 220);
    cairo_show_text(cr, "IVA Total");
    cairo_move_to(cr, 530, 220);
    cairo_show_text(cr, "Total");
    
    // Linha separadora
    cairo_move_to(cr, 50, 225);
    cairo_line_to(cr, 545, 225);
    cairo_stroke(cr);
    
    // Itens do pedido
    double y = 240;
    double subtotal = 0;
    double iva_total = 0;
    
    for (int i = 0; i < pedido->numProdutos; i++) {
        Produto *prod = buscar_produto(pedido->idProduto[i]);
        if (prod) {
            int qtd = pedido->quantidade[i];
            double preco_unit = prod->preco;
            double iva_unit = calcular_iva(preco_unit);
            double subtotal_item = preco_unit * qtd;
            double iva_total_item = iva_unit * qtd;
            double total_item = subtotal_item + iva_total_item;
            
            subtotal += subtotal_item;
            iva_total += iva_total_item;
            
            // Nome do produto
            cairo_move_to(cr, 50, y);
            cairo_show_text(cr, prod->nome);
            
            // Quantidade
            cairo_move_to(cr, 200, y);
            snprintf(buffer, sizeof(buffer), "%d", qtd);
            cairo_show_text(cr, buffer);
            
            // Preço unitário
            cairo_move_to(cr, 250, y);
            snprintf(buffer, sizeof(buffer), "%.2f Kz", preco_unit);
            cairo_show_text(cr, buffer);
            
            // IVA unitário
            cairo_move_to(cr, 320, y);
            snprintf(buffer, sizeof(buffer), "%.2f Kz", iva_unit);
            cairo_show_text(cr, buffer);
            
            // Subtotal
            cairo_move_to(cr, 390, y);
            snprintf(buffer, sizeof(buffer), "%.2f Kz", subtotal_item);
            cairo_show_text(cr, buffer);
            
            // IVA total
            cairo_move_to(cr, 460, y);
            snprintf(buffer, sizeof(buffer), "%.2f Kz", iva_total_item);
            cairo_show_text(cr, buffer);
            
            // Total
            cairo_move_to(cr, 530, y);
            snprintf(buffer, sizeof(buffer), "%.2f Kz", total_item);
            cairo_show_text(cr, buffer);
            
            y += 20;
        }
    }
    
    // Linha separadora
    cairo_move_to(cr, 50, y);
    cairo_line_to(cr, 545, y);
    cairo_stroke(cr);
    y += 20;
    
    // Totais
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, 390, y);
    cairo_show_text(cr, "Subtotal:");
    cairo_move_to(cr, 530, y);
    snprintf(buffer, sizeof(buffer), "%.2f Kz", subtotal);
    cairo_show_text(cr, buffer);
    
    y += 20;
    cairo_move_to(cr, 390, y);
    cairo_show_text(cr, "IVA Total (14%):");
    cairo_move_to(cr, 530, y);
    snprintf(buffer, sizeof(buffer), "%.2f Kz", iva_total);
    cairo_show_text(cr, buffer);
    
    y += 20;
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, 390, y);
    cairo_show_text(cr, "Total:");
    cairo_move_to(cr, 530, y);
    snprintf(buffer, sizeof(buffer), "%.2f Kz", subtotal + iva_total);
    cairo_show_text(cr, buffer);
    
    // Informações de pagamento
    y += 40;
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_move_to(cr, 50, y);
    snprintf(buffer, sizeof(buffer), "Método de Pagamento: %s", pedido->metodoPagamento);
    cairo_show_text(cr, buffer);
    
    y += 20;
    cairo_move_to(cr, 50, y);
    snprintf(buffer, sizeof(buffer), "Valor Pago: %.2f Kz", pedido->valorPago);
    cairo_show_text(cr, buffer);
    
    if (strcmp(pedido->metodoPagamento, "Dinheiro") == 0) {
        y += 20;
        cairo_move_to(cr, 50, y);
        snprintf(buffer, sizeof(buffer), "Troco: %.2f Kz", pedido->troco);
        cairo_show_text(cr, buffer);
    }
    
    // Rodapé
    y = 800;
    cairo_set_font_size(cr, 8);
    cairo_move_to(cr, 50, y);
    cairo_show_text(cr, "Obrigado pela preferência!");
    
    // Finalizar PDF
    cairo_show_page(cr);
    cairo_surface_destroy(surface);
    cairo_destroy(cr);
}

// Função para backup
gboolean fazer_backup(const char *diretorio) {
    char timestamp[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", t);
    
    char arquivo_backup[256];
    snprintf(arquivo_backup, sizeof(arquivo_backup), 
             "%s/backup_%s.dat", diretorio, timestamp);
    
    FILE *backup = fopen(arquivo_backup, "wb");
    if (!backup) {
        return FALSE;
    }
    
    // Salvar todos os dados em um único arquivo de backup
    fwrite(&num_produtos, sizeof(int), 1, backup);
    fwrite(produtos, sizeof(Produto), num_produtos, backup);
    
    fwrite(&num_clientes, sizeof(int), 1, backup);
    fwrite(clientes, sizeof(Cliente), num_clientes, backup);
    
    fwrite(&num_pedidos, sizeof(int), 1, backup);
    fwrite(pedidos, sizeof(Pedido), num_pedidos, backup);
    
    fclose(backup);
    return TRUE;
}
