#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Definição da Estrutura de Linha da Cache
typedef struct linhaCache{ 
    int tag;
    int dirty;
    int lru;
    int valid;
} LinhaCache;

// Definição da Estrutura de Conjunto da Cache
typedef struct conjunto{
    LinhaCache *linha;
} Conjunto;

// Definição da Estrutura de Cache
typedef struct cache{ 
    int politica_de_escrita;
    int tamanho_da_linha;
    int numero_de_linhas;
    int associatividade;
    int hit_time;
    int politica_de_substituicao;
    int numero_de_conjuntos;
    int tag_base2;
    int index_base2;
    int offset_base2;
    Conjunto *conjuntos;
} Cache;

// Inicializar a Cache Inserindo Conjuntos e Suas Linhas
void inicializar_cache(Cache *cache, int politica_de_escrita, int tamanho_da_linha, int numero_de_linhas, int associatividade, int hit_time, int politica_de_substituicao){
    
    int i, j, numero_de_conjuntos, index_base2, offset_base2, tag_base2;
    
    // Calcular Parâmetros da Simulação
    numero_de_conjuntos = numero_de_linhas/associatividade;
    offset_base2 = (int) round(log2(tamanho_da_linha));
    index_base2 = (int) round(log2(numero_de_conjuntos));
    tag_base2 = 32 - index_base2 - offset_base2;
    
    // Colocar os Parâmetros na Cache
    cache->politica_de_escrita = politica_de_escrita;
    cache->tamanho_da_linha = tamanho_da_linha;
    cache->numero_de_linhas = numero_de_linhas;
    cache->associatividade = associatividade;
    cache->hit_time = hit_time;
    cache->politica_de_substituicao = politica_de_substituicao;
    cache->numero_de_conjuntos = numero_de_conjuntos;
    cache->tag_base2 = tag_base2;
    cache->index_base2 = index_base2;
    cache->offset_base2 = offset_base2;
    
    
    // Inicializar Conjuntos e Linhas
    cache->conjuntos = (Conjunto *) malloc(numero_de_conjuntos * sizeof(Conjunto));
    for(i = 0; i < numero_de_conjuntos; i++){
        cache->conjuntos[i].linha = (LinhaCache *) malloc(associatividade * sizeof(LinhaCache));
        for(j = 0; j < associatividade; j++){
            cache->conjuntos[i].linha[j].dirty = 0;
            cache->conjuntos[i].linha[j].tag = 0;
            cache->conjuntos[i].linha[j].lru = 0;
            cache->conjuntos[i].linha[j].valid = 0;
        }
    }
}

// Métodos de Captura de Parâmetros da Simulação
char * get_arquivo(char *arquivo){

    printf("Nome do Arquivo: \n");
    scanf("%s", arquivo);

    return arquivo;
}

int get_politica_de_escrita(){
    
    int politica_de_escrita;

    printf("Politica de Escrita: 0-Write Through 1-Write Back\n");
    scanf("%d", &politica_de_escrita);
    
    if(politica_de_escrita != 0 && politica_de_escrita != 1){
        printf("Erro: Politica de Escrita Deve ser 0 ou 1");
        exit(2);
    }

    return politica_de_escrita;
}

int get_tamanho_da_linha(){

    int tamanho_da_linha;

    printf("Tamanho da Linha (Multiplo de 2)\n");
    scanf("%d", &tamanho_da_linha);
    
    if (tamanho_da_linha <= 0 || (tamanho_da_linha & (tamanho_da_linha - 1)) != 0){
        printf("Erro: Tamanho da Linha Nao esta na base 2!");
        exit(3);
    }

    return tamanho_da_linha;
}

int get_numero_de_linhas(){

    int numero_de_linhas;

    printf("Quantidade de Linhas (Multiplo de 2)\n");
    scanf("%d", &numero_de_linhas);
    
    if (numero_de_linhas <= 0 || (numero_de_linhas & (numero_de_linhas - 1)) != 0){
        printf("Erro: Numero de Linhas Nao esta na base 2!");
        exit(4);
    }

    return numero_de_linhas;
}

int get_associatividade(int numero_de_linhas){

    int associatividade;

    printf("Associatividade:\n");
    scanf("%d", &associatividade);

    if(associatividade <= 0 || (associatividade & (associatividade - 1)) != 0 || associatividade > numero_de_linhas){
        printf("Erro: Numero de Conjuntos Maior que o de Linhas!");
        exit(5);
    }

    return associatividade;
}

int get_hit_time(){

    int hit_time;

    printf("Tempo de Acerto (Hit Time) em ns \n");
    scanf("%d", &hit_time);
    
    if(hit_time < 0){
        printf("Erro: Hit Time Não Pode ser Menor que Zero!");
        exit(6);
    }
    
    return hit_time;
}

