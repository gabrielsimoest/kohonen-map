#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define MAPA_LADO_X 4
#define MAPA_LADO_Y 4
#define DIMENSOES 15
#define AMOSTRAS 40
#define ITERACOES_MAX 2000 // Aumentar iterações para melhor convergência

#define ALPHA_INICIAL 0.4
#define RAIO_INICIAL ((MAPA_LADO_X > MAPA_LADO_Y ? MAPA_LADO_X : MAPA_LADO_Y) / 1.5)
#define TEMPO_CONSTANTE (ITERACOES_MAX / 4.0) // Decaimento mais lento

int main()
{
    // Verificar se a classe deve ser separada do mapa de kohonen mesmo
    int x, y, d;
    float mapa[MAPA_LADO_X][MAPA_LADO_Y][DIMENSOES];

    float dados_entrada[AMOSTRAS][DIMENSOES];

    // LEITURA DOS DADOS
    FILE *in;

    if ((in = fopen("movies.data", "rt")) == NULL)
    {
        printf("Cannot open input file.\n");
        return 1;
    }

    x = 0;
    while (!feof(in) && x < AMOSTRAS)
    {
        int result = fscanf(in, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                            &dados_entrada[x][0], &dados_entrada[x][1], &dados_entrada[x][2],
                            &dados_entrada[x][3], &dados_entrada[x][4], &dados_entrada[x][5],
                            &dados_entrada[x][6], &dados_entrada[x][7], &dados_entrada[x][8],
                            &dados_entrada[x][9], &dados_entrada[x][10], &dados_entrada[x][11],
                            &dados_entrada[x][12], &dados_entrada[x][13], &dados_entrada[x][14]);

        if (result == 15)
        {
            x++;
        }
        else
        {
            break;
        } 
    }

    int total_amostras_lidas = x;
    printf("Total de amostras lidas: %d\n", total_amostras_lidas);

    srand(time(NULL));

    // Inicialização melhorada dos pesos (distribuição mais controlada)
    for (x = 0; x < MAPA_LADO_X; x++) // linha
    {
        for (y = 0; y < MAPA_LADO_Y; y++) // coluna
        {
            for (d = 0; d < DIMENSOES; d++) // atributo/peso
            {
                if (d == 0) {
                    // Para ano: inicializar com gradiente suave
                    float gradiente = (float)(x * MAPA_LADO_Y + y) / (MAPA_LADO_X * MAPA_LADO_Y - 1);
                    mapa[x][y][d] = gradiente + ((float)rand() / RAND_MAX - 0.5) * 0.2;
                } else {
                    // Para gêneros: pequenos valores aleatórios
                    mapa[x][y][d] = ((float)rand() / RAND_MAX) * 0.3;
                }
                
                // Garantir que os valores estejam no intervalo [0,1]
                if (mapa[x][y][d] < 0.0) mapa[x][y][d] = 0.0;
                if (mapa[x][y][d] > 1.0) mapa[x][y][d] = 1.0;
            }
        }
    }
    // Mostrar uma representação visual do mapa
    printf("\n=== MAPA INICIAL (Pesos Aleatorios) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            printf("%.2f ", mapa[x][y][0]);
        }
        printf("\n");
    }

    int v_x, v_y; // Coordenadas do vencedor (v)
    for (int t = 1; t <= ITERACOES_MAX; t++)
    {
        int indice_selecionado = rand() % total_amostras_lidas; // Usar número real
        float min_distancia = INFINITY;
        float alpha_t, R_t;

        for (int x = 0; x < MAPA_LADO_X; x++)
        {
            for (int y = 0; y < MAPA_LADO_Y; y++)
            {
                float soma_quadrados = 0.0;
                for (int d = 0; d < DIMENSOES; d++) // Usar todas as dimensões (ano + gêneros)
                {
                    float diff = dados_entrada[indice_selecionado][d] - mapa[x][y][d];
                    soma_quadrados += diff * diff;
                }
                float dist = sqrt(soma_quadrados);

                if (dist < min_distancia)
                {
                    min_distancia = dist;
                    v_x = x;
                    v_y = y;
                }
            }
        }

        alpha_t = ALPHA_INICIAL * exp(-(float)t / TEMPO_CONSTANTE);
        R_t = RAIO_INICIAL * exp(-(float)t / TEMPO_CONSTANTE);

        for (int i_x = 0; i_x < MAPA_LADO_X; i_x++)
        {
            for (int i_y = 0; i_y < MAPA_LADO_Y; i_y++)
            {
                float dx = (float)(i_x - v_x);
                float dy = (float)(i_y - v_y);
                float dist_quadrada_topologica = dx * dx + dy * dy;

                float h_ci = 0.0;
                if (R_t <= 0.1)
                    h_ci = (i_x == v_x && i_y == v_y) ? 1.0 : 0.0;
                else
                    h_ci = exp(-dist_quadrada_topologica / (2.0f * R_t * R_t));

                if (h_ci > 0.01) // Limiar mais baixo para influência mais suave
                {
                    for (int d = 0; d < DIMENSOES; d++)
                    {
                        float fator_ajuste = alpha_t * h_ci;
                        mapa[i_x][i_y][d] += fator_ajuste * (dados_entrada[indice_selecionado][d] - mapa[i_x][i_y][d]);
                    }
                }
            }
        }
        
        // Mostrar progresso a cada 500 iterações
        if(t % 500 == 0) {
            printf("Iteracao %d/%d - Alpha: %.4f - Raio: %.2f\n", 
                   t, ITERACOES_MAX, alpha_t, R_t);
        }
    }

    printf("\n=== MAPA FINAL (Pesos ESPACIALMENTE ORDENADOS) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            // Mostrar ano normalizado (primeira dimensão)
            printf("%.2f ", mapa[x][y][0]);
        }
        printf("\n");
    }

    // MAPEAR decadaS NO SOM TREINADO (baseado no ano normalizado)
    int contador_decadas[MAPA_LADO_X][MAPA_LADO_Y][10]; // Para diferentes decadas

    for(int i = 0; i < MAPA_LADO_X; i++) {
        for(int j = 0; j < MAPA_LADO_Y; j++) {
            for(int c = 0; c < 10; c++) {
                contador_decadas[i][j][c] = 0;
            }
        }
    }

    for(int sample = 0; sample < total_amostras_lidas; sample++) {
        float min_distancia = INFINITY;
        int bmu_x = 0, bmu_y = 0;
        
        // Encontrar BMU para esta amostra
        for (int i = 0; i < MAPA_LADO_X; i++) {
            for (int j = 0; j < MAPA_LADO_Y; j++) {
                float soma_quadrados = 0.0;
                for (int d = 0; d < DIMENSOES; d++) { // Usar todas as dimensões (ano + gêneros)
                    float diff = dados_entrada[sample][d] - mapa[i][j][d];
                    soma_quadrados += diff * diff;
                }
                float dist = sqrt(soma_quadrados);
                
                if (dist < min_distancia) {
                    min_distancia = dist;
                    bmu_x = i;
                    bmu_y = j;
                }
            }
        }
        
    // Converter ano normalizado para década (aproximação mais refinada)
        float ano_norm = dados_entrada[sample][0];
        int decada = (int)(ano_norm * 9); // Mapear 0-1 para 0-9 décadas
        if(decada > 9) decada = 9;
        if(decada < 0) decada = 0;
        contador_decadas[bmu_x][bmu_y][decada]++;
    }

    // MOSTRAR MAPA DE decadaS DOMINANTES
    printf("\n=== MAPA DE DECADAS DOMINANTES ===\n");
    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            int decada_dominante = 0;
            int max_count = 0;
            
            for(int c = 0; c < 10; c++) {
                if(contador_decadas[i][j][c] > max_count) {
                    max_count = contador_decadas[i][j][c];
                    decada_dominante = c;
                }
            }
            
            if(max_count > 0) {
                printf("%d ", decada_dominante);
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }

    // ANÁLISE DE QUALIDADE DO MAPA
    printf("\n=== ANALISE DE QUALIDADE ===\n");
    int neuronios_puros = 0;
    int neuronios_mistos = 0;
    int neuronios_vazios = 0;

    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            int decadas_no_neuronio = 0;
            int total_amostras_neuronio = 0;
            
            for(int c = 0; c < 10; c++) {
                if(contador_decadas[i][j][c] > 0) {
                    decadas_no_neuronio++;
                    total_amostras_neuronio += contador_decadas[i][j][c];
                }
            }
            
            if(total_amostras_neuronio == 0) {
                neuronios_vazios++;
            } else if(decadas_no_neuronio == 1) {
                neuronios_puros++;
            } else {
                neuronios_mistos++;
            }
        }
    }

    printf("Neuronios puros (1 decada): %d\n", neuronios_puros);
    printf("Neuronios mistos (>1 decada): %d\n", neuronios_mistos);
    printf("Neuronios vazios (0 amostras): %d\n", neuronios_vazios);
    if(neuronios_puros + neuronios_mistos > 0) {
        printf("Pureza do mapa: %.1f%%\n", 
               (float)neuronios_puros / (neuronios_puros + neuronios_mistos) * 100);
    }

    // ANÁLISE DE GÊNEROS POR NEURÔNIO
    printf("\n=== ANALISE DETALHADA DE GÊNEROS POR NEURÔNIO ===\n");
    const char* generos[] = {"Guerra", "Animacao", "Noir", "Cult", "Misterio", 
                            "Crime", "Terror", "Romance", "Comedia", "Acao", 
                            "Ficcao_Cientifica", "Musica", "Drama", "Suspense"};
    
    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            printf("Neuronio (%d,%d): ", i, j);
            
            // Criar array de índices e pesos para ordenação
            int indices[14];
            float pesos[14];
            
            for(int g = 1; g < DIMENSOES; g++) {
                indices[g-1] = g-1;
                pesos[g-1] = mapa[i][j][g];
            }
            
            // Ordenação simples por bubble sort (top 3)
            for(int x = 0; x < 3; x++) {
                for(int y = x+1; y < 14; y++) {
                    if(pesos[y] > pesos[x]) {
                        float temp_peso = pesos[x];
                        int temp_indice = indices[x];
                        pesos[x] = pesos[y];
                        indices[x] = indices[y];
                        pesos[y] = temp_peso;
                        indices[y] = temp_indice;
                    }
                }
            }
            
            printf("Top3: %s(%.2f), %s(%.2f), %s(%.2f) | Ano: %.2f\n", 
                   generos[indices[0]], pesos[0],
                   generos[indices[1]], pesos[1], 
                   generos[indices[2]], pesos[2],
                   mapa[i][j][0]);
        }
    }

    // ANÁLISE DE DISTRIBUIÇÃO TEMPORAL
    printf("\n=== ANALISE DE DISTRIBUICAO DOS ANOS ===\n");
    float min_ano = 1.0, max_ano = 0.0, soma_anos = 0.0;
    for(int i = 0; i < total_amostras_lidas; i++) {
        if(dados_entrada[i][0] < min_ano) min_ano = dados_entrada[i][0];
        if(dados_entrada[i][0] > max_ano) max_ano = dados_entrada[i][0];
        soma_anos += dados_entrada[i][0];
    }
    printf("Ano normalizado - Min: %.3f, Max: %.3f, Media: %.3f\n", 
           min_ano, max_ano, soma_anos/total_amostras_lidas);

    // ANÁLISE DE ORGANIZAÇÃO ESPACIAL
    printf("\n=== ANALISE DE ORGANIZACAO ESPACIAL ===\n");
    printf("Mapa de Anos (organizacao temporal):\n");
    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            printf("%.2f ", mapa[i][j][0]);
        }
        printf("\n");
    }

    // Calcular erro de quantização médio
    float erro_total = 0.0;
    for(int sample = 0; sample < total_amostras_lidas; sample++) {
        float min_distancia = INFINITY;
        
        for (int i = 0; i < MAPA_LADO_X; i++) {
            for (int j = 0; j < MAPA_LADO_Y; j++) {
                float soma_quadrados = 0.0;
                for (int d = 0; d < DIMENSOES; d++) {
                    float diff = dados_entrada[sample][d] - mapa[i][j][d];
                    soma_quadrados += diff * diff;
                }
                float dist = sqrt(soma_quadrados);
                
                if (dist < min_distancia) {
                    min_distancia = dist;
                }
            }
        }
        erro_total += min_distancia;
    }
    printf("Erro de quantizacao medio: %.4f\n", erro_total / total_amostras_lidas);

    printf("\nTreinamento concluido!\n");

    fclose(in);
    return 0;
}