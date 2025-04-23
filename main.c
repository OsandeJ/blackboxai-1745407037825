#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>    // Para manipular data e hora
#include <direct.h>  // Para _mkdir no Windows
#include <conio.h>   // Para getch no Windows
#include <windows.h> // Necessário para manipulação de cores no console

// Definição das cores com o a lib <windows.h>
#define COR_PADRAO 7    // Branco
#define COR_SUCESSO 10  // Verde
#define COR_ERRO 12     // Vermelho

#define MAX_PRODUTOS 100
#define MAX_CLIENTES 100
#define MAX_PEDIDOS 100

typedef struct {
    int id;
    char nome[50];
    char descricao[100];
    float preco;
} Produto;

typedef struct {
    int id;
    char nome[50];
    char telefone[15];
} Cliente;

typedef struct {
    int id;
    int idCliente;
    int idProduto[MAX_PRODUTOS];
    int quantidade[MAX_PRODUTOS];
    int numProdutos;
    float total;
    float valorPago;
    float troco;
    char metodoPagamento[20];
} Pedido;

Produto produtos[MAX_PRODUTOS];
Cliente clientes[MAX_CLIENTES];
Pedido pedidos[MAX_PEDIDOS];
int numProdutos = 0, numClientes = 0, numPedidos = 0;

// FUNÇÕES GERAIS //
void limparTela() {
    system("cls");
}

void alterarCorTexto(int cor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, cor);
}

void divisoria() {
    printf("\n==============================================\n");
}

void salvarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "wb");
    if (!arquivo) {
        printf("Erro ao salvar produtos!\n");
        return;
    }

    fwrite(&numProdutos, sizeof(int), 1, arquivo);
    fwrite(produtos, sizeof(Produto), numProdutos, arquivo);
    
    fclose(arquivo);
}

void salvarClientes() {
    FILE *arquivo = fopen("clientes.dat", "wb");
    if (!arquivo) {
        printf("Erro ao salvar clientes!\n");
        return;
    }

    fwrite(&numClientes, sizeof(int), 1, arquivo);
    fwrite(clientes, sizeof(Cliente), numClientes, arquivo);
    
    fclose(arquivo);
}

void salvarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "wb");
    if (!arquivo) {
        printf("Erro ao salvar pedidos!\n");
        return;
    }

    fwrite(&numPedidos, sizeof(int), 1, arquivo);
    fwrite(pedidos, sizeof(Pedido), numPedidos, arquivo);
    
    fclose(arquivo);
}

void carregarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "rb");
    if (!arquivo) {
        numProdutos = 0; // Se o arquivo não existir, inicializa com zero
        return;
    }

    if (fread(&numProdutos, sizeof(int), 1, arquivo) != 1) {
        numProdutos = 0; // Evita valores inválidos caso o arquivo esteja corrompido
    } else {
        fread(produtos, sizeof(Produto), numProdutos, arquivo);
    }
    
    fclose(arquivo);
}

void carregarClientes() {
    FILE *arquivo = fopen("clientes.dat", "rb");
    if (!arquivo) {
        numClientes = 0;
        return;
    }

    if (fread(&numClientes, sizeof(int), 1, arquivo) != 1) {
        numClientes = 0;
    } else {
        fread(clientes, sizeof(Cliente), numClientes, arquivo);
    }

    fclose(arquivo);
}

void carregarPedidos() {
    FILE *arquivo = fopen("pedidos.dat", "rb");
    if (!arquivo) {
        numPedidos = 0;
        return;
    }

    if (fread(&numPedidos, sizeof(int), 1, arquivo) != 1) {
        numPedidos = 0;
    } else {
        fread(pedidos, sizeof(Pedido), numPedidos, arquivo);
    }

    fclose(arquivo);
}

