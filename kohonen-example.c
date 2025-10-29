//1. Definições e Estruturas
//Definimos as dimensões do mapa (Lado_X, Lado_Y) e a dimensão dos vetores de dados (Dimensoes).
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Definicoes de Configuracao (nao especificadas nas fontes, mas necessarias para implementacao)
#define DIMENSOES 2        // Dimensao do vetor de entrada/peso (ex: x1, x2)
#define MAPA_LADO_X 10     // Largura do mapa (numero de nos)
#define MAPA_LADO_Y 10     // Altura do mapa (numero de nos)
#define ITERACOES_MAX 5000 // Numero de passos de iteracao (deve ser grande, tipicamente > 500 * N de nos [9])

// Estrutura para representar os nos do mapa
typedef struct {
    double pesos[DIMENSOES];  // Vetores de referencia m_i [2, 5]
    int x;                    // Coordenada x na grade topologica
    int y;                    // Coordenada y na grade topologica
} NoMapa;

// O mapa eh uma matriz de Nos
NoMapa som_mapa[MAPA_LADO_X][MAPA_LADO_Y];

// Dados de entrada (Amostra de treinamento X. Uma amostra x(t) eh selecionada a cada passo t [5])
// Exemplo conceitual: 4 vetores de entrada 2D
double dados_entrada[6][DIMENSOES] = {
    {0.1, 0.1},
    {0.9, 0.9},
    {0.1, 0.9},
    {0.9, 0.1}
};
#define NUM_DADOS 4
//2. Funções Auxiliares
//A. Função de Distância (Distância Euclidiana)
//O SOM usa o critério de minimizar a distância euclidiana ∥x−m 
//i
// ∥ para encontrar o vetor de livro de código (codebook) de melhor correspondência (o vencedor).
double calcular_distancia_euclidiana(double x[], double m[]) {
    double soma_quadrados = 0.0;
    for (int d = 0; d < DIMENSOES; d++) {
        soma_quadrados += pow(x[d] - m[d], 2);
    }
    return sqrt(soma_quadrados);
}
// B. Função de Vizinhança e Decaimento de Parâmetros
// Tanto o fator de ganho de aprendizagem α(t) quanto o tamanho da vizinhança N 
// c
// ​
//  (t) devem diminuir monotonicamente com o tempo t para garantir a ordenação global.
// • Raio da Vizinhança R(t): Começa amplo e encolhe.
// • Taxa de Aprendizagem \alpha(t): Diminui com o tempo.
// Parametros de decaimento iniciais (necessarios para a ordenacao [9])
#define ALPHA_INICIAL 0.6
#define RAIO_INICIAL ((MAPA_LADO_X > MAPA_LADO_Y ? MAPA_LADO_X : MAPA_LADO_Y) / 2.0)
#define TEMPO_CONSTANTE (ITERACOES_MAX / 4.0) // Constante de tempo para decaimento

// Calcula o fator de ganho de aprendizado alpha(t) [2, 3]
double calcular_alpha(int t) {
    // Decaimento exponencial ou linear sao metodos tipicos [9]. Usamos exponencial aqui.
    return ALPHA_INICIAL * exp(-(double)t / TEMPO_CONSTANTE);
}

// Calcula o raio da vizinhanca R(t)
double calcular_raio_vizinhanca(int t) {
    return RAIO_INICIAL * exp(-(double)t / TEMPO_CONSTANTE);
}

