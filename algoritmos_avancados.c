///#include <stdio.h>

// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Este código inicial serve como base para o desenvolvimento das estruturas de navegação, pistas e suspeitos.
// Use as instruções de cada região para desenvolver o sistema completo com árvore binária, árvore de busca e tabela hash.

//int main() {

    // 🌱 Nível Novato: Mapa da Mansão com Árvore Binária
    //
    // - Crie uma struct Sala com nome, e dois ponteiros: esquerda e direita.
    // - Use funções como criarSala(), conectarSalas() e explorarSalas().
    // - A árvore pode ser fixa: Hall de Entrada, Biblioteca, Cozinha, Sótão etc.
    // - O jogador deve poder explorar indo à esquerda (e) ou à direita (d).
    // - Finalize a exploração com uma opção de saída (s).
    // - Exiba o nome da sala a cada movimento.
    // - Use recursão ou laços para caminhar pela árvore.
    // - Nenhuma inserção dinâmica é necessária neste nível.

    // 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
    //
    // - Crie uma struct Pista com campo texto (string).
    // - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
    // - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
    // - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
    // - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
    // - Não precisa remover ou balancear a árvore.
    // - Use funções para modularizar: inserirPista(), listarPistas().
    // - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

    // 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
    //
    // - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
    // - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
    // - A chave pode ser o nome do suspeito ou derivada das pistas.
    // - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
    // - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
    // - Adicione um contador para saber qual suspeito foi mais citado.
    // - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
    // - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
    // - Em caso de colisão, use lista encadeada para tratar.
    // - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().

   // return 0;
//}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Detetive Quest - Exploração de mansão usando árvore binária
 * Cada nó (Room) tem um nome e até dois ponteiros para salas: esquerda e direita.
 * O jogador começa no Hall de Entrada e navega até chegar numa sala sem saídas (nó folha).
 *
 * Comandos do jogador:
 *  - 'e' ou 'E' : ir para a sala à esquerda
 *  - 'd' ou 'D' : ir para a sala à direita
 *  - 's' ou 'S' : sair do jogo (encerra antecipadamente)
 *
 * O programa trata tentativas de seguir por caminhos inexistentes informando o jogador.
 */

/* Estrutura que representa uma sala (nó da árvore) */
typedef struct Room {
    char name;             / nome da sala (dinamicamente alocado) */
    struct Room left;      / caminho à esquerda (pode ser NULL) */
    struct Room right;     / caminho à direita (pode ser NULL) */
} Room;

/* --- Funções utilitárias --- */

/* Aloca e retorna uma cópia da string fornecida */
char *duplicate_string(const char *src) {
    if (src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    char *dst = (char *)malloc(len);
    if (!dst) {
        fprintf(stderr, "Erro: falha na alocação de memória para string.\n");
        exit(EXIT_FAILURE);
    }
    memcpy(dst, src, len);
    return dst;
}

/* Cria uma nova sala com o nome fornecido */
Room *create_room(const char *name) {
    Room *r = (Room *)malloc(sizeof(Room));
    if (!r) {
        fprintf(stderr, "Erro: falha na alocação de memória para sala.\n");
        exit(EXIT_FAILURE);
    }
    r->name = duplicate_string(name);
    r->left = NULL;
    r->right = NULL;
    return r;
}

/* Libera recursivamente toda a árvore de salas */
void free_rooms(Room *root) {
    if (!root) return;
    free_rooms(root->left);
    free_rooms(root->right);
    free(root->name);
    free(root);
}

/* Verifica se a sala é uma folha (sem saídas) */
int is_leaf(Room *r) {
    return (r->left == NULL && r->right == NULL);
}

/* Imprime as opções disponíveis na sala atual */
void print_options(Room *current) {
    printf("\nVocê está em: %s\n", current->name);
    printf("Opções:\n");
    if (current->left)  printf("  (E) - Entrar à esquerda -> %s\n", current->left->name);
    else                printf("  (E) - Entrar à esquerda -> (não existe caminho à esquerda)\n");
    if (current->right) printf("  (D) - Entrar à direita  -> %s\n", current->right->name);
    else                printf("  (D) - Entrar à direita  -> (não existe caminho à direita)\n");
    printf("  (S) - Sair do jogo (encerrar)\n");
    printf("Escolha uma opção (E/D/S): ");
}

/* Leitura do comando do jogador (retorna o primeiro caractere não branco) */
char read_choice(void) {
    char buffer[128];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        /* Se ocorrer erro de leitura, tratamos como sair */
        return 's';
    }
    /* encontrar primeiro caractere não branco */
    for (size_t i = 0; buffer[i] != '\0'; ++i) {
        if (!isspace((unsigned char)buffer[i])) {
            return (char)buffer[i];
        }
    }
    return '\0';
}