void editarProduto(int id) {
    FILE *arquivo, *temp;
    int encontrado = 0;
    Produto produto;

    arquivo = fopen("produtos.dat", "rb");
    temp = fopen("temp.dat", "wb");

    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        if (arquivo) fclose(arquivo);
        if (temp) fclose(temp);
        return;
    }

    while (fread(&produto, sizeof(Produto), 1, arquivo)) {
        if (produto.id == id) {
            printf("Produto encontrado!\n");
            printf("Digite o novo nome: ");
            getchar(); // Limpa o buffer do teclado
            fgets(produto.nome, sizeof(produto.nome), stdin);
            produto.nome[strcspn(produto.nome, "\n")] = '\0'; // Remove o '\n'

            printf("Digite a nova descrição: ");
            fgets(produto.descricao, sizeof(produto.descricao), stdin);
            produto.descricao[strcspn(produto.descricao, "\n")] = '\0';

            printf("Digite o novo preço: ");
            scanf("%f", &produto.preco);
            encontrado = 1;
        }
        fwrite(&produto, sizeof(Produto), 1, temp);
    }

    fclose(arquivo);
    fclose(temp);

    remove("produtos.dat");
    rename("temp.dat", "produtos.dat");

    if (encontrado) {
        printf("Produto atualizado com sucesso!\n");
        carregarProdutos(); // Atualiza a lista em memória
    } else {
        printf("Produto não encontrado!\n");
    }
}

void excluirProduto() {
    FILE *arquivo, *temp;
    int id, encontrado = 0;
    Produto produto;

    printf("Digite o ID do produto que deseja excluir: ");
    scanf("%d", &id);

    arquivo = fopen("produtos.dat", "rb");
    temp = fopen("temp.dat", "wb");

    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    while (fread(&produto, sizeof(Produto), 1, arquivo)) {
        if (produto.id == id) {
            encontrado = 1; // Produto encontrado, não será copiado para o novo arquivo
        } else {
            fwrite(&produto, sizeof(Produto), 1, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);
    remove("produtos.dat");
    rename("temp.dat", "produtos.dat");

    if (encontrado)
        printf("Produto removido com sucesso!\n");
    else
        printf("Produto não encontrado!\n");
}

void editarCliente(int id) {
    FILE *arquivo, *temp;
    int numClientesLocal = 0;
    Cliente cliente;
    int encontrado = 0;

    // Abrir o arquivo original para leitura
    arquivo = fopen("clientes.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir clientes.dat!\n");
        return;
    }

    // Ler o número total de clientes (cabeçalho)
    fread(&numClientesLocal, sizeof(int), 1, arquivo);

    // Abrir o arquivo temporário para escrita
    temp = fopen("temp.dat", "wb");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(arquivo);
        return;
    }
    // Reservar espaço para o cabeçalho (será escrito depois)
    fseek(temp, sizeof(int), SEEK_SET);

    // Ler cada registro de Cliente e processar
    for (int i = 0; i < numClientesLocal; i++) {
        fread(&cliente, sizeof(Cliente), 1, arquivo);
        if (cliente.id == id) {
            printf("Cliente encontrado!\n");
            
            // Limpar os campos para evitar resíduos
            memset(cliente.nome, 0, sizeof(cliente.nome));
            memset(cliente.telefone, 0, sizeof(cliente.telefone));

            printf("Digite o novo nome: ");
            getchar(); // Limpa o buffer do teclado
            fgets(cliente.nome, sizeof(cliente.nome), stdin);
            cliente.nome[strcspn(cliente.nome, "\n")] = '\0';

            printf("Digite o novo telefone: ");
            fgets(cliente.telefone, sizeof(cliente.telefone), stdin);
            cliente.telefone[strcspn(cliente.telefone, "\n")] = '\0';

            encontrado = 1;
        }
        // Escrever o registro (modificado ou não) no arquivo temporário
        fwrite(&cliente, sizeof(Cliente), 1, temp);
    }

    fclose(arquivo);

    // Retornar ao início do arquivo temporário para gravar o cabeçalho (novo total)
    fseek(temp, 0, SEEK_SET);
    fwrite(&numClientesLocal, sizeof(int), 1, temp);
    fclose(temp);

    // Substituir o arquivo original pelo temporário
    remove("clientes.dat");
    rename("temp.dat", "clientes.dat");

    if (encontrado) {
        printf("Cliente atualizado com sucesso!\n");
        carregarClientes(); // Atualiza a lista em memória
    } else {
        printf("Cliente não encontrado!\n");
    }
}

