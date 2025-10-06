/*
    Trabalho de Programação - Jogo "Detective Quest"
    Aluno: Renan Felipe Timoteo Bueno   
    Data: 06/10/2025

    Objetivo: Criar um mapa de uma mansão usando uma árvore binária em C.
    O programa monta o mapa e depois deixa o jogador navegar por ele.
*/

#include <stdio.h>
#include <stdlib.h> // Preciso disso pro malloc e free
#include <string.h> // E isso pro strcpy, pra colocar os nomes nas salas

// Criei uma struct pra representar cada sala da mansão.
// Ela tem o nome da sala e dois ponteiros, um pra esquerda e outro pra direita.
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// --- Funções que o programa vai usar ---
Sala* criarSala(char* nome);
void navegar(Sala* sala_atual);
void limparMemoria(Sala* sala_atual);


// --- Programa Principal ---
int main() {
    
    // 1. Montando o mapa da mansão
    // Criei a primeira sala, que vai ser o começo do jogo.
    Sala *hallDeEntrada = criarSala("Hall de Entrada");

    // Agora vou conectando as outras salas, uma por uma.
    // A sala da esquerda do Hall é a Sala de Jantar, a da direita é a Biblioteca.
    hallDeEntrada->esquerda = criarSala("Sala de Jantar");
    hallDeEntrada->direita = criarSala("Biblioteca");

    // Continuei criando os caminhos a partir das salas novas
    hallDeEntrada->esquerda->esquerda = criarSala("Cozinha");
    hallDeEntrada->direita->esquerda = criarSala("Escritorio");
    hallDeEntrada->direita->direita = criarSala("Sala de Musica");
    
    // Adicionei um caminho mais fundo pra ter mais o que explorar
    hallDeEntrada->direita->esquerda->direita = criarSala("Passagem Secreta");

    // 2. Começando o jogo
    printf("--- Detective Quest ---\n");
    printf("Voce e um detetive investigando a misteriosa Mansao Enigma.\n");
    printf("Cuidado onde pisa...\n");

    // Chamo a função de navegação, começando pelo Hall de Entrada.
    navegar(hallDeEntrada);

    // 3. Fim de jogo
    // Depois que o jogador sai, é importante limpar a memória que o malloc usou.
    limparMemoria(hallDeEntrada);
    
    printf("\nVoce decidiu se retirar por enquanto. O misterio aguarda...\n");

    return 0;
}


// Função para criar uma sala nova na memória
Sala* criarSala(char* nome) {
    // Pede um espaco na memoria do tamanho da struct Sala
    Sala* nova_sala = (Sala*) malloc(sizeof(Sala));

    // Copia o nome que a gente passou para dentro da struct
    strcpy(nova_sala->nome, nome);

    // Quando uma sala é criada, ela não tem caminhos ainda.
    nova_sala->esquerda = NULL;
    nova_sala->direita = NULL;
    
    // Deixei esse printf aqui pra testar se as salas estavam sendo criadas certo.
    // printf("DEBUG: Sala '%s' criada no endereco %p\n", nome, nova_sala);

    return nova_sala;
}


// Função principal da navegação. É recursiva.
void navegar(Sala* sala_atual) {
    // Se a sala não existir (caminho vazio), a gente para.
    if (sala_atual == NULL) {
        return;
    }

    printf("\n-----------------------------------\n");
    printf("Voce esta em: %s\n", sala_atual->nome);

    // Verifica se a sala é um beco sem saída (não tem caminho nem pra esq. nem pra dir.)
    if (sala_atual->esquerda == NULL && sala_atual->direita == NULL) {
        printf("Parece que nao ha mais para onde ir a partir daqui.\n");
        return;
    }

    char escolha;
    int escolhaValida = 0; // Uma "flag" pra saber se o jogador digitou algo certo

    // Usei um do-while pra garantir que ele peça a escolha pelo menos uma vez
    // e continue pedindo se o jogador digitar algo errado.
    do {
        printf("\nEscolha seu caminho:\n");

        if (sala_atual->esquerda) { // se o ponteiro da esquerda não for nulo
            printf(" -> Digite 'e' para ir para a ESQUERDA (%s)\n", sala_atual->esquerda->nome);
        }
        if (sala_atual->direita) { // se o ponteiro da direita não for nulo
            printf(" -> Digite 'd' para ir para a DIREITA (%s)\n", sala_atual->direita->nome);
        }
        printf(" -> Digite 's' para SAIR da mansao\n");
        printf("O que voce faz? ");
        
        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e':
                if (sala_atual->esquerda) {
                    navegar(sala_atual->esquerda); // Chama a função de novo para a sala da esquerda
                    escolhaValida = 1;
                } else {
                    printf("A porta da esquerda esta trancada...\n");
                }
                break;
            case 'd':
                if (sala_atual->direita) {
                    navegar(sala_atual->direita); // Chama a função de novo para a sala da direita
                    escolhaValida = 1;
                } else {
                    printf("O caminho da direita esta bloqueado...\n");
                }
                break;
            case 's':
                escolhaValida = 1; // Sair também é uma opção válida
                break;
            default:
                printf("Comando nao reconhecido, detetive. Tente novamente.\n");
                break;
        }
    // O loop continua enquanto a escolha não for válida (e não for 's' para sair)
    } while (!escolhaValida); 
}


// Função para limpar a memória usada pelo mapa.
// Ela vai liberando as salas de baixo pra cima pra não dar erro.
void limparMemoria(Sala* sala_atual) {
    if (sala_atual == NULL) {
        return;
    }
    
    // Primeiro limpa os filhos
    limparMemoria(sala_atual->esquerda);
    limparMemoria(sala_atual->direita);
    
    // Depois que os filhos foram liberados, libera o pai
    free(sala_atual);
}