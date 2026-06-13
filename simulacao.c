#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// Definição da Estrutura de Linha da Cache
typedef struct linhaCache{ 
    int tag;
    int dirty;
    int lru;
} LinhaCache;

// Definição da Estrutura de Conjunto da Cache
typedef struct conjunto{
    LinhaCache *linha;
} Conjunto;

// Definição da Estrutura de Cache
typedef struct cache{ 
    int politica_de_escrita;
    int numero_conjuntos;
    int tamanho_da_linha;
    int numero_de_linhas;
    int associatividade;
    Conjunto *conjuntos;
} Cache;

// Inicializar a Cache Inserindo Conjuntos e Suas Linhas
void inicializar_cache(Cache *cache){
    
    int i, j;
    
    cache->numero_conjuntos = cache->numero_de_linhas / cache->associatividade;
    cache->conjuntos = (Conjunto *) malloc(cache->numero_conjuntos * sizeof(Conjunto));

    for(i = 0; i < cache->numero_conjuntos; i++){

        cache->conjuntos[i].linha = (LinhaCache *) malloc(cache->associatividade * sizeof(LinhaCache));
        for(j = 0; j < cache->associatividade; j++){

            cache->conjuntos[i].linha[j].dirty = 0;
            cache->conjuntos[i].linha[j].tag = 0;
            cache->conjuntos[i].linha[j].lru = 0;

        }
        
    }

}

// Métodos de Captura de Parâmetros da Simulação
int get_politica_de_escrita(){
    
    int politica_de_escrita;

    scanf("%d", &politica_de_escrita);
    if(politica_de_escrita != 0 && politica_de_escrita != 1){
        printf("Erro: Politica de Escrita Deve ser 0 ou 1");
        exit(3);
    }

    return politica_de_escrita;
}

int get_tamanho_da_linha(){

    int tamanho_da_linha;

    scanf("%d", &tamanho_da_linha);
    if (fmod(log2(tamanho_da_linha), 1) > 0){
        printf("Erro: Tamanho da Linha Nao esta na base 2");
        exit(4);
    }

    return tamanho_da_linha;
}

int get_numero_de_linhas(){

    int numero_de_linhas;

    scanf("%d", &numero_de_linhas);
    if (fmod(log2(numero_de_linhas), 1) > 0){
        printf("Erro: Numero de Linhas Nao esta na base 2");
        exit(5);
    }

    return numero_de_linhas;
}

int get_associatividade(int numero_de_linhas){

    int associatividade;

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

    return associatividade;
}

int get_hit_time(){

    int hit_time;

    scanf("%d", &hit_time);
    
    return hit_time;
}

int get_politica_de_substituicao(){

    int politica_de_substituicao;

    scanf("%d", &politica_de_substituicao);
    if(politica_de_substituicao != 0 && politica_de_substituicao != 1){
        printf("Erro: Politica de Substituicao Deve ser 0 ou 1");
        exit(8);
    }

    return politica_de_substituicao;
}

int get_tempo_de_leitura_e_escrita(){
    
    int tempo_de_leitura_e_escrita;

    scanf("%d", &tempo_de_leitura_e_escrita);

    return tempo_de_leitura_e_escrita;
}

// Mostrar Todos os Parâmetros da Simulação
void mostrar_parametros(int politica_de_escrita, int tamanho_da_linha, int numero_de_linhas, int associatividade, int hit_time, int politica_de_substituicao, int tempo_de_leitura_e_escrita){

    printf("Parametros da Simulacao:\n");
    printf("1.Politica de Escrita: - %d\n", politica_de_escrita);
    printf("2.Tamanho da Linha: - %d\n", tamanho_da_linha);
    printf("3.Numero de Linhas: - %d\n", numero_de_linhas);
    printf("4.Associatividade - %d\n", associatividade);
    printf("5.Hit Time - %d\n", hit_time);
    printf("6.Politica de Substituicao: - %d\n", politica_de_substituicao);
    printf("7.Tempos de Leitura/Escrita: - %d\n", tempo_de_leitura_e_escrita);

}

// Mostrar os Totais de Endereços de Leitura, de Escrita e Gerais
void mostrar_total_de_enderecos(FILE *arquivo){

    char linha1[100];
    char linha2[100];
    int quantidade_de_leituras;
    int quantidade_de_escritas;

    while(fscanf(arquivo, "%s %s", &linha1, &linha2) != EOF){
        if(strcmp(linha2, "R") == 0)
            quantidade_de_leituras++;
        else 
            quantidade_de_escritas++;
    }

    printf("Leituras: %d\nEscritas: %d\nTotal %d\n", quantidade_de_leituras, quantidade_de_escritas, quantidade_de_leituras + quantidade_de_escritas);
}

// Mostrar o Hit Time Médio com Base na Fórmula
void mostrar_tempo_medio_de_acerto(int hit_time, int tempo_de_leitura_e_escrita, double hit_rate){

    double miss_rate;
    double tempo_medio_de_acesso = 0.0;
    
    tempo_medio_de_acesso = hit_time + (1.0 - hit_rate) * tempo_de_leitura_e_escrita;

    printf("Tempo medio de acesso: %.4f ns\n");
}

// Principal
int main(){

    FILE *arquivo;
    int politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao, tempo_de_leitura_e_escrita;

    arquivo = fopen("teste.txt", "r");
    if(arquivo == NULL){
        printf("Erro na Abertura do Arquivo \n");
        exit(2);
    }

    int politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao, tempo_de_leitura_e_escrita, hit_rate;

    // Capturar Parâmetros da Simulação
    politica_de_escrita = get_politica_de_escrita();
    tamanho_da_linha = get_tamanho_da_linha();
    numero_de_linhas = get_numero_de_linhas();
    associatividade = get_associatividade(numero_de_linhas);
    hit_time = get_hit_time();
    politica_de_substituicao = get_politica_de_substituicao();
    tempo_de_leitura_e_escrita = get_tempo_de_leitura_e_escrita();

    Cache *cache;
    cache = (Cache *) malloc(sizeof(Cache));
    inicializar_cache(cache);

    ///////////////////////

    

    ///////////////////////

    // Mostrar os Parâmetros Utilizados na Simulação
    mostrar_parametros(politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao, tempo_de_leitura_e_escrita);

    // Mostrar os Totais de Endereços de Leitura, de Escrita e Geral do Arquivo
    mostrar_total_de_enderecos(arquivo);

    // Mostrar Total de Escritas na Memória Principal
    mostrar_total_de_escritas();
    
    // Mostrar Total de Leituras na Memória Principal
    mostrar_total_de_leituras();

    // Mostrar Taxa de Acerto (Hit Rate)
    mostrar_taxa_de_acerto();

    // Mostrar Tempo Médio de Acerto
    mostrar_tempo_medio_de_acerto(hit_time, tempo_de_leitura_e_escrita, hit_rate);
    
    return 0;
}