void corrigirIDs() {
    FILE *arquivo, *temp;
    Cliente cliente;
    int novoId = 1;

    arquivo = fopen("clientes.dat", "rb");
    temp = fopen("temp.dat", "wb");

    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return;
    }

    while (fread(&cliente, sizeof(Cliente), 1, arquivo)) {
        cliente.id = novoId++;
        fwrite(&cliente, sizeof(Cliente), 1, temp);
    }

    fclose(arquivo);
    fclose(temp);
    remove("clientes.dat");
    rename("temp.dat", "clientes.dat");
}


void excluirCliente() {
    FILE *arquivo, *temp;
    int numClientesLocal = 0;
    int idExcluir, encontrado = 0;
    Cliente cliente;

    // Abrir arquivo original
    arquivo = fopen("clientes.dat", "rb");
    if (!arquivo) {
        printf("Erro ao abrir clientes.dat!\n");
        return;
    }

    // Ler quantos clientes existem no arquivo
    fread(&numClientesLocal, sizeof(int), 1, arquivo);

    // Criar arquivo temporário
    temp = fopen("temp.dat", "wb");
    if (!temp) {
        printf("Erro ao criar arquivo temporário!\n");
        fclose(arquivo);
        return;
    }

    // Pedir ao usuário o ID a excluir
    printf("Digite o ID do cliente a ser excluído: ");
    scanf("%d", &idExcluir);

    // Variável para contar quantos clientes vão permanecer
    int novoTotal = 0;

    // Pular numClientesLocal no temp (escreveremos depois)
    fseek(temp, sizeof(int), SEEK_SET);

    // Ler cada cliente e verificar se é para manter ou excluir
    for (int i = 0; i < numClientesLocal; i++) {
        fread(&cliente, sizeof(Cliente), 1, arquivo);

        if (cliente.id == idExcluir) {
            // Encontramos o cliente, não escrevemos no temp
            encontrado = 1;
            printf("Excluindo cliente ID: %d\n", cliente.id);
        } else {
            // Mantém o cliente no arquivo temporário
            fwrite(&cliente, sizeof(Cliente), 1, temp);
            novoTotal++;
        }
    }

    fclose(arquivo);

    // Agora, voltar ao início do arquivo temporário para atualizar o total de clientes
    fseek(temp, 0, SEEK_SET);
    fwrite(&novoTotal, sizeof(int), 1, temp);

    fclose(temp);

    // Substituir o arquivo original
    if (encontrado) {
        remove("clientes.dat");
        rename("temp.dat", "clientes.dat");
        printf("Cliente excluído com sucesso!\n");
    } else {
        remove("temp.dat");
        printf("Cliente com ID %d não encontrado.\n", idExcluir);
    }
}

void listarClientes() {
    FILE *arquivo = fopen("clientes.dat", "rb");
    Cliente cliente;

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    printf("\n=== Lista de Clientes ===\n");
    while (fread(&cliente, sizeof(Cliente), 1, arquivo)) {
        printf("ID: %d | Nome: %s | Telefone: %s\n", cliente.id, cliente.nome, cliente.telefone);
    }
    fclose(arquivo);
}


void listarProdutos() {
    FILE *arquivo = fopen("produtos.dat", "rb");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de produtos ou nenhum produto cadastrado!\n");
        return;
    }

    // Lendo o número de produtos no arquivo
    fread(&numProdutos, sizeof(int), 1, arquivo);
    fread(produtos, sizeof(Produto), numProdutos, arquivo);
    
    fclose(arquivo);

    divisoria();
    printf("            --- Lista de Produtos ---\n");
    divisoria();

    if (numProdutos == 0) {
        printf("Nenhum produto cadastrado.\n");
    } else {
        for (int i = 0; i < numProdutos; i++) {
            printf("ID: %d | Nome: %s | Preço: %.2f Kz\n", 
                   produtos[i].id, produtos[i].nome, produtos[i].preco);
        }
    }

    divisoria();
    printf("\n1 - Editar um produto\n");
    printf("2 - Excluir um produto\n");
    printf("3 - Voltar ao menu\n");
    printf("Escolha uma opção: ");
    
    int opcao;
    scanf("%d", &opcao);

    switch (opcao) {
        case 1:
   			printf("Digite o ID do produto que deseja editar: ");
    		int idProduto;
    		scanf("%d", &idProduto);
    		editarProduto(idProduto);
            break;
        case 2:
            excluirProduto();
            break;
        case 3:
            printf("Voltando ao menu...\n");
            return;
        default:
            printf("Opção inválida!\n");
    }
}

