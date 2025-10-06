/*
    Trabalho de Programação - Jogo "Detective Quest"
    Aluno: Renan Felipe Timoteo Bueno   
    Data: 06/10/2025

    Objetivo: Criar um mapa de uma mansão usando uma árvore binária em C.
    O programa monta o mapa e depois deixa o jogador navegar por ele.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Constantes e Definições de Estruturas ---

// Definimos o tamanho da nossa tabela hash. Um número primo pequeno é uma boa escolha.
#define HASH_SIZE 11

// Estrutura para os cômodos da mansão (Árvore Binária Simples)
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura para os nós da Árvore de Busca de Pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Estrutura para os nós da Tabela Hash (Lida com colisões usando lista encadeada)
typedef struct HashNode {
    char pista[100];
    char suspeito[50];
    struct HashNode *proximo;
} HashNode;


// --- Protótipos das Funções ---

// Funções do Mapa da Mansão
Sala* criarSala(char* nome);
void explorarSalas(Sala* salaAtual, PistaNode** raizPistas, HashNode** tabelaSuspeitos);
void liberarMapa(Sala* salaAtual);

// Funções da Árvore de Busca de Pistas (BST)
PistaNode* inserirPista(PistaNode* raiz, char* pista);
void mostrarPistasColetadas(PistaNode* raiz);
void liberarPistas(PistaNode* raiz);

// Funções da Tabela Hash
unsigned int hashFunction(char* str);
void inserirNaHash(HashNode** tabela, char* pista, char* suspeito);
char* encontrarSuspeito(HashNode** tabela, char* pista);
void liberarTabelaHash(HashNode** tabela);

// Funções de Lógica do Jogo
void montarCenario(Sala** mansao, HashNode** tabela);
int contarPistasParaSuspeito(PistaNode* raizPistas, char* suspeito, HashNode** tabela);
void verificarSuspeitoFinal(PistaNode* raizPistas, HashNode** tabela);


// --- Função Principal ---
int main() {
    // Inicializando as raízes das nossas estruturas de dados
    Sala* mansao = NULL;
    PistaNode* pistasColetadas = NULL;
    HashNode* tabelaSuspeitos[HASH_SIZE] = {NULL}; // Inicializa a tabela com NULOs

    // Monta o mapa da mansão e preenche a tabela de evidências
    montarCenario(&mansao, tabelaSuspeitos);

    // Inicia a exploração do jogo
    printf("--- Detective Quest: O Veredito Final ---\n");
    printf("O relógio badala meia-noite. É hora de desvendar este mistério.\n");
    explorarSalas(mansao, &pistasColetadas, tabelaSuspeitos);

    // Inicia a fase de acusação após o jogador sair da exploração
    verificarSuspeitoFinal(pistasColetadas, tabelaSuspeitos);

    // Limpa toda a memória alocada para evitar vazamentos
    liberarMapa(mansao);
    liberarPistas(pistasColetadas);
    liberarTabelaHash(tabelaSuspeitos);
    
    printf("\nO caso está encerrado por enquanto...\n");
    return 0;
}


// ===============================================================
//               IMPLEMENTAÇÃO DAS FUNÇÕES DO JOGO
// ===============================================================

/**
 * @brief Monta o mapa da mansão e preenche a tabela hash com as evidências.
 * Separei essa lógica do main para deixar o código mais organizado.
 */
void montarCenario(Sala** mansao, HashNode** tabela) {
    // 1. Criando o mapa da mansão (Árvore Binária)
    *mansao = criarSala("Hall de Entrada");
    (*mansao)->esquerda = criarSala("Biblioteca");
    (*mansao)->direita = criarSala("Sala de Jantar");
    (*mansao)->esquerda->direita = criarSala("Escritorio");
    (*mansao)->direita->esquerda = criarSala("Cozinha");
    (*mansao)->direita->direita = criarSala("Jardim de Inverno");
    (*mansao)->esquerda->direita->esquerda = criarSala("Sala do Cofre");

    // 2. Preenchendo a Tabela Hash com todas as pistas e seus suspeitos
    // Estas são as "verdades" do jogo.
    printf("Analisando relatórios e montando a base de evidências...\n\n");
    inserirNaHash(tabela, "Carta amassada com ameacas", "Mordomo Alistair");
    inserirNaHash(tabela, "Frasco de veneno vazio", "Chef Gustav");
    inserirNaHash(tabela, "Documento financeiro suspeito", "Conde Frederick");
    inserirNaHash(tabela, "Peca de xadrez de marfim quebrada", "Conde Frederick");
    inserirNaHash(tabela, "Luva de jardinagem suja de terra", "Jardineira Beatrice");
    inserirNaHash(tabela, "Faca de cozinha com vestigios estranhos", "Chef Gustav");
}