int get_politica_de_substituicao(){

    int politica_de_substituicao;

    printf("Politica de Substituicao (0-LRU 1-Aleatorio) \n");
    scanf("%d", &politica_de_substituicao);
    
    if(politica_de_substituicao != 0 && politica_de_substituicao != 1){
        printf("Erro: Politica de Substituicao Deve ser 0 (LRU) ou 1 (Aleatorio)!");
        exit(7);
    }

    return politica_de_substituicao;
}

int get_tempo_de_leitura_e_escrita(){
    
    int tempo_de_leitura_e_escrita;

    printf("Tempo de Leitura e Escrita na Memoria Principal em ns \n");
    scanf("%d", &tempo_de_leitura_e_escrita);
    
    if(tempo_de_leitura_e_escrita < 0){
        printf("Erro: Tempo de Acesso à Principal Não Pode ser Menor que Zero!");
        exit(8);
    }

    return tempo_de_leitura_e_escrita;
}

// Busca de dado na cache
int procurar_na_cache(Cache *cache, unsigned int endereco, int write_back){
    
    int tag, index, i, j, encontrado, linha_selecionada, ultimo_lru;
    tag = endereco >> (cache->index_base2 + cache->offset_base2);
    index = (endereco >> cache->offset_base2) & (( 1 << cache->index_base2) - 1);
    
    encontrado = 0;
    for(j=0; j<(cache->associatividade); j++){
        // printf("Tag: %d, Procura: %d\n", tag, cache->conjuntos[i].linha[j].tag);
        if(tag == cache->conjuntos[index].linha[j].tag && cache->conjuntos[index].linha[j].valid == 1){
            encontrado = 1;
            ultimo_lru = cache->conjuntos[index].linha[j].lru;
            linha_selecionada = j;
            if(write_back)
                cache->conjuntos[index].linha[j].dirty = 1;
            break;
        }
    }
    
    //LRU de HIT
    if(encontrado && cache->politica_de_substituicao == 0){
        for(i=0; i<(cache->associatividade); i++)
            if(i != linha_selecionada && cache->conjuntos[index].linha[i].lru < cache->conjuntos[index].linha[linha_selecionada].lru)
                cache->conjuntos[index].linha[i].lru++;
        cache->conjuntos[index].linha[linha_selecionada].lru = 0;
    }

    return encontrado;
}

// Miss de Cache: Remoção por Menos Recente Utilização
void cache_lru(Cache *cache, int dirty, int tag, int index, double *acesso_principal_escrita){
    
    int linha_selecionada, valor_mais_antigo = -1, linha_menos_recentemente_utilizada = -1, maior_lru = cache->associatividade;
    
    for(int i=0; i<cache->associatividade; i++)
        if(cache->conjuntos[index].linha[i].valid == 0){
            linha_menos_recentemente_utilizada = i;
            break;
        }
        
    if(linha_menos_recentemente_utilizada == -1){
        for(int i=0; i<cache->associatividade; i++)
            if(cache->conjuntos[index].linha[i].lru > valor_mais_antigo){
                valor_mais_antigo = cache->conjuntos[index].linha[i].lru;
                linha_menos_recentemente_utilizada = i;
            }
        maior_lru = cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].lru;
    }
    
    for(int i=0; i<cache->associatividade; i++)
        if(i != linha_menos_recentemente_utilizada && cache->conjuntos[index].linha[i].valid == 1)
            cache->conjuntos[index].linha[i].lru++;

    if(cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].dirty == 1 && cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].valid == 1)
        (*acesso_principal_escrita)++;
        
    cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].dirty = dirty;
    cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].tag = tag;
    cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].lru = 0;
    cache->conjuntos[index].linha[linha_menos_recentemente_utilizada].valid = 1;
}

// Miss de Cache: Remoção Aleatória
void cache_random(Cache *cache, int dirty, int tag, int index, double *acesso_principal_escrita){
        
    int linha_aleatoria = -1;
    for(int i=0; i<cache->associatividade; i++)
        if(cache->conjuntos[index].linha[i].valid == 0){
            linha_aleatoria = i;
            break;
        }
    
    if(linha_aleatoria == -1)
        linha_aleatoria = rand()%(cache->associatividade);
        
    if(cache->conjuntos[index].linha[linha_aleatoria].dirty == 1)
        (*acesso_principal_escrita)++;
        
    cache->conjuntos[index].linha[linha_aleatoria].dirty = dirty;
    cache->conjuntos[index].linha[linha_aleatoria].tag = tag;
    cache->conjuntos[index].linha[linha_aleatoria].lru = 0;
    cache->conjuntos[index].linha[linha_aleatoria].valid = 1;
}