void cadastrarProduto() {
    setlocale(LC_ALL, "Portuguese");
    if (numProdutos < MAX_PRODUTOS) {
        Produto p;
        p.id = numProdutos + 1;
        divisoria();
        printf("Cadastro de Produto\n");
        divisoria();

        printf("Nome do produto: ");
        getchar(); // Limpa o buffer do teclado
        fgets(p.nome, sizeof(p.nome), stdin);
        p.nome[strcspn(p.nome, "\n")] = '\0'; // Remove o '\n'

        printf("Descrição do produto: ");
        fgets(p.descricao, sizeof(p.descricao), stdin);
        p.descricao[strcspn(p.descricao, "\n")] = '\0'; // Remove o '\n'

        printf("Preço do produto (Kz): ");
        scanf("%f", &p.preco);
        produtos[numProdutos++] = p;

        alterarCorTexto(COR_SUCESSO);
    	printf("Produto cadastrado com sucesso!\n");
    	alterarCorTexto(COR_PADRAO);
    	salvarProdutos();
    } else {
        alterarCorTexto(COR_ERRO);
    	printf("Erro: Limite de produtos atingido!\n");
    	alterarCorTexto(COR_PADRAO);
    }
    printf("\nPressione qualquer tecla para voltar ao menu...");
    getch();
    limparTela();
}

void cadastrarCliente() {
    setlocale(LC_ALL, "Portuguese");
    if (numClientes < MAX_CLIENTES) {
        Cliente c;
        c.id = numClientes + 1;
        divisoria();
        printf("Cadastro de Cliente\n");
        divisoria();

        printf("Nome do cliente: ");
        getchar(); // Limpa o buffer do teclado
        fgets(c.nome, sizeof(c.nome), stdin);
        c.nome[strcspn(c.nome, "\n")] = '\0'; // Remove o '\n'

        printf("Telefone do cliente: ");
        fgets(c.telefone, sizeof(c.telefone), stdin);
        c.telefone[strcspn(c.telefone, "\n")] = '\0'; // Remove o '\n'

        clientes[numClientes++] = c;
        alterarCorTexto(COR_SUCESSO);
    	printf("Cliente cadastrado com sucesso!\n");
    	alterarCorTexto(COR_PADRAO);
	    salvarClientes();
    } else {
        alterarCorTexto(COR_ERRO);
   		printf("Erro: Limite de clientes atingido!\n");
    	alterarCorTexto(COR_PADRAO);
    }
    printf("\nPressione qualquer tecla para voltar ao menu...");
    getch();
    limparTela();
}

#include <sys/stat.h> // Para verificar se a pasta já existe

