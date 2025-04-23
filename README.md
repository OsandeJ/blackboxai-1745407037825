
Built by https://www.blackbox.ai

---

```markdown
# Sistema de Gestão de Pedidos

## Projeto Overview
O Sistema de Gestão de Pedidos é uma aplicação desenvolvida em C, projetada para simplificar o gerenciamento de produtos, clientes e pedidos. Com uma interface de linha de comando, o sistema permite que os usuários cadastrem produtos e clientes, realizem pedidos e visualizem resumos, além de manter um registro persistente dos dados em arquivos.

## Instalação
Para compilar e executar o projeto, siga as etapas abaixo:

1. Certifique-se de ter o compilador GCC instalado. Para sistemas Windows, você pode usar o MinGW.
2. Clone este repositório ou baixe os arquivos do projeto.
3. Navegue até o diretório do projeto no terminal.
4. Compile o código com o seguinte comando:
   ```bash
   gcc main.c -o sistema_gestao
   ```
5. Execute o programa utilizando o comando:
   ```bash
   ./sistema_gestao
   ```

## Uso
Ao iniciar o programa, você será recebido em um menu principal que permite navegar pelas funcionalidades do sistema. As opções incluem:

1. Cadastrar Produto
2. Cadastrar Cliente
3. Fazer Pedido
4. Visualizar Pedidos
5. Pesquisar Cliente por Nome
6. Listar Produtos
7. Sair

Siga as instruções na tela para interagir com o sistema.

## Funcionalidades
- Cadastro de Produtos: Permite adicionar novos produtos ao sistema.
- Cadastro de Clientes: Possibilita o registro de informações sobre clientes.
- Realização de Pedidos: Funcionalidade que permite aos clientes fazer pedidos dos produtos cadastrados.
- Visualização de Pedidos: Exibição de todos os pedidos realizados.
- Pesquisa de Clientes: Pesquisa de clientes pelo nome.
- Listagem de Produtos: Exibição de todos os produtos disponíveis.

## Dependências
Este projeto não possui dependências externas além do compilador C.

## Estrutura do Projeto
```
.
├── main.c                # Código fonte principal do sistema
├── produtos.dat          # Arquivo de dados para armazenar os produtos
├── clientes.dat          # Arquivo de dados para armazenar os clientes
└── pedidos.dat           # Arquivo de dados para armazenar os pedidos
```

## Contribuições
Contribuições são bem-vindas! Se você deseja contribuir para o projeto, por favor, faça um fork deste repositório e envie um pull request.

## Licença
Este projeto é licenciado sob a MIT License.
```