// Leitura
void leitura(Cache *cache, unsigned int endereco, double *hit_cache_leitura, double *acesso_principal_leitura, double *acesso_principal_escrita){
    
    int encontrado = procurar_na_cache(cache, endereco, 0), tag, index;
    
    if(encontrado == 1){
        (*hit_cache_leitura)++;
    } else {
        (*acesso_principal_leitura)++;
        tag = endereco >> (cache->index_base2 + cache->offset_base2);
        index = (endereco >> cache->offset_base2) & (( 1 << cache->index_base2) - 1 );
        if(cache->politica_de_substituicao == 0)
            cache_lru(cache, 0, tag, index, acesso_principal_escrita);
        else 
            cache_random(cache, 0, tag, index, acesso_principal_escrita);
    }
}

// Escrita Write Back
void escrita_writeback(Cache *cache, unsigned int endereco, double *hit_cache_escrita, double *acesso_principal_leitura, double *acesso_principal_escrita){

    int encontrado = procurar_na_cache(cache, endereco, 1), tag, index;

    if(encontrado == 1)
        (*hit_cache_escrita)++;
    else {
        (*acesso_principal_leitura)++;
        tag = endereco >> (cache->index_base2 + cache->offset_base2);
        index = (endereco >> cache->offset_base2) & (( 1 << cache->index_base2) - 1 );
        if(cache->politica_de_substituicao == 0)
            cache_lru(cache, 1, tag, index, acesso_principal_escrita);
        else
            cache_random(cache, 1, tag, index, acesso_principal_escrita);
    }
}

// Escrita Write Through
void escrita_writethrough(Cache *cache, unsigned int endereco, double *hit_cache_escrita, double *acesso_principal_escrita){
    
    int encontrado = procurar_na_cache(cache, endereco, 0);

    if(encontrado == 1)
        (*hit_cache_escrita)++;
        
    (*acesso_principal_escrita)++;
}

// Mostrar Todos os Parâmetros da Simulação
void mostrar_parametros(Cache *cache, int tempo_de_leitura_e_escrita){

    int tamanho_cache = (int) round(log2(cache->tamanho_da_linha * cache->numero_de_linhas));
    char prefixo;
    
    printf("--------------------------------------------------------------------\n");
    printf("Parametros da Simulacao:\n");
    printf("--------------------------------------------------------------------\n");
    printf("%d Byte(s) por Linha\n", cache->tamanho_da_linha);
    printf("%d Linha(s) no Total\n", cache->numero_de_linhas);
        
    if(tamanho_cache < 10)
        printf("Tamanho da Cache: %d B\n", 1 << (tamanho_cache%10));
    else{
        if(tamanho_cache < 20)
            prefixo = 'K';
        else
            if(tamanho_cache < 30)
                prefixo = 'M';
            else
                if(tamanho_cache < 40)
                    prefixo = 'G';
                else
                    if(tamanho_cache < 50)
                        prefixo = 'T';
                    else
                        prefixo = '?';
        printf("Tamanho da Cache: %d %cB\n", 1 << (tamanho_cache%10), prefixo);                
    }
    
    printf("%d Linha(s) por Conjunto\n", cache->associatividade);
    printf("%d Conjunto(s)\n", cache->numero_de_conjuntos);
    if(cache->politica_de_escrita == 0)
        printf("Politica de Escrita: Write Through com No Write Allocate\n");
    else
        printf("Politica de Escrita: Write Back com Write Allocate\n");
    if(cache->politica_de_substituicao == 0)
        printf("Politica de Substituicao: LRU - Menos Recentemente Usado\n");
    else
        printf("Politica de Substituicao: Aleatório\n");
    printf("%d ns de Hit Time\n", cache->hit_time);
    printf("%d ns de Leitura/Escrita na Memória Principal\n", tempo_de_leitura_e_escrita);
    printf("--------------------------------------------------------------------\n");
}

// Calcular e Mostrar as Taxas de Acerto por Operação e Geral
double mostrar_taxas_de_acerto(double hit_cache_leitura, int quantidade_leituras, double hit_cache_escrita, int quantidade_escritas){

    if(quantidade_leituras != 0)
        printf("Hit Rate de Leituras: %.4f - %.0f Hits para %d Tentativas\n", hit_cache_leitura/quantidade_leituras, hit_cache_leitura, quantidade_leituras);
    if(quantidade_escritas != 0)
        printf("Hit Rate de Escritas: %.4f - %.0f Hits para %d Tentativas\n", hit_cache_escrita/quantidade_escritas, hit_cache_escrita, quantidade_escritas);
    if(quantidade_leituras + quantidade_escritas != 0)
        printf("Hit Rate Geral: %.4f - %.0f Hits para %d Tentativas\n", (hit_cache_leitura + hit_cache_escrita)/(quantidade_leituras + quantidade_escritas), (hit_cache_leitura + hit_cache_escrita), (quantidade_leituras + quantidade_escritas));
    
    return (hit_cache_leitura + hit_cache_escrita)/(quantidade_leituras + quantidade_escritas);
}