// Funcao de vizinhanca h_c(t) (Gauassiana simplificada) [3, 8]
// h_c(t) decai com a distancia na grade topologica.
double calcular_h_ci(int tx, int ty, int cx, int cy, double R_t) {
    // Distancia quadrada na grade topologica
    double dist_quadrada_topologica = pow((double)tx - cx, 2) + pow((double)ty - cy, 2);

    // Se o raio for zero, soh a celula vencedora eh atualizada (Ref. h_c = 1 se i=c e 0 caso contrario [3])
    if (R_t <= 0.5) {
        return (tx == cx && ty == cy) ? 1.0 : 0.0;
    }

    // A vizinhanca h_c(t) eh frequentemente definida como uma "curva em sino" (bell curve) [3]
    // Esta eh uma implementacao comum (Gaussiana) baseada na estrutura de h_c [3, 8].
    return exp(-dist_quadrada_topologica / (2 * pow(R_t, 2)));
}
// 3. O Algoritmo Principal do SOM
// A. Inicialização dos Pesos
// Os vetores de referência m 
// i
// ​
//  (0) são escolhidos arbitrariamente, geralmente aleatoriamente.
void inicializar_som() {
    for (int x = 0; x < MAPA_LADO_X; x++) {
        for (int y = 0; y < MAPA_LADO_Y; y++) {
            som_mapa[x][y].x = x;
            som_mapa[x][y].y = y;
            for (int d = 0; d < DIMENSOES; d++) {
                // Inicializacao aleatoria (entre 0.0 e 1.0 para este exemplo)
                // Nota: O uso de rand() nao esta nas fontes, mas a inicializacao aleatoria sim [6].
                som_mapa[x][y].pesos[d] = (double)rand() / RAND_MAX;
            }
        }
    }
}
// B. Treinamento Iterativo
// O loop de treinamento envolve a seleção de um vetor de entrada x(t), a busca pela unidade vencedora c, e a atualização dos vizinhos.
void treinar_som() {
    int c_x, c_y; // Coordenadas do vencedor (c)
    double alpha_t, R_t;

    for (int t = 1; t <= ITERACOES_MAX; t++) {
        // 1. Selecionar vetor de entrada x(t) [5]
        // Selecao aleatoria de um dos 4 dados de entrada
        int indice_entrada = rand() % NUM_DADOS;
        double *x_t = dados_entrada[indice_entrada];

        // 2. Encontrar a Unidade de Melhor Correspondencia (Vencedor 'c')
        double min_distancia = INFINITY;
        
        for (int x = 0; x < MAPA_LADO_X; x++) {
            for (int y = 0; y < MAPA_LADO_Y; y++) {
                double dist = calcular_distancia_euclidiana(x_t, som_mapa[x][y].pesos);
                if (dist < min_distancia) {
                    min_distancia = dist;
                    c_x = x;
                    c_y = y;
                }
            }
        }
        // O vencedor c eh o indice i que minimiza ||x - m_i|| [3, 6]

        // 3. Calcular os parametros de decaimento para esta iteracao t
        alpha_t = calcular_alpha(t);
        R_t = calcular_raio_vizinhanca(t);
        
        // 4. Aplicar a Adaptacao (Atualizar os Pesos)
        for (int i_x = 0; i_x < MAPA_LADO_X; i_x++) {
            for (int i_y = 0; i_y < MAPA_LADO_Y; i_y++) {
                
                // Calcular a intensidade de vizinhanca h_ci (relativa a distancia do vencedor c)
                double h_ci = calcular_h_ci(i_x, i_y, c_x, c_y, R_t);
                
                // O ajuste eh feito para os vetores de referencia m_i que pertencem a vizinhanca [3]
                if (h_ci > 0.001) { // Verifica se esta dentro do raio R_t
                    for (int d = 0; d < DIMENSOES; d++) {
                        // Regra de Atualizacao [3]:
                        // m_i(t+1) = m_i(t) + h_ci(t) * alpha(t) * [x(t) - m_i(t)]
                        // Note: Aqui estamos incorporando alpha_t na h_ci, simplificando a formula.
                        // O termo alpha(t) no algoritmo principal (7) é usado em conjunto com h_c.
                        
                        double fator_ajuste = alpha_t * h_ci;
                        som_mapa[i_x][i_y].pesos[d] += fator_ajuste * (x_t[d] - som_mapa[i_x][i_y].pesos[d]);
                    }
                }
            }
        }
        
        // A ordenacao global ocorre enquanto o raio R(t) estiver encolhendo [3, 9]
    }
}

// Funcao Principal
int main() {
    printf("Iniciando a simulacao do Mapa Auto-Organizado (SOM).\n");

    // Inicializacao
    inicializar_som();

    // Treinamento
    treinar_som();

    printf("Treinamento concluido. Os vetores de peso estao agora espacialmente ordenados.\n");
    // Em uma aplicacao real, os vetores de peso (codebook vectors) seriam agora analisados 
    // ou usados para classificacao [2, 11].

    return 0;
}