/* --- Função principal que controla a navegação do jogador --- */
void play_game(Room *start) {
    Room *current = start;

    printf("=== Bem-vindo a Detetive Quest ===\n");
    printf("Você começa no Hall de Entrada. Navegue até chegar em um cômodo sem saídas.\n");

    while (1) {
        /* Mostra a sala atual e opções */
        print_options(current);

        char choice = read_choice();
        choice = (char)tolower((unsigned char)choice);

        if (choice == 's') {
            /* O jogador optou por sair antecipadamente */
            printf("\nVocê optou por sair do jogo. Até a próxima!\n");
            break;
        } else if (choice == 'e') {
            if (current->left) {
                current = current->left;
                printf("\nIndo para a esquerda...\n");
            } else {
                /* Teste do comportamento ao tentar caminho inexistente:
                   informamos o jogador e permanecemos na mesma sala. */
                printf("\nNão existe um caminho à esquerda a partir de '%s'. Escolha outra opção.\n", current->name);
            }
        } else if (choice == 'd') {
            if (current->right) {
                current = current->right;
                printf("\nIndo para a direita...\n");
            } else {
                /* Caminho inexistente */
                printf("\nNão existe um caminho à direita a partir de '%s'. Escolha outra opção.\n", current->name);
            }
        } else {
            /* Entrada inválida - não garantimos mudança de estado */
            printf("\nOpção inválida. Use 'E', 'D' ou 'S'.\n");
        }

        /* Se chegarmos a uma sala sem saídas, o jogo termina com mensagem final */
        if (is_leaf(current)) {
            printf("\nVocê chegou ao cômodo sem saídas: '%s'.\n", current->name);
            printf("Exploração encerrada. Obrigado por jogar Detetive Quest!\n");
            break;
        }
    }
}

/* --- Construção automática da árvore da mansão --- */
Room *build_mansion_map(void) {
    /* Criamos as salas (nós) dinamicamente */
    Room *hall = create_room("Hall de Entrada");

    /* Primeiro nível */
    Room *salaEstar = create_room("Sala de Estar");
    Room *biblioteca = create_room("Biblioteca");

    hall->left = salaEstar;
    hall->right = biblioteca;

    /* Sala de Estar (segundo nível à esquerda) */
    Room *cozinha = create_room("Cozinha");
    Room *jardim = create_room("Jardim");
    salaEstar->left = cozinha;   /* Cozinha é folha */
    salaEstar->right = jardim;   /* Jardim é folha */

    /* Biblioteca (segundo nível à direita) */
    Room *escritorio = create_room("Escritório");
    Room *quartoSecreto = create_room("Quarto Secreto");
    biblioteca->left = escritorio;       /* Escritório é folha */
    biblioteca->right = quartoSecreto;   /* Quarto Secreto tem sub-salas */

    /* Quarto Secreto (terceiro nível) */
    Room cofre = create_room("Cofre Antigo");      / folha */
    Room laboratorio = create_room("Laboratório"); / folha */
    quartoSecreto->left = cofre;
    quartoSecreto->right = laboratorio;

    /* Estrutura de exemplo criada. Pode ser ampliada facilmente. */
    return hall; /* retorno da raiz */
}

/* --- Função main --- */
int main(void) {
    /* Construir o mapa da mansão automaticamente */
    Room *mansionRoot = build_mansion_map();

    /* Iniciar a exploração (jogo) */
    play_game(mansionRoot);

    /* Liberar toda a memória alocada */
    free_rooms(mansionRoot);

    return 0;