// Principal
int main(){

    // Inicializações
    FILE *arquivo;
    Cache *cache;
    int politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao, tempo_de_leitura_e_escrita, numero_de_conjuntos, tag_base2, index_base2, offset_base2, quantidade_leituras = 0, quantidade_escritas = 0;
    unsigned int endereco;
    double hit_rate, hit_cache_leitura = 0, hit_cache_escrita = 0, acesso_principal_leitura = 0, acesso_principal_escrita = 0;
    char acao, nome_arquivo[20];
    srand(time(NULL));
    
    // Abertura do Arquivo
    get_arquivo(nome_arquivo);
    arquivo = fopen(nome_arquivo, "r");
    if(arquivo == NULL){
        printf("Erro na Abertura do Arquivo!\n");
        exit(1);
    }
    
    // Capturar Parâmetros da Simulação
    politica_de_escrita = get_politica_de_escrita();
    tamanho_da_linha = get_tamanho_da_linha();
    numero_de_linhas = get_numero_de_linhas();
    associatividade = get_associatividade(numero_de_linhas);
    hit_time = get_hit_time();
    politica_de_substituicao = get_politica_de_substituicao();
    tempo_de_leitura_e_escrita = get_tempo_de_leitura_e_escrita();
    
    // Criar Cache e Alocar Seu Espaço de Memória
    cache = (Cache *) malloc(sizeof(Cache));
    inicializar_cache(cache, politica_de_escrita, tamanho_da_linha, numero_de_linhas, associatividade, hit_time, politica_de_substituicao);

    // Analisar Cada Linha do Arquivo
    while(fscanf(arquivo, "%x %c", &endereco, &acao) != EOF){
        
        // Checar Se é Leitura ou Escrita
        if(acao == 'R'){
            
            // Ler na Cache e Incrementar Quantidade de Leituras
            leitura(cache, endereco, &hit_cache_leitura, &acesso_principal_leitura, &acesso_principal_escrita);
            
            // Incrementar Quantidade de Leituras
            quantidade_leituras++;
        }
        else 
            if(acao == 'W'){
                
                // Checar Política de Escrita
                if(politica_de_escrita == 0)
                
                    // Escrever com Write Through + No Write Allocate
                    escrita_writethrough(cache, endereco, &hit_cache_escrita, &acesso_principal_escrita);
                else
                
                    // Escrever com Write Back + Write Allocate
                    escrita_writeback(cache, endereco, &hit_cache_escrita, &acesso_principal_leitura, &acesso_principal_escrita);
                    
                // Incrementar Quantidade de Escritas
                quantidade_escritas++;
            }
            else {
                printf("Erro de endereçamento");
                exit(9);
            }
    }
    
    // Enviar Dados com Dirty Bit Não Substituidos na Cache no Final da Simulação para Principal
    for(int i=0; i<cache->numero_de_conjuntos; i++){
        for(int j=0; j<cache->associatividade; j++){
            if(cache->conjuntos[i].linha[j].dirty == 1 && cache->conjuntos[i].linha[j].valid == 1){
                acesso_principal_escrita++;
            }
        }
    }
    
    // Mostrar os Parâmetros Utilizados na Simulação
    mostrar_parametros(cache, tempo_de_leitura_e_escrita);

    // Mostrar os Totais de Endereços de Leitura, de Escrita e Geral do Arquivo
    printf("Operações de Leitura: %d\n", quantidade_leituras);
    printf("Operações de Escritas: %d\n", quantidade_escritas);
    printf("Total de Operações: %d\n", quantidade_leituras + quantidade_escritas);

    // Mostrar Total de Leituras na Memória Principal
    printf("%.0f Leitura(s) na Memória Principal\n", acesso_principal_leitura);
    
    // Mostrar Total de Escritas na Memória Principal
    printf("%.0f Escrita(s) na Memória Principal\n", acesso_principal_escrita);

    // Mostrar Taxa de Acerto (Hit Rate)
    hit_rate = mostrar_taxas_de_acerto(hit_cache_leitura, quantidade_leituras, hit_cache_escrita, quantidade_escritas);

    // Mostrar Tempo Médio de Acerto
    printf("Tempo Medio de Acesso: %.4f ns\n", hit_time + (1.0 - hit_rate) * tempo_de_leitura_e_escrita);
    
    // Fechar Arquivo
    fclose(arquivo);
    
    // Liberar a Memória Alocada
    for(int i=0; i<cache->numero_de_conjuntos; i++)
        free(cache->conjuntos[i].linha);
    free(cache->conjuntos);
    free(cache);
    
    return 0;
}