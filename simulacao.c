#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    int rotulo;
    int dirty;
    int lru;
}

int main(){

    FILE *arquivo;
    
    char nome_arquivo[30];

    int politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao, tempo_de_leitura_e_escrita;

    printf("Digite o nome do arquivo: \n");
    fgets(nome_arquivo, 30, stdin);

    arquivo = fopen(nome_arquivo, "r");
    if(arquivo == NULL){
        printf("Erro na Abertura do Arquivo \n");
        exit(2);
    }

    
    scanf("%d", &politica_de_escrita);
    if(politica_de_escrita != 0 && politica_de_escrita != 1){
        printf("Erro: Politica de Escrita Deve ser 0 ou 1");
        exit(3);
    }

    scanf("%d", &tamanho_da_linha);
    if (fmod(log2(tamanho_da_linha), 1) > 0){
        printf("Erro: Tamanho da Linha Nao esta na base 2");
        exit(4);
    }

    scanf("%d", &numero_de_linhas);
    if (fmod(log2(numero_de_linhas), 1) > 0){
        printf("Erro: Numero de Linhas Nao esta na base 2");
        exit(5);
    }

    scanf("%d", &associatividade);
    if (fmod(log2(numero_de_linhas), 1) > 0){
        printf("Erro: Numero de Linhas Nao esta na base 2");
        exit(6);
    } else {
        if(associatividade > numero_de_linhas){
            printf("Erro: Numero de Conjuntos Maior que o de Linhas");
            exit(7);
        }
    }

    scanf("%d", &hit_time);

    scanf("%d", &politica_de_substituicao);
    if(politica_de_substituicao != 0 && politica_de_substituicao != 1){
        printf("Erro: Politica de Substituicao Deve ser 0 ou 1");
        exit(8);
    }

    scanf("%d", &tempo_de_leitura_e_escrita);

    printf("Parametros da Simulacao:\n");
    printf("1.Politica de Escrita: - %d\n", politica_de_escrita);
    printf("2.Tamanho da Linha: - %d\n", tamanho_da_linha);
    printf("3.Numero de Linhas: - %d\n", numero_de_linhas);
    printf("4.Associatividade - %d\n", associatividade);
    printf("5.Hit Time - %d\n", hit_time);
    printf("6.Politica de Substituicao: - %d\n", politica_de_substituicao);
    printf("7.Tempos de Leitura/Escrita: - %d\n", tempo_de_leitura_e_escrita);

    
    return 0;
}