void salvarPedidoEmArquivo(Pedido p) {
    setlocale(LC_ALL, "Portuguese");

    // Obter data e hora atuais
    time_t agora = time(NULL);
    struct tm *tempo = localtime(&agora);

    // Criar nome da pasta com a data (ex.: "2025-01-08")
    char nomePasta[20];
    strftime(nomePasta, sizeof(nomePasta), "%Y-%m-%d", tempo);

    // Verificar se a pasta já existe antes de criá-la
    struct stat st = {0};
    if (stat(nomePasta, &st) == -1) {
        _mkdir(nomePasta); // Cria a pasta se não existir
    }

    // Criar nome do arquivo com data e hora (ex.: "2025-01-08_14-30-45.txt")
    char nomeArquivo[50];
    strftime(nomeArquivo, sizeof(nomeArquivo), "%Y-%m-%d_%H-%M-%S.txt", tempo);

    // Montar o caminho completo (ex.: "2025-01-08/2025-01-08_14-30-45.txt")
    char caminhoCompleto[100];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", nomePasta, nomeArquivo);

    // Abrir o arquivo para escrita
    FILE *arquivo = fopen(caminhoCompleto, "w");
    if (!arquivo) {
        alterarCorTexto(COR_ERRO);
        printf("Erro ao criar o arquivo para salvar o pedido.\n");
        alterarCorTexto(COR_PADRAO);
        return;
    }

    // Escrever o cabeçalho do pedido no arquivo
    fprintf(arquivo, "--- Resumo do Pedido ---\n");
    fprintf(arquivo, "Pedido ID: %d\n", p.id);
    fprintf(arquivo, "Cliente: %s\n", clientes[p.idCliente - 1].nome);
    fprintf(arquivo, "Telefone: %s\n\n", clientes[p.idCliente - 1].telefone);

    // Escrever os produtos em formato de tabela
    fprintf(arquivo, "%-15s %-12s %-15s %-15s\n", "Produto", "Quantidade", "Preço Unitário", "Total");
    fprintf(arquivo, "----------------------------------------------------------\n");

    for (int i = 0; i < p.numProdutos; i++) {
        Produto prod = produtos[p.idProduto[i] - 1];
        fprintf(arquivo, "%-15s %-12d %-15.2f %-15.2f\n",
                prod.nome,
                p.quantidade[i],
                prod.preco,
                prod.preco * p.quantidade[i]);
    }

    fprintf(arquivo, "----------------------------------------------------------\n");
    fprintf(arquivo, "TOTAL: %.2f Kz\n", p.total);
    // Linhas adicionadas para a funcionalidade de troco:
    fprintf(arquivo, "Método de Pagamento: %s\n", p.metodoPagamento);
    fprintf(arquivo, "Valor Pago: %.2f Kz\n", p.valorPago);
    fprintf(arquivo, "Troco: %.2f Kz\n", p.troco);

    fclose(arquivo);

    // Exibir mensagem de sucesso
    alterarCorTexto(COR_SUCESSO);
    printf("Pedido salvo com sucesso em: %s\n", caminhoCompleto);
    alterarCorTexto(COR_PADRAO);

    salvarPedidos(); // Salva o pedido na estrutura principal
}