/**
 * @brief Cria um novo cômodo (nó) para a árvore da mansão.
 */
Sala* criarSala(char* nome) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = novaSala->direita = NULL;
    return novaSala;
}


/**
 * @brief Função principal de navegação e coleta de pistas.
 * A função recebe um ponteiro para ponteiro para a raiz das pistas (**),
 * para que possamos modificar a árvore de pistas original.
 */
void explorarSalas(Sala* salaAtual, PistaNode** raizPistas, HashNode** tabelaSuspeitos) {
    if (salaAtual == NULL) return;

    printf("\n------------------------------------------------\n");
    printf("Voce esta em: %s\n", salaAtual->nome);

    // Lógica para encontrar pistas em salas específicas
    char* pistaEncontrada = NULL;
    if (strcmp(salaAtual->nome, "Escritorio") == 0) {
        pistaEncontrada = "Carta amassada com ameacas";
    } else if (strcmp(salaAtual->nome, "Cozinha") == 0) {
        pistaEncontrada = "Frasco de veneno vazio";
    } else if (strcmp(salaAtual->nome, "Sala do Cofre") == 0) {
        pistaEncontrada = "Documento financeiro suspeito";
    } else if (strcmp(salaAtual->nome, "Jardim de Inverno") == 0) {
        pistaEncontrada = "Luva de jardinagem suja de terra";
    }

    if (pistaEncontrada) {
        printf(">> Voce encontrou uma pista: %s\n", pistaEncontrada);
        *raizPistas = inserirPista(*raizPistas, pistaEncontrada);
    } else {
        printf("Voce olha ao redor, mas nao encontra nada de novo.\n");
    }

    if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
        printf("Este é o fim deste caminho.\n");
        return;
    }

    char escolha;
    while (1) {
        printf("\nEscolha seu proximo passo:\n");
        if (salaAtual->esquerda) printf(" (e) Esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita) printf(" (d) Direita: %s\n", salaAtual->direita->nome);
        printf(" (s) Sair para analisar as pistas e fazer uma acusacao\n");
        printf("Sua escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda) {
            explorarSalas(salaAtual->esquerda, raizPistas, tabelaSuspeitos);
            break;
        } else if (escolha == 'd' && salaAtual->direita) {
            explorarSalas(salaAtual->direita, raizPistas, tabelaSuspeitos);
            break;
        } else if (escolha == 's') {
            return;
        } else {
            printf("Comando invalido, detetive!\n");
        }
    }
}


/**
 * @brief Insere uma nova pista na Árvore de Busca Binária (BST).
 * A recursão torna a inserção elegante. A árvore mantém as pistas em ordem alfabética.
 */
PistaNode* inserirPista(PistaNode* raiz, char* pista) {
    // Caso base: se a árvore estiver vazia, cria um novo nó e o retorna como a nova raiz.
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        return novo;
    }
    
    // Se a pista já existe, não faz nada para evitar duplicatas.
    if (strcmp(pista, raiz->pista) == 0) {
        printf("(Esta pista ja foi registrada no seu caderno.)\n");
        return raiz;
    }

    // Recursão: decide se vai para a sub-árvore esquerda ou direita.
    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}


/**
 * @brief Função da fase final: acusação.
 * Lista as pistas e verifica se a acusação é válida.
 */
