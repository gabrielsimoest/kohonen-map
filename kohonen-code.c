#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define MAPA_LADO_X 8
#define MAPA_LADO_Y 8
#define DIMENSOES 14
#define AMOSTRAS 178
#define ITERACOES_MAX 4000 // at least 500 times the number of network units

#define ALPHA_INICIAL 0.6
#define RAIO_INICIAL ((MAPA_LADO_X > MAPA_LADO_Y ? MAPA_LADO_X : MAPA_LADO_Y) / 2.0)
#define TEMPO_CONSTANTE (ITERACOES_MAX / 4.0) // Constante de tempo para decaimento

int main()
{
    // Verificar se a classe deve ser separada do mapa de kohonen mesmo
    int x, y, d;
    float mapa[MAPA_LADO_X][MAPA_LADO_Y][DIMENSOES];

    float dados_entrada[AMOSTRAS][DIMENSOES];

    // LEITURA DOS DADOS
    FILE *in;

    if ((in = fopen("wine.data", "rt")) == NULL)
    {
        printf("Cannot open input file.\n");
        return 1;
    }

    x = 0;
    while (!feof(in) && x < AMOSTRAS)
    {
        int result = fscanf(in, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                            &dados_entrada[x][0], &dados_entrada[x][1], &dados_entrada[x][2],
                            &dados_entrada[x][3], &dados_entrada[x][4], &dados_entrada[x][5],
                            &dados_entrada[x][6], &dados_entrada[x][7], &dados_entrada[x][8],
                            &dados_entrada[x][9], &dados_entrada[x][10], &dados_entrada[x][11],
                            &dados_entrada[x][12], &dados_entrada[x][13]);

        if (result == 14)
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

    // NORMALIZAÇÃO DOS DADOS
    printf("Normalizando dados...\n");
    for(int attr = 1; attr < DIMENSOES; attr++) {
        float min_val = dados_entrada[0][attr];
        float max_val = dados_entrada[0][attr];
        
        // Encontrar min e max
        for(int sample = 1; sample < total_amostras_lidas; sample++) {
            if(dados_entrada[sample][attr] < min_val) min_val = dados_entrada[sample][attr];
            if(dados_entrada[sample][attr] > max_val) max_val = dados_entrada[sample][attr];
        }
        
        // Normalizar para [0,1]
        float range = max_val - min_val;
        if(range > 0) {
            for(int sample = 0; sample < total_amostras_lidas; sample++) {
                dados_entrada[sample][attr] = (dados_entrada[sample][attr] - min_val) / range;
            }
        }
    }

    srand(time(NULL));

    for (x = 0; x < MAPA_LADO_X; x++) // linha
    {
        for (y = 0; y < MAPA_LADO_Y; y++) // coluna
        {
            for (d = 0; d < DIMENSOES; d++) // atributo/peso
            {
                mapa[x][y][d] = (float)rand() / RAND_MAX;
            }
        }
    }
    // Mostrar uma representação visual do mapa
    printf("\n=== MAPA INICIAL (Pesos Aleatorios) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            // Mostrar o primeiro peso (índice 0) de cada neurônio
            printf("%.2f ", mapa[x][y][1]);
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
                for (int d = 1; d < DIMENSOES; d++) // Pular classe (d=0)
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
                if (R_t <= 0.5)
                    h_ci = (i_x == v_x && i_y == v_y) ? 1.0 : 0.0;
                else
                    h_ci = exp(-dist_quadrada_topologica / (2.0f * R_t * R_t));

                if (h_ci > 0.001)
                {
                    for (int d = 1; d < DIMENSOES; d++)
                    {
                        float fator_ajuste = alpha_t * h_ci;
                        mapa[i_x][i_y][d] += fator_ajuste * (dados_entrada[indice_selecionado][d] - mapa[i_x][i_y][d]);
                    }
                }
            }
        }
        
        // Mostrar progresso
        if(t % 1000 == 0) {
            printf("Iteração %d/%d - Alpha: %.4f - Raio: %.2f\n", 
                   t, ITERACOES_MAX, alpha_t, R_t);
        }
    }

    printf("\n=== MAPA FINAL (Pesos ESPACIALMENTE ORDENADOS) ===\n");

    for (x = 0; x < MAPA_LADO_X; x++)
    {
        for (y = 0; y < MAPA_LADO_Y; y++)
        {
            // Mostrar primeiro atributo (não classe)
            printf("%.2f ", mapa[x][y][1]);
        }
        printf("\n");
    }

    // MAPEAR CLASSES NO SOM TREINADO
    int contador_classes[MAPA_LADO_X][MAPA_LADO_Y][4]; // [0]=unused, [1,2,3]=classes

    for(int i = 0; i < MAPA_LADO_X; i++) {
        for(int j = 0; j < MAPA_LADO_Y; j++) {
            for(int c = 0; c < 4; c++) {
                contador_classes[i][j][c] = 0;
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
                for (int d = 1; d < DIMENSOES; d++) { // Pular classe
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
        
        // Incrementar contador da classe
        int classe = (int)dados_entrada[sample][0];
        contador_classes[bmu_x][bmu_y][classe]++;
    }

    // MOSTRAR MAPA DE CLASSES
    printf("\n=== MAPA DE CLASSES DOMINANTES ===\n");
    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            int classe_dominante = 0;
            int max_count = 0;
            
            for(int c = 1; c <= 3; c++) {
                if(contador_classes[i][j][c] > max_count) {
                    max_count = contador_classes[i][j][c];
                    classe_dominante = c;
                }
            }
            
            if(max_count > 0) {
                printf("%d ", classe_dominante);
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }

    // ANÁLISE DE QUALIDADE DO MAPA
    printf("\n=== ANÁLISE DE QUALIDADE ===\n");
    int neuronios_puros = 0;
    int neuronios_mistos = 0;
    int neuronios_vazios = 0;

    for (int i = 0; i < MAPA_LADO_X; i++) {
        for (int j = 0; j < MAPA_LADO_Y; j++) {
            int classes_no_neuronio = 0;
            int total_amostras_neuronio = 0;
            
            for(int c = 1; c <= 3; c++) {
                if(contador_classes[i][j][c] > 0) {
                    classes_no_neuronio++;
                    total_amostras_neuronio += contador_classes[i][j][c];
                }
            }
            
            if(total_amostras_neuronio == 0) {
                neuronios_vazios++;
            } else if(classes_no_neuronio == 1) {
                neuronios_puros++;
            } else {
                neuronios_mistos++;
            }
        }
    }

    printf("Neurônios puros (1 classe): %d\n", neuronios_puros);
    printf("Neurônios mistos (>1 classe): %d\n", neuronios_mistos);
    printf("Neurônios vazios (0 amostras): %d\n", neuronios_vazios);
    if(neuronios_puros + neuronios_mistos > 0) {
        printf("Pureza do mapa: %.1f%%\n", 
               (float)neuronios_puros / (neuronios_puros + neuronios_mistos) * 100);
    }

    printf("\nTreinamento concluído!\n");

    fclose(in);
    return 0;
}