void fazerPedido() {
    setlocale(LC_ALL, "Portuguese");
    if (numProdutos == 0) { // Verifica se existem produtos cadastrados
        alterarCorTexto(COR_ERRO);
        printf("Erro: Nenhum produto cadastrado no sistema!\n");
        alterarCorTexto(COR_PADRAO);
        printf("\nPressione qualquer tecla para voltar ao menu...");
        getch();
        limparTela();
        return;
    }
    
    if (numPedidos < MAX_PEDIDOS) {
        Pedido p;
        p.id = numPedidos + 1;
        divisoria();
        printf("Realização de Pedido\n");
        divisoria();

        printf("ID do cliente: ");
        scanf("%d", &p.idCliente);

        if (p.idCliente < 1 || p.idCliente > numClientes) {
            alterarCorTexto(COR_ERRO);
            printf("Erro: Cliente não encontrado!\n");
            alterarCorTexto(COR_PADRAO);
            printf("\nPressione qualquer tecla para voltar ao menu...");
            getch();
            limparTela();
            return;
        }

        printf("\n--- Produtos Disponíveis ---\n");
        for (int i = 0; i < numProdutos; i++) {
            printf("%d. %s - %.2f Kz\n", i + 1, produtos[i].nome, produtos[i].preco);
        }
        divisoria();

        printf("Quantos produtos no pedido? ");
        scanf("%d", &p.numProdutos);

        if (p.numProdutos > MAX_PRODUTOS) {
            alterarCorTexto(COR_ERRO);
            printf("Erro: Quantidade de produtos excede o limite!\n");
            alterarCorTexto(COR_PADRAO);
            printf("\nPressione qualquer tecla para voltar ao menu...");
            getch();
            limparTela();
            return;
        }

        p.total = 0;
        for (int i = 0; i < p.numProdutos; i++) {
            int escolha;
            printf("Selecione o produto %d pelo número: ", i + 1);
            scanf("%d", &escolha);

            if (escolha < 1 || escolha > numProdutos) {
                alterarCorTexto(COR_ERRO);
                printf("Erro: Produto não encontrado!\n");
                alterarCorTexto(COR_PADRAO);
                printf("\nPressione qualquer tecla para voltar ao menu...");
                getch();
                limparTela();
                return;
            }

            p.idProduto[i] = escolha;

            printf("Quantidade do produto %d: ", i + 1);
            scanf("%d", &p.quantidade[i]);

            Produto prod = produtos[escolha - 1];
            p.total += prod.preco * p.quantidade[i];
        }

        // Escolha do método de pagamento e cálculo do troco
        printf("Escolha o método de pagamento (1 - TPA, 2 - Dinheiro): ");
        int metodo;
        scanf("%d", &metodo);
        if (metodo == 1) {
            strcpy(p.metodoPagamento, "TPA");
            p.valorPago = p.total;  // assume que, no TPA, o valor pago é igual ao total
            p.troco = 0;
        } else {
            strcpy(p.metodoPagamento, "Dinheiro");
            printf("Digite o valor pago pelo cliente: ");
            scanf("%f", &p.valorPago);
            p.troco = p.valorPago - p.total;
        }

        // Armazenar o pedido e salvar em arquivo
        pedidos[numPedidos++] = p;
        salvarPedidoEmArquivo(p);

        divisoria();
        printf("            --- Resumo dos Pedidos ---\n");
        divisoria();

        // Cabeçalho da Tabela
        printf("%-15s %-12s %-15s %-15s\n", "Produto", "Quantidade", "Preço Unitário", "Total");
        printf("-------------------------------------------------------------------------\n");

        for (int j = 0; j < p.numProdutos; j++) {
            Produto prod = produtos[p.idProduto[j] - 1];
            printf("%-15s %-12d %-15.2f %-15.2f\n",
                   prod.nome,
                   p.quantidade[j],
                   prod.preco,
                   prod.preco * p.quantidade[j]);
        }

        divisoria();
        printf("TOTAL: %.2f Kz\n", p.total);

        // EXIBIR VALOR PAGO E TROCO
        printf("Valor Pago: %.2f Kz\n", p.valorPago);
        printf("Troco: %.2f Kz\n", p.troco);

        divisoria();
    }

    printf("\nPressione qualquer tecla para voltar ao menu...");
    getch();
    limparTela();
}

void visualizarPedidos() {
    setlocale(LC_ALL, "Portuguese");

    if (numPedidos == 0) {
        alterarCorTexto(COR_ERRO);
        printf("Erro: Nenhum pedido realizado!\n");
        alterarCorTexto(COR_PADRAO);
        printf("\nPressione qualquer tecla para voltar ao menu...");
        getch();
        limparTela();
        return;
    }

    divisoria();
    printf("            --- Resumo dos Pedidos ---\n");
    divisoria();

    for (int i = 0; i < numPedidos; i++) {
        Pedido p = pedidos[i];
        printf("\nPedido ID: %d\n", p.id);
        printf("Cliente: %s\n", clientes[p.idCliente - 1].nome);
        printf("Telefone: %s\n\n", clientes[p.idCliente - 1].telefone);

        // Cabeçalho da Tabela
        printf("%-15s %-12s %-15s %-15s\n", "Produto", "Quantidade", "Preço Unitário", "Total");
        printf("----------------------------------------------------------\n");

        for (int j = 0; j < p.numProdutos; j++) {
            Produto prod = produtos[p.idProduto[j] - 1];
            printf("%-15s %-12d %-15.2f %-15.2f\n",
                   prod.nome,
                   p.quantidade[j],
                   prod.preco,
                   prod.preco * p.quantidade[j]);
        }

        printf("----------------------------------------------------------\n");
        printf("TOTAL: %.2f Kz\n", p.total);
        
        // Exibir método de pagamento, valor pago e troco
        printf("Método de Pagamento: %s\n", p.metodoPagamento);
        printf("Valor Pago: %.2f Kz\n", p.valorPago);
        printf("Troco: %.2f Kz\n", p.troco);

        printf("\n");
    }

    printf("\nPressione qualquer tecla para voltar ao menu...");
    getch();
    limparTela();
}