void verificarSuspeitoFinal(PistaNode* raizPistas, HashNode** tabela) {
    printf("\n\n================ O JULGAMENTO ================\n");
    printf("Voce reune todos na sala principal. E hora da verdade.\n");
    
    if (raizPistas == NULL) {
        printf("Voce nao coletou nenhuma pista. O caso continua um misterio.\n");
        return;
    }

    printf("Suas pistas coletadas sao:\n");
    mostrarPistasColetadas(raizPistas);
    
    printf("\nOs suspeitos sao: Mordomo Alistair, Chef Gustav, Conde Frederick, Jardineira Beatrice.\n");
    printf("Detetive, digite o nome e sobrenome do culpado: ");
    
    char acusado[50];
    // Leitura do nome completo do acusado (lê até o \n)
    scanf(" %[^\n]", acusado);

    int contagem = contarPistasParaSuspeito(raizPistas, acusado, tabela);

    printf("\nAnalisando as evidencias contra %s...\n", acusado);
    printf("Voce encontrou %d pista(s) que apontam para este suspeito.\n", contagem);

    if (contagem >= 2) {
        printf("\nVEREDITO: CULPADO! Com %d provas contundentes, voce desvendou o caso!\n", contagem);
    } else {
        printf("\nVEREDITO: INOCENTE! Suas provas sao insuficientes. O verdadeiro culpado escapa...\n");
    }
}


/**
 * @brief Conta recursivamente quantas pistas na BST apontam para um suspeito.
 * Combina a travessia na BST com a consulta na Tabela Hash.
 */
int contarPistasParaSuspeito(PistaNode* raizPistas, char* suspeito, HashNode** tabela) {
    if (raizPistas == NULL) {
        return 0;
    }

    // Procura o suspeito associado à pista do nó atual
    char* suspeitoDaPista = encontrarSuspeito(tabela, raizPistas->pista);
    int contagemAtual = 0;
    if (suspeitoDaPista && strcmp(suspeitoDaPista, suspeito) == 0) {
        contagemAtual = 1;
    }

    // Soma o resultado deste nó com os resultados das sub-árvores
    return contagemAtual + 
           contarPistasParaSuspeito(raizPistas->esquerda, suspeito, tabela) +
           contarPistasParaSuspeito(raizPistas->direita, suspeito, tabela);
}


// ===============================================================
//               FUNÇÕES AUXILIARES E DE ESTRUTURAS
// ===============================================================

/**
 * @brief Função hash simples para transformar uma string em um índice da tabela.
 * Soma os valores ASCII dos caracteres. É simples, mas funcional para este caso.
 */
unsigned int hashFunction(char* str) {
    unsigned int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash += str[i];
    }
    return hash % HASH_SIZE;
}


/**
 * @brief Insere uma associação pista-suspeito na tabela hash.
 */
void inserirNaHash(HashNode** tabela, char* pista, char* suspeito) {
    unsigned int index = hashFunction(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    
    // Insere no início da lista encadeada (mais fácil)
    novo->proximo = tabela[index];
    tabela[index] = novo;
}


/**
 * @brief Procura um suspeito na tabela hash com base em uma pista.
 */
char* encontrarSuspeito(HashNode** tabela, char* pista) {
    unsigned int index = hashFunction(pista);
    HashNode* atual = tabela[index];
    
    // Percorre a lista encadeada naquele índice
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL; // Não encontrou
}


/**
 * @brief Percorre a BST em ordem para mostrar as pistas em ordem alfabética.
 */
void mostrarPistasColetadas(PistaNode* raiz) {
    if (raiz != NULL) {
        mostrarPistasColetadas(raiz->esquerda);
        printf(" - %s\n", raiz->pista);
        mostrarPistasColetadas(raiz->direita);
    }
}


// ===============================================================
//               FUNÇÕES DE LIMPEZA DE MEMÓRIA
// ===============================================================

void liberarMapa(Sala* salaAtual) {
    if (salaAtual == NULL) return;
    liberarMapa(salaAtual->esquerda);
    liberarMapa(salaAtual->direita);
    free(salaAtual);
}

void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarTabelaHash(HashNode** tabela) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode* atual = tabela[i];
        while (atual) {
            HashNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
}