void pesquisarClientePorNome() {
    setlocale(LC_ALL, "Portuguese");
    carregarClientes(); // ?? Garante que a lista de clientes está atualizada antes da pesquisa

    divisoria();
    printf("            --- Pesquisa de Cliente ---\n");
    divisoria();

    if (numClientes == 0) {
        alterarCorTexto(COR_ERRO);
        printf("Erro: Nenhum cliente cadastrado até o momento.\n");
        alterarCorTexto(COR_PADRAO);
        printf("\nPressione qualquer tecla para voltar ao menu...");
        getch();
        limparTela();
        return;
    }

    char nomePesquisa[50];
    printf("Digite o nome do cliente para pesquisar: ");
    getchar(); // Limpa o buffer do teclado
    fgets(nomePesquisa, sizeof(nomePesquisa), stdin);
    nomePesquisa[strcspn(nomePesquisa, "\n")] = '\0'; // Remove o '\n'

    int encontrado = 0; // Flag para indicar se encontramos algum cliente
    int clienteID = -1;
    
    divisoria();
    printf("            --- Resultados da Pesquisa ---\n");
    divisoria();

    for (int i = 0; i < numClientes; i++) {
        if (strstr(clientes[i].nome, nomePesquisa) != NULL) { // Busca parcial no nome
            encontrado = 1;
            clienteID = clientes[i].id;
            printf("Cliente ID: %d\n", clientes[i].id);
            printf("Nome: %s\n", clientes[i].nome);
            printf("Telefone: %s\n", clientes[i].telefone);
            divisoria();
        }
    }

    if (!encontrado) {
        alterarCorTexto(COR_ERRO);
        printf("Nenhum cliente encontrado com o nome informado.\n");
        alterarCorTexto(COR_PADRAO);
    } else {
        int opcao;
        printf("\n1 - Editar este cliente\n");
        printf("2 - Excluir este cliente\n");
        printf("3 - Voltar ao menu\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                editarCliente(clienteID);
                break;
            case 2:
                excluirCliente(clienteID);
                break;
            case 3:
                printf("Voltando ao menu...\n");
                getch();
        		limparTela();
                return;
            default:
                printf("Opção inválida!\n");
        }
    }

    printf("\nPressione qualquer tecla para voltar ao menu...");
    getch();
    limparTela();
}

void menu() {
    setlocale(LC_ALL, "Portuguese");
    int opcao;
    do {
        divisoria();
        printf("            --- SISTEMA DE GESTÃO ---\n");
        divisoria();
        printf("1. Cadastrar Produto\n");
        printf("2. Cadastrar Cliente\n");
        printf("3. Fazer Pedido\n");
        printf("4. Visualizar Pedidos\n");
        printf("5. Pesquisar Cliente por Nome\n");
        printf("6. Listar Produtos\n"); // Nova opção adicionada
        //printf("7. Listar Clientes \n");
        printf("0. Sair\n");
        divisoria();
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                limparTela();
                cadastrarProduto();
                break;
            case 2:
                limparTela();
                cadastrarCliente();
                break;
            case 3:
                limparTela();
                fazerPedido();
                break;
            case 4:
                limparTela();
                visualizarPedidos();
                break;
            case 5:
                limparTela();
                pesquisarClientePorNome();
                break;
            case 6: 
                limparTela();
                listarProdutos();
                break;
            //case 7:
            //	limparTela();
        	//listarClientes();
            	break;
            case 0:
                printf("Saindo do programa...\n");
                break;
            default:
                alterarCorTexto(COR_ERRO);
                printf("Opção inválida! Tente novamente.\n");
                alterarCorTexto(COR_PADRAO);
                printf("\nPressione qualquer tecla para voltar ao menu...");
                getch();
                limparTela();
        }
    } while (opcao != 0);
}


int main() {
    setlocale(LC_ALL, "Portuguese");

    // Carregar os dados salvos anteriormente
    carregarProdutos();
    carregarClientes();
    carregarPedidos();

    menu();

    // Salvar os dados antes de sair
    salvarProdutos();
    salvarClientes();
    salvarPedidos();

    return 0;
}